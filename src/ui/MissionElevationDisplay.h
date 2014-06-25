#ifndef MISSONELEVATIONDISPLAY_H
#define MISSONELEVATIONDISPLAY_H

#include <QWidget>

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
    void updateElevationGraph(QList<Waypoint*> waypointList,double averageResolution);
    void setHomeAltOffset();
    void useHomeAltOffset(bool state);
    void showInfoBox();
    void sampleValueChanged();

private:
    int plotElevationGraph(QList<Waypoint*> wpList, int graphId, double homeAltOffset);
    double distanceBetweenLatLng(double lat1, double lon1, double lat2, double lon2);
    double getHomeAlt(Waypoint* wp);
    void addWaypointLabels();

private:
    Ui::MissionElevationDisplay *ui;

    UASInterface* m_uasInterface;
    UASWaypointManager* m_uasWaypointMgr;
    QList<Waypoint*> m_waypointList;
    int m_totalDistance;

    GoogleElevationData* m_elevationData;
    bool m_useHomeAltOffset;
    double m_homeAltOffset;
    bool m_elevationShown;
};

#endif // MISSONELEVATIONDISPLAY_H
