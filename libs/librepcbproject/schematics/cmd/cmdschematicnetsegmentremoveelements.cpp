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
#include "cmdschematicnetsegmentremoveelements.h"
#include "../schematic.h"
#include "../items/si_netpoint.h"
#include "../items/si_netline.h"
#include "../items/si_netsegment.h"

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

CmdSchematicNetSegmentRemoveElements::CmdSchematicNetSegmentRemoveElements(SI_NetSegment& segment) noexcept :
    UndoCommand(tr("Remove net segment elements")),
    mNetSegment(segment)
{
}

CmdSchematicNetSegmentRemoveElements::~CmdSchematicNetSegmentRemoveElements() noexcept
{
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

void CmdSchematicNetSegmentRemoveElements::removeNetPoint(SI_NetPoint& netpoint) throw (Exception)
{
    mNetPoints.append(&netpoint);
}

void CmdSchematicNetSegmentRemoveElements::removeNetLine(SI_NetLine& netline) throw (Exception)
{
    mNetLines.append(&netline);
}

/*****************************************************************************************
 *  Inherited from UndoCommand
 ****************************************************************************************/

bool CmdSchematicNetSegmentRemoveElements::performExecute() throw (Exception)
{
    performRedo(); // can throw

    return true;
}

void CmdSchematicNetSegmentRemoveElements::performUndo() throw (Exception)
{
    mNetSegment.addNetPointsAndNetLines(mNetPoints, mNetLines); // can throw
}

void CmdSchematicNetSegmentRemoveElements::performRedo() throw (Exception)
{
    mNetSegment.removeNetPointsAndNetLines(mNetPoints, mNetLines); // can throw
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
