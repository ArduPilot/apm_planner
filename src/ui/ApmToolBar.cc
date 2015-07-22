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
#include "LinkManager.h"
#include "MainWindow.h"
#include "ArduPilotMegaMAV.h"
#include <QQmlContext>
#include <QGraphicsObject>
#include <QTimer>
#include <QQuickItem>
#include <QQmlEngine>
APMToolBar::APMToolBar(QWindow *parent):
    QQuickView(parent), m_uas(NULL), m_disableOverride(false)
{
    // Configure our QML object
    QLOG_DEBUG() << "qmlBaseDir" << QGC::shareDirectory();
    QUrl url = QUrl::fromLocalFile(QGC::shareDirectory() + "/qml/ApmToolBar.qml");
    QLOG_DEBUG() << url;
    if (!QFile::exists(QGC::shareDirectory() + "/qml/ApmToolBar.qml"))
    {
        QMessageBox::information(0,"Error", "" + QGC::shareDirectory() + "/qml/ApmToolBar.qml" + " not found. Please reinstall the application and try again");
        exit(-1);
    }
    engine()->addImportPath("qml/"); //For local or win32 builds
    engine()->addImportPath(QGC::shareDirectory() +"/qml"); //For installed linux builds
    setSource(url);
    QLOG_DEBUG() << "QML Status:" << status();
    if (status() == QQuickView::Error)
    {
        QString errorstr = "";
        for (int i=0;i<errors().size();i++)
        {
            errorstr += errors().at(i).toString() + "\n";
        }
        QMessageBox::information(0,"Error", "Unable to load ApmToolbar.qml. Please reinstall the application and try again. Errors are as follows:\n" + errorstr);
        return;
    }
    setResizeMode(QQuickView::SizeRootObjectToView);
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
    connect(LinkManager::instance(),SIGNAL(newLink(int)),
            this,SLOT(newLinkCreated(int)));

    for (int i=0;i<LinkManager::instance()->getLinks().size();i++)
    {
        newLinkCreated(LinkManager::instance()->getLinks().at(i));
    }

    connect(&m_heartbeatTimer, SIGNAL(timeout()), this, SLOT(stopHeartbeat()));
    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    if (settings.contains("ADVANCED_MODE"))
    {
       QMetaObject::invokeMethod(rootObject(),"setAdvancedMode", Q_ARG(QVariant, settings.value("ADVANCED_MODE").toBool()));
    }
    connect(LinkManager::instance(),SIGNAL(linkChanged(int)),this,SLOT(updateLinkDisplay(int)));

    connect(this, SIGNAL(triggerDonateView()), this, SLOT(selectDonateView()));
}

void APMToolBar::checkAdvancedMode(bool checked)
{
    QMetaObject::invokeMethod(rootObject(),"setAdvancedMode", Q_ARG(QVariant, checked));
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
    rootObject()->setProperty("uasNameLabel",m_uas->getUASName());

    // Connect the signals from active links
    // disconnect signals from the active serial links
    QList<int> linkidlist = uas->getLinkIdList();
    bool currentlinkfound = false;
    for (int i=0;i<linkidlist.size();i++)
    {
        if (m_currentLinkId == linkidlist.at(i))
        {
            currentlinkfound = true;
        }
    }
    if (!currentlinkfound)
    {
        //We did not find the current link in the list of connected links.
        //Change the current link to match the first connected link in this UAS
        for (int i=0;i<linkidlist.size();i++)
        {
            if (LinkManager::instance()->getLinkConnected(linkidlist.at(i)))
            {
                // Show only the first actve link for a UAS
                m_currentLinkId = linkidlist.at(i);
                updateLinkDisplay(m_currentLinkId);
                break;
            }
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

void APMToolBar::setDonateViewAction(QAction *action)
{
    connect(action, SIGNAL(triggered()), this, SLOT(selectDonateView()));
}

void APMToolBar::setSimulationViewAction(QAction *action)
{
    connect(this, SIGNAL(triggerSimulationView()), action, SIGNAL(triggered()));
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

void APMToolBar::selectDonateView()
{
    QLOG_DEBUG() << "APMToolBar: selectDonateView";
    QString donateUrl = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=UKV3U28LVDGN4";
    QDesktopServices::openUrl(QUrl(donateUrl));
}

void APMToolBar::selectPlotView()
{
    QLOG_DEBUG() << "APMToolBar: selectPlotView";
}

void APMToolBar::connectMAV()
{
    QLOG_DEBUG() << "APMToolBar: connectMAV " << m_uas << "with sLink" << m_currentLinkId;

    if (LinkManager::instance()->getLinkConnected(m_currentLinkId))
    {
        LinkManager::instance()->disconnectLink(m_currentLinkId);
    }
    else
    {
        LinkManager::instance()->connectLink(m_currentLinkId);
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
    QLOG_DEBUG() << "APMToolBar: showConnectionDialog for current serial link " << m_currentLinkId;
    MainWindow::instance()->configLink(m_currentLinkId);
}
void APMToolBar::updateLinkDisplay(int linkid)
{
    if (m_currentLinkId != linkid)
    {
        //We only care about our current link
        QLOG_TRACE() << "APMToolBar::updateLinkDisplay called with non current link. Current:" << m_currentLinkId << "called:" << linkid;
        return;
    }

    QObject *object = rootObject();
    if (!object)
    {
        QLOG_FATAL() << "APMToolBar::updateLinkDisplay, null QML root object.";
        return;
    }
    LinkManager* lm = LinkManager::instance();
    QString linkName = lm->getLinkShortName(linkid);
    QString linkDetail = lm->getLinkDetail(linkid);

    object->setProperty("linkDetailLabel", linkDetail);
    object->setProperty("linkNameLabel", linkName);

    bool connected = LinkManager::instance()->getLinkConnected(linkid);
    setConnection(connected);
    QLOG_DEBUG() << "APMToolBar: updateLinkDisplay" << linkName <<":"<< linkDetail <<" connected:" << connected;
}

void APMToolBar::newLinkCreated(int linkid)
{
    if (LinkManager::instance()->getLinkType(linkid) == LinkInterface::SERIAL_LINK)
    {
        //We want to use this one.
        QLOG_DEBUG() << "APMToolBar: new Serial Link Created" << linkid;
        m_currentLinkId = linkid;
        updateLinkDisplay(m_currentLinkId);

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
void APMToolBar::overrideDisableConnectWidget(bool disable)
{
    m_disableOverride = disable;
}

void APMToolBar::disableConnectWidget(bool disable)
{
    if (!m_disableOverride)
    {
        rootObject()->setProperty("disableConnectWidget",QVariant(disable));
    }
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
