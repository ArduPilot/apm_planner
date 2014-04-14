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

#include "QsLog.h"
#include "CompassConfig.h"
#include <qmath.h>
#include "QGCCore.h"
#include "QGC.h"
#include <QtConcurrent>

static const int COMPASS_ORIENT_NONE = 0;
static const int COMPASS_ORIENT_ROLL_180 = 8;

CompassConfig::CompassConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_validSensorOffsets(false),
    m_timer(NULL),
    m_calibrationState(Idle),
    m_allOffsetsSet(0),
    m_offsetWatcher(NULL)
{
    ui.setupUi(this);

    m_timer = new QTimer(this);
    m_timer->setSingleShot(false);

    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    ui.enableBox->setEnabled(false);
    ui.declinationBox->setEnabled(false);
    ui.calibrationBox->setEnabled(false);
    ui.orientationBox->setEnabled(false);

    ui.calibrationProgressBar->setRange(0, 60);
    ui.calibrationProgressBar->setValue(0);
    ui.dataPointsCollectedLabel->setText(tr("n/a"));
    ui.lastRawDataPointLabel->setText(tr("n/a"));
    updateCalibratedOffsetsLabel(NULL);
    updateCalibrationStateLabel();

    connect(ui.enableCheckBox, SIGNAL(clicked(bool)), this, SLOT(enableClicked(bool)));
    connect(ui.autoDeclinationRadioButton, SIGNAL(clicked(bool)), this, SLOT(autoDeclinationClicked(bool)));
    connect(ui.manualDeclinationRadioButton, SIGNAL(clicked(bool)), this, SLOT(manualDeclinationClicked(bool)));
    connect(ui.orientationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationComboChanged(int)));
    connect(ui.degreesLineEdit, SIGNAL(editingFinished()), this, SLOT(degreeEditFinished()));
    connect(ui.minutesLineEdit, SIGNAL(editingFinished()), this, SLOT(degreeEditFinished()));

    ui.orientationComboBox->addItem("None 0");
    ui.orientationComboBox->addItem("Yaw 45");
    ui.orientationComboBox->addItem("Yaw 90");
    ui.orientationComboBox->addItem("Yaw 135");
    ui.orientationComboBox->addItem("Yaw 180");
    ui.orientationComboBox->addItem("Yaw 225");
    ui.orientationComboBox->addItem("Yaw 270");
    ui.orientationComboBox->addItem("Yaw 315");
    ui.orientationComboBox->addItem("Roll 180");
    ui.orientationComboBox->addItem("Roll 180, Yaw 45");
    ui.orientationComboBox->addItem("Roll 180, Yaw 90");
    ui.orientationComboBox->addItem("Roll 180, Yaw 135");
    ui.orientationComboBox->addItem("Pitch 180");
    ui.orientationComboBox->addItem("Roll 180, Yaw 225");
    ui.orientationComboBox->addItem("Roll 180, Yaw 270");
    ui.orientationComboBox->addItem("Roll 180, Yaw 315");
    ui.orientationComboBox->addItem("Roll 90");
    ui.orientationComboBox->addItem("Roll 90, Yaw 45");
    ui.orientationComboBox->addItem("Roll 90, Yaw 90");
    ui.orientationComboBox->addItem("Roll 90, Yaw 135");
    ui.orientationComboBox->addItem("Roll 270");
    ui.orientationComboBox->addItem("Roll 270, Yaw 45");
    ui.orientationComboBox->addItem("Roll 270, Yaw 90");
    ui.orientationComboBox->addItem("Roll 270, Yaw 135");
    ui.orientationComboBox->addItem("Pitch 90");
    ui.orientationComboBox->addItem("Pitch 270");
//    ui.orientationComboBox->addItem("MAX");

    initConnections();

    connect(m_timer, SIGNAL(timeout()), this, SLOT(progressCounter()));

    connect(ui.liveCalibrationButton, SIGNAL(clicked()),
            this, SLOT(liveCalibrationClicked()));

    connect(ui.onBoardApmButton, SIGNAL(clicked()),
            this,SLOT(setCompassAPMOnBoard()));
    connect(ui.gpsCompassButton, SIGNAL(clicked()),
            this,SLOT(setCompass3DRGPS()));
    connect(ui.px4Button, SIGNAL(clicked()),
            this,SLOT(setCompassPX4OnBoard()));

}

