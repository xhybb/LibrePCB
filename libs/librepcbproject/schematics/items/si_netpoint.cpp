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
#include "si_netpoint.h"
#include "si_netline.h"
#include "si_symbol.h"
#include "si_symbolpin.h"
#include "si_netsegment.h"
#include "../schematic.h"
#include "../../project.h"
#include "../../circuit/circuit.h"
#include "../../circuit/netsignal.h"
#include "../../circuit/componentsignalinstance.h"
#include "../../erc/ercmsg.h"
#include <librepcbcommon/fileio/xmldomelement.h>
#include <librepcbcommon/graphics/graphicsscene.h>
#include <librepcbcommon/scopeguardlist.h>

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

SI_NetPoint::SI_NetPoint(SI_NetSegment& segment, const XmlDomElement& domElement) throw (Exception) :
    SI_Base(segment.getSchematic()), mNetSegment(segment), mSymbolPin(nullptr)
{
    // read attributes
    mUuid = domElement.getAttribute<Uuid>("uuid", true);
    if (domElement.getAttribute<bool>("attached", true)) {
        Uuid symbolUuid = domElement.getAttribute<Uuid>("symbol", true);
        SI_Symbol* symbol = mSchematic.getSymbolByUuid(symbolUuid);
        if (!symbol) {
            throw RuntimeError(__FILE__, __LINE__, symbolUuid.toStr(),
                QString(tr("Invalid symbol UUID: \"%1\"")).arg(symbolUuid.toStr()));
        }
        Uuid pinUuid = domElement.getAttribute<Uuid>("pin", true);
        mSymbolPin = symbol->getPin(pinUuid);
        if (!mSymbolPin) {
            throw RuntimeError(__FILE__, __LINE__, pinUuid.toStr(),
                QString(tr("Invalid symbol pin UUID: \"%1\"")).arg(pinUuid.toStr()));
        }
        mPosition = mSymbolPin->getPosition();
    } else {
        mPosition.setX(domElement.getAttribute<Length>("x", true));
        mPosition.setY(domElement.getAttribute<Length>("y", true));
    }

    init();
}

SI_NetPoint::SI_NetPoint(SI_NetSegment& segment, const Point& position) throw (Exception) :
    SI_Base(segment.getSchematic()), mNetSegment(segment), mUuid(Uuid::createRandom()),
    mPosition(position), mSymbolPin(nullptr)
{
    init();
}

SI_NetPoint::SI_NetPoint(SI_NetSegment& segment, SI_SymbolPin& pin) throw (Exception) :
    SI_Base(segment.getSchematic()), mNetSegment(segment), mUuid(Uuid::createRandom()),
    mPosition(pin.getPosition()), mSymbolPin(&pin)
{
    init();
}

void SI_NetPoint::init() throw (Exception)
{
    // create the graphics item
    mGraphicsItem.reset(new SGI_NetPoint(*this));
    mGraphicsItem->setPos(mPosition.toPxQPointF());

    // create ERC messages
    mErcMsgDeadNetPoint.reset(new ErcMsg(mSchematic.getProject(), *this,
        mUuid.toStr(), "Dead", ErcMsg::ErcMsgType_t::SchematicError,
        QString(tr("Dead net point in schematic page \"%1\": %2"))
        .arg(mSchematic.getName()).arg(mUuid.toStr())));

    if (!checkAttributesValidity()) throw LogicError(__FILE__, __LINE__);
}

SI_NetPoint::~SI_NetPoint() noexcept
{
    mGraphicsItem.reset();
}

/*****************************************************************************************
 *  Getters
 ****************************************************************************************/

bool SI_NetPoint::isVisible() const noexcept
{
    if (mRegisteredLines.count() > 2) {
        return true;
    } else if ((mRegisteredLines.count() > 1) && isAttachedToPin()) {
        return true;
    } else {
        return false;
    }
}

NetSignal& SI_NetPoint::getNetSignalOfNetSegment() const noexcept
{
    return mNetSegment.getNetSignal();
}

/*****************************************************************************************
 *  Setters
 ****************************************************************************************/

void SI_NetPoint::setPinToAttach(SI_SymbolPin* pin) throw (Exception)
{
    if (pin == mSymbolPin) {
        return;
    }
    if ((isUsed()) || ((pin) && (pin->getSchematic() != getSchematic()))) {
        throw LogicError(__FILE__, __LINE__);
    }
    if (isAddedToSchematic()) {
        ScopeGuardList sgl;
        if (mSymbolPin) {
            // detach from current pin
            mSymbolPin->unregisterNetPoint(*this); // can throw
            sgl.add([&](){mSymbolPin->registerNetPoint(*this);});
        }
        if (pin) {
            // attach to new pin
            if (pin->getCompSigInstNetSignal() != &mNetSegment.getNetSignal()) {
                throw LogicError(__FILE__, __LINE__);
            }
            pin->registerNetPoint(*this); // can throw
            sgl.add([&](){pin->unregisterNetPoint(*this);});
            setPosition(pin->getPosition());
        }
        sgl.dismiss();
    }
    mSymbolPin = pin;
    mGraphicsItem->updateCacheAndRepaint();
}

