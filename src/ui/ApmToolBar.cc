/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

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

/**
 * @file
 *   @brief APM Tool Bar
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 */

#include "ApmToolBar.h"
#include "QsLog.h"
#include "SerialLink.h"
#include "LinkManager.h"
#include "MainWindow.h"
#include "ArduPilotMegaMAV.h"
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include <QTimer>

APMToolBar::APMToolBar(QWidget *parent):
    QDeclarativeView(parent), m_uas(NULL), m_currentLink(NULL)
{
    // Configure our QML object
    QLOG_DEBUG() << "qmlBaseDir" << QGC::shareDirectory();
    QUrl url = QUrl::fromLocalFile(QGC::shareDirectory() + "/qml/ApmToolBar.qml");
    QLOG_DEBUG() << url;
    if (!QFile::exists(QGC::shareDirectory() + "/qml/ApmToolBar.qml"))
    {
         QMessageBox::information(0,"Error","ApmToolBar.qml not found. Please reinstall the application and try again");
        exit(-1);
    }
    setSource(url);
    QLOG_DEBUG() << "QML Status:" << status();
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    this->rootContext()->setContextProperty("globalObj", this);

     // set the size of the device box and row spacing for icons
#ifdef Q_OS_MAC
    rootObject()->setProperty("rowSpacerSize", QVariant(3));
    rootObject()->setProperty("linkDeviceSize", QVariant(105));
#elif defined(Q_OS_UNIX)
    rootObject()->setProperty("rowSpacerSize", QVariant(3));
    rootObject()->setProperty("linkDeviceSize", QVariant(100));
#else
    rootObject()->setProperty("rowSpacerSize", QVariant(3));
    rootObject()->setProperty("linkDeviceSize", QVariant(80));
#endif

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUasSet(UASInterface*)));
    connect(LinkManager::instance(),SIGNAL(newLink(LinkInterface*)),
            this,SLOT(newLinkCreated(LinkInterface*)));

    connect(&m_heartbeatTimer, SIGNAL(timeout()), this, SLOT(stopHeartbeat()));
}

void APMToolBar::activeUasSet(UASInterface *uas)
{
    QLOG_DEBUG() << "APMToolBar::ActiveUASSet " << uas;
    if (!uas)
    {
        return;
    }
    if (m_uas)
    {

        disconnect(m_uas,SIGNAL(armingChanged(bool)),
                   this,SLOT(armingChanged(bool)));
        disconnect(m_uas,SIGNAL(armingChanged(int, QString)),
                this,SLOT(armingChanged(int, QString)));
        disconnect(m_uas, SIGNAL(navModeChanged(int,int,QString)),
                this, SLOT(navModeChanged(int,int,QString)));
        disconnect(m_uas, SIGNAL(heartbeat(UASInterface*)),
                   this, SLOT(heartbeat(UASInterface*)));
        disconnect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
                this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));

        // disconnect signals from the active serial links
        QList<SerialLink*> sList = SerialLink::getSerialLinks(uas);

        foreach(SerialLink* slink, sList)  {
            // for all connected serial ports for a UAS disconnect
            disconnect(slink, SIGNAL(connected(LinkInterface*)),
                            this, SLOT(connected(LinkInterface*)));
            disconnect(slink, SIGNAL(disconnected(LinkInterface*)),
                            this, SLOT(disconnected(LinkInterface*)));
            m_currentLink = NULL;
        }
    }

    QLOG_DEBUG() << "APMToolBar::ActiveUASSet " << uas;

    // [TODO} Add active MAV to diplay here
    m_uas = uas;

    connect(m_uas,SIGNAL(armingChanged(bool)),
            this,SLOT(armingChanged(bool)));
    connect(m_uas,SIGNAL(armingChanged(int, QString)),
            this,SLOT(armingChanged(int, QString)));

    connect(m_uas, SIGNAL(navModeChanged(int,int,QString)),
            this, SLOT(navModeChanged(int,int,QString)));
    connect(m_uas, SIGNAL(heartbeat(UASInterface*)),
               this, SLOT(heartbeat(UASInterface*)));
    connect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
            this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));

    if (m_uas->isFixedWing()||m_uas->isGroundRover()) {
        rootObject()->setProperty("enableStatusDisplay", QVariant(false));
    } else {
        rootObject()->setProperty("enableStatusDisplay", QVariant(true));
        rootObject()->setProperty("armed", QVariant(m_uas->isArmed()));
    }

    // Connect the signals from active links
    // disconnect signals from the active serial links
    QList<SerialLink*> sList = SerialLink::getSerialLinks(uas);

    foreach(SerialLink* slink, sList)  {
        // for all connected serial ports for a UAS disconnect
        connect(slink, SIGNAL(connected(LinkInterface*)),
                        this, SLOT(connected(LinkInterface*)));
        connect(slink, SIGNAL(disconnected(LinkInterface*)),
                        this, SLOT(disconnected(LinkInterface*)));
        if(slink->isConnected())
        {
            m_currentLink = slink;
            updateLinkDisplay(slink);
        }
    }
}

void APMToolBar::armingChanged(bool armed)
{
    this->rootObject()->setProperty("armed",armed);
}