void CompassConfig::activeUASSet(UASInterface *uas)
{
    AP2ConfigWidget::activeUASSet(uas);

    if (!m_uas)
    {
        ui.enableBox->setEnabled(false);
        ui.declinationBox->setEnabled(false);
        ui.calibrationBox->setEnabled(false);
        ui.orientationBox->setEnabled(false);
    }
}

void CompassConfig::degreeEditFinished()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    //float degrees = value.toDouble() * (float)(180.0 / M_PI);
    //float minutes  = (degrees - ((int)degreees)) * 60.0;
    bool degok = false;
    bool minok = false;
    double degrees = ui.degreesLineEdit->text().toDouble(&degok);
    double minutes = ui.minutesLineEdit->text().toDouble(&minok);
    if (degrees < 0)
    {
        degrees = degrees - (minutes / 60.0);
    }
    else
    {
        degrees = degrees + (minutes / 60.0);
    }
    degrees = degrees * (M_PI/180.0);
    m_uas->getParamManager()->setParameter(1,"COMPASS_DEC",(float)degrees);
    if (!degok || !minok)
    {
        QMessageBox::information(this,tr("Error"),tr("Error, degrees or minutes entered are nor in valid numeric format. Please re-enter the information"));
        return;
    }
}

void CompassConfig::updateCompassSelection()
{
    ui.savedLabel->setText(tr("UPDATED"));
    QTimer::singleShot(2000, ui.savedLabel, SLOT(clear()));
}

void CompassConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if (parameterName == "MAG_ENABLE")
    {
        if (value.toInt() == 0)
        {
            ui.enableCheckBox->setChecked(false);
            ui.declinationBox->setEnabled(false);
            ui.calibrationBox->setEnabled(false);
            ui.orientationBox->setEnabled(false);
        }
        else
        {
            ui.enableCheckBox->setChecked(true);
            ui.declinationBox->setEnabled(true);
            ui.calibrationBox->setEnabled(true);
            ui.orientationBox->setEnabled(true);
        }
        ui.enableBox->setEnabled(true);
    }
    else if (parameterName == "COMPASS_AUTODEC")
    {
        if (value.toInt() == 0)
        {
            ui.autoDeclinationRadioButton->setChecked(false);
            ui.manualDeclinationRadioButton->setChecked(true);
            ui.degreesLineEdit->setEnabled(true);
            ui.minutesLineEdit->setEnabled(true);
        }
        else
        {
            ui.autoDeclinationRadioButton->setChecked(true);
            ui.manualDeclinationRadioButton->setChecked(false);
            ui.degreesLineEdit->setEnabled(false);
            ui.minutesLineEdit->setEnabled(false);
        }
    }
    else if (parameterName == "COMPASS_DEC")
    {
        float degrees = value.toDouble() * (float)(180.0 / M_PI);
        float minutes  = (degrees - ((int)degrees)) * 60.0;
        if (degrees < 0)
        {
            minutes = minutes * -1;
        }
        ui.degreesLineEdit->setText(QString::number((int)degrees));
        ui.minutesLineEdit->setText(QString::number(minutes,'f',0));
    }
    else if (parameterName == "COMPASS_ORIENT")
    {
        ui.orientationComboBox->blockSignals(true);
        ui.orientationComboBox->setCurrentIndex(value.toInt());
        ui.orientationComboBox->blockSignals(false);
        updateCompassSelection();
    }
    else if (parameterName.contains("COMPASS_OFS"))
    {
        QLOG_DEBUG() << "Clearing " << parameterName;
        if (parameterName == "COMPASS_OFS_X")
        {
            m_allOffsetsSet += 2;
        }
        else if (parameterName == "COMPASS_OFS_Y")
        {
            m_allOffsetsSet += 4;
        }
        else if (parameterName == "COMPASS_OFS_Z")
        {
            m_allOffsetsSet += 8;
        }

        if (m_allOffsetsSet == 15)
        {
            // ie all offsets have been set
            QLOG_DEBUG() << "Start Data Collection";
            startDataCollection();
        }
    }
}

