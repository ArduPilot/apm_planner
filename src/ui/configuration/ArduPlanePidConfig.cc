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

    addParamToMap("RLL2SRV_P", ui.servoRollPSpinBox, 1.0);
    addParamToMap("RLL2SRV_I", ui.servoRollISpinBox, 1.0);
    addParamToMap("RLL2SRV_D", ui.servoRollDSpinBox, 1.0);
    addParamToMap("RLL2SRV_IMAX", ui.servoRollIMAXSpinBox, 0.01); // centi-degrees -> degrees

    addParamToMap("PTCH2SRV_P", ui.servoPitchPSpinBox, 1.0);
    addParamToMap("PTCH2SRV_I", ui.servoPitchISpinBox, 1.0);
    addParamToMap("PTCH2SRV_D", ui.servoPitchDSpinBox, 1.0);
    addParamToMap("PTCH2SRV_IMAX", ui.servoPitchIMAXSpinBox, 0.01); // centi-degrees -> degrees

    addParamToMap("YAW2SRV_RLL", ui.servoYawPSpinBox, 1.0);
    addParamToMap("YAW2SRV_INT", ui.servoYawISpinBox, 1.0);
    addParamToMap("YAW2SRV_DAMP", ui.servoYawDSpinBox, 1.0);
    addParamToMap("YAW2SRV_IMAX", ui.servoYawIMAXSpinBox, 0.01); // centi-degrees -> degrees

    addParamToMap("KFF_PTCH2THR", ui.otherPitchCompSpinBox, 1.0);
    addParamToMap("KFF_PTCHCOMP", ui.otherPtTSpinBox, 1.0);
    addParamToMap("KFF_RDDRMIX", ui.otherRudderMixSpinBox, 1.0);

    addParamToMap("TRIM_THROTTLE", ui.throttleCruiseSpinBox, 1.0);
    addParamToMap("THR_FS_VALUE", ui.throttleFSSpinBox, 1.0);
    addParamToMap("THR_MAX", ui.throttleMaxSpinBox, 1.0);
    addParamToMap("THR_MIN", ui.throttleMinSpinBox, 1.0);

    addParamToMap("TRIM_ARSPD_CM", ui.airspeedCruiseSpinBox, 0.01); // cm/s -> m/s
    addParamToMap("ARSPD_FBW_MAX", ui.airspeedFBWMaxSpinBox, 1.0);
    addParamToMap("ARSPD_FBW_MIN", ui.airspeedFBWMinSpinBox, 1.0);
    addParamToMap("ARSPD_RATIO", ui.airspeedRatioSpinBox, 1.0);

    addParamToMap("NAVL1_DAMPING", ui.l1DampingSpinBox, 1.0);
    addParamToMap("NAVL1_PERIOD", ui.l1PeriodSpinBox, 1.0);

    addParamToMap("LIM_ROLL_CD", ui.navBankMaxSpinBox, 0.01);    // centi-degrees -> degrees
    addParamToMap("LIM_PITCH_MAX", ui.navPitchMaxSpinBox, 0.01); // centi-degrees -> degrees
    addParamToMap("LIM_PITCH_MIN", ui.navPitchMinSpinBox, 0.01); // centi-degrees -> degrees

    addParamToMap("TECS_CLMB_MAX", ui.climbMaxSpinBox, 1.0);
    addParamToMap("TECS_SINK_MIN", ui.sinkMinSpinBox, 1.0);
    addParamToMap("TECS_SINK_MAX", ui.sinkMaxSpinBox, 1.0);
    addParamToMap("TECS_PTCH_DAMP", ui.pitchDampSpinBox, 1.0);
    addParamToMap("TECS_TIME_CONST", ui.timeConstSpinBox, 1.0);

    connect(ui.writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui.refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));
    initConnections();
}

void ArduPlanePidConfig::addParamToMap(const QString& paramName, QDoubleSpinBox* spinBox, double scalar)
{
    QPair<QDoubleSpinBox*, double> tuple;
    tuple.first = spinBox;
    tuple.second = scalar;
    m_nameToBoxMap.insert(paramName, tuple);
}


ArduPlanePidConfig::~ArduPlanePidConfig()
{
}

void ArduPlanePidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if (m_nameToBoxMap.contains(parameterName))
    {
        QDoubleSpinBox* spinBox = m_nameToBoxMap[parameterName].first;
        double scalar = m_nameToBoxMap[parameterName].second;
        spinBox->setValue(value.toDouble()*scalar);
    }
}
void ArduPlanePidConfig::writeButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    QMap<QString, QPair<QDoubleSpinBox*, double> >::const_iterator i = m_nameToBoxMap.constBegin();
    while(i != m_nameToBoxMap.constEnd()) {
        QDoubleSpinBox* spinBox = i.value().first;
        double scalar = i.value().second;
        m_uas->getParamManager()->setParameter(1,i.key(),spinBox->value()/scalar);
        ++i;
    }
}

void ArduPlanePidConfig::refreshButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    QMap<QString, QPair<QDoubleSpinBox*, double> >::const_iterator i = m_nameToBoxMap.constBegin();
    while(i != m_nameToBoxMap.constEnd()) {
        m_uas->getParamManager()->requestParameterUpdate(1,i.key());
        ++i;
    }
}