void APMToolBar::armingChanged(int sysId, QString armingState)
{
    QLOG_DEBUG() << "APMToolBar: sysid " << sysId << " armState" << armingState;
}

void APMToolBar::setFlightViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerFlightView()), action, SIGNAL(triggered()));
}

void APMToolBar::setFlightPlanViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerFlightPlanView()), action, SIGNAL(triggered()));
}

void APMToolBar::setInitialSetupViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerInitialSetupView()), action, SIGNAL(triggered()));
}

void APMToolBar::setConfigTuningViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerConfigTuningView()), action, SIGNAL(triggered()));
}

void APMToolBar::setPlotViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerPlotView()), action, SIGNAL(triggered()));
}

void APMToolBar::setSimulationViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerSimulationView()), action, SIGNAL(triggered()));
}

void APMToolBar::setTerminalViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerTerminalView()), action, SIGNAL(triggered()));
}

void APMToolBar::setConnectMAVAction(QAction *action)
{
    connect(this, SIGNAL(connectMAV()), action, SIGNAL(triggered()));
}

void APMToolBar::selectFlightView()
{
    QLOG_DEBUG() << "APMToolBar: SelectFlightView";
    emit triggerFlightView();
}

void APMToolBar::selectFlightPlanView()
{
    QLOG_DEBUG() << "APMToolBar: SelectFlightPlanView";
    emit triggerFlightPlanView();
}

void APMToolBar::selectInitialSetupView()
{
    QLOG_DEBUG() << "APMToolBar: selectInitialSetupView";
    emit triggerInitialSetupView();
}

void APMToolBar::selectConfigTuningView()
{
    QLOG_DEBUG() << "APMToolBar: selectConfigTuningView";
    emit triggerConfigTuningView();
}

void APMToolBar::selectSimulationView()
{
    QLOG_DEBUG() << "APMToolBar: selectSimulationView";
}

void APMToolBar::selectPlotView()
{
    QLOG_DEBUG() << "APMToolBar: selectPlotView";
}

void APMToolBar::selectTerminalView()
{
    QLOG_DEBUG() << "APMToolBar: selectTerminalView";
}

void APMToolBar::connected(LinkInterface *linkInterface)
{
    QLOG_DEBUG() << "APMToolBar: connecting to link" << linkInterface;

    if (m_uas) {
        // With an active UAS use the list of serial ports from that UAS
        QList<SerialLink*> sList = SerialLink::getSerialLinks(m_uas);
        foreach (SerialLink* sLink, sList) {
            if (sLink == linkInterface){
                m_currentLink = sLink;
                updateLinkDisplay(m_currentLink);
            }
        }

    } else {
        // With no active UAS we make the connected port
        // the current port. (Hopefully this results in an activeUAS
        m_currentLink = linkInterface;
        updateLinkDisplay(m_currentLink);
    }
}

void APMToolBar::disconnected(LinkInterface *linkInterface)
{
    QLOG_DEBUG() << "APMToolBar: disconnected from link" << linkInterface;

    if (m_uas) {
        // With an active UAS use the list of serial ports from that UAS
        QList<SerialLink*> sList = SerialLink::getSerialLinks(m_uas);
        foreach (SerialLink* sLink, sList) {
            if(m_currentLink == sLink) {
                updateLinkDisplay(sLink);
            }
        }

    } else {
        // Else get the global serial list and update the display
        // The global list will really only be one link, but if
        // more we cannot determine which one to display, so it will be
        // the last one disconnected properties.
        QList<SerialLink*> sList = SerialLink::getSerialLinks(LinkManager::instance());
        foreach (SerialLink* sLink, sList) {
            updateLinkDisplay(sLink);
        }
    }
}

void APMToolBar::connectMAV()
{
    QLOG_DEBUG() << "APMToolBar: connectMAV " << m_uas << "with sLink" << m_currentLink;

    if ((m_uas == NULL)&&(m_currentLink == NULL)) {
        // We don't have a link to connect on, create one
        QLOG_DEBUG() << "APMToolBar: Creating a Serial Link";
        MainWindow::instance()->addLink();
        return;
    }

    if (m_currentLink) {
        // Connect/Disconnect the current link
        if(m_currentLink->isConnected()){
            QLOG_DEBUG() << "APMToolBar: Disconnecting m_currentLink " << m_currentLink;
            m_currentLink->disconnect();
        } else {
            QLOG_DEBUG() << "APMToolBar: Connecting m_currentLink " << m_currentLink;
            m_currentLink->connect();
        }

    }
}

void APMToolBar::setConnection(bool connection)
{
    QLOG_DEBUG() << "APMToolBar setConnection:" << connection;
    // Change the image to represent the state
    QObject *object = rootObject();
    object->setProperty("connected", connection);

    if ((m_uas)&&(connection==true)) {
        QLOG_DEBUG() << "APMToolBar: CustomMode" << m_uas->getCustomMode();
        setModeText(m_uas->getCustomModeText());
    } else {
        // disconnected
        rootObject()->setProperty("modeText", "mode");
    }
}

