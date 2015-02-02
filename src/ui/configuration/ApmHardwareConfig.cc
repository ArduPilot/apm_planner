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
 *   @brief APM Hardware Configuration widget source.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *
 */
#include "QsLog.h"
#include "ApmHardwareConfig.h"

ApmHardwareConfig::ApmHardwareConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_paramDownloadState(none),
    m_paramDownloadCount(0),
    m_uas(NULL),
    m_mandatory(false)
{
    ui.setupUi(this);

    ui.mandatoryHardware->setVisible(false);
    ui.optionalHardwareButton->setVisible(false);
    ui.frameTypeButton->setVisible(false);
    ui.compassButton->setVisible(false);
    ui.accelCalibrateButton->setVisible(false);
    ui.failSafeButton->setVisible(false);
    ui.flightModesButton->setVisible(false);
    ui.arduPlaneLevelButton->setVisible(false);
    ui.radioCalibrateButton->setVisible(false);
    ui.batteryMonitorButton->setVisible(false);
    ui.rangeFinderButton->setVisible(false);
    ui.airspeedButton->setVisible(false);
    ui.opticalFlowButton->setVisible(false);
    ui.osdButton->setVisible(false);
    ui.cameraGimbalButton->setVisible(false);
    ui.antennaTrackerButton->setVisible(false);

    ui.hiddenPushButton->setVisible(false); // And it's checked.
    ui.radio3DRLargeButton->setVisible(true); // [SHOW 3DR RADIO]
    ui.antennaTrackerLargeButton->setVisible(false); // [HIDE Antenna Tracking]

    m_apmFirmwareConfig = new ApmFirmwareConfig(this);
    connect(m_apmFirmwareConfig,SIGNAL(showBlankingScreen()),this,SLOT(activateBlankingScreen()));
    ui.stackedWidget->addWidget(m_apmFirmwareConfig); //Firmware placeholder.
    m_buttonToConfigWidgetMap[ui.firmwareButton] = m_apmFirmwareConfig;
    connect(ui.firmwareButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));
    connect(this, SIGNAL(advancedModeChanged(bool)), m_apmFirmwareConfig, SLOT(advancedModeChanged(bool)));

    m_flightConfig = new FlightModeConfig(this);
    ui.stackedWidget->addWidget(m_flightConfig);
    m_buttonToConfigWidgetMap[ui.flightModesButton] = m_flightConfig;
    connect(ui.flightModesButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_frameConfig = new FrameTypeConfig(this);
    ui.stackedWidget->addWidget(m_frameConfig);
    m_buttonToConfigWidgetMap[ui.frameTypeButton] = m_frameConfig;
    connect(ui.frameTypeButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_compassConfig = new CompassConfig(this);
    ui.stackedWidget->addWidget(m_compassConfig);
    m_buttonToConfigWidgetMap[ui.compassButton] = m_compassConfig;
    connect(ui.compassButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_failSafeConfig = new FailSafeConfig(this);
    ui.stackedWidget->addWidget(m_failSafeConfig);
    m_buttonToConfigWidgetMap[ui.failSafeButton] = m_failSafeConfig;
    connect(ui.failSafeButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_accelConfig = new AccelCalibrationConfig(this);
    ui.stackedWidget->addWidget(m_accelConfig);
    m_buttonToConfigWidgetMap[ui.accelCalibrateButton] = m_accelConfig;
    connect(ui.accelCalibrateButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_planeLevel = new ApmPlaneLevel(this);
    ui.stackedWidget->addWidget(m_planeLevel);
    m_buttonToConfigWidgetMap[ui.arduPlaneLevelButton] = m_planeLevel;
    connect(ui.arduPlaneLevelButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_radioConfig = new RadioCalibrationConfig(this);
    ui.stackedWidget->addWidget(m_radioConfig);
    m_buttonToConfigWidgetMap[ui.radioCalibrateButton] = m_radioConfig;
    connect(ui.radioCalibrateButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_radio3drConfig = new Radio3DRConfig(this);
    ui.stackedWidget->addWidget(m_radio3drConfig);
    m_buttonToConfigWidgetMap[ui.radio3DRLargeButton] = m_radio3drConfig;
    connect(ui.radio3DRLargeButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_batteryConfig = new BatteryMonitorConfig(this);
    ui.stackedWidget->addWidget(m_batteryConfig);
    m_buttonToConfigWidgetMap[ui.batteryMonitorButton] = m_batteryConfig;
    connect(ui.batteryMonitorButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_sonarConfig = new RangeFinderConfig(this);
    ui.stackedWidget->addWidget(m_sonarConfig);
    m_buttonToConfigWidgetMap[ui.rangeFinderButton] = m_sonarConfig;
    connect(ui.rangeFinderButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_airspeedConfig = new AirspeedConfig(this);
    ui.stackedWidget->addWidget(m_airspeedConfig);
    m_buttonToConfigWidgetMap[ui.airspeedButton] = m_airspeedConfig;
    connect(ui.airspeedButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_opticalFlowConfig = new OpticalFlowConfig(this);
    ui.stackedWidget->addWidget(m_opticalFlowConfig);
    m_buttonToConfigWidgetMap[ui.opticalFlowButton] = m_opticalFlowConfig;
    connect(ui.opticalFlowButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_osdConfig = new OsdConfig(this);
    ui.stackedWidget->addWidget(m_osdConfig);
    m_buttonToConfigWidgetMap[ui.osdButton] = m_osdConfig;
    connect(ui.osdButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_cameraGimbalConfig = new CameraGimbalConfig(this);
    ui.stackedWidget->addWidget(m_cameraGimbalConfig);
    m_buttonToConfigWidgetMap[ui.cameraGimbalButton] = m_cameraGimbalConfig;
    connect(ui.cameraGimbalButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_antennaTrackerConfig = new AntennaTrackerConfig(this);
    ui.stackedWidget->addWidget(m_antennaTrackerConfig);
    m_buttonToConfigWidgetMap[ui.antennaTrackerButton] = m_antennaTrackerConfig;
    m_buttonToConfigWidgetMap[ui.antennaTrackerLargeButton] = m_antennaTrackerConfig;
    connect(ui.antennaTrackerButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));
    connect(ui.antennaTrackerLargeButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_setupWarningMessage = new SetupWarningMessage(this);
    ui.stackedWidget->addWidget(m_setupWarningMessage);
    m_buttonToConfigWidgetMap[ui.hiddenPushButton] = m_setupWarningMessage;
    connect(ui.hiddenPushButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_uas=0;
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    if (UASManager::instance()->getActiveUAS())
    {
        activeUASSet(UASManager::instance()->getActiveUAS());
    }

    // Setup Parameter Progress bars
    ui.globalParamProgressBar->setRange(0,100);

    ui.mandatoryHardware->setChecked(true);

    // Set start up WarningMessageView view
    ui.stackedWidget->setCurrentWidget(m_buttonToConfigWidgetMap[ui.hiddenPushButton]);
    ui.hiddenPushButton->setChecked(true);
}

void ApmHardwareConfig::advModeChanged(bool state)
{
    emit advancedModeChanged(state);
}

void ApmHardwareConfig::activateBlankingScreen()
{
    ui.stackedWidget->setCurrentWidget(m_setupWarningMessage);
}

void ApmHardwareConfig::activateStackedWidget()
{
    if (ui.failSafeButton == sender())
    {
        QMessageBox::information(this,"Warning","Ensure all rotors/props are REMOVED from your aircraft before proceeding");
    }
    if (m_buttonToConfigWidgetMap.contains(sender()))
    {
        ui.stackedWidget->setCurrentWidget(m_buttonToConfigWidgetMap[sender()]);
    }
}

ApmHardwareConfig::~ApmHardwareConfig()
{
}
void ApmHardwareConfig::uasConnected()
{
    if (!m_uas)
    {
        return;
    }
    QLOG_DEBUG() << "AHC: uasConnected()";
    connect(ui.optionalHardwareButton, SIGNAL(clicked()),
            this, SLOT(optionalClicked()),Qt::UniqueConnection);
    connect(ui.mandatoryHardware, SIGNAL(clicked()),
            this, SLOT(mandatoryClicked()),Qt::UniqueConnection);
    // Hide offline options and show Optional and Mandatory buttons
    ui.radio3DRLargeButton->setVisible(false);
    ui.antennaTrackerLargeButton->setVisible(false);

    ui.mandatoryHardware->setVisible(true);
    ui.mandatoryHardware->setChecked(false);
    ui.optionalHardwareButton->setVisible(true);
    ui.optionalHardwareButton->setChecked(false);

    m_mandatory = true;
    toggleButtonsShown(m_mandatory);
    ui.mandatoryHardware->setChecked(true);

}

void ApmHardwareConfig::uasDisconnected()
{
    if (!m_uas)
    {
        return;
    }
    QLOG_DEBUG() << "AHC: uasDisconnected()";
    // Show offline options and hide Optional and Mandatory buttons
    disconnect(ui.mandatoryHardware, SIGNAL(clicked()),
                this, SLOT(mandatoryClicked()));
    disconnect(ui.optionalHardwareButton, SIGNAL(clicked()),
                this, SLOT(optionalClicked()));

    ui.optionalHardwareButton->setChecked(false);
    ui.optionalHardwareButton->setVisible(false);

    ui.antennaTrackerButton->setVisible(false);

    ui.mandatoryHardware->setVisible(false);
    ui.mandatoryHardware->setChecked(false);

    ui.frameTypeButton->setVisible(false);
    ui.rangeFinderButton->setVisible(false);
    ui.compassButton->setVisible(false);
    ui.accelCalibrateButton->setVisible(false);
    ui.radioCalibrateButton->setVisible(false);

    ui.flightModesButton->setVisible(false);
    ui.failSafeButton->setVisible(false);

    ui.batteryMonitorButton->setVisible(false);
    ui.airspeedButton->setVisible(false);
    ui.opticalFlowButton->setVisible(false);
    ui.osdButton->setVisible(false);
    ui.cameraGimbalButton->setVisible(false);

    ui.radio3DRLargeButton->setVisible(true); // [SHOW 3DR RADIO]
    ui.antennaTrackerLargeButton->setVisible(false); // [HIDE Antenna Tracking]

    ui.stackedWidget->setCurrentWidget(m_buttonToConfigWidgetMap[ui.hiddenPushButton]);
    ui.hiddenPushButton->setChecked(true);
}
void ApmHardwareConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        uasDisconnected();
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));

        disconnect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
                this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));

        m_uas = 0;
    }
    if (!uas)
    {
        return;
    }
    m_uas = uas;
    connect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));

    connect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
            this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));

    uasConnected();

}

void ApmHardwareConfig::optionalClicked()
{
    QLOG_DEBUG() << "optionalClicked";
    if(!m_uas)
        return;
    if (m_mandatory == false) {
        ui.optionalHardwareButton->setChecked(true);
        return; // no change of state
    }
    m_mandatory = false; // show optional options

    toggleButtonsShown(m_mandatory);
    ui.mandatoryHardware->setChecked(false);
}

void ApmHardwareConfig::mandatoryClicked()
{
    QLOG_DEBUG() << "mandatoryClicked";
    if(!m_uas)
        return;
    if (m_mandatory == true){
        ui.mandatoryHardware->setChecked(true);
        return; // no change of state
    }
    m_mandatory = true; // show mandatory options.

    toggleButtonsShown(m_mandatory);
    ui.optionalHardwareButton->setChecked(false);
}

void ApmHardwareConfig::toggleButtonsShown(bool show)
{

    QLOG_DEBUG() << "toggleButtonsShown" << show;

    if(!m_uas)
        return;

    if (m_uas->isMultirotor()){
        QLOG_DEBUG() << "Multirotor";
        // Buttons to disable
        ui.airspeedButton->setVisible(false);

        // Mandatory Options to show
        ui.frameTypeButton->setVisible(show);
        ui.compassButton->setVisible(show);
        ui.accelCalibrateButton->setVisible(show);
        ui.radioCalibrateButton->setVisible(show);
        ui.flightModesButton->setVisible(show);
        ui.failSafeButton->setVisible(show);

        // Optional Options to Hide
        ui.batteryMonitorButton->setVisible(!show);
        ui.opticalFlowButton->setVisible(!show);
        ui.osdButton->setVisible(!show);
        ui.cameraGimbalButton->setVisible(!show);// [SHOW Camera Gimbal]
//        ui.antennaTrackerButton->setShown(!show);// [HIDE Antenna Tracking]
        ui.rangeFinderButton->setVisible(!show);

    } else if (m_uas->isFixedWing()){
        QLOG_DEBUG() << "FixedWing";
        // Buttons to disable
        ui.frameTypeButton->setVisible(false);

        // Mandatory Options to show
        ui.compassButton->setVisible(show);
        ui.accelCalibrateButton->setVisible(show);
        ui.radioCalibrateButton->setVisible(show);
        ui.flightModesButton->setVisible(show);
        ui.failSafeButton->setVisible(show);

        // Optional Options to Hide
        ui.batteryMonitorButton->setVisible(!show);
        ui.opticalFlowButton->setVisible(!show);
        ui.osdButton->setVisible(!show);
        ui.cameraGimbalButton->setVisible(!show); // [SHOW Camera Gimbal]
//        ui.antennaTrackerButton->setShown(!show); // [HIDE Antenna Tracking]
        ui.airspeedButton->setVisible(!show);

    } else {
        // Assume Ground Vehicle et al.
        QLOG_DEBUG() << "Ground Vehicle & Other";
        // Butons to disable
        ui.frameTypeButton->setVisible(false);
        ui.airspeedButton->setVisible(false);

        // Mandatory Options to show
        ui.compassButton->setVisible(show);
        ui.accelCalibrateButton->setVisible(show);
        ui.radioCalibrateButton->setVisible(show);
        ui.flightModesButton->setVisible(show);
        ui.failSafeButton->setVisible(show);

        // Optional Options to Hide
//        ui.radio3DRButton->setShown(!show); [HIDE 3DR RADIO]
        ui.batteryMonitorButton->setVisible(!show);
        ui.opticalFlowButton->setVisible(!show);
        ui.osdButton->setVisible(!show);
        ui.cameraGimbalButton->setVisible(!show); // [SHOW Camera Gimbal]
//        ui.antennaTrackerButton->setShown(!show); // [HIDE Antenna Tracking]
        ui.rangeFinderButton->setVisible(!show);
    }
}

void ApmHardwareConfig::parameterChanged(int uas, int component, int parameterCount, int parameterId,
                                         QString parameterName, QVariant value)
{

    QString countString;
    // Create progress of downloading all parameters for UI
    switch (m_paramDownloadState){
    case none:
        if (parameterId == UINT16_MAX){
            // This is an ACK package, not a full read
            break;
        } else if ((parameterId == 0) && (parameterCount != UINT16_MAX)) {
            // Its a new download List, Start from zero.
            ui.globalParamStateLabel->setText(tr("Downloading Params..."));
        } else {
            break;
        }

        // Otherwise, trigger progress bar update.
    case startRead:
        QLOG_INFO() << "Starting Global Param Progress Bar Updating sys:" << uas;
        m_paramDownloadCount = 1;

        countString = QString::number(m_paramDownloadCount) + "/"
                        + QString::number(parameterCount);
        QLOG_TRACE() << "Global Param Progress Bar: " << countString
                     << "paramId:" << parameterId << "name:" << parameterName
                     << "paramValue:" << value;
        ui.globalParamProgressLabel->setText(countString);
        ui.globalParamProgressBar->setValue((m_paramDownloadCount/(float)parameterCount)*100.0);

        m_paramDownloadState = readingParams;
        break;

    case readingParams:
        m_paramDownloadCount++;
        countString = QString::number(m_paramDownloadCount) + "/"
                        + QString::number(parameterCount);
        QLOG_TRACE() << "Param Progress Bar: " << countString
                     << "paramId:" << parameterId << "name:" << parameterName
                     << "paramValue:" << value;
        ui.globalParamProgressLabel->setText(countString);
        ui.globalParamProgressBar->setValue((m_paramDownloadCount/(float)parameterCount)*100.0);

        if (m_paramDownloadCount == parameterCount){
            m_paramDownloadState = completed;
            ui.globalParamStateLabel->setText(tr("Params Downloaded"));
        }
        break;

    case completed:
        QLOG_INFO() << "Global Finished Downloading Params" << m_paramDownloadCount;
        m_paramDownloadState = none;
        break;

    default:
        ; // Do Nothing
    }
}
