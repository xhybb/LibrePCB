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

#ifndef LIBREPCB_PROJECT_CMDSCHEMATICNETLABELADD_H
#define LIBREPCB_PROJECT_CMDSCHEMATICNETLABELADD_H

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
class Schematic;
class SI_NetLabel;
class SI_NetSegment;

/*****************************************************************************************
 *  Class CmdSchematicNetLabelAdd
 ****************************************************************************************/

/**
 * @brief The CmdSchematicNetLabelAdd class
 */
class CmdSchematicNetLabelAdd final : public UndoCommand
{
    public:

        // Constructors / Destructor
        CmdSchematicNetLabelAdd(SI_NetSegment& segment, const Point& position) noexcept;
        ~CmdSchematicNetLabelAdd() noexcept;

        // Getters
        SI_NetLabel* getNetLabel() const noexcept {return mNetLabel;}


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
        Point mPosition;
        SI_NetLabel* mNetLabel;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb

#endif // LIBREPCB_PROJECT_CMDSCHEMATICNETLABELADD_H
