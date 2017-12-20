/**
******************************************************************************
*
* @file       graphicsitem.h
* @author     Dino Hüllmann
* @brief      Base class for graphics items
* @see        The GNU Public License (GPL) Version 3
* @defgroup   OPMapWidget
* @{
*
*****************************************************************************/
/*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifndef GRAPHICSITEM_H
#define GRAPHICSITEM_H

#include <QObject>
#include <QGraphicsItem>
#include "../internals/pointlatlng.h"

namespace mapcontrol
{
    class MapGraphicItem;
    class OPMapWidget;

    /**
     * @brief Base class for graphics items shown on the map
     *
     * @class GraphicsItem graphicsitem.h "graphicsitem.h"
     */
    class GraphicsItem : public QObject, public QGraphicsItem
    {
        Q_OBJECT
        Q_INTERFACES(QGraphicsItem)
    public:
        GraphicsItem(MapGraphicItem* map, OPMapWidget* parent)
            : map(map), mapwidget(parent)
        {}

        virtual ~GraphicsItem() {}

        virtual void RefreshPos() = 0;
    protected:
        MapGraphicItem* map;
        OPMapWidget* mapwidget;
        QPixmap picture;
        internals::PointLatLng coord;
    };
} // namespace mapcontrol

#endif // GRAPHICSITEM_H
