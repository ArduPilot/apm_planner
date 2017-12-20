/**
******************************************************************************
*
* @file       graphicsusertypes.h
* @author     Dino Hüllmann
* @brief      User type definitions for QGraphicsItems
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
#ifndef GRAPHICSUSERTYPES_H
#define GRAPHICSUSERTYPES_H

#include <QGraphicsItem>

namespace mapcontrol
{
    namespace usertypes
    {
        constexpr int WAYPOINTITEM     = QGraphicsItem::UserType + 1;
        constexpr int UAVITEM          = QGraphicsItem::UserType + 2;
        constexpr int TRAILITEM        = QGraphicsItem::UserType + 3;
        constexpr int HOMEITEM         = QGraphicsItem::UserType + 4;
        constexpr int GPSITEM          = QGraphicsItem::UserType + 5;
        constexpr int WAYPOINTLINEITEM = QGraphicsItem::UserType + 6;
        constexpr int TRAILLINEITEM    = QGraphicsItem::UserType + 7;
    } // namespace usertypes
} // namespace mapcontrol

#endif // GRAPHICSUSERTYPES_H
