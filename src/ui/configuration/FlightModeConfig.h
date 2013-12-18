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
 *   @brief Flight mode configuration
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef FLIGHTMODECONFIG_H
#define FLIGHTMODECONFIG_H

#include <QWidget>
#include "ui_FlightModeConfig.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "AP2ConfigWidget.h"

class FlightModeConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit FlightModeConfig(QWidget *parent = 0);
    ~FlightModeConfig();

private slots:
    void activeUASSet(UASInterface *uas);
    void saveButtonClicked();
    void modeChanged(int sysId, QString status, QString description);
    void remoteControlChannelRawChanged(int chan,float val);
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);

private:
    void addPlaneModes(QComboBox &comboBox);
    void addCopterModes(QComboBox &comboBox);
    void addRoverModes(QComboBox &comboBox);

private:
    Ui::FlightModeConfig ui;
};

#endif // FLIGHTMODECONFIG_H
