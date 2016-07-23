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

#ifndef LIBREPCB_PROJECT_SI_NETPOINT_H
#define LIBREPCB_PROJECT_SI_NETPOINT_H

/*****************************************************************************************
 *  Includes
 ****************************************************************************************/
#include <QtCore>
#include "si_base.h"
#include <librepcbcommon/fileio/if_xmlserializableobject.h>
#include "../../erc/if_ercmsgprovider.h"
#include "../graphicsitems/sgi_netpoint.h"

/*****************************************************************************************
 *  Namespace / Forward Declarations
 ****************************************************************************************/
class QGraphicsItem;

namespace librepcb {
namespace project {

class NetSignal;
class SI_NetLine;
class SI_Symbol;
class SI_SymbolPin;
class SI_NetSegment;
class ErcMsg;

/*****************************************************************************************
 *  Class SI_NetPoint
 ****************************************************************************************/

/**
 * @brief The SI_NetPoint class
 */
class SI_NetPoint final : public SI_Base, public IF_XmlSerializableObject,
                          public IF_ErcMsgProvider
{
        Q_OBJECT
        DECLARE_ERC_MSG_CLASS_NAME(SI_NetPoint)

    public:

        // Constructors / Destructor
        SI_NetPoint() = delete;
        SI_NetPoint(const SI_NetPoint& other) = delete;
        SI_NetPoint(SI_NetSegment& segment, const XmlDomElement& domElement) throw (Exception);
        SI_NetPoint(SI_NetSegment& segment, const Point& position) throw (Exception);
        SI_NetPoint(SI_NetSegment& segment, SI_SymbolPin& pin) throw (Exception);
        ~SI_NetPoint() noexcept;

        // Getters
        const Uuid& getUuid() const noexcept {return mUuid;}
        bool isAttachedToPin() const noexcept {return (mSymbolPin ? true : false);}
        bool isVisible() const noexcept;
        SI_NetSegment& getNetSegment() const noexcept {return mNetSegment;}
        NetSignal& getNetSignalOfNetSegment() const noexcept;
        SI_SymbolPin* getSymbolPin() const noexcept {return mSymbolPin;}
        const QList<SI_NetLine*>& getLines() const noexcept {return mRegisteredLines;}
        bool isUsed() const noexcept {return (mRegisteredLines.count() > 0);}

        // Setters
        void setPinToAttach(SI_SymbolPin* pin) throw (Exception);
        void setPosition(const Point& position) noexcept;

        // General Methods
        void addToSchematic() throw (Exception) override;
        void removeFromSchematic() throw (Exception) override;
        void registerNetLine(SI_NetLine& netline) throw (Exception);
        void unregisterNetLine(SI_NetLine& netline) throw (Exception);
        void updateLines() const noexcept;

        /// @copydoc IF_XmlSerializableObject#serializeToXmlDomElement()
        XmlDomElement* serializeToXmlDomElement() const throw (Exception) override;


        // Inherited from SI_Base
        Type_t getType() const noexcept override {return SI_Base::Type_t::NetPoint;}
        const Point& getPosition() const noexcept override {return mPosition;}
        QPainterPath getGrabAreaScenePx() const noexcept override;
        void setSelected(bool selected) noexcept override;

        // Operator Overloadings
        SI_NetPoint& operator=(const SI_NetPoint& rhs) = delete;
        bool operator==(const SI_NetPoint& rhs) noexcept {return (this == &rhs);}
        bool operator!=(const SI_NetPoint& rhs) noexcept {return (this != &rhs);}


    private:

        void init() throw (Exception);

        /// @copydoc IF_XmlSerializableObject#checkAttributesValidity()
        bool checkAttributesValidity() const noexcept override;


        // General
        QScopedPointer<SGI_NetPoint> mGraphicsItem;
        QMetaObject::Connection mHighlightChangedConnection;

        // Attributes
        SI_NetSegment& mNetSegment;
        Uuid mUuid;
        Point mPosition;
        SI_SymbolPin* mSymbolPin;   ///< only needed if the netpoint is attached to a pin

        // Registered Elements
        QList<SI_NetLine*> mRegisteredLines;    ///< all registered netlines

        // ERC Messages
        /// @brief The ERC message for dead netpoints
        QScopedPointer<ErcMsg> mErcMsgDeadNetPoint;
};

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb

#endif // LIBREPCB_PROJECT_SI_NETPOINT_H
