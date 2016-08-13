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

#ifndef BASICPIDCONFIG_H
#define BASICPIDCONFIG_H

#include "AP2ConfigWidget.h"
#include "ui_BasicPidConfig.h"
class ParamWidget;
class BasicPidConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit BasicPidConfig(QWidget *parent = 0);
    ~BasicPidConfig();
private slots:
    void rPRCValueChanged(QString name,double value);
    void tAValueChanged(QString name,double value);
    void tHValueChanged(QString name,int value);
    void rcFeelValueChanged(QString name, int value);
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);

private:
    void showEvent(QShowEvent *evt);
    void hideEvent(QHideEvent *evt);

    void requestParameterUpdate();

private:
    Ui::BasicPidConfig ui;
    ParamWidget *m_rcFeelWidget;
    ParamWidget *m_throttleHoverWidget;
    ParamWidget *m_rollPitchRateWidget;
    ParamWidget *m_throttleAccelWidget;

    QString rate_rll_p;
    QString rate_rll_i;
    QString rate_rll_d;

    QString rate_pit_p;
    QString rate_pit_i;
    QString rate_pit_d;

    QString thr_accel_p;
    QString thr_accel_i;

    bool m_use_mot_thst_hover; // if AC3.4-rc2+ use this varible set.
};

#endif // BASICPIDCONFIG_H
