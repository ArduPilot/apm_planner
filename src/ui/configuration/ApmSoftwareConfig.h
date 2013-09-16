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
 *   @brief "Software" configuration selection screen
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef APMSOFTWARECONFIG_H
#define APMSOFTWARECONFIG_H

#include <QWidget>
#include "ui_ApmSoftwareConfig.h"
#include "FlightModeConfig.h"
#include "BasicPidConfig.h"
#include "StandardParamConfig.h"
#include "GeoFenceConfig.h"
#include "FailSafeConfig.h"
#include "AdvancedParamConfig.h"
#include "ArduCopterPidConfig.h"
#include "ArduPlanePidConfig.h"
#include "ArduRoverPidConfig.h"
#include "AdvParameterList.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "QGCSettingsWidget.h"

class ApmSoftwareConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit ApmSoftwareConfig(QWidget *parent = 0);
    ~ApmSoftwareConfig();
private slots:
    void activateStackedWidget();
    void activeUASSet(UASInterface *uas);
    void uasConnected();
    void uasDisconnected();
    void apmParamNetworkReplyFinished();
private:
    QString m_apmPdefFilename;
    UASInterface *m_uas;
    Ui::ApmSoftwareConfig ui;
    QPointer<BasicPidConfig> m_basicPidConfig;
    QPointer<FlightModeConfig> m_flightConfig;
    QPointer<StandardParamConfig> m_standardParamConfig;
    QPointer<GeoFenceConfig> m_geoFenceConfig;
    QPointer<AdvancedParamConfig> m_advancedParamConfig;
    QPointer<ArduCopterPidConfig> m_arduCopterPidConfig;
    QPointer<ArduPlanePidConfig> m_arduPlanePidConfig;
    QPointer<ArduRoverPidConfig> m_arduRoverPidConfig;
    QPointer<AdvParameterList> m_advParameterList;
    QPointer<QGCSettingsWidget> m_settingsConfig;
    QMap<QObject*,QWidget*> m_buttonToConfigWidgetMap;
};

#endif // APMSOFTWARECONFIG_H
