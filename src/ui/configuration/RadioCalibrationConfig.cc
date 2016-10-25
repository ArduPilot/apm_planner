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
 *   @brief Radio Calibration Configuration source.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *
 */

#include "RadioCalibrationConfig.h"
#include "logging.h"
#include "QGCMouseWheelEventFilter.h"
#include <QMessageBox>
#include <QSettings>

RadioCalibrationConfig::RadioCalibrationConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_pitchChannel(0),
    m_rollChannel(0),
    m_yawChannel(0),
    m_throttleChannel(0),
    m_pitchWidget(NULL),
    m_throttleWidget(NULL),
    m_pitchCheckBox(NULL),
    m_throttleCheckBox(NULL),
    m_rcMode(0)
{
    ui.setupUi(this);

    readSettings();

    connect(ui.calibrateButton,SIGNAL(clicked()),this,SLOT(calibrateButtonClicked()));
    m_calibrationEnabled = false;
    ui.rollWidget->setMin(800);
    ui.rollWidget->setMax(2200);
    ui.leftVWidget->setMin(800);
    ui.leftVWidget->setMax(2200);
    ui.rightVWidget->setMin(800);
    ui.rightVWidget->setMax(2200);
    ui.yawWidget->setMin(800);
    ui.yawWidget->setMax(2200);

    ui.rollWidget->setName(tr("Roll"));
    ui.yawWidget->setName(tr("Yaw"));

    ui.radio5Widget->setMin(800);
    ui.radio5Widget->setMax(2200);
    ui.radio6Widget->setMin(800);
    ui.radio6Widget->setMax(2200);
    ui.radio7Widget->setMin(800);
    ui.radio7Widget->setMax(2200);
    ui.radio8Widget->setMin(800);
    ui.radio8Widget->setMax(2200);

    ui.rollWidget->setOrientation(Qt::Horizontal);
    ui.yawWidget->setOrientation(Qt::Horizontal);

    ui.radio5Widget->setOrientation(Qt::Horizontal);
    ui.radio5Widget->setName("Radio 5");
    ui.radio6Widget->setOrientation(Qt::Horizontal);
    ui.radio6Widget->setName("Radio 6");
    ui.radio7Widget->setOrientation(Qt::Horizontal);
    ui.radio7Widget->setName("Radio 7");
    ui.radio8Widget->setOrientation(Qt::Horizontal);
    ui.radio8Widget->setName("Radio 8");

    guiUpdateTimer = new QTimer(this);
    connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));

    rcMin << 1100.0 << 1100.0 << 1100.0 << 1100.0 << 1100.0 << 1100.0 << 1100.0 << 1100.0;
    rcMax << 1900.0 << 1900.0 << 1900.0 << 1900.0 << 1900.0 << 1900.0 << 1900.0 << 1900.0;
    rcTrim << 1500.0 << 1500.0 << 1500.0 << 1500.0 << 1500.0 << 1500.0 << 1500.0 << 1500.0;
    rcValue << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0 << 0.0;

    ui.revLeftVCheckBox->hide();
    ui.revRollCheckBox->hide();
    ui.revRightVCheckBox->hide();
    ui.revYawCheckBox->hide();
    ui.elevonConfigGroupBox->hide();

    ui.modeComboBox->insertItem(0, "Mode 1", 1);
    ui.modeComboBox->insertItem(1, "Mode 2", 2);
    ui.modeComboBox->insertItem(2, "Mode 3", 3);
    ui.modeComboBox->insertItem(3, "Mode 4", 4);

    // Disable scroll wheel from easily triggering settings change
    ui.modeComboBox->installEventFilter(QGCMouseWheelEventFilter::getFilter());
    ui.modeComboBox->setCurrentIndex(ui.modeComboBox->findData(m_rcMode));
    modeIndexChanged(ui.modeComboBox->currentIndex());

    initConnections();

    connect(ui.revRollCheckBox, SIGNAL(clicked(bool)), this, SLOT(rollClicked(bool)));
    connect(ui.revYawCheckBox, SIGNAL(clicked(bool)), this, SLOT(yawClicked(bool)));

    connect(ui.elevonCheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsChecked(bool)));
    connect(ui.elevonRevCheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsReversed(bool)));
    connect(ui.elevonCh1CheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsCh1Rev(bool)));
    connect(ui.elevonCh2CheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsCh2Rev(bool)));

    ui.elevonOutputComboBox->addItem("Disabled");
    ui.elevonOutputComboBox->addItem("Up Up");
    ui.elevonOutputComboBox->addItem("Up Down");
    ui.elevonOutputComboBox->addItem("Down Up");
    ui.elevonOutputComboBox->addItem("Down Down");
    ui.elevonOutputComboBox->addItem("Up Up Swap");
    ui.elevonOutputComboBox->addItem("Up Down Swap");
    ui.elevonOutputComboBox->addItem("Down Up Swap");
    ui.elevonOutputComboBox->addItem("Down Down Swap");
    connect(ui.elevonOutputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(elevonOutput()));

    connect(ui.modeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modeIndexChanged(int)));
}

