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
#include <QtWidgets>
#include <QPrinter>
#include "sgi_netpoint.h"
#include "../items/si_netpoint.h"
#include "../schematic.h"
#include "../../project.h"
#include "../../circuit/netsignal.h"
#include <librepcbcommon/schematiclayer.h>

/*****************************************************************************************
 *  Namespace
 ****************************************************************************************/
namespace librepcb {
namespace project {

QRectF SGI_NetPoint::sBoundingRect;

/*****************************************************************************************
 *  Constructors / Destructor
 ****************************************************************************************/

SGI_NetPoint::SGI_NetPoint(SI_NetPoint& netpoint) noexcept :
    SGI_Base(), mNetPoint(netpoint), mLayer(nullptr)
{
    setZValue(Schematic::ZValue_VisibleNetPoints);

    mLayer = getSchematicLayer(SchematicLayer::Nets);
    Q_ASSERT(mLayer);

    if (sBoundingRect.isNull())
    {
        qreal radius = Length(600000).toPx();
        sBoundingRect = QRectF(-radius, -radius, 2*radius, 2*radius);
    }

    updateCacheAndRepaint();
}

SGI_NetPoint::~SGI_NetPoint() noexcept
{
}

/*****************************************************************************************
 *  General Methods
 ****************************************************************************************/

void SGI_NetPoint::updateCacheAndRepaint() noexcept
{
    setToolTip(mNetPoint.getNetSignalOfNetSegment().getName());

    prepareGeometryChange();
    mPointVisible = mNetPoint.isVisible();
    setZValue(mPointVisible ? Schematic::ZValue_VisibleNetPoints : Schematic::ZValue_HiddenNetPoints);
    update();
}

/*****************************************************************************************
 *  Inherited from QGraphicsItem
 ****************************************************************************************/

void SGI_NetPoint::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    bool highlight = mNetPoint.isSelected() || mNetPoint.getNetSignalOfNetSegment().isHighlighted();

    if (mLayer->isVisible() && mPointVisible)
    {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(mLayer->getColor(highlight), Qt::SolidPattern));
        painter->drawEllipse(sBoundingRect);
    }

#ifdef QT_DEBUG
    SchematicLayer* layer = getSchematicLayer(SchematicLayer::LayerID::DEBUG_InvisibleNetPoints); Q_ASSERT(layer);
    if ((layer->isVisible()) && (!mPointVisible))
    {
        // draw circle
        painter->setPen(QPen(layer->getColor(highlight), 0));
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(sBoundingRect);
    }
    layer = getSchematicLayer(SchematicLayer::LayerID::DEBUG_GraphicsItemsBoundingRect); Q_ASSERT(layer);
    if (layer->isVisible())
    {
        // draw bounding rect
        painter->setPen(QPen(layer->getColor(highlight), 0));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(sBoundingRect);
    }
#endif
}

/*****************************************************************************************
 *  Private Methods
 ****************************************************************************************/

SchematicLayer* SGI_NetPoint::getSchematicLayer(int id) const noexcept
{
    return mNetPoint.getSchematic().getProject().getSchematicLayer(id);
}

/*****************************************************************************************
 *  End of File
 ****************************************************************************************/

} // namespace project
} // namespace librepcb
