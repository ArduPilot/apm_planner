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

#ifndef GEOFENCECONFIG_H
#define GEOFENCECONFIG_H

#include "AP2ConfigWidget.h"
#include "ui_GeoFenceConfig.h"

class GeoFenceConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit GeoFenceConfig(QWidget *parent = 0);
    ~GeoFenceConfig();

    void showEvent(QShowEvent *);

private slots:
    void activeUASSet(UASInterface *uas);
    void enabledChangedState(int state);
    void fenceTypeChanged(int index);
    void fenceActionChanged(int index);
    void valueChangedMaxAlt();
    void valueChangedRtlAlt();
    void valueChangedRadius();
    void valueChangedFinalRtlAlt();

    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    
private:
    void enableControls();
    void disableControls();
    void connectAllSignals();
    void disconnectAllSignals();
private:
    Ui::GeoFenceConfig ui;
};

#endif // GEOFENCECONFIG_H
