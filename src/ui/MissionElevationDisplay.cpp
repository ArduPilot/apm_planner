#include "QsLog.h"
#include "qcustomplot.h"
#include "UASInterface.h"
#include "UAS.h"
#include "UASManager.h"

#include "MissionElevationDisplay.h"
#include "ui_MissionElevationDisplay.h"

static const double ElevationDefaultAltMin = 0.0; //m
static const double ElevationDefaultAltMax = 25.0; //m
static const double ElevationDefaultDistanceMax = 50.0; //m

MissionElevationDisplay::MissionElevationDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MissionElevationDisplay),
    m_uasInterface(NULL),
    m_uasWaypointMgr(NULL)
{
    ui->setupUi(this);

    QCustomPlot* customPlot = ui->customPlot;
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    customPlot->xAxis->setLabel("distance (m)");
    customPlot->yAxis->setLabel("altitude (m)");
    // set default ranges for Alt and distance
    customPlot->xAxis->setRange(0,ElevationDefaultDistanceMax); //m
    customPlot->yAxis->setRange(ElevationDefaultAltMin,ElevationDefaultAltMax); //m

    customPlot->replot();

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
}

MissionElevationDisplay::~MissionElevationDisplay()
{
    delete ui;
}

void MissionElevationDisplay::activeUASSet(UASInterface *uas)
{
    if (m_uasInterface){
        disconnect(m_uasWaypointMgr, SIGNAL(waypointEditableChanged(int,Waypoint*)),
                   this, SLOT(updateWaypoint(int,Waypoint*)));
        disconnect(m_uasWaypointMgr, SIGNAL(currentWaypointChanged(quint16)),
                this, SLOT(currentWaypointChanged(quint16)));
        disconnect(m_uasWaypointMgr, SIGNAL(readGlobalWPFromUAS(bool)),
                this, SLOT(updateElevationDisplay()));
        disconnect(m_uasWaypointMgr, SIGNAL(waypointEditableListChanged()),
                   this, SLOT(updateElevationDisplay()));
    }

    m_uasWaypointMgr = NULL;
    m_uasInterface = uas;

    if(m_uasInterface){
        m_uasWaypointMgr = uas->getWaypointManager();
        connect(m_uasWaypointMgr, SIGNAL(waypointEditableChanged(int,Waypoint*)),
                this, SLOT(updateWaypoint(int,Waypoint*)));
        connect(m_uasWaypointMgr, SIGNAL(currentWaypointChanged(quint16)),
                this, SLOT(currentWaypointChanged(quint16)));
        connect(m_uasWaypointMgr, SIGNAL(readGlobalWPFromUAS(bool)),
                this, SLOT(updateElevationDisplay()));
        connect(m_uasWaypointMgr, SIGNAL(waypointEditableListChanged()),
                   this, SLOT(updateElevationDisplay()));

        updateElevationDisplay();
    }
}

void MissionElevationDisplay::updateWaypoint(int uasId, Waypoint *waypoint)
{
    Q_UNUSED(uasId);
    QLOG_DEBUG() << "Elevation Waypoint update: " << waypoint->getId()
                 << " alt:" << waypoint->getAltitude();
    updateElevationDisplay();
}

void MissionElevationDisplay::currentWaypointChanged(quint16 waypointId)
{
    QLOG_TRACE() << "Elevation current waypount update: " << waypointId;
}

void MissionElevationDisplay::updateElevationDisplay()
{
    QLOG_DEBUG() << "updateElevationDisplay";
    m_waypointList =  m_uasWaypointMgr->getGlobalFrameAndNavTypeWaypointList();
    if (m_waypointList.count() == 0)
        return;

    Waypoint* previousWp = NULL;
    double totalDistance = 0.0;
    double homeAlt = 0.0;
    QCustomPlot* customplot = ui->customPlot;
    QCPGraph* graph = customplot->graph(0);
    graph->clearData();

    foreach(Waypoint* wp, m_waypointList){
        double lower = 0.0, upper = 0.0;
        QCPRange xRange = customplot->xAxis->range();
        QCPRange yRange = customplot->yAxis->range();

        if (wp->getAltitude() > yRange.upper){
            upper = wp->getAltitude();
        }

        if (wp->getAltitude() < yRange.lower){
            lower = wp->getAltitude();
        }

        customplot->yAxis->setRange(lower, upper);

        if (wp->getId() == 0){
            // Plot Waypoint at 0 (HOME) and store for next calculation
            homeAlt = wp->getAltitude();
            graph->addData( totalDistance , homeAlt);

        } else {
            // calculate the distance and plot against alt
            double distance = distanceBetweenLatLng(previousWp->getLatitude(), previousWp->getLongitude(),
                                                    wp->getLatitude(), wp->getLongitude());
            totalDistance += distance;
            if ( totalDistance > xRange.upper ){
                customplot->xAxis->setRange(0, totalDistance + 10);
            }

            double adjustedAlt = 0.0;
            if (wp->getFrame() == MAV_FRAME_GLOBAL_RELATIVE_ALT){
                adjustedAlt =  wp->getAltitude() + homeAlt;
            } else {
                adjustedAlt = wp->getAltitude();
            }

            graph->addData(totalDistance, wp->getAltitude() + homeAlt);
        }
        previousWp = wp;
    }
    customplot->rescaleAxes();
    customplot->replot();
}

// When we move to QT5 the below should use QGeoLocation.
double MissionElevationDisplay::distanceBetweenLatLng(double lat1, double lon1, double lat2, double lon2)
{
     double R = 6371000; // m
     double dLat = (lat2-lat1)* (M_PI / 180);
     double dLon = (lon2-lon1)* (M_PI / 180);
     double a = sin(dLat/2) * sin(dLat/2) + cos(lat1* (M_PI / 180)) * cos(lat2* (M_PI / 180)) * sin(dLon/2) * sin(dLon/2);
     double c = 2 * atan2(sqrt(a), sqrt(1-a));
     double d = R * c;
     return d;
}
