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
#include "cmdchangenetsignalofschematicnetsegment.h"
#include <librepcbproject/project.h>
#include <librepcbproject/circuit/netsignal.h>
#include <librepcbproject/circuit/componentsignalinstance.h>
#include <librepcbproject/circuit/cmd/cmdcompsiginstsetnetsignal.h>
#include <librepcbproject/schematics/items/si_netsegment.h>
#include <librepcbproject/schematics/items/si_netpoint.h>
#include <librepcbproject/schematics/items/si_symbolpin.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentadd.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentremove.h>
#include <librepcbproject/schematics/cmd/cmdschematicnetsegmentedit.h>
#include <librepcbproject/boards/items/bi_netpoint.h>
#include <librepcbproject/boards/items/bi_footprintpad.h>
#include <librepcbproject/boards/cmd/cmdboardnetlineremove.h>
#include <librepcbproject/boards/cmd/cmdboardnetpointremove.h>
#include "cmdcombinenetsignals.h"

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

CmdChangeNetSignalOfSchematicNetSegment::CmdChangeNetSignalOfSchematicNetSegment(
        SI_NetSegment& seg, NetSignal& newSig) noexcept :
    UndoCommandGroup(tr("Change netsignal of netsegment")),
    mNetSegment(seg), mNewNetSignal(newSig)
{
}

CmdChangeNetSignalOfSchematicNetSegment::~CmdChangeNetSignalOfSchematicNetSegment() noexcept
{
}

/*****************************************************************************************
 *  Inherited from UndoCommand
 ****************************************************************************************/

bool CmdChangeNetSignalOfSchematicNetSegment::performExecute() throw (Exception)
{
    if (mNewNetSignal == mNetSegment.getNetSignal()) {
        // nothing to do, the netsignal is already correct
        return false;
    } else if (mNetSegment.getNetSignal().getSchematicNetSegments().count() == 1) {
        // this netsegment is the only one in its netsignal,
        // we just need to combine both netsignals
        Circuit& circuit = mNetSegment.getCircuit();
        NetSignal& toBeRemoved = mNetSegment.getNetSignal();
        NetSignal& result = mNewNetSignal;
        appendChild(new CmdCombineNetSignals(circuit, toBeRemoved, result));
    } else {
        // there are still some other netsegments with the same netsignal
        Q_ASSERT(mNetSegment.getNetSignal().getSchematicNetSegments().count() > 1);
        changeNetSignalOfNetSegment();
    }

    // execute all child commands
    return UndoCommandGroup::performExecute(); // can throw
}

void CmdChangeNetSignalOfSchematicNetSegment::changeNetSignalOfNetSegment() throw (Exception)
{
    // remove netsegment
    appendChild(new CmdSchematicNetSegmentRemove(mNetSegment));

    // set netsignal of netsegment
    CmdSchematicNetSegmentEdit* cmd = new CmdSchematicNetSegmentEdit(mNetSegment);
    cmd->setNetSignal(mNewNetSignal);
    appendChild(cmd);

    // change netsignal of all connected symbol pins (resp. their component signals)
    foreach (SI_NetPoint* netpoint, mNetSegment.getNetPoints()) { Q_ASSERT(netpoint);
        if (netpoint->isAttachedToPin()) {
            SI_SymbolPin* pin = netpoint->getSymbolPin(); Q_ASSERT(pin);
            Q_ASSERT(pin->getCompSigInstNetSignal() == &mNetSegment.getNetSignal());
            ComponentSignalInstance* sig = pin->getComponentSignalInstance();
            if (sig) {
                updateCompSigInstNetSignal(*sig);
            }
        }
    }

    // re-add netsegment
    appendChild(new CmdSchematicNetSegmentAdd(mNetSegment));
}

void CmdChangeNetSignalOfSchematicNetSegment::updateCompSigInstNetSignal(ComponentSignalInstance& cmpSig) throw (Exception)
{
    // disconnect traces from pads in all boards
    foreach (BI_FootprintPad* pad, cmpSig.getRegisteredFootprintPads()) { Q_ASSERT(pad);
        foreach (BI_NetPoint* point, pad->getNetPoints()) { Q_ASSERT(point);
            foreach (BI_NetLine* line, point->getLines()) { Q_ASSERT(line);
                appendChild(new CmdBoardNetLineRemove(*line));
            }
            appendChild(new CmdBoardNetPointRemove(*point));
        }
    }

    // change netsignal of component signal instance
    appendChild(new CmdCompSigInstSetNetSignal(cmpSig, &mNewNetSignal));
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
