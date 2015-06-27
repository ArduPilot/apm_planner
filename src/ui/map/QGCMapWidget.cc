#include "QGCMapWidget.h"
#include "QsLog.h"
#include "QGCMapToolBar.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "MAV2DIcon.h"
#include "Waypoint2DIcon.h"
#include "UASWaypointManager.h"
#include "ArduPilotMegaMAV.h"
#include "WaypointNavigation.h"
#include <QInputDialog>

QGCMapWidget::QGCMapWidget(QWidget *parent) :
    mapcontrol::OPMapWidget(parent),
    firingWaypointChange(NULL),
    maxUpdateInterval(2.1f), // 2 seconds
    followUAVEnabled(false),
    trailType(mapcontrol::UAVTrailType::ByTimeElapsed),
    trailInterval(2.0f),
    followUAVID(0),
    mapInitialized(false),
    homeAltitude(0),
    uas(NULL)
{
    // Set the map cache directory
    configuration->SetCacheLocation(QGC::appDataDirectory() + "/mapscache/");

    currWPManager = UASManager::instance()->getActiveUASWaypointManager();
    waypointLines.insert(0, new QGraphicsItemGroup(map));
    connect(currWPManager, SIGNAL(waypointEditableListChanged(int)), this, SLOT(updateWaypointList(int)));
    connect(currWPManager, SIGNAL(waypointEditableChanged(int, Waypoint*)), this, SLOT(updateWaypoint(int,Waypoint*)));
    connect(this, SIGNAL(waypointCreated(Waypoint*)), currWPManager, SLOT(addWaypointEditable(Waypoint*)));
    connect(this, SIGNAL(waypointChanged(Waypoint*)), currWPManager, SLOT(notifyOfChangeEditable(Waypoint*)));
    offlineMode = true;
    // Widget is inactive until shown
    defaultGuidedRelativeAlt = 100.0; // Default set to 100m
    defaultGuidedAltFirstTimeSet = false;
    loadSettings();

    //handy for debugging:
    //this->SetShowTileGridLines(true);

    this->setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *guidedaction = new QAction(this);
    guidedaction->setText("Go To Here (Guided Mode)");
    connect(guidedaction,SIGNAL(triggered()),this,SLOT(guidedActionTriggered()));
    this->addAction(guidedaction);
    guidedaction = new QAction(this);
    guidedaction->setText("Go To Here Alt (Guided Mode)");
    connect(guidedaction,SIGNAL(triggered()),this,SLOT(guidedAltActionTriggered()));
    this->addAction(guidedaction);
    QAction *cameraaction = new QAction(this);
    cameraaction->setText("Point Camera Here");
    connect(cameraaction,SIGNAL(triggered()),this,SLOT(cameraActionTriggered()));
    this->addAction(cameraaction);
}
void QGCMapWidget::guidedActionTriggered()
{
    if (!uas)
    {
        QMessageBox::information(0,"Error","Please connect first");
        return;
    }
    if (!currWPManager)
        return;
    Waypoint wp;
    double tmpAlt;
    // check the frame has not been changed from the last time we executed
    bool aslAglChanged = defaultGuidedFrame != currWPManager->getFrameRecommendation();

    if ( aslAglChanged || !defaultGuidedAltFirstTimeSet)
    {
        defaultGuidedAltFirstTimeSet = true; // so we don't prompt again.
        QString altFrame;
        defaultGuidedFrame = currWPManager->getFrameRecommendation();

        if (defaultGuidedFrame == MAV_FRAME_GLOBAL_RELATIVE_ALT){
            altFrame = "Relative Alt (AGL)";
            tmpAlt = defaultGuidedRelativeAlt;
        } else {
            altFrame = "Abs Alt (ASL)";
            // Waypoint 0 is always home on APM
            tmpAlt = currWPManager->getWaypoint(0)->getAltitude() + defaultGuidedRelativeAlt;
        }

        bool ok = false;
        tmpAlt = QInputDialog::getDouble(this,altFrame,"Enter " + altFrame + " (in meters) of destination point for guided mode",
                                          tmpAlt,0,30000.0,2,&ok);
        if (!ok)
        {
            //Use has chosen cancel. Do not send the waypoint
            return;
        }

        if (defaultGuidedFrame == MAV_FRAME_GLOBAL_RELATIVE_ALT){
            defaultGuidedRelativeAlt = tmpAlt;
        } else {
            defaultGuidedRelativeAlt = tmpAlt - currWPManager->getWaypoint(0)->getAltitude();
        }
    } else if (defaultGuidedFrame == MAV_FRAME_GLOBAL_RELATIVE_ALT){
        tmpAlt = defaultGuidedRelativeAlt;
    } else {
        tmpAlt = currWPManager->getWaypoint(0)->getAltitude() + defaultGuidedRelativeAlt;
    }
    wp.setFrame(static_cast<MAV_FRAME>(defaultGuidedFrame));
    sendGuidedAction(&wp, tmpAlt);
}

