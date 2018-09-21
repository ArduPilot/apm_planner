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
#include "ui_RadioCalibrationConfig.h"
#include "logging.h"
#include "QGCMouseWheelEventFilter.h"
#include <QMessageBox>
#include <QSettings>

RadioCalibrationConfig::RadioCalibrationConfig(QWidget *parent) : AP2ConfigWidget(parent),
    ui(new Ui::RadioCalibrationConfig),
    guiUpdateTimer(nullptr),
    m_calibrationEnabled(false),
    m_pitchChannel(0),
    m_rollChannel(0),
    m_yawChannel(0),
    m_throttleChannel(0),
    m_pitchWidget(nullptr),
    m_throttleWidget(nullptr),
    m_pitchCheckBox(nullptr),
    m_throttleCheckBox(nullptr),
    m_rcMode(0)
{
    ui->setupUi(this);

    readSettings();

    ui->rollWidget->setMin(800);
    ui->rollWidget->setMax(2200);
    ui->leftVWidget->setMin(800);
    ui->leftVWidget->setMax(2200);
    ui->rightVWidget->setMin(800);
    ui->rightVWidget->setMax(2200);
    ui->yawWidget->setMin(800);
    ui->yawWidget->setMax(2200);

    ui->rollWidget->setName(tr("Roll"));
    ui->yawWidget->setName(tr("Yaw"));

    ui->rollWidget->setOrientation(Qt::Horizontal);
    ui->yawWidget->setOrientation(Qt::Horizontal);

    ui->radio5Widget->setMin(800);
    ui->radio5Widget->setMax(2200);
    ui->radio6Widget->setMin(800);
    ui->radio6Widget->setMax(2200);
    ui->radio7Widget->setMin(800);
    ui->radio7Widget->setMax(2200);
    ui->radio8Widget->setMin(800);
    ui->radio8Widget->setMax(2200);
    ui->radio9Widget->setMin(800);
    ui->radio9Widget->setMax(2200);
    ui->radio10Widget->setMin(800);
    ui->radio10Widget->setMax(2200);
    ui->radio11Widget->setMin(800);
    ui->radio11Widget->setMax(2200);
    ui->radio12Widget->setMin(800);
    ui->radio12Widget->setMax(2200);
    ui->radio13Widget->setMin(800);
    ui->radio13Widget->setMax(2200);
    ui->radio14Widget->setMin(800);
    ui->radio14Widget->setMax(2200);
    ui->radio15Widget->setMin(800);
    ui->radio15Widget->setMax(2200);
    ui->radio16Widget->setMin(800);
    ui->radio16Widget->setMax(2200);

    ui->radio5Widget->setName("Ch 5");
    ui->radio6Widget->setName("Ch 6");
    ui->radio7Widget->setName("Ch 7");
    ui->radio8Widget->setName("Ch 8");
    ui->radio9Widget->setName("Ch 9");
    ui->radio10Widget->setName("Ch 10");
    ui->radio11Widget->setName("Ch 11");
    ui->radio12Widget->setName("Ch 12");
    ui->radio13Widget->setName("Ch 13");
    ui->radio14Widget->setName("Ch 14");
    ui->radio15Widget->setName("Ch 15");
    ui->radio16Widget->setName("Ch 16");

    guiUpdateTimer = new QTimer(this);
    connect(guiUpdateTimer,SIGNAL(timeout()),this,SLOT(guiUpdateTimerTick()));

    rcMin.fill(801, RC_CHANNEL_NUM_MAX);    // 800 + 1 -> 800 is minimum
    rcMax.fill(2199, RC_CHANNEL_NUM_MAX);   // 2200 - 1 -> 2200 is maximum
    rcValue.fill(0, RC_CHANNEL_NUM_MAX);
    rcDeadzone.fill(0, RC_CHANNEL_LOWER_CONTROL_CH_MAX);

    ui->revLeftVCheckBox->hide();
    ui->revRollCheckBox->hide();
    ui->revRightVCheckBox->hide();
    ui->revYawCheckBox->hide();
    ui->elevonConfigGroupBox->hide();

    ui->modeComboBox->insertItem(0, "Mode 1", 1);
    ui->modeComboBox->insertItem(1, "Mode 2", 2);
    ui->modeComboBox->insertItem(2, "Mode 3", 3);
    ui->modeComboBox->insertItem(3, "Mode 4", 4);
    connect(ui->modeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(modeIndexChanged(int)));

    // Disable scroll wheel from easily triggering settings change
    ui->modeComboBox->installEventFilter(QGCMouseWheelEventFilter::getFilter());
    ui->modeComboBox->setCurrentIndex(ui->modeComboBox->findData(m_rcMode));
    modeIndexChanged(ui->modeComboBox->currentIndex());

    initConnections();

    connect(ui->calibrateButton,SIGNAL(clicked()),this,SLOT(calibrateButtonClicked()));

    connect(ui->revRollCheckBox, SIGNAL(clicked(bool)), this, SLOT(rollClicked(bool)));
    connect(ui->revYawCheckBox, SIGNAL(clicked(bool)), this, SLOT(yawClicked(bool)));

    connect(ui->elevonCheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsChecked(bool)));
    connect(ui->elevonRevCheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsReversed(bool)));
    connect(ui->elevonCh1CheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsCh1Rev(bool)));
    connect(ui->elevonCh2CheckBox, SIGNAL(clicked(bool)), this, SLOT(elevonsCh2Rev(bool)));

    ui->elevonOutputComboBox->addItem("Disabled");
    ui->elevonOutputComboBox->addItem("Up Up");
    ui->elevonOutputComboBox->addItem("Up Down");
    ui->elevonOutputComboBox->addItem("Down Up");
    ui->elevonOutputComboBox->addItem("Down Down");
    ui->elevonOutputComboBox->addItem("Up Up Swap");
    ui->elevonOutputComboBox->addItem("Up Down Swap");
    ui->elevonOutputComboBox->addItem("Down Up Swap");
    ui->elevonOutputComboBox->addItem("Down Down Swap");
    connect(ui->elevonOutputComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(elevonOutput()));

    connect(ui->pitchDZ_spinBox, SIGNAL(valueChanged(int)), this, SLOT(pitchDZChanged(int)));
    connect(ui->rollDZ_spinBox, SIGNAL(valueChanged(int)), this, SLOT(rollDZChanged(int)));
    connect(ui->throtDZ_spinBox, SIGNAL(valueChanged(int)), this, SLOT(throtDZChanged(int)));
    connect(ui->yawDZ_spinBox, SIGNAL(valueChanged(int)), this, SLOT(yawDZChanged(int)));

    connect(ui->writeDZ_Button, &QPushButton::clicked, this, &RadioCalibrationConfig::writeDZButtonPressed);
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

    if (m_uas->isFixedWing() || m_uas->isGroundRover())
    {
        ui->revLeftVCheckBox->show();
        ui->revRollCheckBox->show();
        ui->revRightVCheckBox->show();
        ui->revYawCheckBox->show();
        ui->elevonConfigGroupBox->show();
    }
    else
    {
        ui->revLeftVCheckBox->hide();
        ui->revRollCheckBox->hide();
        ui->revRightVCheckBox->hide();
        ui->revYawCheckBox->hide();
        ui->elevonConfigGroupBox->hide();
    }
}