RadioCalibrationConfig::~RadioCalibrationConfig()
{
}
void RadioCalibrationConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas, SIGNAL(remoteControlChannelRawChanged(int,float)), this,SLOT(remoteControlChannelRawChanged(int,float)));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas)
    {
        return;
    }
    connect(m_uas,SIGNAL(remoteControlChannelRawChanged(int,float)),this,SLOT(remoteControlChannelRawChanged(int,float)));

    if (m_uas->isFixedWing() || m_uas->isGroundRover()){
        ui.revLeftVCheckBox->show();
        ui.revRollCheckBox->show();
        ui.revRightVCheckBox->show();
        ui.revYawCheckBox->show();
        ui.elevonConfigGroupBox->show();
    } else {
        ui.revLeftVCheckBox->hide();
        ui.revRollCheckBox->hide();
        ui.revRightVCheckBox->hide();
        ui.revYawCheckBox->hide();
        ui.elevonConfigGroupBox->hide();
    }
}
void RadioCalibrationConfig::remoteControlChannelRawChanged(int chan,float val)
{

    //Channel is 0-7 typically?
    //Val will be 0-3000, PWM value.
    if (m_calibrationEnabled) {
        if (val < rcMin[chan])
        {
            rcMin[chan] = val;
        }

        if (val > rcMax[chan])
        {
            rcMax[chan] = val;
        }
    }

    // Raw value
    rcValue[chan] = val;
}

void RadioCalibrationConfig::modeIndexChanged(int index)
{
    Q_UNUSED(index);

    if(m_pitchCheckBox || m_throttleCheckBox){
        disconnect(m_pitchCheckBox, SIGNAL(clicked(bool)), this, SLOT(pitchClicked(bool)));
        disconnect(m_throttleCheckBox, SIGNAL(clicked(bool)), this, SLOT(throttleClicked(bool)));
    }

    if((ui.modeComboBox->currentData() == 1)
        ||(ui.modeComboBox->currentData() == 3)){
        // Mode 1 & 3 (Throttle on right) (Pitch/Aileron left)
        m_throttleWidget = ui.rightVWidget;
        m_throttleCheckBox = ui.revRightVCheckBox;
        m_pitchWidget = ui.leftVWidget;
        m_pitchCheckBox = ui.revLeftVCheckBox;
    } else {
        // Mode 2 & 4 (Throttle on left) (Pitch/Aileron right)
        m_throttleWidget = ui.leftVWidget;
        m_throttleCheckBox = ui.revLeftVCheckBox;
        m_pitchWidget = ui.rightVWidget;
        m_pitchCheckBox = ui.revRightVCheckBox;
    }
    if ((m_pitchChannel != 0 ) && (m_pitchChannel != 0 )){
        m_pitchWidget->setName(tr("%1-Pitch").arg(m_pitchChannel));
        m_throttleWidget->setName(tr("%1-Throttle").arg(m_throttleChannel));
    } else {
        m_pitchWidget->setName(tr("Pitch"));
        m_throttleWidget->setName(tr("Throttle"));
    }

    connect(m_pitchCheckBox, SIGNAL(clicked(bool)), this, SLOT(pitchClicked(bool)));
    connect(m_throttleCheckBox, SIGNAL(clicked(bool)), this, SLOT(throttleClicked(bool)));

    m_rcMode = ui.modeComboBox->currentData().toInt();
    writeSettings();
}

void RadioCalibrationConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if(parameterName.startsWith("RCMAP_PITCH")){
        m_pitchChannel = value.toInt();
        m_pitchWidget->setName(tr("%1-Pitch").arg(m_pitchChannel));
        return;

    } else if(parameterName.startsWith("RCMAP_ROLL")){
        m_rollChannel = value.toInt();
        ui.rollWidget->setName(tr("%1-Roll").arg(value.toString()));
        return;

    } else if(parameterName.startsWith("RCMAP_YAW")){
        m_yawChannel = value.toInt();
        ui.yawWidget->setName(tr("%1-Yaw").arg(value.toString()));
        return;

    } else if(parameterName.startsWith("RCMAP_THROTTLE")){
        m_throttleChannel = value.toInt();
        m_throttleWidget->setName(tr("%1-Throttle").arg(m_throttleChannel));
        return;
    }

    if (parameterName.startsWith("ELEVON_MIXING")){
        ui.elevonCheckBox->setChecked(value.toBool());
    } else if (parameterName.startsWith("ELEVON_REVERSE")){
        ui.elevonRevCheckBox->setChecked(value.toBool());
    } else if (parameterName.startsWith("ELEVON_CH1_REV")){
        ui.elevonCh1CheckBox->setChecked(value.toBool());
    } else if (parameterName.startsWith("ELEVON_CH2_REV")){
        ui.elevonCh2CheckBox->setChecked(value.toBool());
    } else if (parameterName.startsWith("ELEVON_OUTPUT")){
        ui.elevonOutputComboBox->setCurrentIndex(value.toInt());
    }
}

void RadioCalibrationConfig::updateChannelReversalStates()
{
    if(m_uas == NULL)
        return;
    // Update Pitch Reverse Channel
    updateChannelRevState(m_pitchCheckBox, m_pitchChannel);

    // Update Roll Reverse Channel
    updateChannelRevState(ui.revRollCheckBox, m_rollChannel);

    // Update Yaw Reverse Channel
    updateChannelRevState(ui.revYawCheckBox, m_yawChannel);

    // Update Throttle Reverse Channel
    updateChannelRevState(m_throttleCheckBox, m_throttleChannel);
}

void RadioCalibrationConfig::updateChannelRevState(QCheckBox* checkbox, int channelId)
{
    int reverse = m_uas->getParamManager()->getParameterValue(1, "RC" + QString::number(channelId)
                                                             + "_REV").toInt();
    if (reverse == -1) {
        checkbox->setChecked(true);
    } else {
        checkbox->setChecked(false);
    }
}