APMToolBar::~APMToolBar()
{
    QLOG_DEBUG() << "Destory APM Toolbar";
}

void APMToolBar::showConnectionDialog()
{
    // Displays a UI where the user can select a MAV Link.
    QLOG_DEBUG() << "APMToolBar: showConnectionDialog for current serial link " << m_currentLink;
    if(m_currentLink) {
        MainWindow::instance()->configLink(m_currentLink);
    } else {
        //
    }
}

void APMToolBar::updateLinkDisplay(LinkInterface* link)
{
    if (!qobject_cast<SerialLink*>(link))
    {
        //We only want to operate on serial links
        return;
    }
    QLOG_DEBUG() << "APMToolBar: updateLinkDisplay";

    QObject *object = rootObject();

    if (link && object){
        qint64 baudrate = link->getNominalDataRate();
        object->setProperty("baudrateLabel", QString::number(baudrate));

        QString linkName = link->getName();
        object->setProperty("linkNameLabel", linkName);

        setConnection(link->isConnected());
    }
}

void APMToolBar::newLinkCreated(LinkInterface* newLink)
{
    SerialLink* sLink = dynamic_cast<SerialLink*>(newLink);
    if(sLink) {
        QLOG_DEBUG() << "APMToolBar: new Serial Link Created" << newLink;
        m_currentLink = sLink;
        updateLinkDisplay(m_currentLink);
        connect(m_currentLink,SIGNAL(updateLink(LinkInterface*)),this,SLOT(updateLinkDisplay(LinkInterface*)));
        connect(m_currentLink, SIGNAL(connected(LinkInterface*)),
                        this, SLOT(connected(LinkInterface*)));
        connect(m_currentLink, SIGNAL(disconnected(LinkInterface*)),
                        this, SLOT(disconnected(LinkInterface*)));
    }
}

void APMToolBar::setModeText(const QString &text)
{
    QObject *object = rootObject();
    object->setProperty("modeText", text.toUpper());

    // [ToDo] ptentially factor the code below into the APMToolBar
    int customMode = m_uas->getCustomMode();
    bool inRTL;

    switch (m_uas->getSystemType()){
    case MAV_TYPE_FIXED_WING:
        inRTL = (customMode == ApmPlane::RTL);
        break;
    case MAV_TYPE_QUADROTOR:
    case MAV_TYPE_HEXAROTOR:
    case MAV_TYPE_OCTOROTOR:
    case MAV_TYPE_TRICOPTER:
    case MAV_TYPE_HELICOPTER:
        inRTL = (customMode == ApmCopter::RTL);
        break;
    case MAV_TYPE_GROUND_ROVER:
        inRTL = (customMode == ApmRover::RTL);
        break;
    default:
        inRTL = false;
    }

    if (inRTL) {
        object->setProperty("modeTextColor", QColor("red"));
        object->setProperty("modeBkgColor", QColor(0x88, 0x00, 0x00, 0x80));
        object->setProperty("modeBorderColor", QColor("red"));
    } else {
        object->setProperty("modeTextColor", QColor("white"));
        object->setProperty("modeBkgColor", QColor(0x00, 0x88, 0x00, 0x80));
        object->setProperty("modeBorderColor", QColor("white"));
    }
}

void APMToolBar::navModeChanged(int uasid, int mode, const QString &text)
{
    QLOG_DEBUG() << "APMToolBar::mode:" << text;
    Q_UNUSED(uasid);
    Q_UNUSED(mode);

    setModeText(text);
}

void APMToolBar::heartbeat(UASInterface* uas)
{
    QLOG_TRACE() << "APMToolBar::Heartbeat " << uas;

    if (uas != m_uas)
        return; // Only deal with the Active UAS

    QObject *object = rootObject();
    object->setProperty("heartbeat",QVariant(true));

    // Start a timer to turn the heartbeat animation off
    // if the timer is started again, the call is not made
    m_heartbeatTimer.setSingleShot(true);
    m_heartbeatTimer.start(1500);
}

void APMToolBar::stopHeartbeat()
{
    QLOG_TRACE() << "APMToolBar::stopHeartBeatDisplay";
    QObject *object = rootObject();
    object->setProperty("heartbeat",QVariant(false));
    m_heartbeatTimer.stop();
}

void APMToolBar::startAnimation()
{
    rootObject()->setProperty("stopAnimation",QVariant(false));
}

void APMToolBar::stopAnimation()
{
    rootObject()->setProperty("stopAnimation",QVariant(true));
}

void APMToolBar::disableConnectWidget(bool disable)
{
    rootObject()->setProperty("disableConnectWidget",QVariant(disable));
}

void APMToolBar::parameterChanged(int uas, int component, int parameterCount,
                                        int parameterId, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);
    Q_UNUSED(parameterCount);
    Q_UNUSED(parameterId);

    if (parameterName.contains("ARMING_REQUIRE")){
        // Shows Display of ARM status, if enabled
        int arming_required = value.toBool();
        rootObject()->setProperty("armed", QVariant(m_uas->isArmed()));
        rootObject()->setProperty("enableStatusDisplay",
                                  QVariant(arming_required));
    }

}
