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

#include <logging.h>
#include "BasicPidConfig.h"
#include "ParamWidget.h"
#include "ArduPilotMegaMAV.h"
#include <QSettings>

BasicPidConfig::BasicPidConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_use_mot_thst_hover(false)
{
    ui.setupUi(this);

    m_rcFeelWidget = new ParamWidget("RcFeelControl",this);
    ui.verticalLayout->insertWidget(0,m_rcFeelWidget);
    connect(m_rcFeelWidget,SIGNAL(intValueChanged(QString,int)),this,SLOT(rcFeelValueChanged(QString,int)));
    m_rcFeelWidget->setupInt(QString("RC Feel Roll/Pitch"),
        tr("Slide left for softer response to RC input and right for crisper with 0 being extremely soft and 100 being crisp.\nVery Soft=0   Soft=25   Medium=50   Crisp=75   Very Crisp=100")
                                       ,50,0,100);
    m_rcFeelWidget->show();

    m_rollPitchRateWidget = new ParamWidget("RollPitchSensitivity",this);
    ui.verticalLayout->insertWidget(1,m_rollPitchRateWidget);
    connect(m_rollPitchRateWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(rPRCValueChanged(QString,double)));
    m_rollPitchRateWidget->setupDouble(QString("Roll/Pitch Sensitivity"),
                                       "Slide to the right if the copter is sluggish or slide to the left if the copter is twitchy.",0.15,0.08,0.4,0.01);
    m_rollPitchRateWidget->show();

    m_throttleHoverWidget = new ParamWidget("ThrottleHover (%)",this);
    ui.verticalLayout->insertWidget(2,m_throttleHoverWidget);
    connect(m_throttleHoverWidget,SIGNAL(intValueChanged(QString,int)),this,SLOT(tHValueChanged(QString,int)));
    m_throttleHoverWidget->setupInt(QString("Throttle Hover "),
                                    "How much throttle is needed to maintain a steady hover.",48,25,80);
    m_throttleHoverWidget->show();

    m_throttleAccelWidget = new ParamWidget("ThrottleAccel",this);
    ui.verticalLayout->insertWidget(3,m_throttleAccelWidget);
    connect(m_throttleAccelWidget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(tAValueChanged(QString,double)));
    m_throttleAccelWidget->setupDouble(QString("Throttle Sensitivity (Throttle Accel)"),
                                       "Slide to the right to climb more aggressively or slide to the left to climb more gently.",0.75,0.3,1.0,0.05);
    m_throttleAccelWidget->show();

    mapParamNames();
    initConnections();
}

void BasicPidConfig::requestParameterUpdate()
{
    if (!m_uas) return;
    // The List of Params we care about

    QStringList params;
    params << rate_pit_p
            << rate_pit_i
            << rate_pit_d
            << rate_rll_p
            << rate_rll_i
            << rate_rll_d
            << thr_accel_p
            << thr_accel_i
            << thr_mid
            << mot_thrust_hover
            << rc_feel_rp;

    m_use_mot_thst_hover = false; // Set true is MOT_THST_HOVER retruns a value.

    QLOG_DEBUG() << "Basic Tuning Params (fetch): " << params;
    QGCUASParamManager *pm = m_uas->getParamManager();
    for (const auto &parameter : params)
    {
        pm->requestParameterUpdate(1, parameter);
    };
}

