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
 *   @brief Compass sensor configuration
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef COMPASSCONFIG_H
#define COMPASSCONFIG_H

#include "ui_CompassConfig.h"
#include "UASManager.h"
#include "UASInterface.h"
#include "AP2ConfigWidget.h"
#include <QWidget>
#include <QProgressDialog>

class CompassConfig : public AP2ConfigWidget
{
    Q_OBJECT

    static const int COMPASS_ID_1 = 0;
    static const int COMPASS_ID_2 = 1;
    static const int COMPASS_ID_3 = 2;

    static const int dataX = 0;
    static const int dataY = 1;
    static const int dataZ = 2;

    static const int COMPASS_ORIENT_NONE = 0;
    static const int COMPASS_ORIENT_ROLL_180 = 8;

    static const int MAV_SENSOR_OFFSET_GYRO = 0;
    static const int MAV_SENSOR_OFFSET_ACCELEROMETER = 1;
    static const int MAV_SENSOR_OFFSET_MAGNETOMETER = 2;
    static const int MAV_SENSOR_OFFSET_BAROMETER = 3;
    static const int MAV_SENSOR_OFFSET_OPTICALFLOW = 4;
    static const int MAV_SENSOR_OFFSET_MAGNETOMETER2 = 5;

public:
    explicit CompassConfig(QWidget *parent = 0);
    ~CompassConfig();

    void updateCompassSelection();

private:
    enum CompassType {none, APM, ExternalCompass, PX4};

private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void enableClicked(bool enabled);
    void autoDecClicked(bool enabled);
    void orientationComboChanged(int index);
    void liveCalibrationClicked();
    void startDataCollection();

    void finishCompassCalibration();
    void cancelCompassCalibration();
    void progressCounter();

    void activeUASSet(UASInterface *uas);
    void rawImuMessageUpdate(UASInterface* uas, mavlink_raw_imu_t rawImu);
    void scaledImu2MessageUpdate(UASInterface* uas, mavlink_scaled_imu2_t scaledImu);

    void saveOffsets(const Vector3d &ofs, int compassId);
    void degreeEditFinished();

    void setCompassAPMOnBoard();
    void setCompassPX4OnBoard();
    void setCompass3DRGPS();

    void showCompassMotorCalibrationDialog();

private:
    void cleanup();
    void readSettings();
    void writeSettings();
    void updateImuList(const Vector3d& currentReading, Vector3d& compassLastValue,
                       Vector3d& compassOffset, QVector<Vector3d>& list);
    bool isCalibratingCompass() {return m_calibratingCompass;}

private:
    Ui::CompassConfig ui;
    QPointer<QProgressDialog> m_progressDialog;
    QPointer<QTimer> m_timer;

    bool m_calibratingCompass;

    int m_compassId;
    int m_compassId2;
    int m_compassId3;

    // Compass Mag Readings
    QVector<Vector3d> m_compass1RawImuList;
    QVector<Vector3d> m_compass2RawImuList;
    QVector<Vector3d> m_compass3RawImuList;

    Vector3d m_compass1Offset;
    Vector3d m_compass2Offset;
    Vector3d m_compass3Offset;

    Vector3d m_compass1LastValue;
    Vector3d m_compass2LastValue;
    Vector3d m_compass3LastValue;

    bool m_compatibilityMode;
    bool m_haveSecondCompass;
    bool m_haveThirdCompass;

    double m_avgSamples;
    double m_rad;
};

#endif // COMPASSCONFIG_H
