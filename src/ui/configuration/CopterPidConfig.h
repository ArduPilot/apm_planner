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
 *          for version Copter3.3+
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Bill Bonney Carpenter <billbonney@communistech.com>
 */

#ifndef COPTERPIDCONFIG_H
#define COPTERPIDCONFIG_H

#include <QWidget>
#include "ui_CopterPidConfig.h"

#include "AP2ConfigWidget.h"

class CopterPidConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit CopterPidConfig(QWidget *parent = 0);
    ~CopterPidConfig();
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
    void showEvent(QShowEvent *evt);

    void mapParamNamesToBox();
    void requestParameterUpdate();

private:
    bool m_pitchRollLocked;
    QList<QPair<int,QString> > m_ch6ValueToTextList;
    QList<QPair<int,QString> > m_ch78ValueToTextList;
    QMap<QString,QDoubleSpinBox*> m_nameToBoxMap;
    Ui::CopterPidConfig ui;

    QString stb_rll_p;
    QString stb_pit_p;
    QString stb_yaw_p;

    QString rate_rll_p;
    QString rate_rll_i;
    QString rate_rll_imax;
    QString rate_rll_d;
    QString rate_rll_filt_hz;

    QString rate_pit_p;
    QString rate_pit_i;
    QString rate_pit_imax;
    QString rate_pit_d;
    QString rate_pit_filt_hz;

    QString rate_yaw_p;
    QString rate_yaw_i;
    QString rate_yaw_imax;
    QString rate_yaw_d;
    QString rate_yaw_filt_hz;

};

#endif // COPTERPIDCONFIG_H
