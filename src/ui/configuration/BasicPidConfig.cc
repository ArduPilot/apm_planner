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

#include "BasicPidConfig.h"
#include "ParamWidget.h"

BasicPidConfig::BasicPidConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    m_rPRCWidget = new ParamWidget("RollPitchRateControl",this);
    ui.verticalLayout->addWidget(m_rPRCWidget);
    connect(m_rPRCWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(rPRCValueChanged(QString,double)));
    m_rPRCWidget->setupDouble(QString("Roll/Pitch Rate Control ") + "(" + "RATE_RLL_P/RATE_PIT_P/RATE_PIT_I" + ")","How much thrust is applied to rotate the copter at the desired speed",0,0.08,0.2,0.01);
    m_rPRCWidget->show();


    m_rPRDWidget = new ParamWidget("RollPitchRateDamp",this);
    ui.verticalLayout->addWidget(m_rPRDWidget);
    connect(m_rPRDWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(rPRDValueChanged(QString,double)));
    m_rPRDWidget->setupDouble(QString("Roll/Pitch Rate Dampening ") + "(" + "RATE_PIT_D" + ")","How much dampening applies to the rate controller",0,0.001,0.008,0.0005);
    m_rPRDWidget->show();


    m_yARCWidget = new ParamWidget("YawAngleRateControl",this);
    ui.verticalLayout->addWidget(m_yARCWidget);
    connect(m_yARCWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(yARCValueChanged(QString,double)));
    m_yARCWidget->setupDouble(QString("Yaw Angular Rate Control ") + "(" + "RATE_YAW_I" + ")","How much thrust is applied to rotate the copter at the desired speed",0,0.08,0.4,0.01);
    m_yARCWidget->show();

    m_tAWidget = new ParamWidget("ThrottleAccel",this);
    ui.verticalLayout->addWidget(m_tAWidget);
    connect(m_tAWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(tAValueChanged(QString,double)));
    m_tAWidget->setupDouble(QString("Throttle Accel ") + "(" + "THR_ACCEL_I" + ")","how much thrust to give us the desired accel. This will change depending on the weight and thrust of your copter",0,0.3,1.0,0.01);
    m_tAWidget->show();

    m_tHWidget = new ParamWidget("ThrottleHover",this);
    ui.verticalLayout->addWidget(m_tHWidget);
    connect(m_tHWidget,SIGNAL(intValueChanged(QString,int)),this,SLOT(tHValueChanged(QString,int)));
    m_tHWidget->setupInt(QString("Throttle Hover ") + "(" + "THR_MID" + ")","How much throttle is needed to maintain a steady hover",480,200,800);
    m_tHWidget->show();

    initConnections();
}
void BasicPidConfig::rPRCValueChanged(QString name,double value)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"RATE_RLL_P",value);
    m_uas->getParamManager()->setParameter(1,"RATE_RLL_I",value);
    m_uas->getParamManager()->setParameter(1,"RATE_PIT_P",value);
    m_uas->getParamManager()->setParameter(1,"RATE_PIT_I",value);
}

void BasicPidConfig::rPRDValueChanged(QString name,double value)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"RATE_RLL_D",value);
    m_uas->getParamManager()->setParameter(1,"RATE_PIT_D",value);
}

void BasicPidConfig::yARCValueChanged(QString name,double value)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"RATE_YAW_P",value);
    m_uas->getParamManager()->setParameter(1,"RATE_RAW_I",value * 0.1);
}

void BasicPidConfig::tAValueChanged(QString name,double value)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"THR_ACCEL_P",value);
    m_uas->getParamManager()->setParameter(1,"THR_ACCEL_I",value*2.0);
}
void BasicPidConfig::tHValueChanged(QString name,int value)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"THR_MID",value);
}

BasicPidConfig::~BasicPidConfig()
{
}
void BasicPidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName == "RATE_RLL_P")
    {
        m_rPRCWidget->setValue(value.toDouble());
    }
    else if (parameterName == "RATE_RLL_D")
    {
        m_rPRDWidget->setValue(value.toDouble());
    }
    else if (parameterName == "RATE_YAW_P")
    {
        m_yARCWidget->setValue(value.toDouble());
    }
    else if (parameterName == "THR_ACCEL_P")
    {
        m_tAWidget->setValue(value.toDouble());
    }
    else if (parameterName == "THR_MID")
    {
        m_tHWidget->setValue(value.toInt());
    }
}
