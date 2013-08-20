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
    setSource(QUrl::fromLocalFile("qml/ApmToolBar.qml"));
    setResizeMode(QDeclarativeView::SizeRootObjectToView);
    this->rootContext()->setContextProperty("globalObj", this);
    connect(LinkManager::instance(),SIGNAL(newLink(LinkInterface*)),
            this, SLOT(updateLinkDisplay(LinkInterface*)));

    if (LinkManager::instance()->getLinks().count()>=3) {
        updateLinkDisplay(LinkManager::instance()->getLinks().last());
    }

    setConnection(false);

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

void APMToolBar::connectMAV()
{
    QLOG_DEBUG() << "APMToolBar: connectMAV ";

    bool connected = LinkManager::instance()->getLinks().last()->isConnected();
    bool result;

    if (!connected && LinkManager::instance()->getLinks().count() < 3)
    {
        // No Link so prompt to connect one
        MainWindow::instance()->addLink();
    } else if (!connected) {
        // Need to Connect Link
        result = LinkManager::instance()->getLinks().last()->connect();

    } else if (connected && LinkManager::instance()->getLinks().count() > 2) {
        // result need to be the opposite of success.
        result = !LinkManager::instance()->getLinks().last()->disconnect();
    }
    QLOG_DEBUG() << "result = " << result;

    // Change the image to represent the state
    setConnection(result);

    emit MAVConnected(result);
}

void APMToolBar::setConnection(bool connection)
{
    // Change the image to represent the state
    QObject *object = rootObject();
    object->setProperty("connected", connection);
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

    LinkInterface *link = LinkManager::instance()->getLinks().last();
    bool result;

    if (link && LinkManager::instance()->getLinks().count() >= 3)
    {
        // Serial Link so prompt to config it
        connect(link, SIGNAL(updateLink(LinkInterface*)),
                             this, SLOT(updateLinkDisplay(LinkInterface*)));
        result = MainWindow::instance()->configLink(link);

        if (!result)
            QLOG_DEBUG() << "Link Config Failed!";
    } else {
        // No Link so prompt to create one
        MainWindow::instance()->addLink();
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

void APMToolBar::navModeChanged(int uasid, int mode, const QString &text)
{
    QLOG_DEBUG() << "APMToolBar::mode:" << text;
    Q_UNUSED(uasid);

    QObject *object = rootObject();
    object->setProperty("modeText", text.toUpper());

    if (mode == ApmCopter::RTL) {
        object->setProperty("modeTextColor", QColor("red"));
        object->setProperty("modeBkgColor", QColor(0x88, 0x00, 0x00, 0x80));
        object->setProperty("modeBorderColor", QColor("red"));
    } else {
        object->setProperty("modeTextColor", QColor("white"));
        object->setProperty("modeBkgColor", QColor(0x00, 0x88, 0x00, 0x80));
        object->setProperty("modeBorderColor", QColor("white"));
    }
}

void APMToolBar::heartbeat(UASInterface* uas)
{
    QLOG_TRACE() << "APMToolBar::Heartbeat " << uas;
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