void RadioCalibrationConfig::remoteControlChannelRawChanged(int chan, float val)
{
    //Val will be 0-3000, PWM value.
    if((chan < 0) || (chan >= RC_CHANNEL_NUM_MAX))
    {
        return;
    }

    if (m_calibrationEnabled)
    {
        if (val < rcMin[chan])
        {
            rcMin[chan] = static_cast<qint32>(val);
        }

        if (val > rcMax[chan])
        {
            rcMax[chan] = static_cast<qint32>(val);
        }
    }
    // Raw value
    rcValue[chan] = static_cast<qint32>(val);
}

void RadioCalibrationConfig::modeIndexChanged(int index)
{
    Q_UNUSED(index);

    if(m_pitchCheckBox || m_throttleCheckBox){
        disconnect(m_pitchCheckBox, SIGNAL(clicked(bool)), this, SLOT(pitchClicked(bool)));
        disconnect(m_throttleCheckBox, SIGNAL(clicked(bool)), this, SLOT(throttleClicked(bool)));
    }

    if((ui->modeComboBox->currentData() == 1)||(ui->modeComboBox->currentData() == 3))
    {
        // Mode 1 & 3 (Throttle on right) (Pitch/Aileron left)
        m_throttleWidget = ui->rightVWidget;
        m_throttleCheckBox = ui->revRightVCheckBox;
        m_pitchWidget = ui->leftVWidget;
        m_pitchCheckBox = ui->revLeftVCheckBox;
    }
    else
    {
        // Mode 2 & 4 (Throttle on left) (Pitch/Aileron right)
        m_throttleWidget = ui->leftVWidget;
        m_throttleCheckBox = ui->revLeftVCheckBox;
        m_pitchWidget = ui->rightVWidget;
        m_pitchCheckBox = ui->revRightVCheckBox;
    }

    if ((m_pitchChannel != 0 ) && (m_throttleChannel != 0 ))
    {
        m_pitchWidget->setName(tr("%1-Pitch").arg(m_pitchChannel));
        m_throttleWidget->setName(tr("%1-Throt").arg(m_throttleChannel));
    }
    else
    {
        m_pitchWidget->setName(tr("Pitch"));
        m_throttleWidget->setName(tr("Throt"));
    }

    if((m_pitchChannel != 0 ) && (m_throttleChannel != 0 ) && (m_rollChannel != 0) && (m_yawChannel != 0))
    {
        ui->rollDZ_spinBox->setValue(rcDeadzone[m_rollChannel - 1]);
        ui->pitchDZ_spinBox->setValue(rcDeadzone[m_pitchChannel - 1]);
        ui->throtDZ_spinBox->setValue(rcDeadzone[m_throttleChannel - 1]);
        ui->yawDZ_spinBox->setValue(rcDeadzone[m_yawChannel - 1]);
    }

    connect(m_pitchCheckBox, SIGNAL(clicked(bool)), this, SLOT(pitchClicked(bool)));
    connect(m_throttleCheckBox, SIGNAL(clicked(bool)), this, SLOT(throttleClicked(bool)));

    m_rcMode = ui->modeComboBox->currentData().toInt();
    writeSettings();
}

void RadioCalibrationConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if(parameterName.startsWith("RC")) // make all non RC params fast
    {
        if(parameterName.startsWith("RCMAP_PITCH"))
        {
            m_pitchChannel = value.toInt();
            m_pitchWidget->setName(tr("%1-Pitch").arg(m_pitchChannel));
        }
        else if(parameterName.startsWith("RCMAP_ROLL"))
        {
            m_rollChannel = value.toInt();
            ui->rollWidget->setName(tr("%1-Roll").arg(value.toString()));
        }
        else if(parameterName.startsWith("RCMAP_YAW"))
        {
            m_yawChannel = value.toInt();
            ui->yawWidget->setName(tr("%1-Yaw").arg(value.toString()));
        }
        else if(parameterName.startsWith("RCMAP_THROTTLE"))
        {
            m_throttleChannel = value.toInt();
            m_throttleWidget->setName(tr("%1-Throt").arg(m_throttleChannel));
        }
        else if(parameterName.startsWith("RC1_DZ"))
        {
            rcDeadzone[0] = value.toInt();
        }
        else if(parameterName.startsWith("RC2_DZ"))
        {
            rcDeadzone[1] = value.toInt();
        }
        else if(parameterName.startsWith("RC3_DZ"))
        {
            rcDeadzone[2] = value.toInt();
        }
        else if(parameterName.startsWith("RC4_DZ"))
        {
            rcDeadzone[3] = value.toInt();
        }

        if((m_pitchChannel != 0 ) && (m_throttleChannel != 0 ) && (m_rollChannel != 0) && (m_yawChannel != 0))
        {
            ui->rollDZ_spinBox->setValue(rcDeadzone[m_rollChannel - 1]);
            ui->pitchDZ_spinBox->setValue(rcDeadzone[m_pitchChannel - 1]);
            ui->throtDZ_spinBox->setValue(rcDeadzone[m_throttleChannel - 1]);
            ui->yawDZ_spinBox->setValue(rcDeadzone[m_yawChannel - 1]);
        }
    }
    else if(parameterName.startsWith("ELE"))    // make all non ELEVON params fast
    {
        if (parameterName.startsWith("ELEVON_MIXING"))
        {
            ui->elevonCheckBox->setChecked(value.toBool());
        }
        else if (parameterName.startsWith("ELEVON_REVERSE"))
        {
            ui->elevonRevCheckBox->setChecked(value.toBool());
        }
        else if (parameterName.startsWith("ELEVON_CH1_REV"))
        {
            ui->elevonCh1CheckBox->setChecked(value.toBool());
        }
        else if (parameterName.startsWith("ELEVON_CH2_REV"))
        {
            ui->elevonCh2CheckBox->setChecked(value.toBool());
        }
        else if (parameterName.startsWith("ELEVON_OUTPUT"))
        {
            ui->elevonOutputComboBox->setCurrentIndex(value.toInt());
        }
    }

}

