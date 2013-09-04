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

#include "apmtoolbar.h"
#include "QsLog.h"
#include "LinkManager.h"
#include "MainWindow.h"
#include "SerialLink.h"
#include "ArduPilotMegaMAV.h"
#include <QDeclarativeContext>
#include <QGraphicsObject>
#include <QTimer>

APMToolBar::APMToolBar(QWidget *parent):
    QDeclarativeView(parent), m_uas(0)
{
    // Configure our QML object

    QDir qmlBaseDir = QDir(qApp->applicationDirPath());
    QLOG_DEBUG() << "qmlBaseDir" << qmlBaseDir;
    QUrl url = QUrl::fromLocalFile(qmlBaseDir.absolutePath() + "/qml/ApmToolBar.qml");
    QLOG_DEBUG() << url;
    setSource(url);
    QLOG_DEBUG() << "QML Status:" << status();
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    this->rootContext()->setContextProperty("globalObj", this);
    connect(LinkManager::instance(),SIGNAL(newLink(LinkInterface*)),
            this, SLOT(updateLinkDisplay(LinkInterface*)));

    if (LinkManager::instance()->getLinks().count()>=3) {
        updateLinkDisplay(LinkManager::instance()->getLinks().last());
    }

    setConnection(false);

    // set the size of the device box and row spacing for icons
#ifdef Q_OS_MAC
    rootObject()->setProperty("rowSpacerSize", QVariant(3));
    rootObject()->setProperty("linkDeviceSize", QVariant(105));
#elif Q_OS_UNIX
    rootObject()->setProperty("rowSpacerSize", QVariant(3));
    rootObject()->setProperty("linkDeviceSize", QVariant(100));
#else
    rootObject()->setProperty("rowSpacerSize", QVariant(3));
    rootObject()->setProperty("linkDeviceSize", QVariant(80));
#endif

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUasSet(UASInterface*)));
    activeUasSet(UASManager::instance()->getActiveUAS());

    connect(&m_heartbeatTimer, SIGNAL(timeout()), this, SLOT(stopHeartbeat()));
}

void APMToolBar::activeUasSet(UASInterface *uas)
{
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

        // disconnect signals from the active links
        QList<LinkInterface*>* list = uas->getLinks();
        foreach( LinkInterface* link, *list)  {
            SerialLinkInterface* slink = dynamic_cast<SerialLinkInterface*>(link);
            if (slink != NULL) {
                    disconnect(slink, SIGNAL(connected(bool)),
                            this, SLOT(setConnection(bool)));
                }
            };
    }

    m_uas = uas;

    connect(m_uas,SIGNAL(armingChanged(bool)),
            this,SLOT(armingChanged(bool)));
    connect(m_uas,SIGNAL(armingChanged(int, QString)),
            this,SLOT(armingChanged(int, QString)));

    connect(m_uas, SIGNAL(navModeChanged(int,int,QString)),
            this, SLOT(navModeChanged(int,int,QString)));
    connect(m_uas, SIGNAL(heartbeat(UASInterface*)),
               this, SLOT(heartbeat(UASInterface*)));

    if (m_uas->getSystemType() == MAV_TYPE_FIXED_WING) {
        rootObject()->setProperty("disableStatusDisplay", QVariant(true));
    } else {
        rootObject()->setProperty("disableStatusDisplay", QVariant(false));
    }

    // Connect the signals from active links
    QList<LinkInterface*>* list = uas->getLinks();
    foreach( LinkInterface* link, *list)  {
        SerialLinkInterface* slink = dynamic_cast<SerialLinkInterface*>(link);
        if (slink != NULL) {
                connect(slink, SIGNAL(connected(bool)),
                        this, SLOT(setConnection(bool)));
                break;
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

void APMToolBar::selectTerminalView()
{
    QLOG_DEBUG() << "APMToolBar: selectTerminalView";
}

void APMToolBar::connectToActiveMav(UASInterface* uas)
{
    QLOG_DEBUG() << "connectToActiveMav: " << uas;
    bool connected;

    if (uas) {
        // Connected to a MAV
        QList<LinkInterface*>* list = uas->getLinks();
        foreach( LinkInterface* link, *list)  {
            SerialLinkInterface* slink = dynamic_cast<SerialLinkInterface*>(link);
            if (slink != NULL) {
                if (slink->isConnected()) {
                    connected = !slink->disconnect();
                    disconnect(slink, SIGNAL(connected(bool)),
                            this, SLOT(setConnection(bool)));
                } else {
                    connected = slink->connect();
                    connect(slink, SIGNAL(connected(bool)),
                            this, SLOT(setConnection(bool)));
                }
                break;
            } else {
                connected = false; // disconnected
            }
        };

    } else {
        // No active UAS set, pop up Serial Interface
        connected = false; //disconnected

        if (LinkManager::instance()->getLinks().count() < 3) {
            // No Link so prompt to connect one
            MainWindow::instance()->addLink();
        } else {
            // Need to Connect Link
            connected = LinkManager::instance()->getLinks().last()->connect();
        }

        MainWindow::instance()->addLink();
    }
}

void APMToolBar::connectMAV()
{
    QLOG_DEBUG() << "APMToolBar: connectMAV ";

    connectToActiveMav(m_uas);
}

void APMToolBar::setConnection(bool connection)
{
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
    QLOG_DEBUG() << "APMToolBar: showConnectionDialog link count ="
             << LinkManager::instance()->getLinks().count();

    if (m_uas) {
        // Active to a MAV
        QList<LinkInterface*>* list = m_uas->getLinks();
        foreach( LinkInterface* link, *list)  {
            SerialLinkInterface* slink = dynamic_cast<SerialLinkInterface*>(link);
            if (slink != NULL) {
                MainWindow::instance()->configLink(slink);
                break;
            }
        };

    } else {
        // No active UAS set, pop up Serial Interface
        if (LinkManager::instance()->getLinks().count() < 3) {
            // No Link so prompt to connect one
            MainWindow::instance()->addLink();
        } else {
            // Need to Connect to a Link
            MainWindow::instance()->configLink(LinkManager::instance()->getLinks().last());
        }
    }
}

void APMToolBar::updateLinkDisplay(LinkInterface* newLink)
{
    if (!qobject_cast<SerialLink*>(newLink))
    {
        //We only want to operate on serial links
        return;
    }
    QLOG_DEBUG() << "APMToolBar: updateLinkDisplay";

    QObject *object = rootObject();

    if (newLink && object){
        qint64 baudrate = newLink->getNominalDataRate();
        object->setProperty("baudrateLabel", QString::number(baudrate));

        QString linkName = newLink->getName();
        object->setProperty("linkNameLabel", linkName);

        connect(newLink, SIGNAL(connected(bool)),
                this, SLOT(setConnection(bool)));

        setConnection(newLink->isConnected());
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
