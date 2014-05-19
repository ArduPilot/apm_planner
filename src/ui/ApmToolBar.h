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

#ifndef APMTOOLBAR_H
#define APMTOOLBAR_H

#include "UASInterface.h"
#include <QAction>
#include <QDeclarativeView>

class LinkInterface;
class QTimer;

class APMToolBar : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit APMToolBar(QWidget *parent = 0);
    ~APMToolBar();

    void setFlightViewAction(QAction *action);
    void setFlightPlanViewAction(QAction *action);
    void setInitialSetupViewAction(QAction *action);
    void setConfigTuningViewAction(QAction *action);
    void setPlotViewAction(QAction *action);
    void setSimulationViewAction(QAction *action);
    void setTerminalViewAction(QAction *action);
    void setConnectMAVAction(QAction *action);
    
    void setModeText(const QString &text);
signals:
    void triggerFlightView();
    void triggerFlightPlanView();
    void triggerInitialSetupView();
    void triggerConfigTuningView();
    void triggerSimulationView();
    void triggerPlotView();
    void triggerTerminalView();

    void MAVConnected(bool connected);

public slots:
    void selectFlightView();
    void selectFlightPlanView();
    void selectInitialSetupView();
    void selectConfigTuningView();
    void selectSimulationView();
    void selectPlotView();
    void selectTerminalView();

    void checkAdvancedMode(bool checked);

    void connectMAV();
    void showConnectionDialog();
    void setConnection(bool connection);
    void connected(LinkInterface *linkInterface);
    void disconnected(LinkInterface *linkInterface);

    void activeUasSet(UASInterface *uas);
    void armingChanged(int sysId, QString armingState);
    void armingChanged(bool armed);

    void updateLinkDisplay(LinkInterface *link);
    void newLinkCreated(LinkInterface* newLink);

    void navModeChanged(int uasid, int mode, const QString& text);
    void heartbeat(UASInterface* uas);
    void stopHeartbeat();

    void startAnimation();
    void stopAnimation();

    void disableConnectWidget(bool disable);
    void overrideDisableConnectWidget(bool disable);

    void parameterChanged(int uas, int component, int parameterCount,
                          int parameterId, QString parameterName, QVariant value);

private:
    QPointer<UASInterface> m_uas;
    QTimer m_heartbeatTimer;
    QPointer<LinkInterface> m_currentLink;
    bool m_disableOverride;
};

#endif // APMTOOLBAR_H