void CompassConfig::enableClicked(bool enabled)
{
    if (m_uas)
    {
        if (enabled)
        {
            m_uas->getParamManager()->setParameter(1, "MAG_ENABLE", 1);
            ui.autoDeclinationRadioButton->setEnabled(true);
            ui.manualDeclinationRadioButton->setEnabled(true);
            if (ui.manualDeclinationRadioButton->isChecked())
            {
                ui.minutesLineEdit->setEnabled(true);
                ui.degreesLineEdit->setEnabled(true);
            }
        }
        else
        {
            m_uas->getParamManager()->setParameter(1, "MAG_ENABLE", 0);
            ui.autoDeclinationRadioButton->setEnabled(false);
            ui.manualDeclinationRadioButton->setEnabled(false);
            ui.degreesLineEdit->setEnabled(false);
            ui.minutesLineEdit->setEnabled(false);
        }
    }
}

void CompassConfig::autoDeclinationClicked(bool enabled)
{
    if (m_uas)
    {
        if (enabled)
        {
            m_uas->getParamManager()->setParameter(1,"COMPASS_AUTODEC",QVariant(1));
        }
        else
        {
            m_uas->getParamManager()->setParameter(1,"COMPASS_AUTODEC",QVariant(0));
        }
    }
}

void CompassConfig::manualDeclinationClicked(bool enabled)
{
    if (m_uas)
    {
        if (enabled)
        {
            m_uas->getParamManager()->setParameter(1, "COMPASS_AUTODEC", QVariant(0));
        }
        else
        {
            m_uas->getParamManager()->setParameter(1, "COMPASS_AUTODEC", QVariant(1));
        }
    }
}

void CompassConfig::orientationComboChanged(int index)
{
    //COMPASS_ORIENT
    if (!m_uas)
    {
        return;
    }
    QLOG_DEBUG() << "setCompassOrientation index:" << index;
    m_uas->getParamManager()->setParameter(1,"COMPASS_ORIENT",index);

}

void CompassConfig::setCompassAPMOnBoard()
{
    if (!m_uas)
    {
        return;
    }
    // ROTATION_NONE
    QLOG_DEBUG() << "setCompassAPMOnBoard ROTATION_NONE";
    QGCUASParamManager* pm = m_uas->getParamManager();
    if (pm->getParameterValue(1, "COMPASS_ORIENT").toInt() != COMPASS_ORIENT_NONE)
    {
        pm->setParameter(1,"COMPASS_ORIENT",COMPASS_ORIENT_NONE);
    }
    else
    {
        updateCompassSelection();
    }
}

void CompassConfig::setCompassPX4OnBoard()
{
    if (!m_uas)
    {
        return;
    }
    // FMUv1 & FMUv2 is None
    QLOG_DEBUG() << "setCompassPX4OnBoard None 0Deg";
    QGCUASParamManager* pm = m_uas->getParamManager();
    if (pm->getParameterValue(1, "COMPASS_ORIENT").toInt() != COMPASS_ORIENT_NONE)
    {
        pm->setParameter(1,"COMPASS_ORIENT", COMPASS_ORIENT_NONE);
    }
    else
    {
        updateCompassSelection();
    }
}

void CompassConfig::setCompass3DRGPS()
{
    if (!m_uas)
    {
        return;
    }

    // ROTATION_ROLL_180
    QLOG_DEBUG() << "setCompass3DRGPS ROLL_180";
    QGCUASParamManager* pm = m_uas->getParamManager();
    if (pm->getParameterValue(1, "COMPASS_ORIENT").toInt() != COMPASS_ORIENT_ROLL_180)
    {
        pm->setParameter(1,"COMPASS_ORIENT", COMPASS_ORIENT_ROLL_180);
    }
    else
    {
        updateCompassSelection();
    }
}

