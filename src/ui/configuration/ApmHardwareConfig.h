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
 *   @brief APM Hardware Configuration widget header.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 */

#ifndef APMHARDWARECONFIG_H
#define APMHARDWARECONFIG_H

#include <QWidget>
#include "ui_ApmHardwareConfig.h"
#include <UASInterface.h>
#include <UASManager.h>
#include "FrameTypeConfig.h"
#include "CompassConfig.h"
#include "AccelCalibrationConfig.h"
#include "RadioCalibrationConfig.h"
#include "Radio3DRConfig.h"
#include "BatteryMonitorConfig.h"
#include "RangeFinderConfig.h"
#include "AirspeedConfig.h"
#include "OpticalFlowConfig.h"
#include "OsdConfig.h"
#include "CameraGimbalConfig.h"
#include "AntennaTrackerConfig.h"
#include "ApmPlaneLevel.h"
#include "ApmCustomFirmwareConfig.h"
#include "FlightModeConfig.h"
#include "FailSafeConfig.h"

class ApmHardwareConfig : public QWidget
{
    Q_OBJECT

    // Download state machine for parameters
    enum ParamReadWriteState { none, startRead, startWrite, readingParams, writingParams, completed };
    
public:
    explicit ApmHardwareConfig(QWidget *parent = nullptr);

signals:
    void advancedModeChanged(bool stateupdateFirmwareButtons);

public slots:
    void parameterChanged(int uas, int component, int parameterCount, int parameterId, QString parameterName, QVariant value);

    void optionalClicked();
    void mandatoryClicked();
    void toggleButtonsShown(bool show);
    void advModeChanged(bool state);

private:
    QPointer<ApmCustomFirmwareConfig> m_apmCustomFWConfig;

    //Mandatory
    QPointer<FrameTypeConfig> m_frameConfig;
    QPointer<CompassConfig> m_compassConfig;
    QPointer<AccelCalibrationConfig> m_accelConfig;
    QPointer<ApmPlaneLevel> m_planeLevel;
    QPointer<RadioCalibrationConfig> m_radioConfig;
    QPointer<FlightModeConfig> m_flightConfig;
    QPointer<FailSafeConfig> m_failSafeConfig;

    //Optional
    QPointer<Radio3DRConfig> m_radio3drConfig;
    QPointer<BatteryMonitorConfig> m_batteryConfig;
    QPointer<RangeFinderConfig> m_sonarConfig;
    QPointer<AirspeedConfig> m_airspeedConfig;
    QPointer<OpticalFlowConfig> m_opticalFlowConfig;
    QPointer<OsdConfig> m_osdConfig;
    QPointer<CameraGimbalConfig> m_cameraGimbalConfig;
    QPointer<AntennaTrackerConfig> m_antennaTrackerConfig;

private slots:
    void activeUASSet(UASInterface *uas);
    void activateStackedWidget();
    void uasConnected();
    void uasDisconnected();
    void firmwareWhileConnected();

private:
    Ui::ApmHardwareConfig ui;
    UASInterface *m_uas{nullptr};

    //This is a map between the buttons, and the widgets they should be displying
    QMap<QObject*,QWidget*> m_buttonToConfigWidgetMap;

    ParamReadWriteState m_paramDownloadState{none};
    int m_paramDownloadCount{0};
    int m_paramTotalCount{0};
    bool m_mandatory{false};

    QMap<QString, UASParameter*> m_parameterList;
    QString m_paramFileToCompare;
};

#endif // APMHARDWARECONFIG_H