void QGCMapWidget::guidedAltActionTriggered()
{
    if (!uas)
    {
        QMessageBox::information(0,"Error","Please connect first");
        return;
    }
    if (!currWPManager)
        return;

    Waypoint wp;
    double tmpAlt;
    if(  defaultGuidedFrame != currWPManager->getFrameRecommendation()){

        defaultGuidedFrame = currWPManager->getFrameRecommendation();
        QLOG_DEBUG() << "Changing from Frame type to:"
                     << (defaultGuidedFrame == MAV_FRAME_GLOBAL_RELATIVE_ALT? "AGL": "ASL");
    }

    wp.setFrame(static_cast<MAV_FRAME>(defaultGuidedFrame));
    QString altFrame;

    if (wp.getFrame() == MAV_FRAME_GLOBAL_RELATIVE_ALT){
        altFrame = "Relative Alt (AGL)";
        tmpAlt = defaultGuidedRelativeAlt;
    } else {
        altFrame = "Abs Alt (ASL)";
        // Waypoint 0 is always home on APM
        tmpAlt = currWPManager->getWaypoint(0)->getAltitude() + defaultGuidedRelativeAlt;
    }

    bool ok = false;
    tmpAlt = QInputDialog::getDouble(this,altFrame,"Enter " + altFrame + " (in meters) of destination point for guided mode",
                                      tmpAlt,0,30000.0,2,&ok);
    if (!ok)
    {
        //Use has chosen cancel. Do not send the waypoint
        return;
    }
    if (defaultGuidedFrame == MAV_FRAME_GLOBAL_RELATIVE_ALT){
        defaultGuidedRelativeAlt = tmpAlt;
    } else {
        defaultGuidedRelativeAlt = tmpAlt - currWPManager->getWaypoint(0)->getAltitude();
    }
    sendGuidedAction(&wp, tmpAlt);
}

void QGCMapWidget::sendGuidedAction(Waypoint* wp, double alt)
{
    // Create new waypoint and send it to the WPManager to send out.
    internals::PointLatLng pos = map->FromLocalToLatLng(mousePressPos.x(), mousePressPos.y());
    QLOG_DEBUG() << "Guided action requested. Lat:" << pos.Lat() << "Lon:" << pos.Lng()
                 << "Alt:" << alt << "MAV_FRAME:"
                 << (defaultGuidedFrame == MAV_FRAME_GLOBAL_RELATIVE_ALT? "AGL": "ASL");
    wp->setLongitude(pos.Lng());
    wp->setLatitude(pos.Lat());
    wp->setAltitude(alt);
    currWPManager->goToWaypoint(wp);
}

void QGCMapWidget::cameraActionTriggered()
{
    if (!uas)
    {
        QMessageBox::information(0,"Error","Please connect first");
        return;
    }
    ArduPilotMegaMAV *newmav = qobject_cast<ArduPilotMegaMAV*>(this->uas);
    if (newmav)
    {
        newmav->setMountConfigure(4,true,true,true);
        internals::PointLatLng pos = map->FromLocalToLatLng(mousePressPos.x(), mousePressPos.y());
        newmav->setMountControl(pos.Lat(),pos.Lng(),100,true);
    }
}

void QGCMapWidget::mousePressEvent(QMouseEvent *event)
{
    QLOG_DEBUG() << "mousePressEvent pos:" << event->pos() << " posF:" << event->pos();
    mousePressPos = event->pos();
    mapcontrol::OPMapWidget::mousePressEvent(event);
}

void QGCMapWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QLOG_DEBUG() << "mouseReleaseEvent pos:" << event->pos() << " posF:" << event->pos();
    mousePressPos = event->pos();
    mapcontrol::OPMapWidget::mouseReleaseEvent(event);
}

QGCMapWidget::~QGCMapWidget()
{
    SetShowHome(false);	// doing this appears to stop the map lib crashing on exit
    SetShowUAV(false);	//   "          "
    storeSettings();
}

