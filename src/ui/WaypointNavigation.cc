
#include <QtGlobal>

#include "WaypointNavigation.h"

/*static*/ QPointF
WaypointNavigation::p(float t,
                      const QPointF& p0,
                      const QPointF& m0,
                      const QPointF& p1,
                      const QPointF& m1)
{
    Q_ASSERT(0.0f <= t && t <= 1.0f);
    const float t2 = t * t;
    const float t3 = t2 * t;
    return (2*t3 - 3*t2 + 1) * p0
         + (t3 - 2*t2 + t) * m0
         + (-2*t3 + 3*t2) * p1
         + (t3 - t2) * m1;
}

/*static*/ QPointF
WaypointNavigation::toQPointF(const Waypoint& wp,
                              mapcontrol::MapGraphicItem& map)
{
    internals::PointLatLng pt(wp.getLatitude(), wp.getLongitude());
    core::Point local = map.FromLatLngToLocal(pt);
    return QPointF(local.X(), local.Y());
}

/*static*/ QPainterPath
WaypointNavigation::path(QList<Waypoint*>& waypoints,
                         mapcontrol::MapGraphicItem& map)
{
    Q_ASSERT(waypoints.size() > 0);

    Waypoint* home = waypoints[0];
    QPainterPath path(toQPointF(*home, map));

    if (waypoints.size() == 1)
        return path;

    QList<int> wayPointsWithPath; // This QList of waypoints for a path to be drawn.
    wayPointsWithPath << MAV_CMD_NAV_WAYPOINT // Here corresponding Waypoints can be added or removed.
                      << MAV_CMD_NAV_LOITER_UNLIM
                      << MAV_CMD_NAV_LOITER_TURNS
                      << MAV_CMD_NAV_LOITER_TIME
                      << MAV_CMD_NAV_LAND
                      << MAV_CMD_NAV_TAKEOFF;

    QPointF m1; // spline velocity at destination
    for (int i = 1; i < waypoints.size(); ++i)
    {
        const Waypoint& wp1 = *waypoints[i];
        QPointF p1 = toQPointF(wp1, map);

        if (wayPointsWithPath.contains(wp1.getAction()))
        {
            path.lineTo(p1);
            continue;
        }

        if (wp1.getAction() != MAV_CMD_NAV_SPLINE_WAYPOINT)
        {
            continue;
        }

        const Waypoint& wp0 = *waypoints[i-1];
        QPointF p0 = toQPointF(wp0, map);
        const Waypoint& wp2 = i < waypoints.size() - 1 ? *waypoints[i+1] : *waypoints[i];
        QPointF p2 = toQPointF(wp2, map);

        // segment start types
        // stop - vehicle is not moving at origin
        // straight-fast - vehicle is moving, previous segment is straight.  vehicle will fly straight through the waypoint before beginning it's spline path to the next wp
        // spline-fast - vehicle is moving, previous segment is splined, vehicle will fly through waypoint but previous segment should have it flying in the correct direction (i.e. exactly parallel to position difference vector from previous segment's origin to this segment's destination)

        // calculate spline velocity at origin
        QPointF m0;
        if (i == 1 // home
            || ((wp0.getAction() != MAV_CMD_NAV_WAYPOINT && wp0.getAction() != MAV_CMD_NAV_SPLINE_WAYPOINT) || wp0.getParam1() != 0)) // loiter time
        {
            // if vehicle is stopped at the origin, set origin velocity to 0.1 * distance vector from origin to destination
            m0 = (p1 - p0) * 0.1f;
        }
        else
        {
            // look at previous segment to determine velocity at origin
            if (wp0.getAction() == MAV_CMD_NAV_WAYPOINT)
            {
                // previous segment is straight, vehicle is moving so vehicle should fly straight through the origin
                // before beginning it's spline path to the next waypoint.
                // Note: we are using the previous segment's origin and destination

                Q_ASSERT(i > 1);

                const Waypoint& wp_1 = *waypoints[i-2];
                QPointF p_1 = toQPointF(wp_1, map);

                m0 = (p0 - p_1);
            }
            else
            {
                // previous segment is splined, vehicle will fly through origin
                // we can use the previous segment's destination velocity as this segment's origin velocity
                // Note: previous segment will leave destination velocity parallel to position difference vector
                //       from previous segment's origin to this segment's destination)

                Q_ASSERT(wp1.getAction() == MAV_CMD_NAV_SPLINE_WAYPOINT);

                m0 = m1;
            }
        }

        // calculate spline velocity at destination (m1)
        if (i == waypoints.size() - 1
            || wp1.getParam1() != 0)
        {
            // if vehicle stops at the destination set destination velocity to 0.1 * distance vector from origin to destination
            m1 = (p1 - p0) * 0.1f;
        }
        else if (wp2.getAction() == MAV_CMD_NAV_WAYPOINT)
        {
            // if next segment is straight, vehicle's final velocity should face along the next segment's position
            m1 = (p2 - p1);
        }
        else if (wp2.getAction() == MAV_CMD_NAV_SPLINE_WAYPOINT)
        {
            // if next segment is splined, vehicle's final velocity should face parallel to the line from the origin to the next destination
            m1 = (p2 - p0);
        }
        else
        {
            // if vehicle stops at the destination set destination velocity to 0.1 * distance vector from origin to destination
            m1 = (p1 - p0) * 0.1f;
        }

        // code below ensures we don't get too much overshoot when the next segment is short
        float vel_len = length(m0 + m1);
        float pos_len = length(p1 - p0) * 4.0f;
        if (vel_len > pos_len)
        {
            // if total start+stop velocity is more than twice position difference
            // use a scaled down start and stop velocityscale the  start and stop velocities down
            float vel_scaling = pos_len / vel_len;
            m0 *= vel_scaling;
            m1 *= vel_scaling;
        }

        // draw spline
        for (float t = 0.0f; t <= 1.0f; t += 1/100.0f) // update_spline() called at 100Hz
        {
            path.lineTo(p(t, p0, m0, p1, m1));
        }
    }

    return path;
}