void CompassConfig::liveCalibrationClicked()
{
    QLOG_DEBUG() << "live Calibration Started";
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        cleanup();
        return;
    }

    QGCUASParamManager *pm = m_uas->getParamManager();

    switch (m_calibrationState)
    {
    case Idle:
        QMessageBox::information(this, tr("Live Compass calibration"),
                                 tr("Data will be collected for 60 seconds, Please click ok and move the apm around all axes."));

        if ((pm->getParameterValue(1, "COMPASS_OFS_X") != 0.0f)
                ||(pm->getParameterValue(1, "COMPASS_OFS_Y") != 0.0f)
                || (pm->getParameterValue(1, "COMPASS_OFS_Z") != 0.0f))
        {
            m_calibrationState = Clearing;
            updateCalibrationStateLabel();

            // Initialiase to zero
            pm->setParameter(1,"COMPASS_OFS_X", QVariant(static_cast<float>(0.0f)));
            pm->setParameter(1,"COMPASS_OFS_Y", QVariant(static_cast<float>(0.0f)));
            pm->setParameter(1,"COMPASS_OFS_Z", QVariant(static_cast<float>(0.0f)));
            m_allOffsetsSet = 1; // Add 2 for X, 4 for Y, 8 for Z, add 1 means it's enabled.
        }
        else
        {
            startDataCollection();
        }
        break;

    case Clearing:
    case Collecting:
    case Finishing:
        cancelCompassCalibration();
        break;
    }
}

void CompassConfig::startDataCollection()
{
    m_calibrationState = Collecting;
    updateCalibrationStateLabel();

    connect(m_uas, SIGNAL(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)),
            this, SLOT(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)));
    connect(m_uas, SIGNAL(sensorOffsetsMessageUpdate(UASInterface*,mavlink_sensor_offsets_t)),
            this, SLOT(sensorUpdateMessage(UASInterface*,mavlink_sensor_offsets_t)));
    m_uas->enableRawSensorDataTransmission(10);

    ui.liveCalibrationButton->setText(tr("Cancel Calibration"));
    ui.calibrationProgressBar->setRange(0, 60);
    ui.calibrationProgressBar->setValue(0);
    m_timer->start(1); // second counting progress timer
}

void CompassConfig::progressCounter()
{
    int newValue = ui.calibrationProgressBar->value() + 1;
    ui.calibrationProgressBar->setValue(newValue);
    if (newValue < 60)
    {
        m_timer->start(1000);
    }
    else
    {
        finishCompassCalibration();
    }
}

void CompassConfig::cancelCompassCalibration()
{
    QLOG_INFO() << "cancelCompassCalibration";
    m_uas->enableRawSensorDataTransmission(2);
    disconnect(m_uas, SIGNAL(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)),
                this, SLOT(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)));
    m_rawImuList.clear();
    ui.dataPointsCollectedLabel->setText(tr("n/a"));
    ui.lastRawDataPointLabel->setText(tr("n/a"));
    updateCalibratedOffsetsLabel(NULL);
    cleanup();
}

void CompassConfig::finishedCalculatingOffsets(void)
{
    if (m_offsetWatcher)
    {
        QVector3D result = m_offsetWatcher->result();
        updateCalibratedOffsetsLabel(&result);
        m_offsetWatcher->deleteLater();
        m_offsetWatcher = NULL;
    }
}

void CompassConfig::updateCalibratedOffsetsLabel(QVector3D *offsets)
{
    if (offsets)
    {
        ui.calibratedOffsetsLabel->setText(tr("X:%1 Y:%2 Z:%3").arg(offsets->x(), 3).arg(offsets->y(), 3).arg(offsets->z(), 3));
    }
    else
    {
        ui.calibratedOffsetsLabel->setText(tr("n/a"));
    }
}

void CompassConfig::updateCalibrationStateLabel(void)
{
    switch (m_calibrationState)
    {
    case Idle:
        ui.calibrationStateLabel->setText(tr("Not calibrating..."));
        ui.calibrationDataBox->setEnabled(false);
        break;

    case Clearing:
        ui.calibrationStateLabel->setText(tr("Clearing old offsets..."));
        ui.calibrationDataBox->setEnabled(true);
        break;

    case Collecting:
        ui.calibrationStateLabel->setText(tr("Collecting data..."));
        ui.calibrationDataBox->setEnabled(true);
        break;

    case Finishing:
        ui.calibrationStateLabel->setText(tr("Writing calibrated data..."));
        ui.calibrationDataBox->setEnabled(true);
        break;
    }
}

static QVector3D calculateOffsets(Vector3DList dataList)
{
    QLOG_INFO() << "Calculating offset for " << dataList.count() << " elements.";
    Vector3D center;
    center.setToLeastSquaresSphericalCenter(dataList);
    QLOG_INFO() << "Finished calculating offset for " << dataList.count() << " elements.";
    return center.toQVector3D();
}

