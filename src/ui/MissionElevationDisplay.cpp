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
#include "logging.h"
#include "qcustomplot.h"
#include "UASInterface.h"
#include "UAS.h"
#include "UASManager.h"
#include "GoogleElevationData.h"

#include "MissionElevationDisplay.h"
#include "ui_MissionElevationDisplay.h"

#include <QInputDialog>
#include <QMessageBox>

static const double ElevationDefaultAltMin = 0.0; //m
static const double ElevationDefaultAltMax = 25.0; //m
static const double ElevationDefaultDistanceMax = 50.0; //m

static const int ElevationGraphMissionId = 0; //m
static const int ElevationGraphElevationId = 1; //m

MissionElevationDisplay::MissionElevationDisplay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MissionElevationDisplay),
    m_uasInterface(NULL),
    m_uasWaypointMgr(NULL),
    m_totalDistance(0),
    m_elevationData(NULL),
    m_useHomeAltOffset(false),
    m_homeAltOffset(0.0),
    m_elevationShown(false)
{
    ui->setupUi(this);

    ui->sampleSpinBox->setEnabled(false);

    QCustomPlot* customPlot = ui->customPlot;
    customPlot->addGraph(); // Mission Elevation Graph (ElevationGraphMissionId)
    customPlot->graph(ElevationGraphMissionId)->setPen(QPen(Qt::blue)); // line color blue for mission data
    customPlot->graph(ElevationGraphMissionId)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue

    customPlot->addGraph(); // Google Elevation Graph (ElevationGraphElevationId)
    customPlot->graph(ElevationGraphElevationId)->setPen(QPen(Qt::red)); // line color red for elevation data
    customPlot->graph(ElevationGraphElevationId)->setBrush(QBrush(QColor(255, 0, 0, 20))); // first graph will be filled with translucent blue
    customPlot->graph(ElevationGraphElevationId)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDiamond, 10));
    customPlot->xAxis->setLabel("distance (m)");
    customPlot->yAxis->setLabel("altitude (m)");
    // set default ranges for Alt and distance
    customPlot->xAxis->setRange(0,ElevationDefaultDistanceMax); //m
    customPlot->yAxis->setRange(ElevationDefaultAltMin,ElevationDefaultAltMax); //m

    QFont legendFont = font();
    legendFont.setPointSize(9);
    customPlot->legend->setFont(legendFont);

    // set a more compact font size for bottom and left axis tick labels:
    customPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 9));
    customPlot->xAxis->setLabelFont(QFont(QFont().family(), 9));
    customPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 9));
    customPlot->yAxis->setLabelFont(QFont(QFont().family(), 9));

    customPlot->replot();

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());

    connect(ui->infoButton, SIGNAL(clicked()), this, SLOT(showInfoBox()));
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
                this, SLOT(updateDisplay()));
        disconnect(m_uasWaypointMgr, SIGNAL(waypointEditableListChanged()),
                   this, SLOT(updateDisplay()));
        disconnect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(updateElevationData()));
        disconnect(ui->setHomeAltButton, SIGNAL(clicked()), this, SLOT(setHomeAltOffset()));
        disconnect(ui->homeAltCheckBox, SIGNAL(clicked(bool)), this, SLOT(useHomeAltOffset(bool)));
        disconnect(ui->sampleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sampleValueChanged()));
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
                this, SLOT(updateDisplay()));
        connect(m_uasWaypointMgr, SIGNAL(waypointEditableListChanged()),
                   this, SLOT(updateDisplay()));
        connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(updateElevationData()));
        connect(ui->setHomeAltButton, SIGNAL(clicked()), this, SLOT(setHomeAltOffset()));
        connect(ui->homeAltCheckBox, SIGNAL(clicked(bool)), this, SLOT(useHomeAltOffset(bool)));
        connect(ui->sampleSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sampleValueChanged()));

        updateDisplay();
    }
}

void MissionElevationDisplay::updateWaypoint(int uasId, Waypoint *waypoint)
{
    Q_UNUSED(uasId);
    QLOG_DEBUG() << "Elevation Waypoint update: " << waypoint->getId()
                 << " alt:" << waypoint->getAltitude();

    if(m_waypointList.count() >= 2){
        Waypoint* oldWp = m_waypointList.value(waypoint->getId());
        if (m_elevationShown && ((oldWp->getLatitude() != waypoint->getLatitude())
           || (oldWp->getLongitude() != waypoint->getLongitude()))){
            // Waypoint Moved, so need to refresh elevation.
            ui->refreshButton->setText("Refresh Elevation");
            ui->refreshButton->setEnabled(true);
        }
    }

    updateDisplay();
}

void MissionElevationDisplay::sampleValueChanged()
{
    if (m_elevationShown){
        ui->refreshButton->setText("Refresh Elevation");
        ui->refreshButton->setEnabled(true);
    }
}