void SI_NetPoint::setPosition(const Point& position) noexcept
{
    if (position != mPosition) {
        mPosition = position;
        mGraphicsItem->setPos(mPosition.toPxQPointF());
        updateLines();
    }
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

void SI_NetPoint::addToSchematic() throw (Exception)
{
    if (isAddedToSchematic() || isUsed()) {
        throw LogicError(__FILE__, __LINE__);
    }

    if (isAttachedToPin()) {
        // check if mNetSignal is correct (would be a bug if not)
        if (mSymbolPin->getCompSigInstNetSignal() != &mNetSegment.getNetSignal()) {
            throw LogicError(__FILE__, __LINE__);
        }
        mSymbolPin->registerNetPoint(*this); // can throw
    }

    mHighlightChangedConnection = connect(&getNetSignalOfNetSegment(),
                                          &NetSignal::highlightedChanged,
                                          [this](){mGraphicsItem->update();});
    mErcMsgDeadNetPoint->setVisible(true);
    SI_Base::addToSchematic(mGraphicsItem.data());
}

void SI_NetPoint::removeFromSchematic() throw (Exception)
{
    if ((!isAddedToSchematic()) || isUsed()) {
        throw LogicError(__FILE__, __LINE__);
    }

    if (isAttachedToPin()) {
        // check if mNetSignal is correct (would be a bug if not)
        if (mSymbolPin->getCompSigInstNetSignal() != &mNetSegment.getNetSignal()) {
            throw LogicError(__FILE__, __LINE__);
        }
        mSymbolPin->unregisterNetPoint(*this); // can throw
    }

    disconnect(mHighlightChangedConnection);
    mErcMsgDeadNetPoint->setVisible(false);
    SI_Base::removeFromSchematic(mGraphicsItem.data());
}

void SI_NetPoint::registerNetLine(SI_NetLine& netline) throw (Exception)
{
    if ((!isAddedToSchematic()) || (mRegisteredLines.contains(&netline))
        || (netline.getSchematic() != mSchematic))
    {
        throw LogicError(__FILE__, __LINE__);
    }
    mRegisteredLines.append(&netline);
    netline.updateLine();
    mGraphicsItem->updateCacheAndRepaint();
    mErcMsgDeadNetPoint->setVisible(mRegisteredLines.isEmpty());
}

void SI_NetPoint::unregisterNetLine(SI_NetLine& netline) throw (Exception)
{
    if ((!isAddedToSchematic()) || (!mRegisteredLines.contains(&netline))) {
        throw LogicError(__FILE__, __LINE__);
    }
    mRegisteredLines.removeOne(&netline);
    netline.updateLine();
    mGraphicsItem->updateCacheAndRepaint();
    mErcMsgDeadNetPoint->setVisible(mRegisteredLines.isEmpty());
}

void SI_NetPoint::updateLines() const noexcept
{
    foreach (SI_NetLine* line, mRegisteredLines) {
        line->updateLine();
    }
}

XmlDomElement* SI_NetPoint::serializeToXmlDomElement() const throw (Exception)
{
    if (!checkAttributesValidity()) throw LogicError(__FILE__, __LINE__);

    QScopedPointer<XmlDomElement> root(new XmlDomElement("netpoint"));
    root->setAttribute("uuid", mUuid);
    root->setAttribute("attached", isAttachedToPin());
    if (isAttachedToPin()) {
        root->setAttribute("symbol", mSymbolPin->getSymbol().getUuid());
        root->setAttribute("pin", mSymbolPin->getLibPinUuid());
    } else {
        root->setAttribute("x", mPosition.getX());
        root->setAttribute("y", mPosition.getY());
    }
    return root.take();
}

/*****************************************************************************************
 *  Inherited from SI_Base
 ****************************************************************************************/

QPainterPath SI_NetPoint::getGrabAreaScenePx() const noexcept
{
    return mGraphicsItem->shape().translated(mPosition.toPxQPointF());
}

void SI_NetPoint::setSelected(bool selected) noexcept
{
    SI_Base::setSelected(selected);
    mGraphicsItem->update();
}

/*****************************************************************************************
 *  Private Methods
 ****************************************************************************************/

bool SI_NetPoint::checkAttributesValidity() const noexcept
{
    if (mUuid.isNull())                             return false;
    if (isAttachedToPin() && (&mNetSegment.getNetSignal() != mSymbolPin->getCompSigInstNetSignal())) return false;
    return true;
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
