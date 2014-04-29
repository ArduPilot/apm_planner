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

#include <QsLog.h>
#include "BasicPidConfig.h"
#include "ParamWidget.h"

BasicPidConfig::BasicPidConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    m_rollPitchRateWidget = new ParamWidget("RollPitchRateControl",this);
    ui.verticalLayout->insertWidget(0,m_rollPitchRateWidget);
    connect(m_rollPitchRateWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(rPRCValueChanged(QString,double)));
    m_rollPitchRateWidget->setupDouble(QString("Roll/Pitch Rate Control"),
                                       "Slide to the right if the copter is sluggish or slide to the left if the copter is twitchy.",0.15,0.08,0.4,0.01);
    m_rollPitchRateWidget->show();

    m_rcFeelWidget = new ParamWidget("RcFeelControl",this);
    ui.verticalLayout->insertWidget(1,m_rcFeelWidget);
    connect(m_rcFeelWidget,SIGNAL(intValueChanged(QString,int)),this,SLOT(rcFeelValueChanged(QString,int)));
    m_rcFeelWidget->setupInt(QString("RC Feel"),
        tr("RC feel for roll/pitch which controls vehicle response to user input with 0 being extremely soft and 100 being crisp.\nVery Soft=0   Soft=25   Medium=50   Crisp=75   Very Crisp=100")
                                       ,50,0,100);
    m_rcFeelWidget->show();

    m_throttleHoverWidget = new ParamWidget("ThrottleHover",this);
    ui.verticalLayout->insertWidget(2,m_throttleHoverWidget);
    connect(m_throttleHoverWidget,SIGNAL(intValueChanged(QString,int)),this,SLOT(tHValueChanged(QString,int)));
    m_throttleHoverWidget->setupInt(QString("Throttle Hover "),
                                    "How much throttle is needed to maintain a steady hover.",480,300,700);
    m_throttleHoverWidget->show();

    m_throttleAccelWidget = new ParamWidget("ThrottleAccel",this);
    ui.verticalLayout->insertWidget(3,m_throttleAccelWidget);
    connect(m_throttleAccelWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(tAValueChanged(QString,double)));
    m_throttleAccelWidget->setupDouble(QString("Throttle Accel"),
                                       "Slide to the right to climb more aggressively or slide to the left to climb more gently.",0.75,0.3,1.0,0.05);
    m_throttleAccelWidget->show();

    initConnections();
}

void BasicPidConfig::rPRCValueChanged(QString name,double value)
{
    Q_UNUSED(name);

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

void BasicPidConfig::tAValueChanged(QString name,double value)
{
    Q_UNUSED(name);

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
    Q_UNUSED(name);

    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"THR_MID",value);
}

void BasicPidConfig::rcFeelValueChanged(QString name, int value)
{
    Q_UNUSED(name);

    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"RC_FEEL_RP",value);
}

BasicPidConfig::~BasicPidConfig()
{
}

void BasicPidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if (parameterName == "RATE_RLL_P")
    {
        QLOG_DEBUG() << "BasicPID: RATE_RLL_P:" << value.toDouble();
        m_rollPitchRateWidget->setValue(value.toDouble());
    }
    else if (parameterName == "RATE_RLL_D")
    {
        QLOG_DEBUG() << "BasicPID: RATE_RLL_D:" << value.toDouble();
        m_rollPitchRateWidget->setValue(value.toDouble());
    }
    else if (parameterName == "THR_ACCEL_P")
    {
         QLOG_DEBUG() << "BasicPID: THR_ACCEL_P:" << value.toDouble();
        m_throttleAccelWidget->setValue(value.toDouble());
    }
    else if (parameterName == "THR_MID")
    {
         QLOG_DEBUG() << "BasicPID: THR_MID:" << value.toInt();
        m_throttleHoverWidget->setValue(value.toInt());
    }
    else if (parameterName == "RC_FEEL_RP")
    {
        QLOG_DEBUG() << "BasicPID: RC_FEEL:" << value.toInt();
        m_rcFeelWidget->setValue(value.toInt());
    }
}
