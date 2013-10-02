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

AccelCalibrationConfig::AccelCalibrationConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_muted(false)
{
    ui.setupUi(this);
    connect(ui.calibrateAccelButton,SIGNAL(clicked()),this,SLOT(calibrateButtonClicked()));
    connect(ui.levelAccelButton,SIGNAL(clicked()),this,SLOT(levelButtonClicked()));

    m_accelAckCount=0;
    m_isLeveling = false;
    initConnections();
}

AccelCalibrationConfig::~AccelCalibrationConfig()
{
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
    if (m_uas->isFixedWing())
    {
        //Show fixed wing level stuff here
        ui.levelAccelButton->setVisible(true);
        ui.levelOutputLabel->setVisible(true);
        ui.levelDescLabel->setVisible(true);
    }
    else if (m_uas->isMultirotor())
    {
        ui.levelAccelButton->setVisible(false);
        ui.levelOutputLabel->setVisible(false);
        ui.levelDescLabel->setVisible(false);
    }
    else if (m_uas->isGroundRover())
    {
        ui.levelAccelButton->setVisible(false);
        ui.levelOutputLabel->setVisible(false);
        ui.levelDescLabel->setVisible(false);
    }
    else
    {
        ui.levelAccelButton->setVisible(false);
        ui.levelOutputLabel->setVisible(false);
        ui.levelDescLabel->setVisible(false);
    }
}

void AccelCalibrationConfig::uasDisconnected()
{

}

void AccelCalibrationConfig::levelButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    // Mute Audio until calibrated to avoid HeartBeat Warning message
    if (GAudioOutput::instance()->isMuted() == false) {
        GAudioOutput::instance()->mute(true);
        m_muted = true;
    }

    MainWindow::instance()->toolBar().stopAnimation();

    MAV_CMD command = MAV_CMD_PREFLIGHT_CALIBRATION;
    int confirm = 0;
    float param1 = 1.0;
    float param2 = 0.0;
    float param3 = 0.0;
    float param4 = 0.0;
    float param5 = 0.0;
    float param6 = 0.0;
    float param7 = 0.0;
    int component = 1;
    m_uas->executeCommand(command, confirm, param1, param2, param3, param4, param5, param6, param7, component);
    if (m_muted) { // turns audio backon, when you leave the page
        GAudioOutput::instance()->mute(false);
        m_muted = false;
    }
    m_isLeveling = true;
}

void AccelCalibrationConfig::calibrateButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    // Mute Audio until calibrated to avoid HeartBeat Warning message
    if (GAudioOutput::instance()->isMuted() == false) {
        GAudioOutput::instance()->mute(true);
        m_muted = true;
    }

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
    }
    else if (m_accelAckCount <= 5)
    {
        m_uas->executeCommandAck(m_accelAckCount++,true);
    }
    else
    {
        m_uas->executeCommandAck(m_accelAckCount++,true);
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
}
void AccelCalibrationConfig::uasTextMessageReceived(int uasid, int componentid, int severity, QString text)
{
    //command received: " Severity 1
    //Place APM Level and press any key" severity 5
    if (m_isLeveling)
    {
        ui.levelOutputLabel->setText(text);
        if (text.toLower().contains("success"))
        {
            m_isLeveling = false;
        }
    }
    if (severity == 5)
    {
        //This is a calibration instruction
        if (m_accelAckCount == 0)
        {
            //Calibration Sucessful\r"
            ui.calibrateAccelButton->setText("Continue");
            m_accelAckCount++;
        }
        if (m_accelAckCount == 7)
        {
            //All finished
            //ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
            ui.outputLabel->setText(text);
            m_accelAckCount++;
        }
        if (m_accelAckCount == 8)
        {
            if (m_muted) { // turns audio backon, when you complete fail or success
                GAudioOutput::instance()->mute(false);
                m_muted = false;
            }

            if (text.contains("Calibration") && text.contains("successful"))
            {
                m_accelAckCount = 0;
            }
            else if (text.contains("Calibration") && text.contains("FAILED")) //Failure
            {
                m_accelAckCount = 0;
            }
            ui.outputLabel->setText(ui.outputLabel->text() + "\n" + text);
            MainWindow::instance()->toolBar().startAnimation();
        }
        else
        {
            ui.outputLabel->setText(text.replace("press any key","click Continue below"));
            if (!this->isVisible())
            {
                //Clear out!
                m_uas->executeCommandAck(m_accelAckCount++,true);
                ui.calibrateAccelButton->setText("Calibrate\nAccelerometer");
            }
        }
    }

}