void QGCMapWidget::showEvent(QShowEvent* event)
{
    // Disable OP's standard UAV, we have more than one
    SetShowUAV(false);
    loadSettings();

    // Pass on to parent widget
    OPMapWidget::showEvent(event);



    internals::PointLatLng pos_lat_lon = internals::PointLatLng(m_lastLat, m_lastLon);

    if (!mapInitialized)
    {
        connect(UASManager::instance(), SIGNAL(UASCreated(UASInterface*)), this, SLOT(addUAS(UASInterface*)), Qt::UniqueConnection);
        connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this, SLOT(activeUASSet(UASInterface*)), Qt::UniqueConnection);
        connect(UASManager::instance(), SIGNAL(homePositionChanged(double,double,double)), this, SLOT(updateHomePosition(double,double,double)));
        connect(UASManager::instance(),SIGNAL(UASDeleted(UASInterface*)),this,SLOT(deleteUas(UASInterface*)));

        foreach (UASInterface* uas, UASManager::instance()->getUASList())
        {
            addUAS(uas);
        }

        //this->SetUseOpenGL(true);
        SetMouseWheelZoomType(internals::MouseWheelZoomType::MousePositionWithoutCenter);	    // set how the mouse wheel zoom functions
        SetFollowMouse(true);				    // we want a contiuous mouse position reading

        SetShowHome(true);					    // display the HOME position on the map
        Home->SetSafeArea(0);                         // set radius (meters)
        Home->SetShowSafeArea(false);                                         // show the safe area
        Home->SetCoord(pos_lat_lon);             // set the HOME position

        setFrameStyle(QFrame::NoFrame);      // no border frame
        setBackgroundBrush(QBrush(Qt::black)); // tile background

        // Set current home position
        updateHomePosition(UASManager::instance()->getHomeLatitude(), UASManager::instance()->getHomeLongitude(), UASManager::instance()->getHomeAltitude());

        // Set currently selected system
        activeUASSet(UASManager::instance()->getActiveUAS());

        // Connect map updates to the adapter slots
        connect(this, SIGNAL(WPValuesChanged(WayPointItem*)), this, SLOT(handleMapWaypointEdit(WayPointItem*)));
        connect(map, SIGNAL(mapChanged()), this, SLOT(redrawWaypointLines()));

        // Start timer
        connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateGlobalPosition()));
        mapInitialized = true;
        //QTimer::singleShot(800, this, SLOT(loadSettings()));
    }
    SetCurrentPosition(pos_lat_lon);         // set the map position
    setFocus();
    updateTimer.start(maxUpdateInterval*1000);
    // Update all UAV positions
    updateGlobalPosition();
}

void QGCMapWidget::hideEvent(QHideEvent* event)
{
    updateTimer.stop();
    storeSettings();
    OPMapWidget::hideEvent(event);
}

/**
 * @param changePosition Load also the last position from settings and update the map position.
 */
void QGCMapWidget::loadSettings()
{
    QSettings settings;
    settings.beginGroup("QGC_MAPWIDGET");
    m_lastLat = settings.value("LAST_LATITUDE", 0.0f).toDouble();
    m_lastLon = settings.value("LAST_LONGITUDE", 0.0f).toDouble();
    m_lastZoom = settings.value("LAST_ZOOM", 1.0f).toDouble();

    SetMapType(static_cast<MapType::Types>(settings.value("MAP_TYPE", MapType::GoogleHybrid).toInt()));

    trailType = static_cast<mapcontrol::UAVTrailType::Types>(settings.value("TRAIL_TYPE", trailType).toInt());
    trailInterval = settings.value("TRAIL_INTERVAL", trailInterval).toFloat();
    settings.endGroup();

    // SET CORRECT MENU CHECKBOXES
    // Set the correct trail interval
    if (trailType == mapcontrol::UAVTrailType::ByDistance)
    {
        // XXX
#ifdef Q_OS_WIN
#pragma message ("WARNING: Settings loading for trail type not implemented")
#else
#warning Settings loading for trail type not implemented
#endif
    }
    else if (trailType == mapcontrol::UAVTrailType::ByTimeElapsed)
    {
        // XXX
    }

    // SET TRAIL TYPE
    foreach (mapcontrol::UAVItem* uav, GetUAVS())
    {
        // Set the correct trail type
        uav->SetTrailType(trailType);
        // Set the correct trail interval
        if (trailType == mapcontrol::UAVTrailType::ByDistance)
        {
            uav->SetTrailDistance(trailInterval);
        }
        else if (trailType == mapcontrol::UAVTrailType::ByTimeElapsed)
        {
            uav->SetTrailTime(trailInterval);
        }
    }

    // SET INITIAL POSITION AND ZOOM
    internals::PointLatLng pos_lat_lon = internals::PointLatLng(m_lastLat, m_lastLon);
    SetCurrentPosition(pos_lat_lon);        // set the map position
    SetZoom(m_lastZoom); // set map zoom level
}

