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
#include "cmdcombineschematicnetpoints.h"
#include <librepcbcommon/scopeguard.h>
#include <librepcbproject/circuit/netsignal.h>
#include <librepcbproject/schematics/items/si_netpoint.h>
#include <librepcbproject/schematics/items/si_netline.h>
#include <librepcbproject/schematics/items/si_netlabel.h>
#include <librepcbproject/schematics/items/si_netsegment.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentremoveelements.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentaddelements.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetlabelremove.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetlabeladd.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentremove.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentadd.h>

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

CmdCombineSchematicNetPoints::CmdCombineSchematicNetPoints(SI_NetPoint& toBeRemoved,
                                                           SI_NetPoint& result) noexcept :
    UndoCommandGroup(tr("Combine Schematic Netpoints")),
    mNetPointToBeRemoved(toBeRemoved), mResultingNetPoint(result)
{
}

CmdCombineSchematicNetPoints::~CmdCombineSchematicNetPoints() noexcept
{
}

/*****************************************************************************************
 *  Inherited from UndoCommand
 ****************************************************************************************/

bool CmdCombineSchematicNetPoints::performExecute() throw (Exception)
{
    // if an error occurs, undo all already executed child commands
    auto undoScopeGuard = scopeGuard([&](){performUndo();});

    // TODO: do not create redundant netlines!

    // if the netpoints are from two different netsegment, remove & combine both segments
    // TODO: maybe add the ability to change netsegment of netpoints/netlines instead
    //       of remove these items and add a completely new items (attributes are lost!)
    SI_NetSegment& netSegmentToBeRemoved = mNetPointToBeRemoved.getNetSegment();
    SI_NetSegment& resultingNetSegment = mResultingNetPoint.getNetSegment();
    if (netSegmentToBeRemoved != resultingNetSegment) {
        // remove netsegment
        execNewChildCmd(new CmdSchematicNetSegmentRemove(netSegmentToBeRemoved)); // can throw
        // copy all netpoints
        QHash<SI_NetPoint*, SI_NetPoint*> netPointMap;
        foreach (SI_NetPoint* netpoint, netSegmentToBeRemoved.getNetPoints()) {
            if (netpoint == &mNetPointToBeRemoved) {
                netPointMap.insert(&mNetPointToBeRemoved, &mResultingNetPoint);
            } else {
                SI_NetPoint* createdNetPoint = nullptr;
                auto* cmd = new CmdSchematicNetSegmentAddElements(resultingNetSegment);
                if (netpoint->isAttachedToPin()) {
                    SI_SymbolPin* pin = netpoint->getSymbolPin(); Q_ASSERT(pin);
                    createdNetPoint = cmd->addNetPoint(*pin);
                } else {
                    createdNetPoint = cmd->addNetPoint(netpoint->getPosition());
                }
                execNewChildCmd(cmd); // can throw
                Q_ASSERT(createdNetPoint);
                netPointMap.insert(netpoint, createdNetPoint);
            }
        }
        // copy all netlines
        foreach (SI_NetLine* netline, netSegmentToBeRemoved.getNetLines()) {
            SI_NetPoint* p1 = netPointMap.value(&netline->getStartPoint()); Q_ASSERT(p1);
            SI_NetPoint* p2 = netPointMap.value(&netline->getEndPoint()); Q_ASSERT(p2);
            auto* cmd = new CmdSchematicNetSegmentAddElements(resultingNetSegment);
            cmd->addNetLine(*p1, *p2);
            execNewChildCmd(cmd); // can throw
        }
        // copy all netlabels
        foreach (SI_NetLabel* netlabel, netSegmentToBeRemoved.getNetLabels()) {
            CmdSchematicNetLabelAdd* cmd = new CmdSchematicNetLabelAdd(resultingNetSegment, netlabel->getPosition());
            execNewChildCmd(cmd); // can throw
        }
    } else {
        // both netpoints are in the same netsegment

        // change netpoint of all affected netlines
        foreach (SI_NetLine* line, mNetPointToBeRemoved.getLines()) {
            SI_NetPoint* otherPoint = line->getOtherPoint(mNetPointToBeRemoved); Q_ASSERT(otherPoint);
            // TODO: maybe add the ability to change start-/endpoint of lines instead
            //       of remove the line and add a completely new line (attributes are lost!)
            auto* cmd = new CmdSchematicNetSegmentRemoveElements(line->getNetSegment());
            cmd->removeNetLine(*line);
            execNewChildCmd(cmd); // can throw
            if (otherPoint != &mResultingNetPoint) {
                auto* cmd = new CmdSchematicNetSegmentAddElements(resultingNetSegment);
                cmd->addNetLine(mResultingNetPoint, *otherPoint);
                execNewChildCmd(cmd); // can throw
            }
        }

        // remove the unused netpoint
        auto* cmd = new CmdSchematicNetSegmentRemoveElements(mNetPointToBeRemoved.getNetSegment());
        cmd->removeNetPoint(mNetPointToBeRemoved);
        execNewChildCmd(cmd); // can throw
    }

    undoScopeGuard.dismiss(); // no undo required
    return true;
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
