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

#include "AccelCalibrationConfig.h"
#include "GAudioOutput.h"
#include "MainWindow.h"


const char* COUNTDOWN_STRING = "<h3>Calibrate MAV%03d<br>Time remaining until timeout: <b>%d</b><h3>";
const char* CALIBRATE_BUTTON_TEXT = "Full\nAccel Calibration";
const char* CONTINUE_BUTTON_TEXT = "Continue\nPress SpaceBar";


AccelCalibrationConfig::AccelCalibrationConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_muted(false),
    m_isCalibrating(false),
    m_countdownCount(CALIBRATION_TIMEOUT_SEC)
{
    ui.setupUi(this);
    connect(ui.calibrateAccelButton,SIGNAL(clicked()),this,SLOT(calibrateButtonClicked()));
    connect(ui.calibrateAccelSimpleButton,SIGNAL(clicked()),this,SLOT(calibrateSimpleButtonClicked()));

    m_accelAckCount=-1;
    initConnections();
    //coutdownLabel
    connect(&m_countdownTimer,SIGNAL(timeout()),this,SLOT(countdownTimerTick()));
}

AccelCalibrationConfig::~AccelCalibrationConfig()
{
}
void AccelCalibrationConfig::countdownTimerTick()
{
    ui.coutdownLabel->setText(QString().asprintf(COUNTDOWN_STRING, m_uas->getUASID(), m_countdownCount--));
    if (m_countdownCount <= 0)
    {
        ui.coutdownLabel->setText("Command timed out, please try again");
        m_countdownTimer.stop();
        ui.calibrateAccelButton->setText(ui.calibrateAccelButton->text());
        cancelCalibration();
    }
}

void AccelCalibrationConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(textMessageReceived(int,int,int,QString)),
                   this,SLOT(uasTextMessageReceived(int,int,int,QString)));
        disconnect(m_uas,SIGNAL(connected()), this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()), this,SLOT(uasDisconnected()));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas)
    {
        return;
    }
    connect(m_uas,SIGNAL(textMessageReceived(int,int,int,QString)),
            this,SLOT(uasTextMessageReceived(int,int,int,QString)));
    connect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    uasConnected();

}
void AccelCalibrationConfig::uasConnected()
{
    cancelCalibration();
}

void AccelCalibrationConfig::uasDisconnected()
{
}


void AccelCalibrationConfig::calibrateSimpleButtonClicked() {
    if (!m_uas) {
        showNullMAVErrorMessageBox();
        return;
    }

    ui.outputLabel->clear();

    // Mute Audio until calibrated to avoid HeartBeat Warning message
    if (GAudioOutput::instance()->isMuted() == false) {
        GAudioOutput::instance()->mute(true);
        m_muted = true;
    }

    // Simple Accel Calibration
    MAV_CMD command = MAV_CMD_PREFLIGHT_CALIBRATION;
    int confirm = 0;
    float param1 = 0.0;
    float param2 = 0.0;
    float param3 = 0.0;
    float param4 = 0.0;
    float param5 = 4.0; // 4 = Simple Accel Calibration
    float param6 = 0.0;
    float param7 = 0.0;
    int component = 1;
    m_uas->executeCommand(command, confirm, param1, param2, param3, param4,
                          param5, param6, param7, component);
    m_isCalibrating = true;
    ui.outputLabel->setText("Simple Accel Calibration...");
}