void QGCMapWidget::storeSettings()
{
    QSettings settings;
    settings.beginGroup("QGC_MAPWIDGET");
    internals::PointLatLng pos = CurrentPosition();
    if ((pos.Lat() != 0.0f)&&(pos.Lng()!=0.0f)){
        settings.setValue("LAST_LATITUDE", pos.Lat());
        settings.setValue("LAST_LONGITUDE", pos.Lng());
    }
    settings.setValue("LAST_ZOOM", ZoomReal());
    settings.setValue("TRAIL_TYPE", static_cast<int>(trailType));
    settings.setValue("TRAIL_INTERVAL", trailInterval);
    settings.setValue("MAP_TYPE", static_cast<int>(GetMapType()));
    settings.endGroup();
    settings.sync();
}

void QGCMapWidget::mouseDoubleClickEvent(QMouseEvent* event)
{
    // If a waypoint manager is available
    if (currWPManager)
    {
        // Create new waypoint
        internals::PointLatLng pos = map->FromLocalToLatLng(event->pos().x(), event->pos().y());
        currWPManager->blockSignals(true);
        Waypoint* wp = currWPManager->createWaypoint();
        //            wp->setFrame(MAV_FRAME_GLOBAL_RELATIVE_ALT);
        wp->setLatitude(pos.Lat());
        wp->setLongitude(pos.Lng());
        currWPManager->blockSignals(false);
        currWPManager->notifyOfChangeEditable(NULL); // yes: NULL to fire waypointEditableListChanged
    }

    OPMapWidget::mouseDoubleClickEvent(event);
}


/**
 *
 * @param uas the UAS/MAV to monitor/display with the map widget
 */
void QGCMapWidget::addUAS(UASInterface* uas)
{
    QLOG_DEBUG() << "addUAS" << uas->getUASName();

    connect(uas, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)), this, SLOT(updateGlobalPosition(UASInterface*,double,double,double,quint64)));
    connect(uas, SIGNAL(systemSpecsChanged(int)), this, SLOT(updateSystemSpecs(int)));
    if (!waypointLines.value(uas->getUASID(), NULL)) {
        waypointLines.insert(uas->getUASID(), new QGraphicsItemGroup(map));
    } else {
        foreach (QGraphicsItem* item, waypointLines.value(uas->getUASID())->childItems()) {
            delete item;
        }
    }
}

/**
 *
 * @param uas the UAS/MAV to remove from the map widget
 */
void QGCMapWidget::deleteUas(UASInterface* uas)
{
}

void QGCMapWidget::activeUASSet(UASInterface* uas)
{
    // Only execute if proper UAS is set
    if (!uas)
    {
        this->uas = 0;
        return;
    }
    if (this->uas == uas) return;

    QLOG_DEBUG() << "activeUASSet" << uas->getUASName();

    // Disconnect old MAV manager
    if (currWPManager)
    {
        // Disconnect the waypoint manager / data storage from the UI
        disconnect(currWPManager, SIGNAL(waypointEditableListChanged(int)), this, SLOT(updateWaypointList(int)));
        disconnect(currWPManager, SIGNAL(waypointEditableChanged(int, Waypoint*)), this, SLOT(updateWaypoint(int,Waypoint*)));
        disconnect(this, SIGNAL(waypointCreated(Waypoint*)), currWPManager, SLOT(addWaypointEditable(Waypoint*)));
        disconnect(this, SIGNAL(waypointChanged(Waypoint*)), currWPManager, SLOT(notifyOfChangeEditable(Waypoint*)));

        QGraphicsItemGroup* group = waypointLine(this->uas ? this->uas->getUASID() : 0);
        if (group)
        {
            // Delete existing waypoint lines
            foreach (QGraphicsItem* item, group->childItems())
            {
                delete item;
            }
        }
    }

    this->uas = uas;
    this->currWPManager = uas->getWaypointManager();

    updateSelectedSystem(uas->getUASID());
    followUAVID = uas->getUASID();
    updateWaypointList(uas->getUASID());

    // Connect the waypoint manager / data storage to the UI
    connect(currWPManager, SIGNAL(waypointEditableListChanged(int)), this, SLOT(updateWaypointList(int)));
    connect(currWPManager, SIGNAL(waypointEditableChanged(int, Waypoint*)), this, SLOT(updateWaypoint(int,Waypoint*)));
    connect(this, SIGNAL(waypointCreated(Waypoint*)), currWPManager, SLOT(addWaypointEditable(Waypoint*)));
    connect(this, SIGNAL(waypointChanged(Waypoint*)), currWPManager, SLOT(notifyOfChangeEditable(Waypoint*)));
}

