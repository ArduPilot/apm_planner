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

#include "ArduPlanePidConfig.h"

#include "QGCCore.h"

ArduPlanePidConfig::ArduPlanePidConfig(QWidget *parent) : AP2ConfigWidget(parent)
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

    m_nameToBoxMap["RLL2SRV_P"] = ui.servoRollPSpinBox;
    m_nameToBoxMap["RLL2SRV_I"] = ui.servoRollISpinBox;
    m_nameToBoxMap["RLL2SRV_D"] = ui.servoRollDSpinBox;
    m_nameToBoxMap["RLL2SRV_IMAX"] = ui.servoRollIMAXSpinBox;

    m_nameToBoxMap["PTCH2SRV_P"] = ui.servoPitchPSpinBox;
    m_nameToBoxMap["PTCH2SRV_I"] = ui.servoPitchISpinBox;
    m_nameToBoxMap["PTCH2SRV_D"] = ui.servoPitchDSpinBox;
    m_nameToBoxMap["PTCH2SRV_IMAX"] = ui.servoPitchIMAXSpinBox;

    m_nameToBoxMap["YW2SRV_P"] = ui.servoYawPSpinBox;
    m_nameToBoxMap["YW2SRV_I"] = ui.servoYawISpinBox;
    m_nameToBoxMap["YW2SRV_D"] = ui.servoYawDSpinBox;
    m_nameToBoxMap["YW2SRV_IMAX"] = ui.servoYawIMAXSpinBox;

    m_nameToBoxMap["ALT2PTCH_P"] = ui.navAltPSpinBox;
    m_nameToBoxMap["ALT2PTCH_I"] = ui.navAltISpinBox;
    m_nameToBoxMap["ALT2PTCH_D"] = ui.navAltDSpinBox;
    m_nameToBoxMap["ALT2PTCH_IMAX"] = ui.navAltIMAXSpinBox;

    m_nameToBoxMap["ARSP2PTCH_P"] = ui.navASPSpinBox;
    m_nameToBoxMap["ARSP2PTCH_I"] = ui.navASISpinBox;
    m_nameToBoxMap["ARSP2PTCH_D"] = ui.navASDSpinBox;
    m_nameToBoxMap["ARSP2PTCH_IMAX"] = ui.navASIMAXSpinBox;

    m_nameToBoxMap["ENRGY2THR_P"] = ui.energyPSpinBox;
    m_nameToBoxMap["ENRGY2THR_I"] = ui.energyISpinBox;
    m_nameToBoxMap["ENRGY2THR_D"] = ui.energyDSpinBox;
    m_nameToBoxMap["ENRGY2THR_IMAX"] = ui.energyIMAXSpinBox;

    m_nameToBoxMap["KFF_PTCH2THR"] = ui.otherPitchCompSpinBox;
    m_nameToBoxMap["KFF_PTCHCOMP"] = ui.otherPtTSpinBox;
    m_nameToBoxMap["KFF_RDDRMIX"] = ui.otherRudderMixSpinBox;

    m_nameToBoxMap["TRIM_THROTTLE"] = ui.throttleCruiseSpinBox;
    m_nameToBoxMap["THR_FS_VALUE"] = ui.throttleFSSpinBox;
    m_nameToBoxMap["THR_MAX"] = ui.throttleMaxSpinBox;
    m_nameToBoxMap["THR_MIN"] = ui.throttleMinSpinBox;

    m_nameToBoxMap["TRIM_ARSPD_CM"] = ui.airspeedCruiseSpinBox;
    m_nameToBoxMap["ARSPD_FBW_MAX"] = ui.airspeedFBWMaxSpinBox;
    m_nameToBoxMap["ARSPD_FBW_MIN"] = ui.airspeedFBWMinSpinBox;
    m_nameToBoxMap["ARSPD_RATIO"] = ui.airspeedRatioSpinBox;

    m_nameToBoxMap["NAVL1_DAMPING"] = ui.l1DampingSpinBox;
    m_nameToBoxMap["NAVL1_PERIOD"] = ui.l1PeriodSpinBox;

    m_nameToBoxMap["LIM_ROLL_CD"] = ui.navBankMaxSpinBox;
    m_nameToBoxMap["LIM_PITCH_MAX"] = ui.navPitchMaxSpinBox;
    m_nameToBoxMap["LIM_PITCH_MIN"] = ui.navPitchMinSpinBox;

    connect(ui.writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui.refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));
    initConnections();
}

ArduPlanePidConfig::~ArduPlanePidConfig()
{
}
void ArduPlanePidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (m_nameToBoxMap.contains(parameterName))
    {
        m_nameToBoxMap[parameterName]->setValue(value.toDouble());
    }
}
void ArduPlanePidConfig::writeButtonClicked()
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
}

void ArduPlanePidConfig::refreshButtonClicked()
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

}