void AccelCalibrationConfig::calibrateButtonClicked() {
    if (!m_uas) {
        showNullMAVErrorMessageBox();
        return;
    }

    ui.outputLabel->clear();
    ui.calibrateAccelButton->setFocus();

    // Mute Audio until calibrated to avoid HeartBeat Warning message
    if (GAudioOutput::instance()->isMuted() == false) {
        GAudioOutput::instance()->mute(true);
        m_muted = true;
    }
    m_uas->getLinks()->at(0)->disableTimeouts();

    MainWindow::instance()->toolBar().stopAnimation();

    if (m_accelAckCount == -1) {
        // Start full 3D Accel Calibration.
        MAV_CMD command = MAV_CMD_PREFLIGHT_CALIBRATION;
        int confirm = 0;
        float param1 = 0.0;
        float param2 = 0.0;
        float param3 = 0.0;
        float param4 = 0.0;
        float param5 = 1.0; // 1 = Full 3D Accel Cal
        float param6 = 0.0;
        float param7 = 0.0;
        int component = 1;
        m_uas->executeCommand(command, confirm, param1, param2, param3, param4,
                              param5, param6, param7, component);
        m_countdownCount = CALIBRATION_TIMEOUT_SEC;
        ui.coutdownLabel->setText(QString().asprintf(
            COUNTDOWN_STRING, m_uas->getUASID(), m_countdownCount--));
        m_countdownTimer.start(1000);

        m_isCalibrating = true; // Guard against showing unwanted GCS Text Messages.
        m_accelAckCount = 0;

        ui.outputLabel->clear();

    } else if (m_accelAckCount <= 6) {
        QLOG_DEBUG() << "m_accelAckCount = " << m_accelAckCount;
        m_countdownCount = CALIBRATION_TIMEOUT_SEC;
        m_uas->executeCommandAck(m_accelAckCount, true);

    } else {
        // Auto Reset if bad state
        cancelCalibration();
    }
}

void AccelCalibrationConfig::cancelCalibration()
{
    QLOG_INFO() << "Cancel Accelerometer Calibration.";
    if (m_accelAckCount >= 0) {
        ui.coutdownLabel->setText("");
        m_countdownTimer.stop();
        ui.calibrateAccelButton->setText(CALIBRATE_BUTTON_TEXT);

        for (int i = 0; i < m_accelAckCount; i++) {
            QLOG_WARN() << "Canceling " << i << " of " << m_accelAckCount;
            m_uas->executeCommandAck(i,true);
        }
        m_accelAckCount = -1;
    }
}

void AccelCalibrationConfig::hideEvent(QHideEvent *evt)
{
    Q_UNUSED(evt);

    if (m_muted) { // turns audio backon, when you leave the page
        GAudioOutput::instance()->mute(false);
        m_muted = false;
    }

    MainWindow::instance()->toolBar().startAnimation();

    if (!m_uas || !m_accelAckCount)
    {
        return;
    }
    cancelCalibration();
    m_uas->getLinks()->at(0)->enableTimeouts();
}

void AccelCalibrationConfig::uasTextMessageReceived(int uasid, int componentid, int severity, QString text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);

    QLOG_DEBUG() << "Severity:" << severity << " text:" <<text;

    if (severity <= MAV_SEVERITY_CRITICAL)
    {
        //This is a calibration instruction
        if (!m_isCalibrating
            || text.startsWith("PreArm:")
            || text.startsWith("EKF")
            || text.startsWith("Arm")
            || text.startsWith("Initialising")
        ) {
            // Don't show these warning messages
            return;
        }

        if (text.startsWith("Place ") && m_accelAckCount != -1) {
            //Instruction
            if (m_accelAckCount == 0) {
                ui.calibrateAccelButton->setText(CONTINUE_BUTTON_TEXT);
            }
            ui.outputLabel->setText(text);
            m_accelAckCount++;

        } else if (text.contains("Calibration successful") || text.contains("SUCCESS: executed CMD: 241")) {
            // Calibration complete success
            if (m_muted) { // turns audio back on, when you complete fail or success
                GAudioOutput::instance()->mute(false);
                m_muted = false;
            }
            ui.coutdownLabel->setText("");
            m_countdownTimer.stop();
            ui.calibrateAccelButton->setText(CALIBRATE_BUTTON_TEXT);
            ui.calibrateAccelButton->clearFocus();
            ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
            MainWindow::instance()->toolBar().startAnimation();
            m_isCalibrating = false;
            m_accelAckCount = -1;

        } else if (text.contains("FAILED") || text.contains("Failed CMD: 241")) {
            //Calibration complete success or failure
            if (m_muted) { // turns audio back on, when you complete fail or success
                GAudioOutput::instance()->mute(false);
                m_muted = false;
            }
            cancelCalibration();
            ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
            MainWindow::instance()->toolBar().startAnimation();
            m_isCalibrating = false;

        } else {
            ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
        }
    }

}