void RadioCalibrationConfig::updateChannelReversalStates()
{
    if(m_uas == nullptr)
    {
        return;
    }

    // Update Pitch Reverse Channel
    updateChannelRevState(m_pitchCheckBox, m_pitchChannel);
    // Update Roll Reverse Channel
    updateChannelRevState(ui->revRollCheckBox, m_rollChannel);
    // Update Yaw Reverse Channel
    updateChannelRevState(ui->revYawCheckBox, m_yawChannel);
    // Update Throttle Reverse Channel
    updateChannelRevState(m_throttleCheckBox, m_throttleChannel);
}

void RadioCalibrationConfig::updateChannelRevState(QCheckBox* checkbox, int channelId)
{
    int reverse = m_uas->getParamManager()->getParameterValue(1, "RC" + QString::number(channelId) + "_REV").toInt();
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

    ui->rollWidget->setValue(rcValue[m_rollChannel-1]);
    m_pitchWidget->setValue(rcValue[m_pitchChannel-1]);
    m_throttleWidget->setValue(rcValue[m_throttleChannel-1]);
    ui->yawWidget->setValue(rcValue[m_yawChannel-1]);
    ui->radio5Widget->setValue(rcValue[4]);
    ui->radio6Widget->setValue(rcValue[5]);
    ui->radio7Widget->setValue(rcValue[6]);
    ui->radio8Widget->setValue(rcValue[7]);
    ui->radio9Widget->setValue(rcValue[8]);
    ui->radio10Widget->setValue(rcValue[9]);
    ui->radio11Widget->setValue(rcValue[10]);
    ui->radio12Widget->setValue(rcValue[11]);
    ui->radio13Widget->setValue(rcValue[12]);
    ui->radio14Widget->setValue(rcValue[13]);
    ui->radio15Widget->setValue(rcValue[14]);
    ui->radio16Widget->setValue(rcValue[15]);

    if (m_calibrationEnabled)
    {
        ui->rollWidget->setMin(rcMin[m_rollChannel-1]);
        ui->rollWidget->setMax(rcMax[m_rollChannel-1]);
        m_pitchWidget->setMin(rcMin[m_pitchChannel-1]);
        m_pitchWidget->setMax(rcMax[m_pitchChannel-1]);
        m_throttleWidget->setMin(rcMin[m_throttleChannel-1]);
        m_throttleWidget->setMax(rcMax[m_throttleChannel-1]);
        ui->yawWidget->setMin(rcMin[m_yawChannel-1]);
        ui->yawWidget->setMax(rcMax[m_yawChannel-1]);
        ui->radio5Widget->setMin(rcMin[4]);
        ui->radio5Widget->setMax(rcMax[4]);
        ui->radio6Widget->setMin(rcMin[5]);
        ui->radio6Widget->setMax(rcMax[5]);
        ui->radio7Widget->setMin(rcMin[6]);
        ui->radio7Widget->setMax(rcMax[6]);
        ui->radio8Widget->setMin(rcMin[7]);
        ui->radio8Widget->setMax(rcMax[7]);
        ui->radio9Widget->setMin(rcMin[8]);
        ui->radio9Widget->setMax(rcMax[8]);
        ui->radio10Widget->setMin(rcMin[9]);
        ui->radio10Widget->setMax(rcMax[9]);
        ui->radio11Widget->setMin(rcMin[10]);
        ui->radio11Widget->setMax(rcMax[10]);
        ui->radio12Widget->setMin(rcMin[11]);
        ui->radio12Widget->setMax(rcMax[11]);
        ui->radio13Widget->setMin(rcMin[12]);
        ui->radio13Widget->setMax(rcMax[12]);
        ui->radio14Widget->setMin(rcMin[13]);
        ui->radio14Widget->setMax(rcMax[13]);
        ui->radio15Widget->setMin(rcMin[14]);
        ui->radio15Widget->setMax(rcMax[14]);
        ui->radio16Widget->setMin(rcMin[15]);
        ui->radio16Widget->setMax(rcMax[15]);
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
        ui->calibrateButton->setText("End Calibration");
        m_calibrationEnabled = true;

        // reset min max values
        rcMax.fill(0, RC_CHANNEL_NUM_MAX);
        rcMin.fill(3000, RC_CHANNEL_NUM_MAX);

        ui->rollWidget->showMinMax();
        m_pitchWidget->showMinMax();
        ui->yawWidget->showMinMax();
        m_throttleWidget->showMinMax();
        ui->radio5Widget->showMinMax();
        ui->radio6Widget->showMinMax();
        ui->radio7Widget->showMinMax();
        ui->radio8Widget->showMinMax();
        ui->radio9Widget->showMinMax();
        ui->radio10Widget->showMinMax();
        ui->radio11Widget->showMinMax();
        ui->radio12Widget->showMinMax();
        ui->radio13Widget->showMinMax();
        ui->radio14Widget->showMinMax();
        ui->radio15Widget->showMinMax();
        ui->radio16Widget->showMinMax();

        QMessageBox::information(this,"Information","Click OK, then move all sticks to their extreme positions, watching the min/max values to ensure you get the most range from your controller. This includes all switches");
    }
    else
    {
        ui->calibrateButton->setText("Calibrate");
        QMessageBox::information(this,"Trims","Ensure all sticks are centered and throttle is in the downmost position, click OK to continue");

        m_calibrationEnabled = false;
        ui->rollWidget->hideMinMax();
        m_pitchWidget->hideMinMax();
        ui->yawWidget->hideMinMax();
        m_throttleWidget->hideMinMax();
        ui->radio5Widget->hideMinMax();
        ui->radio6Widget->hideMinMax();
        ui->radio7Widget->hideMinMax();
        ui->radio8Widget->hideMinMax();
        ui->radio9Widget->hideMinMax();
        ui->radio10Widget->hideMinMax();
        ui->radio11Widget->hideMinMax();
        ui->radio12Widget->hideMinMax();
        ui->radio13Widget->hideMinMax();
        ui->radio14Widget->hideMinMax();
        ui->radio15Widget->hideMinMax();
        ui->radio16Widget->hideMinMax();

        //Send calibrations.
        QString minTpl("RC%1_MIN");
        QString maxTpl("RC%1_MAX");
        QString trimTpl("RC%1_TRIM");

        QString statusstr;
        statusstr = "Below you will find the detected radio calibration information\n";
        statusstr += "Normal values are between 1100 to 1900, trim close to 1500\n\n";
        statusstr += "Channel\tMin\tCenter\tMax\n";
        statusstr += "--------------------\n";

        for (auto i = 0; i < RC_CHANNEL_NUM_MAX; ++i)
        {
            if(rcValue[i] > 0)  // send only if we have a valid value
            {
                statusstr += QString::number(i+1) + "\t" + QString::number(rcMin[i]) + "\t" + QString::number(rcValue[i]) + "\t" + QString::number(rcMax[i]) + "\n";
            }
        }

        if (validRadioSettings())
        {
            // disconnect while writing parametrs to avoid updating the values (rcValue)
            disconnect(m_uas, SIGNAL(remoteControlChannelRawChanged(int,float)), this,SLOT(remoteControlChannelRawChanged(int,float)));

            for (auto i = 0; i < RC_CHANNEL_NUM_MAX; ++i)
            {
                if(rcValue[i] > 0)  // send only if we have a valid value
                {
                    QLOG_DEBUG() << "SENDING MIN" << minTpl.arg(i+1) << rcMin[i];
                    QLOG_DEBUG() << "SENDING TRIM" << trimTpl.arg(i+1) << rcValue[i];
                    QLOG_DEBUG() << "SENDING MAX" << maxTpl.arg(i+1) << rcMax[i];

                    // Send Calibrations
                    m_uas->getParamManager()->setParameter(1, minTpl.arg(i+1), rcMin[i]);
                    m_uas->getParamManager()->setParameter(1, trimTpl.arg(i+1), rcValue[i]); // Save the Trim Values.
                    m_uas->getParamManager()->setParameter(1, maxTpl.arg(i+1), rcMax[i]);
                }
            }
            // reconnect after writing
            connect(m_uas, SIGNAL(remoteControlChannelRawChanged(int,float)), this,SLOT(remoteControlChannelRawChanged(int,float)));



            QMessageBox::information(this,"Status",statusstr); // Show Calibraitions to the user
        }
        else
        {
            QMessageBox::warning(this,"Status","FAILED: Invalid PWM signals\n" + statusstr);
        }

        ui->rollWidget->setMin(800);
        ui->rollWidget->setMax(2200);
        m_pitchWidget->setMin(800);
        m_pitchWidget->setMax(2200);
        m_throttleWidget->setMin(800);
        m_throttleWidget->setMax(2200);
        ui->yawWidget->setMin(800);
        ui->yawWidget->setMax(2200);
        ui->radio5Widget->setMin(800);
        ui->radio5Widget->setMax(2200);
        ui->radio6Widget->setMin(800);
        ui->radio6Widget->setMax(2200);
        ui->radio7Widget->setMin(800);
        ui->radio7Widget->setMax(2200);
        ui->radio8Widget->setMin(800);
        ui->radio8Widget->setMax(2200);
        ui->radio9Widget->setMin(800);
        ui->radio9Widget->setMax(2200);
        ui->radio10Widget->setMin(800);
        ui->radio10Widget->setMax(2200);
        ui->radio11Widget->setMin(800);
        ui->radio11Widget->setMax(2200);
        ui->radio12Widget->setMin(800);
        ui->radio12Widget->setMax(2200);
        ui->radio13Widget->setMin(800);
        ui->radio13Widget->setMax(2200);
        ui->radio14Widget->setMin(800);
        ui->radio14Widget->setMax(2200);
        ui->radio15Widget->setMin(800);
        ui->radio15Widget->setMax(2200);
        ui->radio16Widget->setMin(800);
        ui->radio16Widget->setMax(2200);
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
        m_uas->setParameter(1, "ELEVON_OUTPUT", ui->elevonOutputComboBox->currentIndex());
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
    for(auto count = 0; count < RC_CHANNEL_LOWER_CONTROL_CH_MAX; ++count)
    {
        // Any invalid range and we abort.
        if (!isInRange(rcValue[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX))
        {
            QLOG_ERROR() << QString().sprintf("isRadioControlActive: Error Channel %d out of range: rcValue=%d", count+1, rcValue[count]);
            return false;
        }
    }

    // now check the other channels
    for(auto count = RC_CHANNEL_LOWER_CONTROL_CH_MAX; count < RC_CHANNEL_NUM_MAX; ++count)
    {
        if ((rcValue[count] > 0))   // Only active channels are validated.
        {
            if (!isInRange(rcValue[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX))
            {
                QLOG_ERROR() << QString().sprintf("isRadioControlActive: Error Channel %d out of range: rcValue=%d", count+1, rcValue[count]);
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
    for(auto count = 0; count < RC_CHANNEL_LOWER_CONTROL_CH_MAX; ++count)
    {
        // Any invalid range and we abort.
        if (!isInRange(rcMin[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)
                ||!isInRange(rcMax[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX))
        {
            QLOG_ERROR() << QString().sprintf("validRadioSettings: Error Channel %d out of range: rcMin=%d rcMax=%d",
                                              count+1, rcMin[count], rcMax[count]);
            return false;
        }
    }

    // for channels other than the lower 4 we verify only if non-zero.
    for(auto count = RC_CHANNEL_LOWER_CONTROL_CH_MAX; count < RC_CHANNEL_NUM_MAX; ++count){
        // Only check if we have received a non-zero value on the channel
        // that the settings are valid.
        if ((rcValue[count] > 0))
        {
            // Any invalid range and we abort.
            if (!isInRange(rcMin[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX)
                ||!isInRange(rcMax[count], RC_CHANNEL_PWM_MIN, RC_CHANNEL_PWM_MAX))
            {
                QLOG_ERROR() << QString().sprintf("validRadioSettings: Error Channel %d out of range: rcMin=%d rcMax=%d",
                                                  count+1, rcMin[count], rcMax[count]);
                return false;
            }
        }
    }

    return true;
}

bool RadioCalibrationConfig::isInRange(double value, double min, double max)
{
    if ((value > min)&&(value < max))
    {
        return true;
    }
    return false;
}

void RadioCalibrationConfig::pitchDZChanged(int value)
{
    if(m_pitchChannel != 0)
    {
        rcDeadzone[m_pitchChannel - 1] = value;
    }
}

void RadioCalibrationConfig::rollDZChanged(int value)
{
    if(m_rollChannel != 0)
    {
        rcDeadzone[m_rollChannel -1] = value;
    }
}

void RadioCalibrationConfig::throtDZChanged(int value)
{
    if(m_throttleChannel != 0)
    {
        rcDeadzone[m_throttleChannel - 1] = value;
    }
}

void RadioCalibrationConfig::yawDZChanged(int value)
{
    if(m_yawChannel != 0)
    {
        rcDeadzone[m_yawChannel -1] = value;
    }
}

void RadioCalibrationConfig::writeDZButtonPressed()
{
    QLOG_DEBUG() << "Writing deadzones to UAS";
    // set deadzones
    QString deadZoneTpl("RC%1_DZ");
    for(auto i = 0; i < RC_CHANNEL_LOWER_CONTROL_CH_MAX; ++i)
    {
        m_uas->getParamManager()->setParameter(1, deadZoneTpl.arg(i+1), rcDeadzone[i]);
        QLOG_DEBUG() << deadZoneTpl.arg(i+1) << " = " << rcDeadzone[i];
    }
}

