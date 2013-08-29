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

#ifndef CAMERAGIMBALCONFIG_H
#define CAMERAGIMBALCONFIG_H

#include <QWidget>
#include "AP2ConfigWidget.h"
#include "ui_CameraGimbalConfig.h"

class CameraGimbalConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit CameraGimbalConfig(QWidget *parent = 0);
    ~CameraGimbalConfig();
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void updateTilt();
    void updateRoll();
    void updatePan();
    void updateShutter();
    void updateRetractAngles();
    void updateNeutralAngles();
    void updateControlAngles();
private:
    Ui::CameraGimbalConfig ui;
    QString m_shutterPrefix;
    QString m_rollPrefix;
    QString m_tiltPrefix;
    QString m_panPrefix;
};

#endif // CAMERAGIMBALCONFIG_H