void CompassConfig::requestNewOffsets(void)
{
    if (m_offsetWatcher)
    {
        // a watcher is already pending to deliver results... come back later.
        QLOG_INFO() << "request denied for " << m_rawImuList.count() << " elements...";
        return;
    }

    m_offsetWatcher = new QFutureWatcher<QVector3D>(this);
    connect(m_offsetWatcher, SIGNAL(finished()), this, SLOT(finishedCalculatingOffsets()));
    m_offsetWatcher->setFuture(QtConcurrent::run(calculateOffsets, m_rawImuList));
}

void CompassConfig::cleanup()
{
    if (m_offsetWatcher)
    {
        m_offsetWatcher->cancel();
        m_offsetWatcher->waitForFinished();
        m_offsetWatcher->deleteLater();
        m_offsetWatcher = NULL;
    }
    m_timer->stop();
    ui.liveCalibrationButton->setText(tr("Live Calibration"));
    m_rawImuList.clear();
    m_calibrationState = Idle;
    updateCalibrationStateLabel();
    m_validSensorOffsets = false;
}

void CompassConfig::finishCompassCalibration()
{
    QLOG_INFO() << "finishCompassCalibration with " << m_rawImuList.count() << " data points";
    disconnect(m_uas, SIGNAL(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)),
                this, SLOT(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)));
    m_uas->enableRawSensorDataTransmission(2);
    m_timer->stop();
    m_calibrationState = Finishing;

    // Now calulate the offsets
    if (m_rawImuList.count() < 10)
    {
        QLOG_ERROR() << "Not enough data points for calculation:" ;
        QMessageBox::warning(this, tr("Compass Calibration Failed"), tr("Not enough data points to calibrate the compass."));
        return;
    }

    // Calculate and send the update message
    Vector3D center;
    center.setToLeastSquaresSphericalCenter(m_rawImuList);
    saveOffsets(center);
}

void CompassConfig::saveOffsets(const Vector3D &magOffset)
{
    float xOffset = static_cast<float>(magOffset.x());
    float yOffset = static_cast<float>(magOffset.y());
    float zOffset = static_cast<float>(magOffset.z());

    QLOG_INFO() << "New Mag Offset to be set are: " << xOffset
                << ", " <<  yOffset
                << ", " <<  zOffset;

    QGCUASParamManager* paramMgr = m_uas->getParamManager();

    paramMgr->setParameter(1, "COMPASS_LEARN", 0);
    // set values
    paramMgr->setParameter(1, "COMPASS_OFS_X", xOffset);
    paramMgr->setParameter(1, "COMPASS_OFS_Y", yOffset);
    paramMgr->setParameter(1, "COMPASS_OFS_Z", zOffset);

    cleanup();

    QVector3D offset = magOffset.toQVector3D();
    updateCalibratedOffsetsLabel(&offset);

    ui.calibrationStateLabel->setText(tr("Saving Mag Offsets..."));
    QTimer::singleShot(2000, this, SLOT(updateCalibrationStateLabel()));
}

void CompassConfig::rawImuMessageUpdate(UASInterface* uas, mavlink_raw_imu_t rawImu)
{
    if (m_uas == uas && m_validSensorOffsets)
    {
        QLOG_DEBUG() << "RAW IMU x:" << rawImu.xmag << " y:" << rawImu.ymag << " z:" << rawImu.zmag;

        Vector3D mag(rawImu.xmag, rawImu.ymag, rawImu.zmag);
        if (m_oldMag != mag)
        {
            m_oldMag = mag;

            Vector3D magOffset(m_sensorOffsets.mag_ofs_x, m_sensorOffsets.mag_ofs_y, m_sensorOffsets.mag_ofs_z);
            m_rawImuList.append(mag - magOffset);

            ui.dataPointsCollectedLabel->setText(tr("%1").arg(m_rawImuList.count()));
            ui.lastRawDataPointLabel->setText(tr("X:%1 Y:%2 Z:%3").arg(rawImu.xmag).arg(rawImu.ymag).arg(rawImu.zmag));

            // calculating the least squares takes considerable amount of time. Lets do that in the background...
            requestNewOffsets();
        }
    }
}

void CompassConfig::sensorUpdateMessage(UASInterface* uas, mavlink_sensor_offsets_t sensorOffsets)
{
    if (m_uas == uas)
    {
        m_sensorOffsets = sensorOffsets;
        m_validSensorOffsets = true;
    }
}