/**
 * Updates the global position of one MAV and append the last movement to the trail
 *
 * @param uas The unmanned air system
 * @param lat Latitude in WGS84 ellipsoid
 * @param lon Longitutde in WGS84 ellipsoid
 * @param alt Altitude over mean sea level
 * @param usec Timestamp of the position message in milliseconds FIXME will move to microseconds
 */
void QGCMapWidget::updateGlobalPosition(UASInterface* uas, double lat, double lon, double alt, quint64 usec)
{
    Q_UNUSED(usec);

    // Immediate update
    if (maxUpdateInterval == 0)
    {
        // Get reference to graphic UAV item
        mapcontrol::UAVItem* uav = GetUAV(uas->getUASID());
        // Check if reference is valid, else create a new one
        if (uav == NULL)
        {
            MAV2DIcon* newUAV = new MAV2DIcon(map, this, uas);
            newUAV->setParentItem(map);
            UAVS.insert(uas->getUASID(), newUAV);
            uav = GetUAV(uas->getUASID());
            // Set the correct trail type
            uav->SetTrailType(trailType);
            // Set the correct trail interval
            if (trailType == mapcontrol::UAVTrailType::ByDistance)
            {
                uav->SetTrailDistance(trailInterval);
            }
            else if (trailType == mapcontrol::UAVTrailType::ByTimeElapsed)
            {
                uav->SetTrailTime(trailInterval);
            }
        }

        // Set new lat/lon position of UAV icon
        internals::PointLatLng pos_lat_lon = internals::PointLatLng(lat, lon);
        uav->SetUAVPos(pos_lat_lon, alt);

        if(this->uas == uas){
            // save the last know postion
            m_lastLat = uas->getLatitude();
            m_lastLon = uas->getLongitude();
        }

        // Follow status
        if (followUAVEnabled && (uas->getUASID() == followUAVID) && isValidGpsLocation(uas)){
            SetCurrentPosition(pos_lat_lon);
        }
        // Convert from radians to degrees and apply
        uav->SetUAVHeading((uas->getYaw()/M_PI)*180.0f);
    }
}

bool QGCMapWidget::isValidGpsLocation(UASInterface* system) const
{
    if ((system->getLatitude() == 0.0f)
            ||(system->getLongitude() == 0.0f)){
        return false;
    }
    return true;
}

/**
 * Pulls in the positions of all UAVs from the UAS manager
 */
void QGCMapWidget::updateGlobalPosition()
{
    QList<UASInterface*> systems = UASManager::instance()->getUASList();
    foreach (UASInterface* system, systems)
    {
        // Get reference to graphic UAV item
        mapcontrol::UAVItem* uav = GetUAV(system->getUASID());
        // Check if reference is valid, else create a new one
        if (uav == NULL)
        {
            MAV2DIcon* newUAV = new MAV2DIcon(map, this, system);
            AddUAV(system->getUASID(), newUAV);
            uav = newUAV;
            uav->SetTrailTime(1);       // [TODO] This should be based on a user setting
            uav->SetTrailDistance(5);    // [TODO] This should be based on a user setting
            uav->SetTrailType(mapcontrol::UAVTrailType::ByTimeElapsed);
        }

        // Set new lat/lon position of UAV icon
        internals::PointLatLng pos_lat_lon = internals::PointLatLng(system->getLatitude(), system->getLongitude());
        uav->SetUAVPos(pos_lat_lon, system->getAltitudeAMSL());
        // Follow status
        if (followUAVEnabled && (system->getUASID() == followUAVID) && isValidGpsLocation(system)) {
            SetCurrentPosition(pos_lat_lon);
        }
        // Convert from radians to degrees and apply
        uav->SetUAVHeading((system->getYaw()/M_PI)*180.0f);
    }
}

void QGCMapWidget::updateLocalPosition()
{
    QList<UASInterface*> systems = UASManager::instance()->getUASList();
    foreach (UASInterface* system, systems)
    {
        // Get reference to graphic UAV item
        mapcontrol::UAVItem* uav = GetUAV(system->getUASID());
        // Check if reference is valid, else create a new one
        if (uav == NULL)
        {
            MAV2DIcon* newUAV = new MAV2DIcon(map, this, system);
            AddUAV(system->getUASID(), newUAV);
            uav = newUAV;
            uav->SetTrailTime(1);
            uav->SetTrailDistance(5);
            uav->SetTrailType(mapcontrol::UAVTrailType::ByTimeElapsed);
        }

        // Set new lat/lon position of UAV icon
        internals::PointLatLng pos_lat_lon = internals::PointLatLng(system->getLatitude(), system->getLongitude());
        uav->SetUAVPos(pos_lat_lon, system->getAltitudeAMSL());
        // Follow status
        if (followUAVEnabled && (system->getUASID() == followUAVID) && isValidGpsLocation(system)) {
            SetCurrentPosition(pos_lat_lon);
        }
        // Convert from radians to degrees and apply
        uav->SetUAVHeading((system->getYaw()/M_PI)*180.0f);
    }
}

