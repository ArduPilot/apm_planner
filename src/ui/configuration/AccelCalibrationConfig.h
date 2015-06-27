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
 *   @brief Accelerometer calibration for multi-rotor helicoptors
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef ACCELCALIBRATIONCONFIG_H
#define ACCELCALIBRATIONCONFIG_H

#include <QWidget>
#include "ui_AccelCalibrationConfig.h"
#include "UASManager.h"
#include "UASInterface.h"
#include "AP2ConfigWidget.h"

class AccelCalibrationConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
    static const int CALIBRATION_TIMEOUT_SEC = 40;
public:
    explicit AccelCalibrationConfig(QWidget *parent = 0);
    ~AccelCalibrationConfig();
protected:
    void hideEvent(QHideEvent *evt);
private slots:
    void activeUASSet(UASInterface *uas);
    void calibrateButtonClicked();
    void uasTextMessageReceived(int uasid, int componentid, int severity, QString text);
    void uasConnected();
    void uasDisconnected();
    void countdownTimerTick();

private:
    int m_accelAckCount;
    Ui::AccelCalibrationConfig ui;
    bool m_muted;
    bool m_isCalibrating;
    QTimer m_countdownTimer;
    int m_countdownCount;
};

#endif // ACCELCALIBRATIONCONFIG_H
