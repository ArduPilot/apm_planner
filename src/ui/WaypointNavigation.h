#ifndef WAYPOINT_NAVIGATION_H
#define WAYPOINT_NAVIGATION_H

#include <math.h>

#include <QPointF>
#include <QPainterPath>

#include "src/mapwidget/mapgraphicitem.h"
#include "Waypoint.h"

/**
 * @brief Class representing (hermite cube) spline interpolation between waypoints
 *
 * @see https://github.com/diydrones/MissionPlanner/blob/e2823a5d29205064286e38f12a518b8534c6aea3/Controls/Waypoints/Spline2.cs
 * @see https://github.com/diydrones/MissionPlanner/blob/e2823a5d29205064286e38f12a518b8534c6aea3/Controls/Waypoints/Spline.cs
 * @see https://github.com/diydrones/ardupilot/blob/master/libraries/AC_WPNav/AC_WPNav.h
 * @see https://github.com/diydrones/ardupilot/blob/master/libraries/AC_WPNav/AC_WPNav.cpp
 */
class WaypointNavigation
{
private:

    /**
     * @brief Calculate the (hermite cubic) spline interpreation
     *        at time t ([0, 1]) between points p0 with velocity m0,
     *        and p1 with velocity m1.
     */
    static QPointF p(float t,
                     const QPointF& p0,
                     const QPointF& m0,
                     const QPointF& p1,
                     const QPointF& m1);

    /**
     * @brief Converts a waypoint (lat/lon) to the map (local) coordinates.
     */
    static QPointF toQPointF(const Waypoint& wp,
                             mapcontrol::MapGraphicItem& map);

    /**
     * @brief length of the vector represented by v.
     */
    static float length(const QPointF& v)
    {
        return sqrtf(v.x()*v.x() + v.y()*v.y());
    }

public:

    /**
     * @brief Returns a QPainterPath (in the map coordinates)
     *        the UAS would take between the waypoints.
     */
    static QPainterPath path(QList<Waypoint*>& waypoints,
                             mapcontrol::MapGraphicItem& map);
};

#endif
