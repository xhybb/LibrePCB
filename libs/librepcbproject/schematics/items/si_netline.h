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

#ifndef LIBREPCB_PROJECT_SI_NETLINE_H
#define LIBREPCB_PROJECT_SI_NETLINE_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "si_base.h"
#include <librepcbcommon/fileio/if_xmlserializableobject.h>
#include "../graphicsitems/sgi_netline.h"

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
namespace librepcb {
namespace project {

class NetSignal;
class SI_NetPoint;
class SI_NetSegment;

/*****************************************************************************************
 *  Class SI_NetLine
 ****************************************************************************************/

/**
 * @brief The SI_NetLine class
 */
class SI_NetLine final : public SI_Base, public IF_XmlSerializableObject
{
        Q_OBJECT

    public:

        // Constructors / Destructor
        SI_NetLine() = delete;
        SI_NetLine(const SI_NetLine& other) = delete;
        SI_NetLine(SI_NetSegment& segment, const XmlDomElement& domElement) throw (Exception);
        SI_NetLine(SI_NetPoint& startPoint, SI_NetPoint& endPoint, const Length& width) throw (Exception);
        ~SI_NetLine() noexcept;

        // Getters
        SI_NetSegment& getNetSegment() const noexcept {return mNetSegment;}
        const Uuid& getUuid() const noexcept {return mUuid;}
        const Length& getWidth() const noexcept {return mWidth;}
        SI_NetPoint& getStartPoint() const noexcept {return *mStartPoint;}
        SI_NetPoint& getEndPoint() const noexcept {return *mEndPoint;}
        SI_NetPoint* getOtherPoint(const SI_NetPoint& firstPoint) const noexcept;
        NetSignal& getNetSignalOfNetSegment() const noexcept;
        bool isAttachedToSymbol() const noexcept;

        // Setters
        void setWidth(const Length& width) noexcept;

        // General Methods
        void addToSchematic() throw (Exception) override;
        void removeFromSchematic() throw (Exception) override;
        void updateLine() noexcept;

        /// @copydoc IF_XmlSerializableObject#serializeToXmlDomElement()
        XmlDomElement* serializeToXmlDomElement() const throw (Exception) override;


        // Inherited from SI_Base
        Type_t getType() const noexcept override {return SI_Base::Type_t::NetLine;}
        const Point& getPosition() const noexcept override {return mPosition;}
        QPainterPath getGrabAreaScenePx() const noexcept override;
        void setSelected(bool selected) noexcept override;

        // Operator Overloadings
        SI_NetLine& operator=(const SI_NetLine& rhs) = delete;


    private:

        void init() throw (Exception);

        /// @copydoc IF_XmlSerializableObject#checkAttributesValidity()
        bool checkAttributesValidity() const noexcept override;


        // General
        SI_NetSegment& mNetSegment;
        QScopedPointer<SGI_NetLine> mGraphicsItem;
        Point mPosition; ///< the center of startpoint and endpoint
        QMetaObject::Connection mHighlightChangedConnection;

        // Attributes
        Uuid mUuid;
        SI_NetPoint* mStartPoint;
        SI_NetPoint* mEndPoint;
        Length mWidth;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb

#endif // LIBREPCB_PROJECT_SI_NETLINE_H
