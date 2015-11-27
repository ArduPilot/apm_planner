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

#include "ArduRoverPidConfig.h"

#include "QGCCore.h"

ArduRoverPidConfig::ArduRoverPidConfig(QWidget *parent) : AP2ConfigWidget(parent)
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


    nameToBoxMap["STEER2SRV_P"] = ui.steer2ServoPSpinBox;
    nameToBoxMap["STEER2SRV_I"] = ui.steer2ServoISpinBox;
    nameToBoxMap["STEER2SRV_D"] = ui.steer2ServoDSpinBox;
    nameToBoxMap["STEER2SRV_IMAX"] = ui.steer2ServoIMAXSpinBox;

    nameToBoxMap["NAVL1_PERIOD"] = ui.L1PeriodSpinBox;
    nameToBoxMap["NAVL1_DAMPING"] = ui.L1DampeningSpinBox;

    nameToBoxMap["CRUISE_THROTTLE"] = ui.throttleCruiseSpinBox;
    nameToBoxMap["THR_MIN"] = ui.throttleMinSpinBox;
    nameToBoxMap["THR_MAX"] = ui.throttleMaxSpinBox;
    nameToBoxMap["FS_THR_VALUE"] = ui.throttleFSSpinBox;

    nameToBoxMap["SPEED2THR_P"] = ui.speed2ThrottlePSpinBox;
    nameToBoxMap["SPEED2THR_I"] = ui.speed2ThrottleISpinBox;
    nameToBoxMap["SPEED2THR_D"] = ui.speed2ThrottleDSpinBox;
    nameToBoxMap["SPEED2THR_IMAX"] = ui.speed2ThrottleIMAXSpinBox;

    nameToBoxMap["CRUISE_SPEED"] = ui.roverCruiseSpinBox;
    nameToBoxMap["SPEED_TURN_GAIN"] = ui.roverTurnSpeedSpinBox;
    nameToBoxMap["SPEED_TURN_DIST"] = ui.roverTurnDistSpinBox;
    nameToBoxMap["WP_RADIUS"] = ui.roverWPRadiusSpinBox;

    nameToBoxMap["SONAR_TRIGGER_CM"] = ui.sonarTriggerSpinBox;
    nameToBoxMap["SONAR_TURN_ANGLE"] = ui.sonarTurnAngleSpinBox;
    nameToBoxMap["SONAR_TURN_TIME"] = ui.sonarTurnTimeSpinBox;
    nameToBoxMap["SONAR_DEBOUNCE"] = ui.sonaeDebounceSpinBox;

    connect(ui.writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui.refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));
    initConnections();
}

ArduRoverPidConfig::~ArduRoverPidConfig()
{
}
void ArduRoverPidConfig::writeButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    for (QMap<QString,QDoubleSpinBox*>::const_iterator i=nameToBoxMap.constBegin();i!=nameToBoxMap.constEnd();i++)
    {
        m_uas->getParamManager()->setParameter(1,i.key(),i.value()->value());
    }
}

void ArduRoverPidConfig::refreshButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    for (QMap<QString,QDoubleSpinBox*>::const_iterator i=nameToBoxMap.constBegin();i!=nameToBoxMap.constEnd();i++)
    {
        m_uas->getParamManager()->requestParameterUpdate(1,i.key());
    }
}

void ArduRoverPidConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas)
    Q_UNUSED(component)

    if (nameToBoxMap.contains(parameterName))
    {
        nameToBoxMap[parameterName]->setValue(value.toFloat());
    }
}
