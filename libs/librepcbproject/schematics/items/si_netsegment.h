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

#ifndef LIBREPCB_PROJECT_SI_NETSEGMENT_H
#define LIBREPCB_PROJECT_SI_NETSEGMENT_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "si_base.h"
#include <librepcbcommon/fileio/if_xmlserializableobject.h>
#include <librepcbcommon/uuid.h>

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {
namespace project {

class NetSignal;
class SI_NetPoint;
class SI_NetLine;
class SI_NetLabel;

/*****************************************************************************************
 *  Class SI_NetSegment
 ****************************************************************************************/

/**
 * @brief The SI_NetSegment class
 *
 * @todo Do not allow to create empty netsegments!
 */
class SI_NetSegment final : public SI_Base, public IF_XmlSerializableObject
{
        Q_OBJECT

    public:

        // Constructors / Destructor
        SI_NetSegment() = delete;
        SI_NetSegment(const SI_NetSegment& other) = delete;
        SI_NetSegment(Schematic& schematic, const XmlDomElement& domElement) throw (Exception);
        SI_NetSegment(Schematic& schematic, NetSignal& signal) throw (Exception);
        ~SI_NetSegment() noexcept;

        // Getters
        const Uuid& getUuid() const noexcept {return mUuid;}
        NetSignal& getNetSignal() const noexcept {return *mNetSignal;}
        bool isUsed() const noexcept;
        void getSelectedItems(QList<SI_Base*>& items,
                              bool floatingPoints,
                              bool attachedPoints,
                              bool floatingPointsFromFloatingLines,
                              bool attachedPointsFromFloatingLines,
                              bool floatingPointsFromAttachedLines,
                              bool attachedPointsFromAttachedLines,
                              bool floatingLines,
                              bool attachedLines) const noexcept;
        QList<SI_Base*> getItemsAtScenePos(const Point& pos) const noexcept;
        int getNetPointsAtScenePos(const Point& pos, QList<SI_NetPoint*>& points) const noexcept;
        int getNetLinesAtScenePos(const Point& pos, QList<SI_NetLine*>& lines) const noexcept;
        int getNetLabelsAtScenePos(const Point& pos, QList<SI_NetLabel*>& labels) const noexcept;
        QSet<QString> getForcedNetNames() const noexcept;
        QString getForcedNetName() const noexcept;

        // Setters
        void setNetSignal(NetSignal& netsignal) throw (Exception);

        // NetPoint Methods
        const QList<SI_NetPoint*>& getNetPoints() const noexcept {return mNetPoints;}
        SI_NetPoint* getNetPointByUuid(const Uuid& uuid) const noexcept;

        // NetLine Methods
        const QList<SI_NetLine*>& getNetLines() const noexcept {return mNetLines;}
        SI_NetLine* getNetLineByUuid(const Uuid& uuid) const noexcept;

        // NetPoint+NetLine Methods
        void addNetPointsAndNetLines(const QList<SI_NetPoint*>& netpoints,
                                     const QList<SI_NetLine*>& netlines) throw (Exception);
        void removeNetPointsAndNetLines(const QList<SI_NetPoint*>& netpoints,
                                        const QList<SI_NetLine*>& netlines) throw (Exception);

        // NetLabel Methods
        const QList<SI_NetLabel*>& getNetLabels() const noexcept {return mNetLabels;}
        SI_NetLabel* getNetLabelByUuid(const Uuid& uuid) const noexcept;
        void addNetLabel(SI_NetLabel& netlabel) throw (Exception);
        void removeNetLabel(SI_NetLabel& netlabel) throw (Exception);

        // General Methods
        void addToSchematic() throw (Exception) override;
        void removeFromSchematic() throw (Exception) override;
        void setSelectionRect(const QRectF rectPx) noexcept;
        void clearSelection() const noexcept;

        /// @copydoc IF_XmlSerializableObject#serializeToXmlDomElement()
        XmlDomElement* serializeToXmlDomElement() const throw (Exception) override;


        // Inherited from SI_Base
        Type_t getType() const noexcept override {return SI_Base::Type_t::NetSegment;}
        const Point& getPosition() const noexcept override {static Point p(0, 0); return p;}
        QPainterPath getGrabAreaScenePx() const noexcept override;
        bool isSelected() const noexcept;
        void setSelected(bool selected) noexcept override;

        // Operator Overloadings
        SI_NetSegment& operator=(const SI_NetSegment& rhs) = delete;
        bool operator==(const SI_NetSegment& rhs) noexcept {return (this == &rhs);}
        bool operator!=(const SI_NetSegment& rhs) noexcept {return (this != &rhs);}


    private:

        /// @copydoc IF_XmlSerializableObject#checkAttributesValidity()
        bool checkAttributesValidity() const noexcept override;
        bool areAllNetPointsConnectedTogether() const noexcept;
        void findAllConnectedNetPoints(const SI_NetPoint& p, QList<const SI_NetPoint*>& points) const noexcept;


        // Attributes
        Uuid mUuid;
        NetSignal* mNetSignal;

        // Items
        QList<SI_NetPoint*> mNetPoints;
        QList<SI_NetLine*> mNetLines;
        QList<SI_NetLabel*> mNetLabels;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb

#endif // LIBREPCB_PROJECT_SI_NETSEGMENT_H
