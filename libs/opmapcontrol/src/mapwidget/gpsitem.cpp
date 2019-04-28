/**
******************************************************************************
*
* @file       gpsitem.cpp
* @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
* @brief      A graphicsItem representing a UAV
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
#include "../internals/pureprojection.h"
#include "gpsitem.h"
#include "mapgraphicitem.h"
#include "opmapwidget.h"
#include "trailitem.h"
#include "traillineitem.h"
namespace mapcontrol
{
    GPSItem::GPSItem(MapGraphicItem* map,OPMapWidget* parent,QString uavPic) :
        GraphicsItem(map, parent),
        altitude(0),
        trailtype(UAVTrailType::ByDistance),
        trail(nullptr),
        trailLine(nullptr),
        showtrail(false),
        showtrailline(true),
        trailtime(5),
        traildistance(10),
        showUAV(true)
    {
        picture.load(uavPic);
        core::Point localposition = map->FromLatLngToLocal(mapwidget->CurrentPosition());
        this->setPos(localposition.X(), localposition.Y());
        this->setZValue(4);
        trail = new QGraphicsItemGroup(this);
        trail->setParentItem(map);
        trailLine = new QGraphicsItemGroup(this);
        trailLine->setParentItem(map);
        this->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        timer.start();
    }

    GPSItem::~GPSItem()
    {
    }

    void GPSItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        if(showUAV)
        {
            painter->drawPixmap(-picture.width()/2,-picture.height()/2,picture);
        }
    }

    QRectF GPSItem::boundingRect()const
    {
        return QRectF(-picture.width()/2,-picture.height()/2,picture.width(),picture.height());
    }

    void GPSItem::SetUAVPos(const internals::PointLatLng &position, const int &altitude)
    {
        if(coord.IsEmpty())
            lastcoord = coord;
        if(coord != position)
        {

            if(trailtype == UAVTrailType::ByTimeElapsed)
            {
                if(timer.elapsed()>trailtime*1000)
                {
                    trail->addToGroup(new TrailItem(position, altitude, Qt::green, map));
                    if(!lasttrailline.IsEmpty())
                        trailLine->addToGroup((new TrailLineItem(lasttrailline, position, Qt::green, map)));
                    lasttrailline = position;
                    timer.restart();
                }

            }
            else if(trailtype == UAVTrailType::ByDistance)
            {
                if((traildistance == 0) || (qAbs(internals::PureProjection::DistanceBetweenLatLng(lastcoord, position) * 1000) > traildistance))
                {
                    trail->addToGroup(new TrailItem(position, altitude, Qt::green, map));
                    if(!lasttrailline.IsEmpty())
                        trailLine->addToGroup((new TrailLineItem(lasttrailline, position, Qt::green, map)));
                    lasttrailline = position;
                    lastcoord = position;
                }
            }
            coord = position;
            this->altitude = altitude;

            this->update();
        }
    }

    /**
      * Rotate the UAV Icon on the map
      */
    void GPSItem::SetUAVHeading(const qreal &value)
    {
        if (this->rotation() != value)
        {
            this->setRotation(value);
        }
    }

    int GPSItem::type()const
    {
        return Type;
    }

    void GPSItem::RefreshPos()
    {
        core::Point localposition = map->FromLatLngToLocal(coord);
        this->setPos(localposition.X(),localposition.Y());
        foreach(QGraphicsItem* i,trail->childItems())
        {
            TrailItem* w=qgraphicsitem_cast<TrailItem*>(i);
            if(w)
                w->setPos(map->FromLatLngToLocal(w->coord).X(),map->FromLatLngToLocal(w->coord).Y());
        }
        foreach(QGraphicsItem* i,trailLine->childItems())
        {
            TrailLineItem* ww=qgraphicsitem_cast<TrailLineItem*>(i);
            if(ww)
                ww->setLine(map->FromLatLngToLocal(ww->coord1).X(),map->FromLatLngToLocal(ww->coord1).Y(),map->FromLatLngToLocal(ww->coord2).X(),map->FromLatLngToLocal(ww->coord2).Y());
        }

    }

    void GPSItem::SetTrailType(const UAVTrailType::Types &value)
    {
        trailtype=value;
        if(trailtype==UAVTrailType::ByTimeElapsed)
            timer.restart();
    }

    void GPSItem::SetShowTrail(const bool &value)
    {
        showtrail=value;
        trail->setVisible(value);

    }

    void GPSItem::SetShowTrailLine(const bool &value)
    {
        showtrailline=value;
        trailLine->setVisible(value);
    }

    void GPSItem::DeleteTrail()const
    {
        foreach(QGraphicsItem* i,trail->childItems())
            delete i;
        foreach(QGraphicsItem* i,trailLine->childItems())
            delete i;
    }

    double GPSItem::Distance3D(const internals::PointLatLng &coord, const int &altitude)
    {
       return sqrt(pow(internals::PureProjection::DistanceBetweenLatLng(this->coord,coord)*1000,2)+
       pow(static_cast<float>(this->altitude-altitude),2));

    }

    void GPSItem::SetUavPic(QString UAVPic)
    {
        picture.load(":/uavs/images/"+UAVPic);
    }
}
