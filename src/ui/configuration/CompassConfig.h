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
    RawImuTuple():m_x(0.0f),
        m_y(0.0f),
        m_z(0.0f){}

    RawImuTuple(double x, double y, double z):
        m_x(x),
        m_y(y),
        m_z(z){}

    void set(double x, double y, double z) { m_x = x; m_y = y, m_z = z ;}
    void set(const RawImuTuple& value) { m_x = value.getX(); m_y = value.getY(); m_z = value.getZ();}

    void setX(double x) { m_x = x; }
    void setY(double y) { m_y = y; }
    void setZ(double z) { m_z = z; }

    double getX() const { return m_x; }
    double getY() const { return m_y; }
    double getZ() const { return m_z; }

    bool operator !=(const RawImuTuple &value) const
    {
        return (m_x!=value.m_x && m_y!=value.m_y && m_z!=value.m_z);
    }

    RawImuTuple operator -(const RawImuTuple &value) const {
        return RawImuTuple(m_x-value.m_x, m_y-value.m_y, m_z-value.m_x);
    }

private:
    double m_x;
    double m_y;
    double m_z;
};


using namespace alglib;

class CompassConfig : public AP2ConfigWidget
{
    Q_OBJECT

    static const int COMPASS_ID_1 = 0;
    static const int COMPASS_ID_2 = 1;
    static const int COMPASS_ID_3 = 2;
    
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
    void scaledImu2MessageUpdate(UASInterface* uas, mavlink_scaled_imu2_t scaledImu);

    real_1d_array* leastSq(QVector<RawImuTuple> *rawImuList);
    void saveOffsets(real_1d_array *ofs, real_1d_array *ofs2);
    void degreeEditFinished();

    void setCompassAPMOnBoard();
    void setCompassPX4OnBoard();
    void setCompass3DRGPS();

    void showCompassMotorCalibrationDialog();

private:
    void cleanup();
    void readSettings();
    void writeSettings();
    void updateImuList(const RawImuTuple& currentReading, RawImuTuple& compassLastValue,
                       RawImuTuple& compassOffset, QVector<RawImuTuple>& list);
    bool isCalibratingCompass() {return m_calibratingCompass;}

private:
    Ui::CompassConfig ui;
    QPointer<QProgressDialog> m_progressDialog;
    QPointer<QTimer> m_timer;

    bool m_calibratingCompass;

    int m_compassId;
    int m_compassId2;
    int m_compassId3;

    QVector<RawImuTuple> m_compass1RawImuList;
    QVector<RawImuTuple> m_compass2RawImuList;
    QVector<RawImuTuple> m_compass3RawImuList;

    RawImuTuple m_compass1Offset;
    RawImuTuple m_compass2Offset;
    RawImuTuple m_compass3Offset;

    RawImuTuple m_compass1LastValue;
    RawImuTuple m_compass2LastValue;
    RawImuTuple m_compass3LastValue;

    bool m_compatibilityMode;
    bool m_haveSecondCompass;
    bool m_haveThirdCompass;
};

#endif // COMPASSCONFIG_H