void QGCMapWidget::updateLocalPositionEstimates()
{
    updateLocalPosition();
}


void QGCMapWidget::updateSystemSpecs(int uas)
{
    foreach (mapcontrol::UAVItem* p, UAVS.values())
    {
        MAV2DIcon* icon = dynamic_cast<MAV2DIcon*>(p);
        if (icon && icon->getUASId() == uas)
        {
            // Set new airframe
            icon->setAirframe(UASManager::instance()->getUASForId(uas)->getAirframe());
            icon->drawIcon();
        }
    }
}

/**
 * Does not update the system type or configuration, only the selected status
 */
void QGCMapWidget::updateSelectedSystem(int uas)
{
    foreach (mapcontrol::UAVItem* p, UAVS.values())
    {
        MAV2DIcon* icon = dynamic_cast<MAV2DIcon*>(p);
        if (icon)
        {
            // Set as selected if ids match
            icon->setSelectedUAS((icon->getUASId() == uas));
        }
    }
}


// MAP NAVIGATION
void QGCMapWidget::showGoToDialog()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Please enter coordinates"),
                                         tr("Coordinates (Lat,Lon):"), QLineEdit::Normal,
                                         QString("%1,%2").arg(CurrentPosition().Lat(), 0, 'g', 6).arg(CurrentPosition().Lng(), 0, 'g', 6), &ok);
    if (ok && !text.isEmpty())
    {
        QStringList split = text.split(",");
        if (split.length() == 2)
        {
            bool convert;
            double latitude = split.first().toDouble(&convert);
            ok &= convert;
            double longitude = split.last().toDouble(&convert);
            ok &= convert;

            if (ok)
            {
                internals::PointLatLng pos_lat_lon = internals::PointLatLng(latitude, longitude);
                SetCurrentPosition(pos_lat_lon);        // set the map position
            }
        }
    }
}


void QGCMapWidget::updateHomePosition(double latitude, double longitude, double altitude)
{
    Home->SetCoord(internals::PointLatLng(latitude, longitude));
    Home->SetAltitude(altitude);
    homeAltitude = altitude;
    SetShowHome(true);                      // display the HOME position on the map
}

void QGCMapWidget::goHome()
{
    SetCurrentPosition(Home->Coord());
    SetZoom(18); //zoom to "large RC park" size
}

void QGCMapWidget::lastPosition()
{
    internals::PointLatLng pos_lat_lon = internals::PointLatLng(m_lastLat, m_lastLon);
    SetCurrentPosition(pos_lat_lon);
    SetZoom(m_lastZoom); //zoom to "large RC park" size
}

/**
 * Limits the update rate on the specified interval. Set to zero (0) to run at maximum
 * telemetry speed. Recommended rate is 2 s.
 */
void QGCMapWidget::setUpdateRateLimit(float seconds)
{
    maxUpdateInterval = seconds;
    updateTimer.start(maxUpdateInterval*1000);
}

