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

#include "CopterPidConfig.h"

#include "ui_CopterPidConfig.h"

#include "QGCCore.h"
#include "ArduPilotMegaMAV.h"

CopterPidConfig::CopterPidConfig(QWidget *parent) :
    AP2ConfigWidget(parent),
    mp_ui(new Ui::CopterPidConfig)
{
    mp_ui->setupUi(this);

    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    m_pitchRollLocked = false;
    connect(mp_ui->checkBox,SIGNAL(clicked(bool)),this,SLOT(lockCheckBoxClicked(bool)));
    connect(mp_ui->stabilPitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
    connect(mp_ui->stabilRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
    connect(mp_ui->ratePitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
    connect(mp_ui->ratePitchISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
    connect(mp_ui->ratePitchDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
    connect(mp_ui->ratePitchIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));

    connect(mp_ui->rateRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
    connect(mp_ui->rateRollISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
    connect(mp_ui->rateRollDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
    connect(mp_ui->rateRollIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));

    connect(mp_ui->writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(mp_ui->refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));

    m_ch6ValueToTextList.append(QPair<int,QString>(0,"RC6_NONE"));
    m_ch6ValueToTextList.append(QPair<int,QString>(1,"RC6_STABILIZE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(2,"RC6_STABILIZE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(3,"RC6_YAW_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(24,"RC6_YAW_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(4,"RC6_RATE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(5,"RC6_RATE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(6,"RC6_YAW_RATE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(26,"RC6_YAW_RATE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(7,"RC6_THROTTLE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(9,"RC6_RELAY"));
    m_ch6ValueToTextList.append(QPair<int,QString>(10,"RC6_WP_SPEED"));
    m_ch6ValueToTextList.append(QPair<int,QString>(12,"RC6_LOITER_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(13,"RC6_HELI_EXTERNAL_GYRO"));
    m_ch6ValueToTextList.append(QPair<int,QString>(14,"RC6_THR_HOLD_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(17,"RC6_OPTFLOW_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(18,"RC6_OPTFLOW_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(19,"RC6_OPTFLOW_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(21,"RC6_RATE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(22,"RC6_LOITER_RATE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(23,"RC6_LOITER_RATE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(25,"RC6_ACRO_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(27,"RC6_LOITER_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(28,"RC6_LOITER_RATE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(29,"RC6_STABILIZE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(30,"RC6_AHRS_YAW_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(31,"RC6_AHRS_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(32,"RC6_INAV_TC"));
    m_ch6ValueToTextList.append(QPair<int,QString>(33,"RC6_THROTTLE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(34,"RC6_ACCEL_Z_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(35,"RC6_ACCEL_Z_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(36,"RC6_ACCEL_Z_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(38,"RC6_DECLINATION"));
    m_ch6ValueToTextList.append(QPair<int,QString>(39,"RC6_CIRCLE_RATE"));
    m_ch6ValueToTextList.append(QPair<int,QString>(41,"RC6_SONAR_GAIN"));
    m_ch6ValueToTextList.append(QPair<int,QString>(42,"RC6_EKF_VERTICAL_POS"));
    m_ch6ValueToTextList.append(QPair<int,QString>(43,"RC6_EKF_HORIZONTAL_POS"));
    m_ch6ValueToTextList.append(QPair<int,QString>(44,"RC6_EKF_ACCEL_NOISE"));
    m_ch6ValueToTextList.append(QPair<int,QString>(45,"RC6_RC_FEEL_RP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(46,"RC6_RATE_PITCH_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(47,"RC6_RATE_PITCH_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(48,"RC6_RATE_PITCH_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(49,"RC6_RATE_ROLL_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(50,"RC6_RATE_ROLL_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(51,"RC6_RATE_ROLL_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(52,"RC6_RATE_PITCH_FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(53,"RC6_RATE_ROLL_FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(54,"RC6_RATE_YAW_FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(55,"RC6_RATE_MOT_YAW_HEADROOM"));
    m_ch6ValueToTextList.append(QPair<int,QString>(56,"RC6_RATE_YAW_FILT"));

    populateCombobox(m_ch6ValueToTextList, mp_ui->ch6OptComboBox);

    m_ch78ValueToTextList.append(QPair<int,QString>(0,"Do nothing"));
    m_ch78ValueToTextList.append(QPair<int,QString>(2,"Flip"));
    m_ch78ValueToTextList.append(QPair<int,QString>(3,"Simple mode"));
    m_ch78ValueToTextList.append(QPair<int,QString>(4,"RTL"));
    m_ch78ValueToTextList.append(QPair<int,QString>(5,"Save Trim"));
    m_ch78ValueToTextList.append(QPair<int,QString>(7,"Save WP"));
    m_ch78ValueToTextList.append(QPair<int,QString>(8,"Multi Mode"));
    m_ch78ValueToTextList.append(QPair<int,QString>(9,"Camera Trigger"));
    m_ch78ValueToTextList.append(QPair<int,QString>(10,"Sonar"));
    m_ch78ValueToTextList.append(QPair<int,QString>(11,"Fence"));
    m_ch78ValueToTextList.append(QPair<int,QString>(12,"ResetToArmedYaw"));
    m_ch78ValueToTextList.append(QPair<int,QString>(13,"Super Simple Mode"));
    m_ch78ValueToTextList.append(QPair<int,QString>(14,"Acro Trainer"));
    m_ch78ValueToTextList.append(QPair<int,QString>(15,"Sprayer"));
    m_ch78ValueToTextList.append(QPair<int,QString>(16,"Auto"));
    m_ch78ValueToTextList.append(QPair<int,QString>(17,"Auto Tune"));
    m_ch78ValueToTextList.append(QPair<int,QString>(18,"Land"));
    m_ch78ValueToTextList.append(QPair<int,QString>(19,"EPM Cargo Gripper"));
    m_ch78ValueToTextList.append(QPair<int,QString>(20,"Enable NavEKF"));
    m_ch78ValueToTextList.append(QPair<int,QString>(21,"Parachute Enable"));
    m_ch78ValueToTextList.append(QPair<int,QString>(22,"Parachute Release"));
    m_ch78ValueToTextList.append(QPair<int,QString>(23,"Parachute 3POS switch"));
    m_ch78ValueToTextList.append(QPair<int,QString>(24,"Mission Reset"));
    m_ch78ValueToTextList.append(QPair<int,QString>(25,"Roll/Pitch FF enable"));
    m_ch78ValueToTextList.append(QPair<int,QString>(26,"Accel Limiting Enable"));
    m_ch78ValueToTextList.append(QPair<int,QString>(27,"Retract Mount"));
    m_ch78ValueToTextList.append(QPair<int,QString>(28,"Relay Pin on/off"));
    m_ch78ValueToTextList.append(QPair<int,QString>(29,"Landing Gear Ctrl"));
    m_ch78ValueToTextList.append(QPair<int,QString>(30,"Lost Copter Sound"));
    m_ch78ValueToTextList.append(QPair<int,QString>(31,"EStop Switch"));
    m_ch78ValueToTextList.append(QPair<int,QString>(32,"Motor Interlock on/off"));
    m_ch78ValueToTextList.append(QPair<int,QString>(33,"Brake Mode"));

    populateCombobox(m_ch78ValueToTextList, mp_ui->ch7OptComboBox);
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch8OptComboBox);

    initConnections();
}

void CopterPidConfig::mapParamNamesToBox()
{
    // TODO: Move to baseclass and syncronize with BasicPidConfig class
    // as it uses exactly the same mechanics
    // version check
    ArduPilotMegaMAV* apmMav = dynamic_cast<ArduPilotMegaMAV*>(m_uas);
    if (apmMav == nullptr)
    {
        QLOG_INFO() << "CopterPidConfig Class only supports ArduPilotMegaMAV vehicles.";
        return;
    }

    APMFirmwareVersion version = apmMav->getFirmwareVersion();
    QLOG_DEBUG() << "Extended Tuning - PID Config is set up for " << version.versionString();

    if(version.majorNumber() == 3)
    {
        if(version.minorNumber() >= 6)
        {
            QLOG_DEBUG() << "Using parameter set for ArduCopter 3.6+";
            setupPID_APM_36();
        }
        else if(version.minorNumber() >= 4)
        {
            QLOG_DEBUG() << "Using parameter set for ArduCopter 3.4+";
            setupPID_APM_34();
        }
    }
    else if(version.majorNumber() == 4)
    {
        QLOG_DEBUG() << "Using parameter set for ArduCopter 4.0+";
        setupPID_APM_40();
    }
    else
    {
        QLOG_DEBUG() << "Using default parameter set";
        setupPID_Default();
    }
}


void CopterPidConfig::lockCheckBoxClicked(bool checked)
{
    m_pitchRollLocked = checked;
}
void CopterPidConfig::stabilLockedChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(mp_ui->stabilPitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
        disconnect(mp_ui->stabilRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
        mp_ui->stabilPitchPSpinBox->setValue(value);
        mp_ui->stabilRollPSpinBox->setValue(value);
        connect(mp_ui->stabilPitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
        connect(mp_ui->stabilRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
    }
}
void CopterPidConfig::ratePChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(mp_ui->ratePitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
        disconnect(mp_ui->rateRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
        mp_ui->ratePitchPSpinBox->setValue(value);
        mp_ui->rateRollPSpinBox->setValue(value);
        connect(mp_ui->ratePitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
        connect(mp_ui->rateRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
    }
}
void CopterPidConfig::rateIChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(mp_ui->ratePitchISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
        disconnect(mp_ui->rateRollISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
        mp_ui->ratePitchISpinBox->setValue(value);
        mp_ui->rateRollISpinBox->setValue(value);
        connect(mp_ui->ratePitchISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
        connect(mp_ui->rateRollISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
    }
}
void CopterPidConfig::rateDChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(mp_ui->ratePitchDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
        disconnect(mp_ui->rateRollDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
        mp_ui->ratePitchDSpinBox->setValue(value);
        mp_ui->rateRollDSpinBox->setValue(value);
        connect(mp_ui->ratePitchDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
        connect(mp_ui->rateRollDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
    }
}
void CopterPidConfig::rateIMAXChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(mp_ui->ratePitchIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
        disconnect(mp_ui->rateRollIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
        mp_ui->ratePitchIMAXSpinBox->setValue(value);
        mp_ui->rateRollIMAXSpinBox->setValue(value);
        connect(mp_ui->ratePitchIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
        connect(mp_ui->rateRollIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
    }
}

void CopterPidConfig::showEvent(QShowEvent *evt)
{
    mapParamNamesToBox();
    refreshButtonClicked();
    QWidget::showEvent(evt);
}

CopterPidConfig::~CopterPidConfig()
{
}

void CopterPidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas)
    Q_UNUSED(component)

    if (m_nameToBoxMap.contains(parameterName))
    {
        m_nameToBoxMap[parameterName]->setValue(value.toDouble());
        m_nameToBoxMap[parameterName]->setEnabled(true);
    }
    else if (parameterName == m_ch6MaxParamName)
    {
        mp_ui->ch6MaxSpinBox->setValue(value.toDouble() / 1000.0);
        mp_ui->ch6MaxSpinBox->setEnabled(true);
    }
    else if (parameterName == m_ch6MinParamName)
    {
        mp_ui->ch6MinSpinBox->setValue(value.toDouble() / 1000.0);
        mp_ui->ch6MinSpinBox->setEnabled(true);
    }
    else if (parameterName == "TUNE")
    {
        mp_ui->ch6OptComboBox->setEnabled(true);
        for (int i=0;i<m_ch6ValueToTextList.size();i++)
        {
            if (m_ch6ValueToTextList[i].first == value.toInt())
            {
                mp_ui->ch6OptComboBox->setCurrentIndex(i);
            }
        }
    }
    else if ((parameterName == m_channel7Option) || (parameterName == m_channel8Option) || (parameterName == m_channel9Option))
    {
        QComboBox *p_box{nullptr};
        if (parameterName == m_channel7Option)
        {
            p_box = mp_ui->ch7OptComboBox;
        }
        else if (parameterName == m_channel8Option)
        {
            p_box = mp_ui->ch8OptComboBox;
        }
        else if (parameterName == m_channel9Option)
        {
            p_box = mp_ui->ch9OptComboBox;
        }

        p_box->setEnabled(true);
        for (int i = 0; i < m_ch78ValueToTextList.size(); i++)
        {
            if (m_ch78ValueToTextList[i].first == value.toInt())
            {
                p_box->setCurrentIndex(i);
            }
        }
    }
}

void CopterPidConfig::writeButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    for (auto iter = m_nameToBoxMap.constBegin(); iter != m_nameToBoxMap.constEnd(); iter++)
    {
        m_uas->getParamManager()->setParameter(1, iter.key(), iter.value()->value());
    }

    m_uas->getParamManager()->setParameter(1, "TUNE", m_ch6ValueToTextList[mp_ui->ch6OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1, m_channel7Option, m_ch78ValueToTextList[mp_ui->ch7OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1, m_channel8Option, m_ch78ValueToTextList[mp_ui->ch8OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1, m_channel9Option, m_ch78ValueToTextList[mp_ui->ch9OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1, m_ch6MaxParamName, mp_ui->ch6MaxSpinBox->value() * 1000.0);
    m_uas->getParamManager()->setParameter(1, m_ch6MinParamName, mp_ui->ch6MinSpinBox->value() * 1000.0);
}

void CopterPidConfig::refreshButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    for (auto iter = m_nameToBoxMap.constBegin(); iter != m_nameToBoxMap.constEnd(); ++iter)
    {
        iter.value()->setEnabled(false);
        m_uas->getParamManager()->requestParameterUpdate(1,iter.key());
    }
    m_uas->getParamManager()->requestParameterUpdate(1, "TUNE");
    m_uas->getParamManager()->requestParameterUpdate(1, m_channel7Option);
    m_uas->getParamManager()->requestParameterUpdate(1, m_channel8Option);
    m_uas->getParamManager()->requestParameterUpdate(1, m_channel9Option);
    m_uas->getParamManager()->requestParameterUpdate(1, m_ch6MaxParamName);
    m_uas->getParamManager()->requestParameterUpdate(1, m_ch6MinParamName);
}

void CopterPidConfig::setupPID_Default()
{
    m_nameToBoxMap["STB_RLL_P"] = mp_ui->stabilRollPSpinBox;        // stabilize roll P
    m_nameToBoxMap["STB_PIT_P"] = mp_ui->stabilPitchPSpinBox;       // stabilize pitch P
    m_nameToBoxMap["STB_YAW_P"] = mp_ui->stabilYawPSpinBox;         // stabilize yaw P

    m_nameToBoxMap["RATE_RLL_P"] = mp_ui->rateRollPSpinBox;             // rate roll P
    m_nameToBoxMap["RATE_RLL_I"] = mp_ui->rateRollISpinBox;             // rate roll I
    m_nameToBoxMap["RATE_RLL_D"] = mp_ui->rateRollDSpinBox;             // rate roll D
    m_nameToBoxMap["RATE_RLL_FILT_HZ"] = mp_ui->rateRollFilter1SpinBox; // rate roll filter Hz
    m_nameToBoxMap["RATE_RLL_IMAX"] = mp_ui->rateRollIMAXSpinBox;       // rate roll I Max

    m_nameToBoxMap["RATE_PIT_P"] = mp_ui->ratePitchPSpinBox;         // rate pitch P
    m_nameToBoxMap["RATE_PIT_I"] = mp_ui->ratePitchISpinBox;         // rate pitch I
    m_nameToBoxMap["RATE_PIT_D"] = mp_ui->ratePitchDSpinBox;         // rate pitch D
    m_nameToBoxMap["RATE_PIT_FILT_HZ"] = mp_ui->ratePitchFilter1SpinBox; // rate pitch filter Hz
    m_nameToBoxMap["RATE_PIT_IMAX"] = mp_ui->ratePitchIMAXSpinBox;   // rate pitch I Max

    m_nameToBoxMap["RATE_YAW_P"] = mp_ui->rateYawPSpinBox;           // rate yaw P
    m_nameToBoxMap["RATE_YAW_I"] = mp_ui->rateYawISpinBox;           // rate yaw I
    m_nameToBoxMap["RATE_YAW_D"] = mp_ui->rateYawDSpinBox;           // rate yaw D
    m_nameToBoxMap["RATE_YAW_FILT_HZ"] = mp_ui->rateYawFilter1SpinBox;   // rate yaw filter Hz
    m_nameToBoxMap["RATE_YAW_IMAX"] = mp_ui->rateYawIMAXSpinBox;     // rate yaw I Max

    m_nameToBoxMap["VEL_XY_P"] = mp_ui->velXYPSpinBox;               // horizontal velocity P
    m_nameToBoxMap["VEL_XY_I"] = mp_ui->velXYISpinBox;               // horizontal velocity I
    m_nameToBoxMap["VEL_XY_IMAX"] = mp_ui->velXYIMAXSpinBox;         // horizontal velocity I Max
    m_nameToBoxMap["VEL_XY_FILT_HZ"] = mp_ui->velXYFilterSpinBox;      // horizontal velocity filter Hz

    m_nameToBoxMap["ACCEL_Z_P"] = mp_ui->accelZPSpinBox;               // vertical acceleration P
    m_nameToBoxMap["ACCEL_Z_I"] = mp_ui->accelZISpinBox;               // vertical acceleration I
    m_nameToBoxMap["ACCEL_Z_D"] = mp_ui->accelZDSpinBox;               // vertical acceleration D
    m_nameToBoxMap["ACCEL_Z_IMAX"] = mp_ui->accelZIMAXSpinBox;         // vertical acceleration I Max
    m_nameToBoxMap["ACCEL_Z_FILT"] = mp_ui->vertAccFilter1SpinBox1;    // vertical acceleration filter Hz

    m_nameToBoxMap["VEL_Z_P"] = mp_ui->velZPSpinBox;     // vertical velocity P

    m_nameToBoxMap["POS_Z_P"] = mp_ui->posZPSpinBox;     // vertical position P

    m_nameToBoxMap["POS_XY_P"] = mp_ui->posXYPSpinBox;   // horizontal position P

    m_nameToBoxMap["WPNAV_SPEED"] = mp_ui->wpNavSpeedSpinBox;           // wpnav speed cm per sec
    m_nameToBoxMap["WPNAV_RADIUS"] = mp_ui->wpNavRadiusSpinBox;         // wpnav radius cm per sec
    m_nameToBoxMap["WPNAV_SPEED_DN"] = mp_ui->wpNavSpeedDownSpinBox;    // wpnav speed down cm per sec
    m_nameToBoxMap["WPNAV_LOIT_SPEED"] = mp_ui->wpNavLoiterSpeedSpinBox;// wpnav loiter speed cm per sec
    m_nameToBoxMap["WPNAV_SPEED_UP"] = mp_ui->wpNavSpeedUpSpinBox;      // wpnav speed up cm per sec

    // rc option channel names
    m_channel7Option = "CH7_OPT";
    m_channel8Option = "CH8_OPT";

    // Tune min/max parameter names
    m_ch6MaxParamName = "TUNE_HIGH";
    m_ch6MinParamName = "TUNE_LOW";

    // setup UI
    mp_ui->velXYDSpinBox->hide();   // default has no horizontal velocity D param
    mp_ui->LabelHorVel_D->hide();

    hideAPM_40_Filters();
}

void CopterPidConfig::setupPID_APM_34()
{
    // AC3.4+ paramter names
    m_nameToBoxMap["ATC_ANG_RLL_P"] = mp_ui->stabilRollPSpinBox;        // stabilize roll P
    m_nameToBoxMap["ATC_ANG_PIT_P"] = mp_ui->stabilPitchPSpinBox;       // stabilize pitch P
    m_nameToBoxMap["ATC_ANG_YAW_P"] = mp_ui->stabilYawPSpinBox;         // stabilize yaw P

    m_nameToBoxMap["ATC_RAT_RLL_P"] = mp_ui->rateRollPSpinBox;          // rate roll P
    m_nameToBoxMap["ATC_RAT_RLL_I"] = mp_ui->rateRollISpinBox;          // rate roll I
    m_nameToBoxMap["ATC_RAT_RLL_D"] = mp_ui->rateRollDSpinBox;          // rate roll D
    m_nameToBoxMap["ATC_RAT_RLL_FILT"] = mp_ui->rateRollFilter1SpinBox; // rate roll filter Hz
    m_nameToBoxMap["ATC_RAT_RLL_IMAX"] = mp_ui->rateRollIMAXSpinBox;    // rate roll I Max

    m_nameToBoxMap["ATC_RAT_PIT_P"] = mp_ui->ratePitchPSpinBox;         // rate pitch P
    m_nameToBoxMap["ATC_RAT_PIT_I"] = mp_ui->ratePitchISpinBox;         // rate pitch I
    m_nameToBoxMap["ATC_RAT_PIT_D"] = mp_ui->ratePitchDSpinBox;         // rate pitch D
    m_nameToBoxMap["ATC_RAT_PIT_FILT"] = mp_ui->ratePitchFilter1SpinBox; // rate pitch filter Hz
    m_nameToBoxMap["ATC_RAT_PIT_IMAX"] = mp_ui->ratePitchIMAXSpinBox;   // rate pitch I Max

    m_nameToBoxMap["ATC_RAT_YAW_P"] = mp_ui->rateYawPSpinBox;           // rate yaw P
    m_nameToBoxMap["ATC_RAT_YAW_I"] = mp_ui->rateYawISpinBox;           // rate yaw I
    m_nameToBoxMap["ATC_RAT_YAW_D"] = mp_ui->rateYawDSpinBox;           // rate yaw D
    m_nameToBoxMap["ATC_RAT_YAW_FILT"] = mp_ui->rateYawFilter1SpinBox;   // rate yaw filter Hz
    m_nameToBoxMap["ATC_RAT_YAW_IMAX"] = mp_ui->rateYawIMAXSpinBox;     // rate yaw I Max

    m_nameToBoxMap["VEL_XY_P"] = mp_ui->velXYPSpinBox;               // horizontal velocity P
    m_nameToBoxMap["VEL_XY_I"] = mp_ui->velXYISpinBox;               // horizontal velocity I
    m_nameToBoxMap["VEL_XY_IMAX"] = mp_ui->velXYIMAXSpinBox;         // horizontal velocity I Max
    m_nameToBoxMap["VEL_XY_FILT_HZ"] = mp_ui->velXYFilterSpinBox;    // horizontal velocity filter Hz

    m_nameToBoxMap["ACCEL_Z_P"] = mp_ui->accelZPSpinBox;               // vertical acceleration P
    m_nameToBoxMap["ACCEL_Z_I"] = mp_ui->accelZISpinBox;               // vertical acceleration I
    m_nameToBoxMap["ACCEL_Z_D"] = mp_ui->accelZDSpinBox;               // vertical acceleration D
    m_nameToBoxMap["ACCEL_Z_IMAX"] = mp_ui->accelZIMAXSpinBox;         // vertical acceleration I Max
    m_nameToBoxMap["ACCEL_Z_FILT"] = mp_ui->vertAccFilter1SpinBox1;    // vertical acceleration filter Hz

    m_nameToBoxMap["VEL_Z_P"] = mp_ui->velZPSpinBox;     // vertical velocity P

    m_nameToBoxMap["POS_Z_P"] = mp_ui->posZPSpinBox;     // vertical position P

    m_nameToBoxMap["POS_XY_P"] = mp_ui->posXYPSpinBox;   // horizontal position P

    m_nameToBoxMap["WPNAV_SPEED"] = mp_ui->wpNavSpeedSpinBox;           // wpnav speed cm per sec
    m_nameToBoxMap["WPNAV_RADIUS"] = mp_ui->wpNavRadiusSpinBox;         // wpnav radius cm per sec
    m_nameToBoxMap["WPNAV_SPEED_DN"] = mp_ui->wpNavSpeedDownSpinBox;    // wpnav speed down cm per sec
    m_nameToBoxMap["WPNAV_LOIT_SPEED"] = mp_ui->wpNavLoiterSpeedSpinBox;// wpnav loiter speed cm per sec
    m_nameToBoxMap["WPNAV_SPEED_UP"] = mp_ui->wpNavSpeedUpSpinBox;      // wpnav speed up cm per sec

    // rc option channel names
    m_channel7Option = "CH7_OPT";
    m_channel8Option = "CH8_OPT";

    // Tune min/max parameter names
    m_ch6MaxParamName = "TUNE_HIGH";
    m_ch6MinParamName = "TUNE_LOW";

    // setup UI
    mp_ui->velXYDSpinBox->hide();   // 3.4+ has no horizontal velocity D param
    mp_ui->LabelHorVel_D->hide();

    hideAPM_40_Filters();
}

void CopterPidConfig::setupPID_APM_36()
{
    // AC3.6+ paramter names
    m_nameToBoxMap["ATC_ANG_RLL_P"] = mp_ui->stabilRollPSpinBox;        // stabilize roll P
    m_nameToBoxMap["ATC_ANG_PIT_P"] = mp_ui->stabilPitchPSpinBox;       // stabilize pitch P
    m_nameToBoxMap["ATC_ANG_YAW_P"] = mp_ui->stabilYawPSpinBox;         // stabilize yaw P

    m_nameToBoxMap["ATC_RAT_RLL_P"] = mp_ui->rateRollPSpinBox;          // rate roll P
    m_nameToBoxMap["ATC_RAT_RLL_I"] = mp_ui->rateRollISpinBox;          // rate roll I
    m_nameToBoxMap["ATC_RAT_RLL_D"] = mp_ui->rateRollDSpinBox;          // rate roll D
    m_nameToBoxMap["ATC_RAT_RLL_FILT"] = mp_ui->rateRollFilter1SpinBox;  // rate roll filter Hz
    m_nameToBoxMap["ATC_RAT_RLL_IMAX"] = mp_ui->rateRollIMAXSpinBox;    // rate roll I Max
    m_nameToBoxMap["ATC_RAT_RLL_FF"] = mp_ui->RLLFFSpinBox;             // rate roll feed froward

    m_nameToBoxMap["ATC_RAT_PIT_P"] = mp_ui->ratePitchPSpinBox;         // rate pitch P
    m_nameToBoxMap["ATC_RAT_PIT_I"] = mp_ui->ratePitchISpinBox;         // rate pitch I
    m_nameToBoxMap["ATC_RAT_PIT_D"] = mp_ui->ratePitchDSpinBox;         // rate pitch D
    m_nameToBoxMap["ATC_RAT_PIT_FILT"] = mp_ui->ratePitchFilter1SpinBox; // rate pitch filter Hz
    m_nameToBoxMap["ATC_RAT_PIT_IMAX"] = mp_ui->ratePitchIMAXSpinBox;   // rate pitch I Max
    m_nameToBoxMap["ATC_RAT_PIT_FF"] = mp_ui->PITFFSpinBox;             // rate pitch feed froward

    m_nameToBoxMap["ATC_RAT_YAW_P"] = mp_ui->rateYawPSpinBox;           // rate yaw P
    m_nameToBoxMap["ATC_RAT_YAW_I"] = mp_ui->rateYawISpinBox;           // rate yaw I
    m_nameToBoxMap["ATC_RAT_YAW_D"] = mp_ui->rateYawDSpinBox;           // rate yaw D
    m_nameToBoxMap["ATC_RAT_YAW_FILT"] = mp_ui->rateYawFilter1SpinBox;   // rate yaw filter Hz
    m_nameToBoxMap["ATC_RAT_YAW_IMAX"] = mp_ui->rateYawIMAXSpinBox;     // rate yaw I Max
    m_nameToBoxMap["ATC_RAT_YAW_FF"] = mp_ui->YAWFFSpinBox;             // rate yaw feed froward


    m_nameToBoxMap["PSC_VELXY_P"] = mp_ui->velXYPSpinBox;               // horizontal velocity P
    m_nameToBoxMap["PSC_VELXY_I"] = mp_ui->velXYISpinBox;               // horizontal velocity I
    m_nameToBoxMap["PSC_VELXY_D"] = mp_ui->velXYDSpinBox;              // horizontal velocity D
    m_nameToBoxMap["PSC_VELXY_IMAX"] = mp_ui->velXYIMAXSpinBox;         // horizontal velocity I Max
    m_nameToBoxMap["PSC_VELXY_FILT"] = mp_ui->velXYFilterSpinBox;    // horizontal velocity filter Hz

    m_nameToBoxMap["PSC_ACCZ_P"] = mp_ui->accelZPSpinBox;               // vertical acceleration P
    m_nameToBoxMap["PSC_ACCZ_I"] = mp_ui->accelZISpinBox;               // vertical acceleration I
    m_nameToBoxMap["PSC_ACCZ_D"] = mp_ui->accelZDSpinBox;               // vertical acceleration D
    m_nameToBoxMap["PSC_ACCZ_IMAX"] = mp_ui->accelZIMAXSpinBox;         // vertical acceleration I Max
    m_nameToBoxMap["PSC_ACCZ_FILT"] = mp_ui->vertAccFilter1SpinBox1;    // vertical acceleration filter Hz

    m_nameToBoxMap["PSC_VELZ_P"] = mp_ui->velZPSpinBox;     // vertical velocity P

    m_nameToBoxMap["PSC_POSZ_P"] = mp_ui->posZPSpinBox;     // vertical position P

    m_nameToBoxMap["PSC_POSXY_P"] = mp_ui->posXYPSpinBox;   // horizontal position P

    m_nameToBoxMap["WPNAV_SPEED"] = mp_ui->wpNavSpeedSpinBox;           // wpnav speed cm per sec
    m_nameToBoxMap["WPNAV_RADIUS"] = mp_ui->wpNavRadiusSpinBox;         // wpnav radius cm per sec
    m_nameToBoxMap["WPNAV_SPEED_DN"] = mp_ui->wpNavSpeedDownSpinBox;    // wpnav speed down cm per sec
    m_nameToBoxMap["LOIT_SPEED"] = mp_ui->wpNavLoiterSpeedSpinBox;      // wpnav loiter speed cm per sec
    m_nameToBoxMap["WPNAV_SPEED_UP"] = mp_ui->wpNavSpeedUpSpinBox;      // wpnav speed up cm per sec

    // Tune option list
    setupTuneOption_APM_36();
    populateCombobox(m_ch6ValueToTextList, mp_ui->ch6OptComboBox);

    // Tune min/max parameter names
    m_ch6MaxParamName = "TUNE_HIGH";
    m_ch6MinParamName = "TUNE_LOW";

    // Option list for RC7 & RC8
    setupRC78Option_APM_36();
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch7OptComboBox);
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch8OptComboBox);
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch9OptComboBox);

    // rc option channel names
    m_channel7Option = "CH7_OPT";
    m_channel8Option = "CH8_OPT";
    m_channel9Option = "CH9_OPT";

    // setup UI
    mp_ui->velXYDSpinBox->show();   // 3.6+ has horizontal velocity d Param
    mp_ui->LabelHorVel_D->show();

    hideAPM_40_Filters();
}

void CopterPidConfig::setupPID_APM_40()
{
    // AC4.0+ paramter names
    m_nameToBoxMap["ATC_ANG_RLL_P"] = mp_ui->stabilRollPSpinBox;        // stabilize roll P
    m_nameToBoxMap["ATC_ANG_PIT_P"] = mp_ui->stabilPitchPSpinBox;       // stabilize pitch P
    m_nameToBoxMap["ATC_ANG_YAW_P"] = mp_ui->stabilYawPSpinBox;         // stabilize yaw P

    m_nameToBoxMap["ATC_RAT_RLL_P"] = mp_ui->rateRollPSpinBox;          // rate roll P
    m_nameToBoxMap["ATC_RAT_RLL_I"] = mp_ui->rateRollISpinBox;          // rate roll I
    m_nameToBoxMap["ATC_RAT_RLL_D"] = mp_ui->rateRollDSpinBox;          // rate roll D
    m_nameToBoxMap["ATC_RAT_RLL_FLTD"] = mp_ui->rateRollFilter1SpinBox;  // rate roll d-Term filter Hz
    m_nameToBoxMap["ATC_RAT_RLL_FLTE"] = mp_ui->rateRollFilter2SpinBox;  // rate roll error filter Hz
    m_nameToBoxMap["ATC_RAT_RLL_FLTT"] = mp_ui->rateRollFilter3SpinBox;  // rate roll target filter Hz
    m_nameToBoxMap["ATC_RAT_RLL_IMAX"] = mp_ui->rateRollIMAXSpinBox;    // rate roll I Max
    m_nameToBoxMap["ATC_RAT_RLL_FF"] = mp_ui->RLLFFSpinBox;             // rate roll feed froward

    m_nameToBoxMap["ATC_RAT_PIT_P"] = mp_ui->ratePitchPSpinBox;         // rate pitch P
    m_nameToBoxMap["ATC_RAT_PIT_I"] = mp_ui->ratePitchISpinBox;         // rate pitch I
    m_nameToBoxMap["ATC_RAT_PIT_D"] = mp_ui->ratePitchDSpinBox;         // rate pitch D
    m_nameToBoxMap["ATC_RAT_PIT_FLTD"] = mp_ui->ratePitchFilter1SpinBox;  // rate pitch d-Term filter Hz
    m_nameToBoxMap["ATC_RAT_PIT_FLTE"] = mp_ui->ratePitchFilter2SpinBox;  // rate pitch error filter Hz
    m_nameToBoxMap["ATC_RAT_PIT_FLTT"] = mp_ui->ratePitchFilter3SpinBox;  // rate pitch target filter Hz
    m_nameToBoxMap["ATC_RAT_PIT_IMAX"] = mp_ui->ratePitchIMAXSpinBox;   // rate pitch I Max
    m_nameToBoxMap["ATC_RAT_PIT_FF"] = mp_ui->PITFFSpinBox;             // rate pitch feed froward

    m_nameToBoxMap["ATC_RAT_YAW_P"] = mp_ui->rateYawPSpinBox;           // rate yaw P
    m_nameToBoxMap["ATC_RAT_YAW_I"] = mp_ui->rateYawISpinBox;           // rate yaw I
    m_nameToBoxMap["ATC_RAT_YAW_D"] = mp_ui->rateYawDSpinBox;           // rate yaw D
    m_nameToBoxMap["ATC_RAT_YAW_FLTD"] = mp_ui->rateYawFilter1SpinBox;  // rate yaw d-Term filter Hz
    m_nameToBoxMap["ATC_RAT_YAW_FLTE"] = mp_ui->rateYawFilter2SpinBox;  // rate yaw error filter Hz
    m_nameToBoxMap["ATC_RAT_YAW_FLTT"] = mp_ui->rateYawFilter3SpinBox;  // rate yaw target filter Hz
    m_nameToBoxMap["ATC_RAT_YAW_IMAX"] = mp_ui->rateYawIMAXSpinBox;     // rate yaw I Max
    m_nameToBoxMap["ATC_RAT_YAW_FF"] = mp_ui->YAWFFSpinBox;             // rate yaw feed froward

    m_nameToBoxMap["PSC_VELXY_P"] = mp_ui->velXYPSpinBox;               // horizontal velocity P
    m_nameToBoxMap["PSC_VELXY_I"] = mp_ui->velXYISpinBox;               // horizontal velocity I
    m_nameToBoxMap["PSC_VELXY_D"] = mp_ui->velXYDSpinBox;               // horizontal velocity D
    m_nameToBoxMap["PSC_VELXY_IMAX"] = mp_ui->velXYIMAXSpinBox;         // horizontal velocity I Max
    m_nameToBoxMap["PSC_VELXY_FILT"] = mp_ui->velXYFilterSpinBox;       // horizontal velocity filter Hz

    m_nameToBoxMap["PSC_ACCZ_P"] = mp_ui->accelZPSpinBox;               // vertical acceleration P
    m_nameToBoxMap["PSC_ACCZ_I"] = mp_ui->accelZISpinBox;               // vertical acceleration I
    m_nameToBoxMap["PSC_ACCZ_D"] = mp_ui->accelZDSpinBox;               // vertical acceleration D
    m_nameToBoxMap["PSC_ACCZ_IMAX"] = mp_ui->accelZIMAXSpinBox;         // vertical acceleration I Max
    m_nameToBoxMap["PSC_ACCZ_FLTD"] = mp_ui->vertAccFilter1SpinBox1;    // vertical acceleration d-term filter Hz
    m_nameToBoxMap["PSC_ACCZ_FLTE"] = mp_ui->vertAccFilter1SpinBox2;    // vertical acceleration error filter Hz
    m_nameToBoxMap["PSC_ACCZ_FLTT"] = mp_ui->vertAccFilter1SpinBox3;    // vertical acceleration target filter Hz

    m_nameToBoxMap["PSC_VELZ_P"] = mp_ui->velZPSpinBox;     // vertical velocity P
    m_nameToBoxMap["PSC_POSZ_P"] = mp_ui->posZPSpinBox;     // vertical position P
    m_nameToBoxMap["PSC_POSXY_P"] = mp_ui->posXYPSpinBox;   // horizontal position P

    m_nameToBoxMap["WPNAV_SPEED"] = mp_ui->wpNavSpeedSpinBox;           // wpnav speed cm per sec
    m_nameToBoxMap["WPNAV_RADIUS"] = mp_ui->wpNavRadiusSpinBox;         // wpnav radius cm per sec
    m_nameToBoxMap["WPNAV_SPEED_DN"] = mp_ui->wpNavSpeedDownSpinBox;    // wpnav speed down cm per sec
    m_nameToBoxMap["LOIT_SPEED"] = mp_ui->wpNavLoiterSpeedSpinBox;      // wpnav loiter speed cm per sec
    m_nameToBoxMap["WPNAV_SPEED_UP"] = mp_ui->wpNavSpeedUpSpinBox;      // wpnav speed up cm per sec

    // Tune min/max parameter names
    m_ch6MaxParamName = "TUNE_MAX";
    m_ch6MinParamName = "TUNE_MIN";

    // Tune option list - same as for APM_36 plus "SysID Magnitude" param
    setupTuneOption_APM_36();
    m_ch6ValueToTextList.append(QPair<int,QString>(58, "SysID Magnitude"));
    populateCombobox(m_ch6ValueToTextList, mp_ui->ch6OptComboBox);

    // Option list for RC7 & RC8
    setupRC78Option_APM_36();
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch7OptComboBox);
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch8OptComboBox);
    populateCombobox(m_ch78ValueToTextList, mp_ui->ch9OptComboBox);

    // rc option channel names
    m_channel7Option = "RC7_OPTION";
    m_channel8Option = "RC8_OPTION";
    m_channel9Option = "RC9_OPTION";

    // setup UI
    mp_ui->velXYDSpinBox->show();   // 4.0+ has horizontal velocity d Param
    mp_ui->LabelHorVel_D->show();

    // 4.0+ changed filter control for roll pitch and yaw
    mp_ui->rateRollFilter1Label->setText("D-term Filter");
    mp_ui->rateRollFilter2Label->setText("Error Filter");
    mp_ui->rateRollFilter2SpinBox->show();
    mp_ui->rateRollFilter3Label->setText("Target Filter");
    mp_ui->rateRollFilter3SpinBox->show();

    mp_ui->ratePitchFilter1Label->setText("D-term Filter");
    mp_ui->ratePitchFilter2Label->setText("Error Filter");
    mp_ui->ratePitchFilter2SpinBox->show();
    mp_ui->ratePitchFilter3Label->setText("Target Filter");
    mp_ui->ratePitchFilter3SpinBox->show();

    mp_ui->rateYawFilter1Label->setText("D-term Filter");
    mp_ui->rateYawFilter2Label->setText("Error Filter");
    mp_ui->rateYawFilter2SpinBox->show();
    mp_ui->rateYawFilter3Label->setText("Target Filter");
    mp_ui->rateYawFilter3SpinBox->show();

    mp_ui->vertAccFilterLabel1->setText("D-term Filter");
    mp_ui->vertAccFilterLabel2->setText("Error Filter");
    mp_ui->vertAccFilter1SpinBox2->show();
    mp_ui->vertAccFilterLabel3->setText("Target Filter");
    mp_ui->vertAccFilter1SpinBox3->show();
}

void CopterPidConfig::hideAPM_40_Filters()
{
    mp_ui->rateRollFilter1Label->setText("Filter");
    mp_ui->rateRollFilter2SpinBox->hide();
    mp_ui->rateRollFilter2Label->hide();
    mp_ui->rateRollFilter3SpinBox->hide();
    mp_ui->rateRollFilter3Label->hide();

    mp_ui->ratePitchFilter1Label->setText("Filter");
    mp_ui->ratePitchFilter2Label->hide();
    mp_ui->ratePitchFilter2SpinBox->hide();
    mp_ui->ratePitchFilter3Label->hide();
    mp_ui->ratePitchFilter3SpinBox->hide();

    mp_ui->rateYawFilter1Label->setText("Filter");
    mp_ui->rateYawFilter2Label->hide();
    mp_ui->rateYawFilter2SpinBox->hide();
    mp_ui->rateYawFilter3Label->hide();
    mp_ui->rateYawFilter3SpinBox->hide();

    mp_ui->vertAccFilterLabel1->setText("Filter");
    mp_ui->vertAccFilterLabel2->hide();
    mp_ui->vertAccFilter1SpinBox2->hide();
    mp_ui->vertAccFilterLabel3->hide();
    mp_ui->vertAccFilter1SpinBox3->hide();
}

void CopterPidConfig::setupTuneOption_APM_36()
{
    m_ch6ValueToTextList.clear();

    m_ch6ValueToTextList.append(QPair<int,QString>(0,  "None"));
    m_ch6ValueToTextList.append(QPair<int,QString>(1,  "Stab Roll/Pitch kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(4,  "Rate Roll/Pitch kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(5,  "Rate Roll/Pitch kI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(21, "Rate Roll/Pitch kD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(3,  "Stab Yaw kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(6,  "Rate Yaw kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(26, "Rate Yaw kD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(56, "Rate Yaw Filter"));
    m_ch6ValueToTextList.append(QPair<int,QString>(55, "Motor Yaw Headroom"));
    m_ch6ValueToTextList.append(QPair<int,QString>(14, "AltHold kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(7,  "Throttle Rate kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(34, "Throttle Accel kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(35, "Throttle Accel kI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(36, "Throttle Accel kD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(12, "Loiter Pos kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(22, "Velocity XY kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(28, "Velocity XY kI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(10, "WP Speed"));
    m_ch6ValueToTextList.append(QPair<int,QString>(25, "Acro RollPitch kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(40, "Acro Yaw kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(45, "RC Feel"));
    m_ch6ValueToTextList.append(QPair<int,QString>(13, "Heli Ext Gyro"));
    m_ch6ValueToTextList.append(QPair<int,QString>(38, "Declination"));
    m_ch6ValueToTextList.append(QPair<int,QString>(39, "Circle Rate"));
    m_ch6ValueToTextList.append(QPair<int,QString>(41, "RangeFinder Gain"));
    m_ch6ValueToTextList.append(QPair<int,QString>(46, "Rate Pitch kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(47, "Rate Pitch kI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(48, "Rate Pitch kD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(49, "Rate Roll kP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(50, "Rate Roll kI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(51, "Rate Roll kD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(52, "Rate Pitch FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(53, "Rate Roll FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(54, "Rate Yaw FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(57, "Winch"));

}

void CopterPidConfig::setupRC78Option_APM_36()
{
    m_ch78ValueToTextList.clear();

    m_ch78ValueToTextList.append(QPair<int,QString>(0,"Do nothing"));
    m_ch78ValueToTextList.append(QPair<int,QString>(2,"Flip"));
    m_ch78ValueToTextList.append(QPair<int,QString>(3,"Simple mode"));
    m_ch78ValueToTextList.append(QPair<int,QString>(4,"RTL"));
    m_ch78ValueToTextList.append(QPair<int,QString>(5,"Save Trim"));
    m_ch78ValueToTextList.append(QPair<int,QString>(7,"Save WP"));
    m_ch78ValueToTextList.append(QPair<int,QString>(9,"Camera Trigger"));
    m_ch78ValueToTextList.append(QPair<int,QString>(10,"RangeFinder"));
    m_ch78ValueToTextList.append(QPair<int,QString>(11,"Fence"));
    m_ch78ValueToTextList.append(QPair<int,QString>(13,"Super Simple Mode"));
    m_ch78ValueToTextList.append(QPair<int,QString>(14,"Acro Trainer"));
    m_ch78ValueToTextList.append(QPair<int,QString>(15,"Sprayer"));
    m_ch78ValueToTextList.append(QPair<int,QString>(16,"Auto"));
    m_ch78ValueToTextList.append(QPair<int,QString>(17,"Auto Tune"));
    m_ch78ValueToTextList.append(QPair<int,QString>(18,"Land"));
    m_ch78ValueToTextList.append(QPair<int,QString>(19,"Gripper"));
    m_ch78ValueToTextList.append(QPair<int,QString>(21,"Parachute Enable"));
    m_ch78ValueToTextList.append(QPair<int,QString>(22,"Parachute Release"));
    m_ch78ValueToTextList.append(QPair<int,QString>(23,"Parachute 3pos"));
    m_ch78ValueToTextList.append(QPair<int,QString>(24,"Auto Mission Reset"));
    m_ch78ValueToTextList.append(QPair<int,QString>(25,"AttCon Feed Forward"));
    m_ch78ValueToTextList.append(QPair<int,QString>(26,"AttCon Accel Limits"));
    m_ch78ValueToTextList.append(QPair<int,QString>(27,"Retract Mount"));
    m_ch78ValueToTextList.append(QPair<int,QString>(28,"Relay on/off"));
    m_ch78ValueToTextList.append(QPair<int,QString>(34,"Relay2 on/off"));
    m_ch78ValueToTextList.append(QPair<int,QString>(35,"Relay3 on/off"));
    m_ch78ValueToTextList.append(QPair<int,QString>(36,"Relay4 on/off"));
    m_ch78ValueToTextList.append(QPair<int,QString>(29,"Landing Gear"));
    m_ch78ValueToTextList.append(QPair<int,QString>(30,"Lost Copter Sound"));
    m_ch78ValueToTextList.append(QPair<int,QString>(31,"Motor Emergency Stop"));
    m_ch78ValueToTextList.append(QPair<int,QString>(32,"Motor Interlock"));
    m_ch78ValueToTextList.append(QPair<int,QString>(33,"Brake"));
    m_ch78ValueToTextList.append(QPair<int,QString>(37,"Throw"));
    m_ch78ValueToTextList.append(QPair<int,QString>(38,"ADSB-Avoidance"));
    m_ch78ValueToTextList.append(QPair<int,QString>(39,"PrecLoiter"));
    m_ch78ValueToTextList.append(QPair<int,QString>(40,"Object Avoidance"));
    m_ch78ValueToTextList.append(QPair<int,QString>(41,"ArmDisarm"));
    m_ch78ValueToTextList.append(QPair<int,QString>(42,"SmartRTL"));
    m_ch78ValueToTextList.append(QPair<int,QString>(43,"InvertedFlight"));
    m_ch78ValueToTextList.append(QPair<int,QString>(44,"Winch Enable"));
    m_ch78ValueToTextList.append(QPair<int,QString>(45,"WinchControl"));
}

void CopterPidConfig::populateCombobox(const ValueNamePairList &ValueToText, QComboBox *Box)
{
    Box->clear();
    for(const auto &item: ValueToText)
    {
        Box->addItem(item.second);
    }
}

