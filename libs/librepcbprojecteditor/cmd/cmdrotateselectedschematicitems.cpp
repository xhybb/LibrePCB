/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * http://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "cmdrotateselectedschematicitems.h"
#include <librepcbcommon/gridproperties.h>
#include <librepcbproject/project.h>
#include <librepcbproject/schematics/schematic.h>
#include <librepcbproject/schematics/items/si_symbol.h>
#include <librepcbproject/schematics/items/si_symbolpin.h>
#include <librepcbproject/schematics/items/si_netpoint.h>
#include <librepcbproject/schematics/items/si_netline.h>
#include <librepcbproject/schematics/items/si_netlabel.h>
#include <librepcbproject/schematics/cmd/cmdsymbolinstanceedit.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetlabeledit.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetpointedit.h>
#include <librepcbproject/schematics/schematicselectionquery.h>

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

CmdRotateSelectedSchematicItems::CmdRotateSelectedSchematicItems(Schematic& schematic,
                                                                 const Angle& angle) noexcept :
    UndoCommandGroup(tr("Rotate Schematic Elements")), mSchematic(schematic), mAngle(angle)
{
}

CmdRotateSelectedSchematicItems::~CmdRotateSelectedSchematicItems() noexcept
{
}

/*****************************************************************************************
 *  Inherited from UndoCommand
 ****************************************************************************************/

bool CmdRotateSelectedSchematicItems::performExecute() throw (Exception)
{
    // get all selected items
    QScopedPointer<SchematicSelectionQuery> query(mSchematic.createSelectionQuery());
    query->addSelectedSymbols();
    query->addSelectedNetPoints(SchematicSelectionQuery::NetPointFilter::Floating);
    query->addSelectedNetLines(SchematicSelectionQuery::NetLineFilter::All);
    query->addSelectedNetLabels();
    query->addNetPointsOfNetLines(SchematicSelectionQuery::NetLineFilter::All,
                                  SchematicSelectionQuery::NetPointFilter::Floating);

    // find the center of all elements
    Point center = Point(0, 0);
    int count = 0;
    foreach (SI_Symbol* symbol, query->getSymbols()) {
        center += symbol->getPosition();
        ++count;
    }
    foreach (SI_NetPoint* netpoint, query->getNetPoints()) {
        center += netpoint->getPosition();
        ++count;
    }
    foreach (SI_NetLabel* netlabel, query->getNetLabels()) {
        center += netlabel->getPosition();
        ++count;
    }
    if (count > 0) {
        center /= count;
        center.mapToGrid(mSchematic.getGridProperties().getInterval());
    } else {
        // no items selected --> nothing to do here
        return false;
    }

    // rotate all selected elements
    foreach (SI_Symbol* symbol, query->getSymbols()) {
        CmdSymbolInstanceEdit* cmd = new CmdSymbolInstanceEdit(*symbol);
        cmd->rotate(mAngle, center, false);
        appendChild(cmd);
    }
    foreach (SI_NetPoint* netpoint, query->getNetPoints()) {
        CmdSchematicNetPointEdit* cmd = new CmdSchematicNetPointEdit(*netpoint);
        cmd->setPosition(netpoint->getPosition().rotated(mAngle, center), false);
        appendChild(cmd);
    }
    foreach (SI_NetLabel* netlabel, query->getNetLabels()) {
        CmdSchematicNetLabelEdit* cmd = new CmdSchematicNetLabelEdit(*netlabel);
        cmd->rotate(mAngle, center, false);
        appendChild(cmd);
    }

    // execute all child commands
    return UndoCommandGroup::performExecute(); // can throw
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
