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
 *   @brief Fail Safe radio/gps configuration
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef FAILSAFECONFIG_H
#define FAILSAFECONFIG_H

#include <QWidget>
#include "ui_FailSafeConfig.h"
#include "AP2ConfigWidget.h"
class FailSafeConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit FailSafeConfig(QWidget *parent = 0);
    ~FailSafeConfig();
private slots:
    void activeUASSet(UASInterface *uas);
    void remoteControlChannelRawChanges(int chan,float value);
    void hilActuatorsChanged(uint64_t time, float act1, float act2, float act3, float act4, float act5, float act6, float act7, float act8);
    void armingChanged(bool armed);
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void batteryFailChecked(bool checked);
    void fsLongClicked(bool checked);
    void fsShortClicked(bool checked);
    void gcsChecked(bool checked);
    void throttleActionChecked(bool checked);
    void throttleChecked(bool checked);
    void throttlePwmChanged();
    void throttleFailSafeChanged(int index);
    void gpsStatusChanged(UASInterface* uas,int fixtype);
    void batteryVoltChanged();
    void navModeChanged(int uasid, int mode, const QString& text);
    void batteryCapChanged();
private:
    QString m_lowVoltParam;
    Ui::FailSafeConfig ui;
};

#endif // FAILSAFECONFIG_H
