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
#include "CopterPidConfig.h"
#include "ArduPlanePidConfig.h"
#include "ArduRoverPidConfig.h"
#include "AdvParameterList.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "QGCSettingsWidget.h"

class ApmSoftwareConfig : public QWidget
{
    Q_OBJECT

    // Download state machine for parameters
    enum ParamReadWriteState { none, startRead, startWrite, readingParams, writingParams, completed };
    
public:
    explicit ApmSoftwareConfig(QWidget *parent = 0);
    ~ApmSoftwareConfig();

public slots:
    void parameterChanged(int uas, int component, int parameterCount, int parameterId, QString parameterName, QVariant value);
    void advModeChanged(bool state);

private slots:
    void activateStackedWidget();
    void activeUASSet(UASInterface *uas);
    void uasConnected();
    void uasDisconnected();
    void apmParamNetworkReplyFinished();
    void populateTimerTick();
    void updateUAS();
    void reloadView();

private:

    //Parameter from XML
    class ParamConfig
    {
    public:
        QString name;
        QString docs;
        QString param;
        double min;
        double max;
        double increment;
        QList<QPair<int,QString> > valuelist;
        bool isAdvanced;
        bool isRange;
    };

    //List of parameters from XML file, to be loaded on a timer
    QList<ParamConfig> m_paramConfigList;

    //Parameter loading timer
    QTimer m_populateTimer;

    QString m_apmPdefFilename;
    UASInterface *m_uas;
    Ui::ApmSoftwareConfig ui;
    QPointer<BasicPidConfig> m_basicPidConfig;
    QPointer<FlightModeConfig> m_flightConfig;
    QPointer<StandardParamConfig> m_standardParamConfig;
    QPointer<GeoFenceConfig> m_geoFenceConfig;
    QPointer<AdvancedParamConfig> m_advancedParamConfig;
    QPointer<ArduCopterPidConfig> m_arduCopterPidConfig;
    QPointer<CopterPidConfig> m_copterPidConfig;
    QPointer<ArduPlanePidConfig> m_arduPlanePidConfig;
    QPointer<ArduRoverPidConfig> m_arduRoverPidConfig;
    QPointer<AdvParameterList> m_advParameterList;
    QPointer<QGCSettingsWidget> m_settingsConfig;
    QMap<QObject*,QWidget*> m_buttonToConfigWidgetMap;

    ParamReadWriteState m_paramDownloadState;
    int m_paramDownloadCount;
    int m_paramTotalCount;

    bool m_isAdvancedMode;
};

#endif // APMSOFTWARECONFIG_H