void MissionElevationDisplay::currentWaypointChanged(quint16 waypointId)
{
    QLOG_TRACE() << "Elevation current waypount update: " << waypointId;
}

void MissionElevationDisplay::updateDisplay()
{
    QLOG_DEBUG() << "updateElevationDisplay";

    QList<Waypoint*> list = m_uasWaypointMgr->getGlobalFrameAndNavTypeWaypointList(false);
    m_waypointList.clear();
    foreach (Waypoint* wp, list) {
        // Create a copy
        m_waypointList.insert(wp->getId(), new Waypoint(*wp));
    }

    if (m_waypointList.count() == 0)
        return;

    m_totalDistance = plotElevationGraph(m_waypointList.values(), ElevationGraphMissionId, m_homeAltOffset);
    addWaypointLabels();
}

void MissionElevationDisplay::updateElevationGraph(QList<Waypoint *> waypointList, double averageResolution)
{
    if (m_waypointList.count() == 0)
        return;
    int distance = plotElevationGraph(waypointList, ElevationGraphElevationId, 0.0);
    ui->resolutionLabel->setText(QString::number(averageResolution)+"(m)");
    if (distance > m_totalDistance)
        m_totalDistance = distance;
}

int MissionElevationDisplay::plotElevationGraph(QList<Waypoint *> waypointList, int graphId, double homeAltOffset)
{
    Waypoint* previousWp = NULL;
    double totalDistance = 0.0;
    double homeAlt = 0.0;
    QCustomPlot* customplot = ui->customPlot;
    QCPGraph* graph = customplot->graph(graphId);
    graph->data()->clear();

    foreach(Waypoint* wp, waypointList){
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
            graph->addData( totalDistance, homeAlt + homeAltOffset);

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
                adjustedAlt =  wp->getAltitude() + homeAlt + homeAltOffset;
            } else {
                adjustedAlt = wp->getAltitude();
            }

            graph->addData(totalDistance, adjustedAlt);
        }
        previousWp = wp;
    }
    customplot->rescaleAxes();
    customplot->replot();

    return totalDistance;
}

void MissionElevationDisplay::addWaypointLabels()
{
    QCustomPlot* customPlot = ui->customPlot;
    customPlot->clearItems();
    double totalDistance = 0.0;
    double distance = 0.0;
    Waypoint* previousWp = NULL;

    foreach(Waypoint* wp, m_waypointList){
        if(previousWp != NULL) {
            distance = distanceBetweenLatLng(previousWp->getLatitude(), previousWp->getLongitude(),
                                                wp->getLatitude(), wp->getLongitude());
        }

        totalDistance += distance;
        QCPItemTracer *itemTracer = new QCPItemTracer(customPlot);
        itemTracer->setGraph(customPlot->graph(ElevationGraphMissionId));
        itemTracer->setStyle(QCPItemTracer::tsNone);
        itemTracer->setGraphKey(totalDistance);

        QCPItemText *itemText = new QCPItemText(customPlot);
        itemText->setText("WP" + (QString::number(wp->getId())) + " (+" + (QString::number(distance,'f', 1)) +"m)");
        itemText->position->setParentAnchor(itemTracer->position);

        previousWp = wp;
    }
}

void MissionElevationDisplay::updateElevationData()
{
    if(m_elevationData == NULL){
        m_elevationData = new GoogleElevationData();
        connect(m_elevationData, SIGNAL(elevationDataReady(QList<Waypoint*>,double)),
                this, SLOT(updateElevationGraph(QList<Waypoint*>,double)));
        m_elevationShown = true;
    }
    int samples = m_waypointList.count()*ui->sampleSpinBox->value();
    m_elevationData->requestElevationData(m_waypointList.values(), m_totalDistance, samples); // 5 samples between waypoints
    if (m_elevationShown == true) {
        ui->refreshButton->setEnabled(false);
        ui->refreshButton->setText("Updated");
    }
    ui->sampleSpinBox->setEnabled(true);
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

void MissionElevationDisplay::useHomeAltOffset(bool checked)
{
    m_useHomeAltOffset = checked;

    if (checked == false){
        m_homeAltOffset = 0.0;
    }
    updateDisplay();
}

void MissionElevationDisplay::setHomeAltOffset()
{
    bool ok;
    double homeAlt = QInputDialog::getDouble(this, "Home Alt Setting", "Alt (m)", m_homeAltOffset, -999999.0,999999.0,2,&ok);

    if (ok){
        ui->homeAltCheckBox->setChecked(true);
        m_homeAltOffset = homeAlt;
        updateDisplay();
    }
}

void MissionElevationDisplay::showInfoBox()
{
    QMessageBox::information(this, "Elevation Display", "The Elevation Display will show your mission elevation (blue) against Google's elevation data for that area (red)"
                             "\nWARNING: The datas resolution can be reduced in some areas, so please use caution.",QMessageBox::Ok);
}
