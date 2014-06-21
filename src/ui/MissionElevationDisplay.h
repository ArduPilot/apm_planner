#ifndef MISSONELEVATIONDISPLAY_H
#define MISSONELEVATIONDISPLAY_H

#include <QWidget>

class QCustomPlot;
class UASInterface;
class UASWaypointManager;
class Waypoint;

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
    void updateElevationDisplay();

private:
    double distanceBetweenLatLng(double lat1, double lon1, double lat2, double lon2);

private:
    Ui::MissionElevationDisplay *ui;

    UASInterface* m_uasInterface;
    UASWaypointManager* m_uasWaypointMgr;
    QList<Waypoint*> m_waypointList;
};

#endif // MISSONELEVATIONDISPLAY_H
