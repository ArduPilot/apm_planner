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
 *   @brief PID configuration for ArduCoptor (multi-rotor air vehicle)
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef ARDUCOPTERPIDCONFIG_H
#define ARDUCOPTERPIDCONFIG_H

#include <QWidget>
#include "ui_ArduCopterPidConfig.h"

#include "AP2ConfigWidget.h"

class ArduCopterPidConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit ArduCopterPidConfig(QWidget *parent = 0);
    ~ArduCopterPidConfig();
private slots:
    void writeButtonClicked();
    void refreshButtonClicked();
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void lockCheckBoxClicked(bool checked);
    void stabilLockedChanged(double value);
    void ratePChanged(double value);
    void rateIChanged(double value);
    void rateDChanged(double value);
    void rateIMAXChanged(double value);
private:
    bool m_pitchRollLocked;
    QList<QPair<int,QString> > m_ch6ValueToTextList;
    QList<QPair<int,QString> > m_ch78ValueToTextList;
    QMap<QString,QDoubleSpinBox*> m_nameToBoxMap;
    Ui::ArduCopterPidConfig ui;
};

#endif // ARDUCOPTERPIDCONFIG_H
