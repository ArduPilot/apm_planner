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
 *   @brief PID configuration for ArduPlane (Fixed-wing air vehicle)
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */
#ifndef ARDUPLANEPIDCONFIG_H
#define ARDUPLANEPIDCONFIG_H

#include <QWidget>
#include "ui_ArduPlanePidConfig.h"
#include "AP2ConfigWidget.h"

class ArduPlanePidConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit ArduPlanePidConfig(QWidget *parent = 0);
    ~ArduPlanePidConfig();
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void writeButtonClicked();
    void refreshButtonClicked();

private:
    void addParamToMap(const QString& paramName, QDoubleSpinBox *spinBox, double scalar);
private:
    QMap<QString, QPair<QDoubleSpinBox* ,double> > m_nameToBoxMap; // Param = comboxBox + scalar
    Ui::ArduPlanePidConfig ui;
};

#endif // ARDUPLANEPIDCONFIG_H
