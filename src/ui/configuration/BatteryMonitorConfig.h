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
 *   @brief Battery monitor select and configuration
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef BATTERYMONITORCONFIG_H
#define BATTERYMONITORCONFIG_H

#include <QWidget>
#include "AP2ConfigWidget.h"
#include "ui_BatteryMonitorConfig.h"

class BatteryMonitorConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit BatteryMonitorConfig(QWidget *parent = 0);
    ~BatteryMonitorConfig();
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void monitorCurrentIndexChanged(int index);
    void sensorCurrentIndexChanged(int index);
    void apmVerCurrentIndexChanged(int index);
    void calcDividerSet();
    void measuredVoltsSet();
    void measuredCurrentSet();
    void ampsPerVoltSet();
    void batteryCapacitySet();
    void alertOnLowClicked(bool checked);
    void activeUASSet(UASInterface *uas);
    void batteryChanged(UASInterface* uas, double voltage, double current, double percent, int seconds);
private:
    QString m_voltDividerParam;
    QString m_ampPerVoltParam;
    float m_maxVoltOut; // ie. 3.3V or 5.0V
    double m_savedAmpsPerVolts;
    double m_savedVoltDivider;
    Ui::BatteryMonitorConfig ui;
    void checkSensorType();
    inline float calculatemVPerAmp(float maxvoltsout,float maxamps);
    inline float calculatemVPerVolt(float maxvoltsout,float maxvolts);
};

#endif // BATTERYMONITORCONFIG_H
