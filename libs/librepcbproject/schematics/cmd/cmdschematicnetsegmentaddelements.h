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

#ifndef LIBREPCB_PROJECT_CMDSCHEMATICNETSEGMENTADDELEMENTS_H
#define LIBREPCB_PROJECT_CMDSCHEMATICNETSEGMENTADDELEMENTS_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include <librepcbcommon/undocommand.h>
#include <librepcbcommon/units/point.h>

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {
namespace project {

class NetSignal;
class SI_NetSegment;
class SI_SymbolPin;
class SI_NetPoint;
class SI_NetLine;

/*****************************************************************************************
 *  Class CmdSchematicNetSegmentAddElements
 ****************************************************************************************/

/**
 * @brief The CmdSchematicNetSegmentAddElements class
 */
class CmdSchematicNetSegmentAddElements final : public UndoCommand
{
    public:

        // Constructors / Destructor
        CmdSchematicNetSegmentAddElements(SI_NetSegment& segment) noexcept;
        ~CmdSchematicNetSegmentAddElements() noexcept;

        // General Methods
        SI_NetPoint* addNetPoint(SI_NetPoint& netpoint) throw (Exception);
        SI_NetPoint* addNetPoint(const Point& position) throw (Exception);
        SI_NetPoint* addNetPoint(SI_SymbolPin& pin) throw (Exception);
        SI_NetLine* addNetLine(SI_NetLine& netline) throw (Exception);
        SI_NetLine* addNetLine(SI_NetPoint& startPoint, SI_NetPoint& endPoint) throw (Exception);


    private:

        // Private Methods

        /// @copydoc UndoCommand::performExecute()
        bool performExecute() throw (Exception) override;

        /// @copydoc UndoCommand::performUndo()
        void performUndo() throw (Exception) override;

        /// @copydoc UndoCommand::performRedo()
        void performRedo() throw (Exception) override;


        // Private Member Variables

        SI_NetSegment& mNetSegment;
        QList<SI_NetPoint*> mNetPoints;
        QList<SI_NetLine*> mNetLines;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb

#endif // LIBREPCB_PROJECT_CMDSCHEMATICNETSEGMENTADDELEMENTS_H
