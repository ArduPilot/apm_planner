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


class RC_FUNCTION {
public:
    static const int Disabled = 0;
    static const int Manual = 1;
    static const int Flap = 2;
    static const int Flap_auto = 3;
    static const int Aileron = 4;
    static const int unused = 5;
    static const int mount_pan = 6;
    static const int mount_tilt = 7;
    static const int mount_roll = 8;
    static const int mount_open = 9;
    static const int camera_trigger = 10;
    static const int release = 11;
    static const int mount2_pan = 12;
    static const int mount2_tilt = 13;
    static const int mount2_roll = 14;
    static const int mount2_open = 15;
    static const int DifferentialSpoiler1 = 16;
    static const int DifferentialSpoiler2 = 17;
    static const int AileronWithInput = 18;
    static const int Elevator = 19;
    static const int ElevatorWithInput = 20;
    static const int Rudder = 21;
    static const int sprayer_pump = 22;
    static const int sprayer_spinner = 23;
    static const int flaperon1 = 24;
    static const int flaperon2 = 25;
    static const int steering = 26;
};

#define CAM_TRIGG_TYPE_SERVO 0
#define CAM_TRIGG_TYPE_RELAY 1

class CameraGimbalConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit CameraGimbalConfig(QWidget *parent = 0);
    ~CameraGimbalConfig();
    void showEvent(QShowEvent *);

public slots:
    void activeUASSet(UASInterface *uas);

private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);

    void updateTilt();
    void updateRoll();
    void updatePan();

    void updateTilt(int index);
    void updateRoll(int index);
    void updatePan(int index);

    void updateCameraTriggerOutputCh(int index);
    void updateCameraTriggerSettings();

    void updateRetractAngles();
    void updateNeutralAngles();
    void updateControlAngles();

    void updateMountMode(int index);

private:
    void initConnections();
    void connectSignals();
    void disconnectSignals();

    void addOutputRcChannels(QComboBox *comboBox);
    void addInputRcChannels(QComboBox *comboBox);
    void addTriggerTypes(QComboBox* comboBox);

    void refreshMountParameters(QString mount, QString parameter, QVariant &value);
    void refreshCameraTriggerParameters(QString parameter, QVariant value);
    void refreshRcFunctionComboxBox(QString rcChannelName, QVariant &value);

    void updateCameraGimbalParams(QString& prefix, const QString& newChPrefix,
                                  const QString& mountType, int rcFunction,
                                  QComboBox *outputChCombo, QComboBox* inputChCombo,
                                  QSpinBox* servoMin, QSpinBox* servoMax, QCheckBox* servoReverse,
                                  QSpinBox* angleMin, QSpinBox* angleMax, QCheckBox *stabilize);
    void requestParameterUpdate();

private:
    Ui::CameraGimbalConfig ui;

    QString m_newTriggerPrefix;
    QString m_newRollPrefix;
    QString m_newTiltPrefix;
    QString m_newPanPrefix;

    QString m_triggerPrefix;
    QString m_rollPrefix;
    QString m_tiltPrefix;
    QString m_panPrefix;

    QList<QString> m_cameraParams;
};

#endif // CAMERAGIMBALCONFIG_H
