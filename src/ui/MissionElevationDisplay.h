/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>
(c) author: Bill Bonney <billbonney@communistech.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
#ifndef MISSONELEVATIONDISPLAY_H
#define MISSONELEVATIONDISPLAY_H

#include <QWidget>
#include <QMap>

class QCustomPlot;
class UASInterface;
class UASWaypointManager;
class Waypoint;
class GoogleElevationData;

namespace Ui {
class MissionElevationDisplay;
}

class MissionElevationDisplay : public QWidget
{
    Q_OBJECT

public:
    explicit MissionElevationDisplay(QWidget *parent = 0);
    ~MissionElevationDisplay();

public slots:
    void activeUASSet(UASInterface* uas);

private slots:
    void updateWaypoint(int uasId, Waypoint* waypoint);
    void currentWaypointChanged(quint16 waypointId);
    void updateDisplay();
    void updateElevationData();
    void updateElevationGraph(QList<Waypoint*> waypointList, double averageResolution);
    void setHomeAltOffset();
    void useHomeAltOffset(bool state);
    void showInfoBox();
    void sampleValueChanged();

private:
    int plotElevationGraph(QList<Waypoint *> waypointList, int graphId, double homeAltOffset);
    double distanceBetweenLatLng(double lat1, double lon1, double lat2, double lon2);
    double getHomeAlt(Waypoint* wp);
    void addWaypointLabels();

private:
    Ui::MissionElevationDisplay *ui;

    UASInterface* m_uasInterface;
    UASWaypointManager* m_uasWaypointMgr;
    QMap<int, Waypoint*> m_waypointList; // Ordered Map of waypoint IDs to waypoints.
    int m_totalDistance;

    GoogleElevationData* m_elevationData;
    bool m_useHomeAltOffset;
    double m_homeAltOffset;
    bool m_elevationShown;
};

#endif // MISSONELEVATIONDISPLAY_H
