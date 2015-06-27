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

AccelCalibrationConfig::AccelCalibrationConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_muted(false),
    m_isCalibrating(false),
    m_countdownCount(CALIBRATION_TIMEOUT_SEC)
{
    ui.setupUi(this);
    connect(ui.calibrateAccelButton,SIGNAL(clicked()),this,SLOT(calibrateButtonClicked()));

    m_accelAckCount=0;
    initConnections();
    //coutdownLabel
    connect(&m_countdownTimer,SIGNAL(timeout()),this,SLOT(countdownTimerTick()));
}

AccelCalibrationConfig::~AccelCalibrationConfig()
{
}
void AccelCalibrationConfig::countdownTimerTick()
{
    ui.coutdownLabel->setText(QString().sprintf(COUNTDOWN_STRING, m_uas->getUASID(), m_countdownCount--));
    if (m_countdownCount <= 0)
    {
        ui.coutdownLabel->setText("Command timed out, please try again");
        m_countdownTimer.stop();
        ui.calibrateAccelButton->setText("Calibrate\nAccelerometer");
        m_accelAckCount = 0;
    }
}

void AccelCalibrationConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(textMessageReceived(int,int,int,QString)),this,SLOT(uasTextMessageReceived(int,int,int,QString)));
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas)
    {
        return;
    }
    connect(m_uas,SIGNAL(textMessageReceived(int,int,int,QString)),this,SLOT(uasTextMessageReceived(int,int,int,QString)));
    connect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    uasConnected();

}
void AccelCalibrationConfig::uasConnected()
{

}

void AccelCalibrationConfig::uasDisconnected()
{

}

void AccelCalibrationConfig::calibrateButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    ui.outputLabel->clear();

    m_isCalibrating = true; // this is to guard against showing unwanted GCS Text Messages.

    // Mute Audio until calibrated to avoid HeartBeat Warning message
    if (GAudioOutput::instance()->isMuted() == false) {
        GAudioOutput::instance()->mute(true);
        m_muted = true;
    }
    m_uas->getLinks()->at(0)->disableTimeouts();

    MainWindow::instance()->toolBar().stopAnimation();

    if (m_accelAckCount == 0)
    {
        MAV_CMD command = MAV_CMD_PREFLIGHT_CALIBRATION;
        int confirm = 0;
        float param1 = 0.0;
        float param2 = 0.0;
        float param3 = 0.0;
        float param4 = 0.0;
        float param5 = 1.0;
        float param6 = 0.0;
        float param7 = 0.0;
        int component = 1;
        m_uas->executeCommand(command, confirm, param1, param2, param3, param4, param5, param6, param7, component);
        m_countdownCount = CALIBRATION_TIMEOUT_SEC;
        ui.coutdownLabel->setText(QString().sprintf(COUNTDOWN_STRING, m_uas->getUASID(), m_countdownCount--));
        m_countdownTimer.start(1000);
    }
    else if (m_accelAckCount <= 10)
    {
        m_uas->executeCommandAck(m_accelAckCount++,true);
        m_countdownCount = CALIBRATION_TIMEOUT_SEC;
    }
    else
    {
        m_uas->executeCommandAck(m_accelAckCount++,true);
        ui.coutdownLabel->setText("");
        m_countdownTimer.stop();
        ui.calibrateAccelButton->setText("Calibrate\nAccelerometer");
        if (m_accelAckCount > 8)
        {
            //We've clicked too many times! Reset.
            for (int i=0;i<8;i++)
            {
                m_uas->executeCommandAck(i,true);
            }
            m_accelAckCount = 0;
        }
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
    for (int i=m_accelAckCount;i<8;i++)
    {
        m_uas->executeCommandAck(i,true); //Clear out extra commands.
    }
    m_uas->getLinks()->at(0)->enableTimeouts();
}
void AccelCalibrationConfig::uasTextMessageReceived(int uasid, int componentid, int severity, QString text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);

    if ((severity == 5 /*SEVERITY_USER_RESPONSE*/)||(severity == 3 /*SEVERITY_HIGH*/))
    {
        //This is a calibration instruction
        if (!m_isCalibrating || text.startsWith("PreArm:") || text.startsWith("EKF") || text.startsWith("Arm"))
        {
            // Don't show these warning messages
            return;
        }

        if (text.contains("Place") && text.contains ("and press any key"))
        {
            //Instruction
            if (m_accelAckCount == 0)
            {
                //Calibration Sucessful\r"
                ui.calibrateAccelButton->setText("Continue\nPress SpaceBar");
                ui.calibrateAccelButton->setShortcut(QKeySequence(Qt::Key_Space));
            }
            ui.outputLabel->setText(text);
            m_accelAckCount++;
        }
        else if (text.contains("Calibration successful") || text.contains("FAILED"))
        {
            //Calibration complete success or failure
            if (m_muted) { // turns audio back on, when you complete fail or success
                GAudioOutput::instance()->mute(false);
                m_muted = false;
            }
            ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
            ui.coutdownLabel->setText("");
            m_countdownTimer.stop();
            MainWindow::instance()->toolBar().startAnimation();
            m_accelAckCount = 0;
            ui.calibrateAccelButton->setText("Calibrate\nAccelerometer");
            ui.calibrateAccelButton->setShortcut(QKeySequence());
            m_isCalibrating = false;
        }
        else
        {
            ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
        }
    }

}