void RadioCalibrationConfig::guiUpdateTimerTick()
{
    if (m_rollChannel==0 || m_pitchChannel==0 || m_throttleChannel==0 || m_yawChannel==0){
        // If not all parameters have been downloaded, don't update RC channels
        // which results in out-of-range array access.
        return;
    }

    ui.rollWidget->setValue(rcValue[m_rollChannel-1]);
    m_pitchWidget->setValue(rcValue[m_pitchChannel-1]);
    m_throttleWidget->setValue(rcValue[m_throttleChannel-1]);
    ui.yawWidget->setValue(rcValue[m_yawChannel-1]);
    ui.radio5Widget->setValue(rcValue[4]);
    ui.radio6Widget->setValue(rcValue[5]);
    ui.radio7Widget->setValue(rcValue[6]);
    ui.radio8Widget->setValue(rcValue[7]);
    if (m_calibrationEnabled)
    {
        ui.rollWidget->setMin(rcMin[m_rollChannel-1]);
        ui.rollWidget->setMax(rcMax[m_rollChannel-1]);
        m_pitchWidget->setMin(rcMin[m_pitchChannel-1]);
        m_pitchWidget->setMax(rcMax[m_pitchChannel-1]);
        m_throttleWidget->setMin(rcMin[m_throttleChannel-1]);
        m_throttleWidget->setMax(rcMax[m_throttleChannel-1]);
        ui.yawWidget->setMin(rcMin[m_yawChannel-1]);
        ui.yawWidget->setMax(rcMax[m_yawChannel-1]);
        ui.radio5Widget->setMin(rcMin[4]);
        ui.radio5Widget->setMax(rcMax[4]);
        ui.radio6Widget->setMin(rcMin[5]);
        ui.radio6Widget->setMax(rcMax[5]);
        ui.radio7Widget->setMin(rcMin[6]);
        ui.radio7Widget->setMax(rcMax[6]);
        ui.radio8Widget->setMin(rcMin[7]);
        ui.radio8Widget->setMax(rcMax[7]);
    }

    updateChannelReversalStates();
}
void RadioCalibrationConfig::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    guiUpdateTimer->start(100);
}
void RadioCalibrationConfig::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    guiUpdateTimer->stop();
}
void RadioCalibrationConfig::calibrateButtonClicked()
{
    if (!isRadioControlActive()){
        QMessageBox::warning(this,tr("Radio Control"), tr("Radio Control is not active or turned on"));
        return;
    }

    if (!m_calibrationEnabled)
    {
        if (QMessageBox::question(this,"Warning!","You are about to start radio calibration.\nPlease ensure all motor power is disconnected AND all props are removed from the vehicle.\nAlso ensure transmitter and reciever are powered and connected\n\nClick OK to confirm, or cancel to abort radio calibration",QMessageBox::Ok,QMessageBox::Cancel) != QMessageBox::Ok)
        {
            QMessageBox::information(this,"Warning!","Radio calibration aborted");
            return;
        }
        ui.calibrateButton->setText("End Calibration");
        m_calibrationEnabled = true;
        for (int i=0;i<RC_CHANNEL_NUM_MAX;i++)
        {
            rcMin[i] = 1500;
            rcMax[i] = 1500;
        }
        ui.rollWidget->showMinMax();
        m_pitchWidget->showMinMax();
        ui.yawWidget->showMinMax();
        ui.radio5Widget->showMinMax();
        ui.radio6Widget->showMinMax();
        ui.radio7Widget->showMinMax();
        m_throttleWidget->showMinMax();
        ui.radio8Widget->showMinMax();
        QMessageBox::information(this,"Information","Click OK, then move all sticks to their extreme positions, watching the min/max values to ensure you get the most range from your controller. This includes all switches");
    }
    else
    {
        ui.calibrateButton->setText("Calibrate");
        QMessageBox::information(this,"Trims","Ensure all sticks are centered and throttle is in the downmost position, click OK to continue");

        m_calibrationEnabled = false;
        ui.rollWidget->hideMinMax();
        m_pitchWidget->hideMinMax();
        ui.yawWidget->hideMinMax();
        ui.radio5Widget->hideMinMax();
        ui.radio6Widget->hideMinMax();
        m_throttleWidget->hideMinMax();
        ui.radio7Widget->hideMinMax();
        ui.radio8Widget->hideMinMax();

        //Send calibrations.
        QString minTpl("RC%1_MIN");
        QString maxTpl("RC%1_MAX");
        QString trimTpl("RC%1_TRIM");

        QString statusstr;
        statusstr = "Below you will find the detected radio calibration information\n";
        statusstr += "Normal values are between 1100 to 1900, trim close to 1500\n\n";
        statusstr += "Channel\tMin\tCenter\tMax\n";
        statusstr += "--------------------\n";

        for (int i=0;i< RC_CHANNEL_NUM_MAX;i++)
        {
            statusstr += QString::number(i+1) + "\t" + QString::number(rcMin[i]) + "\t" + QString::number(rcValue[i]) + "\t" + QString::number(rcMax[i]) + "\n";
        }

        if (validRadioSettings()){
            for (int i=0;i< RC_CHANNEL_NUM_MAX;i++)
            {
                QLOG_DEBUG() << "SENDING MIN" << minTpl.arg(i+1) << rcMin[i];
                QLOG_DEBUG() << "SENDING TRIM" << trimTpl.arg(i+1) << rcValue[i];
                QLOG_DEBUG() << "SENDING MAX" << maxTpl.arg(i+1) << rcMax[i];

                // Send Calibrations
                m_uas->getParamManager()->setParameter(1, minTpl.arg(i+1), rcMin[i]);
                m_uas->getParamManager()->setParameter(1, trimTpl.arg(i+1), rcValue[i]); // Save the Trim Values.
                m_uas->getParamManager()->setParameter(1, maxTpl.arg(i+1), rcMax[i]);
            }

            QMessageBox::information(this,"Status",statusstr); // Show Calibraitions to the user
        } else {
            QMessageBox::warning(this,"Status","FAILED: Invalid PWM signals\n" + statusstr);
        }


        ui.rollWidget->setMin(800);
        ui.rollWidget->setMax(2200);
        m_pitchWidget->setMin(800);
        m_pitchWidget->setMax(2200);
        m_throttleWidget->setMin(800);
        m_throttleWidget->setMax(2200);
        ui.yawWidget->setMin(800);
        ui.yawWidget->setMax(2200);
        ui.radio5Widget->setMin(800);
        ui.radio5Widget->setMax(2200);
        ui.radio6Widget->setMin(800);
        ui.radio6Widget->setMax(2200);
        ui.radio7Widget->setMin(800);
        ui.radio7Widget->setMax(2200);
        ui.radio8Widget->setMin(800);
        ui.radio8Widget->setMax(2200);

    }
}

void RadioCalibrationConfig::pitchClicked(bool state)
{
    setParamChannelRev("RCMAP_PITCH", state);
}

void RadioCalibrationConfig::rollClicked(bool state)
{
    setParamChannelRev("RCMAP_ROLL", state);
}

void RadioCalibrationConfig::yawClicked(bool state)
{
    setParamChannelRev("RCMAP_YAW",state);
}

void RadioCalibrationConfig::throttleClicked(bool state)
{
    setParamChannelRev("RCMAP_THROTTLE", state);
}