void QGCMapWidget::cacheVisibleRegion()
{
    internals::RectLatLng rect = map->SelectedArea();

    if (rect.IsEmpty())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Cannot cache tiles for offline use");
        msgBox.setInformativeText("Please select an area first by holding down SHIFT or ALT and selecting the area with the left mouse button.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
    else
    {
        RipMap();
        // Set empty area = unselect area
        map->SetSelectedArea(internals::RectLatLng());
    }
}


// WAYPOINT MAP INTERACTION FUNCTIONS

void QGCMapWidget::handleMapWaypointEdit(mapcontrol::WayPointItem* waypoint)
{
    // Block circle updates
    Waypoint* wp = iconsToWaypoints.value(waypoint, NULL);

    // Delete UI element if wp doesn't exist
    if (!wp)
        WPDelete(waypoint);

    // Protect from vicious double update cycle
    if (firingWaypointChange == wp) return;
    // Not in cycle, block now from entering it
    firingWaypointChange = wp;

    QLOG_DEBUG() << "UPDATING WP FROM MAP" << wp->getId();

    // Update WP values
    internals::PointLatLng pos = waypoint->Coord();

    // Block waypoint signals
    wp->blockSignals(true);
    wp->setLatitude(pos.Lat());
    wp->setLongitude(pos.Lng());
    // XXX Magic values
//    wp->setAltitude(homeAltitude + 50.0f);
//    wp->setAcceptanceRadius(10.0f);
    wp->blockSignals(false);


    internals::PointLatLng coord = waypoint->Coord();
    QString coord_str = " " + QString::number(coord.Lat(), 'f', 6) + "   " + QString::number(coord.Lng(), 'f', 6);
    // // QLOG_DEBUG() << "MAP WP COORD (MAP):" << coord_str << __FILE__ << __LINE__;
    QString wp_str = QString::number(wp->getLatitude(), 'f', 6) + "   " + QString::number(wp->getLongitude(), 'f', 6);
    // // QLOG_DEBUG() << "MAP WP COORD (WP):" << wp_str << __FILE__ << __LINE__;

    firingWaypointChange = NULL;

    emit waypointChanged(wp);
}

// WAYPOINT UPDATE FUNCTIONS

/**
 * This function is called if a a single waypoint is updated and
 * also if the whole list changes.
 */
void QGCMapWidget::updateWaypoint(int uas, Waypoint* wp)
{
    QLOG_DEBUG() << __FILE__ << __LINE__ << "UPDATING WP FUNCTION CALLED";
    // Source of the event was in this widget, do nothing
    if (firingWaypointChange == wp) {
        return;
    }
    // Currently only accept waypoint updates from the UAS in focus
    // this has to be changed to accept read-only updates from other systems as well.
    UASInterface* uasInstance = UASManager::instance()->getUASForId(uas);
    if (currWPManager)
    {
        // Only accept waypoints in global coordinate frame
        if (((wp->getFrame() == MAV_FRAME_GLOBAL) || (wp->getFrame() == MAV_FRAME_GLOBAL_RELATIVE_ALT)) && (wp->isNavigationType() || wp->visibleOnMapWidget()))
        {
            // We're good, this is a global waypoint

            // Get the index of this waypoint
            int wpindex = currWPManager->getIndexOf(wp);
            // If not found, return (this should never happen, but helps safety)
            if (wpindex < 0) return;
            // Mark this wp as currently edited
            firingWaypointChange = wp;

            QLOG_DEBUG() << "UPDATING WAYPOINT" << wpindex << "IN 2D MAP";

            // Check if wp exists yet in map
            if (!waypointsToIcons.contains(wp))
            {
                QLOG_TRACE() << "UPDATING NEW WAYPOINT" << wpindex << "IN 2D MAP";
                // Create icon for new WP
                QColor wpColor(Qt::red);
                if (uasInstance) wpColor = uasInstance->getColor();
                Waypoint2DIcon* icon = new Waypoint2DIcon(map, this, wp, wpColor, wpindex);
                ConnectWP(icon);
                icon->setParentItem(map);
                // Update maps to allow inverse data association
                waypointsToIcons.insert(wp, icon);
                iconsToWaypoints.insert(icon, wp);
            }
            else
            {
                QLOG_TRACE() << "UPDATING EXISTING WAYPOINT" << wpindex << "IN 2D MAP";
                // Waypoint exists, block it's signals and update it
                mapcontrol::WayPointItem* icon = waypointsToIcons.value(wp);
                // Make sure we don't die on a null pointer
                // this should never happen, just a precaution
                if (!icon) return;
                // Block outgoing signals to prevent an infinite signal loop
                // should not happen, just a precaution
                this->blockSignals(true);
                // Update the WP
                Waypoint2DIcon* wpicon = dynamic_cast<Waypoint2DIcon*>(icon);
                if (wpicon)
                {
                    // Let icon read out values directly from waypoint
                    icon->SetNumber(wpindex);
                    wpicon->updateWaypoint();
                }
                else
                {
                    // Use safe standard interfaces for non Waypoint-class based wps
                    icon->SetCoord(internals::PointLatLng(wp->getLatitude(), wp->getLongitude()));
                    icon->SetAltitude(wp->getAltitude());
                    icon->SetHeading(wp->getYaw());
                    icon->SetNumber(wpindex);
                }
                // Re-enable signals again
                this->blockSignals(false);
            }

            redrawWaypointLines(uas);

            firingWaypointChange = NULL;
        }
        else
        {
            // Check if the index of this waypoint is larger than the global
            // waypoint list. This implies that the coordinate frame of this
            // waypoint was changed and the list containing only global
            // waypoints was shortened. Thus update the whole list
            if (waypointsToIcons.count() > currWPManager->getGlobalFrameAndNavTypeCount())
            {
                updateWaypointList(uas);
            }
        }
    }
}

void QGCMapWidget::redrawWaypointLines()
{
    redrawWaypointLines(uas ? uas->getUASID() : 0);
}

void QGCMapWidget::redrawWaypointLines(int uas)
{
//    QLOG_DEBUG() << "REDRAW WAYPOINT LINES FOR UAS" << uas;

    if (!currWPManager)
        return;

    QGraphicsItemGroup* group = waypointLine(uas);
    if (!group)
        return;
    Q_ASSERT(group->parentItem() == map);

    // Delete existing waypoint lines
    foreach (QGraphicsItem* item, group->childItems())
    {
        QLOG_TRACE() << "DELETE EXISTING WAYPOINT LINES" << item;
        delete item;
    }

    QList<Waypoint*> wps = currWPManager->getGlobalFrameAndNavTypeWaypointList(true);
    if (wps.size() > 1)
    {
        QPainterPath path = WaypointNavigation::path(wps, *map);
        if (path.elementCount() > 1)
        {
            QGraphicsPathItem* gpi = new QGraphicsPathItem(map);
            gpi->setPath(path);

            QColor color(Qt::red);
            UASInterface* uasInstance = UASManager::instance()->getUASForId(uas);
            if (uasInstance) color = uasInstance->getColor();
            QPen pen(color);
            pen.setWidth(2);
            gpi->setPen(pen);

            QLOG_TRACE() << "ADDING WAYPOINT LINES" << gpi;
            group->addToGroup(gpi);
        }
    }
}

/**
 * Update the whole list of waypoints. This is e.g. necessary if the list order changed.
 * The UAS manager will emit the appropriate signal whenever updating the list
 * is necessary.
 */
void QGCMapWidget::updateWaypointList(int uas)
{
    QLOG_DEBUG() << "UPDATE WP LIST IN 2D MAP CALLED FOR UAS" << uas;
    // Currently only accept waypoint updates from the UAS in focus
    // this has to be changed to accept read-only updates from other systems as well.
    if (currWPManager)
    {
        // Delete first all old waypoints
        // this is suboptimal (quadratic, but wps should stay in the sub-100 range anyway)
        QList<Waypoint* > wps = currWPManager->getGlobalFrameAndNavTypeWaypointList(false);
        foreach (Waypoint* wp, waypointsToIcons.keys())
        {
            if (!wps.contains(wp))
            {
                QLOG_TRACE() << "DELETE EXISTING WP" << wp->getId();
                // Get icon to work on
                mapcontrol::WayPointItem* icon = waypointsToIcons.value(wp);
                waypointsToIcons.remove(wp);
                iconsToWaypoints.remove(icon);
                WPDelete(icon);
            }
        }

        // Update all existing waypoints
        foreach (Waypoint* wp, waypointsToIcons.keys())
        {
            QLOG_TRACE() << "UPDATING EXISTING WP" << wp->getId();
            updateWaypoint(uas, wp);
        }

        // Update all potentially new waypoints
        foreach (Waypoint* wp, wps)
        {
            // Update / add only if new
            if (!waypointsToIcons.contains(wp))
            {
                QLOG_TRACE() << "UPDATING NEW WP" << wp->getId();
                updateWaypoint(uas, wp);
            }
        }

        redrawWaypointLines(uas);
    }
}


//// ADAPTER / HELPER FUNCTIONS
//float QGCMapWidget::metersToPixels(double meters)
//{
//    return meters/map->Projection()->GetGroundResolution(map->ZoomTotal(),coord.Lat());
//}

//double QGCMapWidget::headingP1P2(internals::PointLatLng p1, internals::PointLatLng p2)
//{
//    double lat1 = p1.Lat() * deg_to_rad;
//    double lon1 = p2.Lng() * deg_to_rad;

//    double lat2 = p2.Lat() * deg_to_rad;
//    double lon2 = p2.Lng() * deg_to_rad;

//    double delta_lon = lon2 - lon1;

//    double y = sin(delta_lon) * cos(lat2);
//    double x = cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(delta_lon);
//    double heading = atan2(y, x) * rad_to_deg;

//    heading += 360;
//    while (heading < 0) bear += 360;
//    while (heading >= 360) bear -= 360;

//    return heading;
//}

//internals::PointLatLng QGCMapWidget::targetLatLon(internals::PointLatLng source, double heading, double dist)
//{
//    double lat1 = source.Lat() * deg_to_rad;
//    double lon1 = source.Lng() * deg_to_rad;

//    heading *= deg_to_rad;

//    double ad = dist / earth_mean_radius;

//    double lat2 = asin(sin(lat1) * cos(ad) + cos(lat1) * sin(ad) * cos(heading));
//    double lon2 = lon1 + atan2(sin(bear) * sin(ad) * cos(lat1), cos(ad) - sin(lat1) * sin(lat2));

//    return internals::PointLatLng(lat2 * rad_to_deg, lon2 * rad_to_deg);
//}
