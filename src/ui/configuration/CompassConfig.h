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
// Using alglib for least squares calc (could migrate to Eigen Lib?)
#include "libs/alglib/src/ap.h"
#include "libs/alglib/src/optimization.h"
#include "libs/alglib/src/interpolation.h"


class RawImuTuple{
public:
    RawImuTuple():magX(0.0f),
        magY(0.0f),
        magZ(0.0f){}

public:
    float magX;
    float magY;
    float magZ;
};

using namespace alglib;

class CompassConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit CompassConfig(QWidget *parent = 0);
    ~CompassConfig();

    static void sphere_error(const alglib::real_1d_array &xi, alglib::real_1d_array &fi, void *obj);
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
    void sensorUpdateMessage(UASInterface* uas, mavlink_sensor_offsets_t sensorOffsets);

    real_1d_array* leastSq(QVector<RawImuTuple> *rawImuList);
    void saveOffsets(real_1d_array &ofs);
    void degreeEditFinished();

    void setCompassAPMOnBoard();
    void setCompassPX4OnBoard();
    void setCompass3DRGPS();

private:
    void cleanup();

private:
    bool m_validSensorOffsets;
    Ui::CompassConfig ui;
    QPointer<QProgressDialog> m_progressDialog;
    QPointer<QTimer> m_timer;
    QVector<RawImuTuple> m_rawImuList;
    mavlink_sensor_offsets_t m_sensorOffsets;
    double m_oldxmag;
    double m_oldymag;
    double m_oldzmag;
    int m_allOffsetsSet;
};

#endif // COMPASSCONFIG_H