void RadioCalibrationConfig::setParamChannelRev(const QString& param, bool state)
{
    if(m_uas){
        int channel = m_uas->getParamManager()->getParameterValue(1, param).toInt();
        QString channelString = QString("RC" + QString::number(channel) + "_REV");
        if (state)
            m_uas->setParameter(1, channelString, -1.0);
        else
            m_uas->setParameter(1, channelString, 1.0); // We use 0 as the default, not 1.0 (which you can also use)
    }
}

void RadioCalibrationConfig::elevonsChecked(bool state)
{
    if(m_uas){
        if (state)
            m_uas->setParameter(1, "ELEVON_MIXING", 1.0);
        else
            m_uas->setParameter(1, "ELEVON_MIXING", 0.0);
        }
}

void RadioCalibrationConfig::elevonsReversed(bool state)
{
    if(m_uas){
        if (state)
            m_uas->setParameter(1, "ELEVON_REVERSE", 1.0);
        else
            m_uas->setParameter(1, "ELEVON_REVERSE", 0.0);
        }
}

void RadioCalibrationConfig::elevonsCh1Rev(bool state)
{
    setParamChannelRev("ELEVON_CH1_REV", state);
}

void RadioCalibrationConfig::elevonsCh2Rev(bool state)
{
    setParamChannelRev("ELEVON_CH2_REV", state);
}

void RadioCalibrationConfig::elevonOutput()
{
    if(m_uas){
        m_uas->setParameter(1, "ELEVON_OUTPUT", ui.elevonOutputComboBox->currentIndex());
    }
}

void RadioCalibrationConfig::readSettings()
{
    QSettings settings;
    settings.beginGroup("RCCALIBRATION_VIEW");
    m_rcMode = settings.value("RC_MODE",2).toInt();
    settings.endGroup();
}

void RadioCalibrationConfig::writeSettings()
{
    QSettings settings;
    settings.beginGroup("RCCALIBRATION_VIEW");
    settings.setValue("RC_MODE",m_rcMode);
    settings.endGroup();
    settings.sync();
}

bool RadioCalibrationConfig::isRadioControlActive()
{
    // Check the lower 4 channels are active for radio connected.
    for(int count=0; count < RC_CHANNEL_LOWER_CONTROL_CH_MAX; count++){
        // Any invalid range and we abort.
        if (!isInRange(rcValue[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)){
            QLOG_ERROR() << QString().sprintf("isRadioControlActive: Error Channel %d out of range: rcValue=%f",
                                              count+1, rcValue[count]);
            return false;
        }
    }

    for(int count=RC_CHANNEL_LOWER_CONTROL_CH_MAX; count < RC_CHANNEL_NUM_MAX; count++){
        if ((rcValue[count]>0.0)){ // Only active channels are validated.
            if (!isInRange(rcValue[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)){
                QLOG_ERROR() << QString().sprintf("isRadioControlActive: Error Channel %d out of range: rcValue=%f",
                                                  count+1, rcValue[count]);
                return false;
            }
        }
    }
    return true;
}

bool RadioCalibrationConfig::validRadioSettings()
{
    // Check lower 4 channels have been set correctly zero values not allowed.
    // i.e. Aileron (Roll), Elevator (Pitch), Throttle, Rudder (Yaw)
    for(int count=0; count< RC_CHANNEL_LOWER_CONTROL_CH_MAX; count++){
        // Any invalid range and we abort.
        if (!isInRange(rcMin[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)
                ||!isInRange(rcMax[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)
                ||!isInRange(rcTrim[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)){
            QLOG_ERROR() << QString().sprintf("validRadioSettings: Error Channel %d out of range: rcMin=%f rcMax=%f rcTrim=%f",
                                              count+1, rcMin[count], rcMax[count], rcTrim[count]);
            return false;
        }
    }

    // for channels other than the lower 4 we verify only if non-zero.
    for(int count=RC_CHANNEL_LOWER_CONTROL_CH_MAX; count< RC_CHANNEL_NUM_MAX; count++){
        // Only check if we have received a non-zero value on the channel
        // that the settings are valid.
        if ((rcValue[count]>0.0)){
            // Any invalid range and we abort.
            if (!isInRange(rcMin[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)
                ||!isInRange(rcMax[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)
                ||!isInRange(rcTrim[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)){
                QLOG_ERROR() << QString().sprintf("validRadioSettings: Error Channel %d out of range: rcMin=%f rcMax=%f rcTrim=%f",
                                                  count+1, rcMin[count], rcMax[count], rcTrim[count]);
                return false;
            }
        }
    }

    return true;
}

bool RadioCalibrationConfig::isInRange(double value, double min, double max)
{
    if ((value > min)&&(value<max)){
        return true;
    }
    return false;
}