void BasicPidConfig::mapParamNames()
{
    // TODO: Move to baseclass and syncronize with CopterPidConfig class
    // as it uses exactly the same mechanics
    // version check
    ArduPilotMegaMAV* apmMav = dynamic_cast<ArduPilotMegaMAV*>(m_uas);
    if (apmMav == nullptr)
    {
        QLOG_INFO() << "BasicPidConfig Class only supports ArduPilotMegaMAV vehicles.";
        return;
    }

    APMFirmwareVersion version = apmMav->getFirmwareVersion();
    QLOG_DEBUG() << "Basic Tuning - Config is set up for " << version.versionString();

    if(version.majorNumber() == 3)
    {
        if(version.minorNumber() >= 6)
        {
            QLOG_DEBUG() << "Using parameter set for ArduCopter 3.6+";
            rate_rll_p = "ATC_RAT_RLL_P"; //  "RATE_RLL_P";
            rate_rll_i = "ATC_RAT_RLL_I"; //  "RATE_RLL_I";
            rate_rll_d = "ATC_RAT_RLL_D"; //  "RATE_RLL_D";

            rate_pit_p = "ATC_RAT_PIT_P"; //  "RATE_PIT_P";
            rate_pit_i = "ATC_RAT_PIT_I"; //  "RATE_PIT_I";
            rate_pit_d = "ATC_RAT_PIT_D"; //  "RATE_PIT_D";

            thr_accel_p = "PSC_ACCZ_P"; // THR_ACCEL_P
            thr_accel_i = "PSC_ACCZ_I";  // THR_ACCEL_I
            thr_mid     = "THR_MID";

            mot_thrust_hover = "MOT_THST_HOVER";
            rc_feel_rp       = "ATC_INPUT_TC";      // since ArduCopter 3.6

            // has new scaling and different description since ArduCopter 3.6
            m_rcFeelWidget->setupDouble(QString("RC Feel Roll/Pitch"), tr("Attitude control input time constant (aka smoothing). Low numbers lead to sharper response, higher numbers to softer response.\nVery Smooth=1.0 / No Smothing=0.0")
                                               ,0.5,0.0,1.0,0.000001);

        }
        else if(version.minorNumber() >= 4)
        {
            QLOG_DEBUG() << "Using parameter set for ArduCopter 3.4+";

            rate_rll_p = "ATC_RAT_RLL_P"; //  "RATE_RLL_P";
            rate_rll_i = "ATC_RAT_RLL_I"; //  "RATE_RLL_I";
            rate_rll_d = "ATC_RAT_RLL_D"; //  "RATE_RLL_D";

            rate_pit_p = "ATC_RAT_PIT_P"; //  "RATE_PIT_P";
            rate_pit_i = "ATC_RAT_PIT_I"; //  "RATE_PIT_I";
            rate_pit_d = "ATC_RAT_PIT_D"; //  "RATE_PIT_D";

            thr_accel_p = "ACCEL_Z_P"; // THR_ACCEL_P
            thr_accel_i = "ACCEL_Z_I"; // THR_ACCEL_I
            thr_mid     = "THR_MID";

            mot_thrust_hover = "MOT_THST_HOVER";
            rc_feel_rp       = "RC_FEEL_RP";
        }
    }
    else
    {
        QLOG_DEBUG() << "Using default parameter set";

        rate_rll_p = "RATE_RLL_P";
        rate_rll_i = "RATE_RLL_I";
        rate_rll_d = "RATE_RLL_D";

        rate_pit_p = "RATE_PIT_P";
        rate_pit_i = "RATE_PIT_I";
        rate_pit_d = "RATE_PIT_D";

        thr_accel_p = "THR_ACCEL_P";
        thr_accel_i = "THR_ACCEL_I";
        thr_mid     = "THR_MID";

        mot_thrust_hover = "MOT_THST_HOVER";
        rc_feel_rp       = "RC_FEEL_RP";

    }
}

void BasicPidConfig::showEvent(QShowEvent *evt)
{
    mapParamNames();
    requestParameterUpdate();
    QWidget::showEvent(evt);
}

void BasicPidConfig::hideEvent(QHideEvent *evt)
{
    QWidget::hideEvent(evt);
}

void BasicPidConfig::rPRCValueChanged(QString name,double value)
{
    Q_UNUSED(name);

    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    m_uas->getParamManager()->setParameter(1,rate_rll_p,value);
    m_uas->getParamManager()->setParameter(1,rate_rll_i,value);
    m_uas->getParamManager()->setParameter(1,rate_pit_p,value);
    m_uas->getParamManager()->setParameter(1,rate_pit_i,value);
}

void BasicPidConfig::tAValueChanged(QString name,double value)
{
    Q_UNUSED(name);

    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,thr_accel_p,value);
    m_uas->getParamManager()->setParameter(1,thr_accel_i,value*2.0);
}

void BasicPidConfig::tHValueChanged(QString name,int value)
{
    Q_UNUSED(name);

    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    if (m_use_mot_thst_hover) {
        float mot_thst_hover = value / 100.0;
        QLOG_DEBUG() << mot_thrust_hover << ":" << mot_thst_hover;
        m_uas->getParamManager()->setParameter(1, mot_thrust_hover, mot_thst_hover);
    } else {
        float throtle_mid = value * 10.0;
        QLOG_DEBUG() << thr_mid << ":" << throtle_mid;
        m_uas->getParamManager()->setParameter(1, thr_mid, throtle_mid);
    }

}

void BasicPidConfig::rcFeelValueChanged(QString name, int value)
{
    Q_UNUSED(name);

    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1, rc_feel_rp, value);
}

BasicPidConfig::~BasicPidConfig()
{
}

void BasicPidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if (parameterName == rate_rll_p)
    {
        QLOG_DEBUG() << "BasicPID: " << parameterName << ":" << value.toDouble();
        m_rollPitchRateWidget->setValue(value.toDouble());
    }
    else if (parameterName == thr_accel_p)
    {
        QLOG_DEBUG() << "BasicPID: " << parameterName << ":" << value.toDouble();
        m_throttleAccelWidget->setValue(value.toDouble());
    }
    else if (parameterName == thr_mid)
    {
        QLOG_DEBUG() << "BasicPID: " << parameterName << ":" << value.toInt();
        m_throttleHoverWidget->setValue(value.toFloat() / 10.0);
        m_use_mot_thst_hover = false;
    }
    else if (parameterName == mot_thrust_hover)
    {
        QLOG_DEBUG() << "BasicPID: " << parameterName << ":" << value.toFloat();
        m_throttleHoverWidget->setValue(value.toFloat() * 100.0);
        m_use_mot_thst_hover = true;
    }
    else if (parameterName == rc_feel_rp)
    {
        QLOG_DEBUG() << "BasicPID: " << parameterName << ":" << value.toDouble();
        m_rcFeelWidget->setValue(value.toDouble());
    }
}
