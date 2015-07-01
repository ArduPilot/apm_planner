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

#include "QGCCore.h"

CopterPidConfig::CopterPidConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    m_pitchRollLocked = false;
    connect(ui.checkBox,SIGNAL(clicked(bool)),this,SLOT(lockCheckBoxClicked(bool)));
    connect(ui.stabilPitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
    connect(ui.stabilRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
    connect(ui.ratePitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
    connect(ui.ratePitchISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
    connect(ui.ratePitchDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
    connect(ui.ratePitchIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));

    connect(ui.rateRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
    connect(ui.rateRollISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
    connect(ui.rateRollDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
    connect(ui.rateRollIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));


    m_nameToBoxMap["STB_RLL_P"] = ui.stabilRollPSpinBox;
    m_nameToBoxMap["STB_PIT_P"] = ui.stabilPitchPSpinBox;
    m_nameToBoxMap["STB_YAW_P"] = ui.stabilYawPSpinBox;
    m_nameToBoxMap["POS_XY_P"] = ui.posXYPSpinBox;

    m_nameToBoxMap["RATE_RLL_P"] = ui.rateRollPSpinBox;
    m_nameToBoxMap["RATE_RLL_I"] = ui.rateRollISpinBox;
    m_nameToBoxMap["RATE_RLL_D"] = ui.rateRollDSpinBox;
    m_nameToBoxMap["RATE_RLL_FILT_HZ"] = ui.rateRollFiltHzSpinBox;
    m_nameToBoxMap["RATE_RLL_IMAX"] = ui.rateRollIMAXSpinBox;

    m_nameToBoxMap["RATE_PIT_P"] = ui.ratePitchPSpinBox;
    m_nameToBoxMap["RATE_PIT_I"] = ui.ratePitchISpinBox;
    m_nameToBoxMap["RATE_PIT_D"] = ui.ratePitchDSpinBox;
    m_nameToBoxMap["RATE_PIT_FILT_HZ"] = ui.ratePitchFiltHzSpinBox;
    m_nameToBoxMap["RATE_PIT_IMAX"] = ui.ratePitchIMAXSpinBox;

    m_nameToBoxMap["RATE_YAW_P"] = ui.rateYawPSpinBox;
    m_nameToBoxMap["RATE_YAW_I"] = ui.rateYawISpinBox;
    m_nameToBoxMap["RATE_YAW_D"] = ui.rateYawDSpinBox;
    m_nameToBoxMap["RATE_YAW_FILT_HZ"] = ui.rateYawFiltHzSpinBox;
    m_nameToBoxMap["RATE_YAW_IMAX"] = ui.rateYawIMAXSpinBox;

    m_nameToBoxMap["VEL_XY_P"] = ui.velXYPSpinBox;
    m_nameToBoxMap["VEL_XY_I"] = ui.velXYISpinBox;
    m_nameToBoxMap["VEL_XY_IMAX"] = ui.velXYIMAXSpinBox;
    m_nameToBoxMap["VEL_XY_FILT_HZ"] = ui.velXYFiltHzSpinBox;

    m_nameToBoxMap["ACCEL_Z_P"] = ui.accelZPSpinBox;
    m_nameToBoxMap["ACCEL_Z_I"] = ui.accelZISpinBox;
    m_nameToBoxMap["ACCEL_Z_IMAX"] = ui.accelZIMAXSpinBox;
    m_nameToBoxMap["ACCEL_Z_FILT_HZ"] = ui.accelZFiltHzSpinBox;

    m_nameToBoxMap["VEL_Z_P"] = ui.velZPSpinBox;

    m_nameToBoxMap["POS_Z_P"] = ui.posZPSpinBox;

    m_nameToBoxMap["POS_XY_P"] = ui.posXYPSpinBox;

    m_nameToBoxMap["WPNAV_SPEED"] = ui.wpNavSpeedSpinBox;
    m_nameToBoxMap["WPNAV_RADIUS"] = ui.wpNavRadiusSpinBox;
    m_nameToBoxMap["WPNAV_SPEED_DN"] = ui.wpNavSpeedDownSpinBox;
    m_nameToBoxMap["WPNAV_LOIT_SPEED"] = ui.wpNavLoiterSpeedSpinBox;
    m_nameToBoxMap["WPNAV_SPEED_UP"] = ui.wpNavSpeedUpSpinBox;

    //m_nameToBoxMap["TUNE_HIGH"] = ui.ch6MaxSpinBox;
   // m_nameToBoxMap["TUNE_LOW"] = ui.ch6MinSpinBox;

    connect(ui.writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui.refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));

    m_ch6ValueToTextList.append(QPair<int,QString>(0,"CH6_NONE"));
    m_ch6ValueToTextList.append(QPair<int,QString>(1,"CH6_STABILIZE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(2,"CH6_STABILIZE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(3,"CH6_YAW_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(24,"CH6_YAW_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(4,"CH6_RATE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(5,"CH6_RATE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(6,"CH6_YAW_RATE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(26,"CH6_YAW_RATE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(7,"CH6_THROTTLE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(9,"CH6_RELAY"));
    m_ch6ValueToTextList.append(QPair<int,QString>(10,"CH6_WP_SPEED"));
    m_ch6ValueToTextList.append(QPair<int,QString>(12,"CH6_LOITER_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(13,"CH6_HELI_EXTERNAL_GYRO"));
    m_ch6ValueToTextList.append(QPair<int,QString>(14,"CH6_THR_HOLD_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(17,"CH6_OPTFLOW_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(18,"CH6_OPTFLOW_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(19,"CH6_OPTFLOW_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(21,"CH6_RATE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(22,"CH6_LOITER_RATE_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(23,"CH6_LOITER_RATE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(25,"CH6_ACRO_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(27,"CH6_LOITER_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(28,"CH6_LOITER_RATE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(29,"CH6_STABILIZE_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(30,"CH6_AHRS_YAW_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(31,"CH6_AHRS_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(32,"CH6_INAV_TC"));
    m_ch6ValueToTextList.append(QPair<int,QString>(33,"CH6_THROTTLE_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(34,"CH6_ACCEL_Z_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(35,"CH6_ACCEL_Z_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(36,"CH6_ACCEL_Z_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(38,"CH6_DECLINATION"));
    m_ch6ValueToTextList.append(QPair<int,QString>(39,"CH6_CIRCLE_RATE"));
    m_ch6ValueToTextList.append(QPair<int,QString>(41,"CH6_SONAR_GAIN"));
    m_ch6ValueToTextList.append(QPair<int,QString>(42,"CH6_EKF_VERTICAL_POS"));
    m_ch6ValueToTextList.append(QPair<int,QString>(43,"CH6_EKF_HORIZONTAL_POS"));
    m_ch6ValueToTextList.append(QPair<int,QString>(44,"CH6_EKF_ACCEL_NOISE"));
    m_ch6ValueToTextList.append(QPair<int,QString>(45,"CH6_RC_FEEL_RP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(46,"CH6_RATE_PITCH_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(47,"CH6_RATE_PITCH_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(48,"CH6_RATE_PITCH_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(49,"CH6_RATE_ROLL_KP"));
    m_ch6ValueToTextList.append(QPair<int,QString>(50,"CH6_RATE_ROLL_KI"));
    m_ch6ValueToTextList.append(QPair<int,QString>(51,"CH6_RATE_ROLL_KD"));
    m_ch6ValueToTextList.append(QPair<int,QString>(52,"CH6_RATE_PITCH_FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(53,"CH6_RATE_ROLL_FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(54,"CH6_RATE_YAW_FF"));
    m_ch6ValueToTextList.append(QPair<int,QString>(55,"CH6_RATE_MOT_YAW_HEADROOM"));
    m_ch6ValueToTextList.append(QPair<int,QString>(56,"CH6_RATE_YAW_FILT"));

    for (int i=0;i<m_ch6ValueToTextList.size();i++)
    {
        ui.ch6OptComboBox->addItem(m_ch6ValueToTextList[i].second);
    }

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

    for (int i=0;i<m_ch78ValueToTextList.size();i++)
    {
        ui.ch7OptComboBox->addItem(m_ch78ValueToTextList[i].second);
        ui.ch8OptComboBox->addItem(m_ch78ValueToTextList[i].second);
    }

    initConnections();
}
void CopterPidConfig::lockCheckBoxClicked(bool checked)
{
    m_pitchRollLocked = checked;
}
void CopterPidConfig::stabilLockedChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(ui.stabilPitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
        disconnect(ui.stabilRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
        ui.stabilPitchPSpinBox->setValue(value);
        ui.stabilRollPSpinBox->setValue(value);
        connect(ui.stabilPitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
        connect(ui.stabilRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(stabilLockedChanged(double)));
    }
}
void CopterPidConfig::ratePChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(ui.ratePitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
        disconnect(ui.rateRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
        ui.ratePitchPSpinBox->setValue(value);
        ui.rateRollPSpinBox->setValue(value);
        connect(ui.ratePitchPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
        connect(ui.rateRollPSpinBox,SIGNAL(valueChanged(double)),this,SLOT(ratePChanged(double)));
    }
}
void CopterPidConfig::rateIChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(ui.ratePitchISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
        disconnect(ui.rateRollISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
        ui.ratePitchISpinBox->setValue(value);
        ui.rateRollISpinBox->setValue(value);
        connect(ui.ratePitchISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
        connect(ui.rateRollISpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIChanged(double)));
    }
}
void CopterPidConfig::rateDChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(ui.ratePitchDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
        disconnect(ui.rateRollDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
        ui.ratePitchDSpinBox->setValue(value);
        ui.rateRollDSpinBox->setValue(value);
        connect(ui.ratePitchDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
        connect(ui.rateRollDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateDChanged(double)));
    }
}
void CopterPidConfig::rateIMAXChanged(double value)
{
    if (m_pitchRollLocked)
    {
        disconnect(ui.ratePitchIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
        disconnect(ui.rateRollIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
        ui.ratePitchIMAXSpinBox->setValue(value);
        ui.rateRollIMAXSpinBox->setValue(value);
        connect(ui.ratePitchIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
        connect(ui.rateRollIMAXSpinBox,SIGNAL(valueChanged(double)),this,SLOT(rateIMAXChanged(double)));
    }
}

CopterPidConfig::~CopterPidConfig()
{
}
void CopterPidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (m_nameToBoxMap.contains(parameterName))
    {
        m_nameToBoxMap[parameterName]->setValue(value.toDouble());
    }
    else if (parameterName == "TUNE_HIGH")
    {
        ui.ch6MaxSpinBox->setValue(value.toDouble() / 1000.0);
    }
    else if (parameterName == "TUNE_LOW")
    {
        ui.ch6MinSpinBox->setValue(value.toDouble() / 1000.0);
    }
    else if (parameterName == "TUNE")
    {
        for (int i=0;i<m_ch6ValueToTextList.size();i++)
        {
            if (m_ch6ValueToTextList[i].first == value.toInt())
            {
                ui.ch6OptComboBox->setCurrentIndex(i);
            }
        }
    }
    else if (parameterName == "CH7_OPT")
    {
        for (int i=0;i<m_ch78ValueToTextList.size();i++)
        {
            if (m_ch78ValueToTextList[i].first == value.toInt())
            {
                ui.ch7OptComboBox->setCurrentIndex(i);
            }
        }
    }
    else if (parameterName == "CH8_OPT")
    {
        for (int i=0;i<m_ch78ValueToTextList.size();i++)
        {
            if (m_ch78ValueToTextList[i].first == value.toInt())
            {
                ui.ch8OptComboBox->setCurrentIndex(i);
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
    for (QMap<QString,QDoubleSpinBox*>::const_iterator i=m_nameToBoxMap.constBegin();i!=m_nameToBoxMap.constEnd();i++)
    {
        m_uas->getParamManager()->setParameter(1,i.key(),i.value()->value());
    }

    m_uas->getParamManager()->setParameter(1,"TUNE",m_ch6ValueToTextList[ui.ch6OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1,"CH7_OPT",m_ch78ValueToTextList[ui.ch7OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1,"CH8_OPT",m_ch78ValueToTextList[ui.ch8OptComboBox->currentIndex()].first);
    m_uas->getParamManager()->setParameter(1,"TUNE_HIGH",ui.ch6MaxSpinBox->value() * 1000.0);
    m_uas->getParamManager()->setParameter(1,"TUNE_LOW",ui.ch6MinSpinBox->value() * 1000.0);
}

void CopterPidConfig::refreshButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    for (QMap<QString,QDoubleSpinBox*>::const_iterator i=m_nameToBoxMap.constBegin();i!=m_nameToBoxMap.constEnd();i++)
    {
        m_uas->getParamManager()->requestParameterUpdate(1,i.key());
    }
    m_uas->getParamManager()->requestParameterUpdate(1,"TUNE");
    m_uas->getParamManager()->requestParameterUpdate(1,"CH7_OPT");
    m_uas->getParamManager()->requestParameterUpdate(1,"CH8_OPT");
    m_uas->getParamManager()->requestParameterUpdate(1,"TUNE_HIGH");
    m_uas->getParamManager()->requestParameterUpdate(1,"TUNE_LOW");
}
