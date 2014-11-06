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
#include "CompassMotorCalibrationDialog.h"
#include <qmath.h>
#include "QGCCore.h"

static const int COMPASS_ORIENT_NONE = 0;
static const int COMPASS_ORIENT_ROLL_180 = 8;

static const int MAV_SENSOR_OFFSET_GYRO = 0;
static const int MAV_SENSOR_OFFSET_ACCELEROMETER = 1;
static const int MAV_SENSOR_OFFSET_MAGNETOMETER = 2;
static const int MAV_SENSOR_OFFSET_BAROMETER = 3;
static const int MAV_SENSOR_OFFSET_OPTICALFLOW = 4;
static const int MAV_SENSOR_OFFSET_MAGNETOMETER2 = 5;

CompassConfig::CompassConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_progressDialog(NULL),
    m_timer(NULL),
    m_rawImuList(),
    m_compatibilityMode(false),
    m_haveSecondCompass(false),
    m_haveThirdCompass(false)
{
    ui.setupUi(this);

    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    ui.autoDecCheckBox->setEnabled(false);
    ui.enableCheckBox->setEnabled(false);
    ui.orientationComboBox->setEnabled(false);
    ui.degreesLineEdit->setEnabled(false);
    ui.minutesLineEdit->setEnabled(false);
    connect(ui.enableCheckBox,SIGNAL(clicked(bool)),this,SLOT(enableClicked(bool)));
    connect(ui.autoDecCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoDecClicked(bool)));
    connect(ui.orientationComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(orientationComboChanged(int)));
    connect(ui.degreesLineEdit,SIGNAL(editingFinished()),this,SLOT(degreeEditFinished()));
    connect(ui.minutesLineEdit,SIGNAL(editingFinished()),this,SLOT(degreeEditFinished()));

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

    connect(ui.liveCalibrationButton, SIGNAL(clicked()),
            this, SLOT(liveCalibrationClicked()));

    connect(ui.onBoardApmButton, SIGNAL(clicked()),
            this,SLOT(setCompassAPMOnBoard()));
    connect(ui.gpsCompassButton, SIGNAL(clicked()),
            this,SLOT(setCompass3DRGPS()));
    connect(ui.px4Button, SIGNAL(clicked()),
            this,SLOT(setCompassPX4OnBoard()));

    connect(ui.compassMotButton, SIGNAL(clicked()), this, SLOT(showCompassMotorCalibrationDialog()));

}

void CompassConfig::activeUASSet(UASInterface *uas)
{
    AP2ConfigWidget::activeUASSet(uas);
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

CompassConfig::~CompassConfig()
{
    delete m_timer;
    delete m_progressDialog;
    m_rawImuList.clear();
}

void CompassConfig::updateCompassSelection()
{
    ui.savedLabel->setText(tr("UPDATED"));
    QTimer::singleShot(2000,ui.savedLabel,SLOT(clear()));
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
            ui.autoDecCheckBox->setEnabled(false);
            ui.degreesLineEdit->setEnabled(false);
            ui.minutesLineEdit->setEnabled(false);
            ui.orientationComboBox->setEnabled(false);
        }
        else
        {
            ui.enableCheckBox->setChecked(true);
            ui.autoDecCheckBox->setEnabled(true);
            ui.degreesLineEdit->setEnabled(true);
            ui.minutesLineEdit->setEnabled(true);
            ui.orientationComboBox->setEnabled(true);
        }
        ui.enableCheckBox->setEnabled(true);
    }
    else if (parameterName == "COMPASS_AUTODEC")
    {
        if (value.toInt() == 0)
        {
            ui.autoDecCheckBox->setChecked(false);
        }
        else
        {
            ui.autoDecCheckBox->setChecked(true);
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
}

void CompassConfig::enableClicked(bool enabled)
{
    if (m_uas)
    {
        if (enabled)
        {
            m_uas->getParamManager()->setParameter(1,"MAG_ENABLE",QVariant(1));
            ui.autoDecCheckBox->setEnabled(true);
            if (!ui.autoDecCheckBox->isChecked())
            {
                ui.minutesLineEdit->setEnabled(true);
                ui.degreesLineEdit->setEnabled(true);
            }
        }
        else
        {
            m_uas->getParamManager()->setParameter(1,"MAG_ENABLE",QVariant(0));
            ui.autoDecCheckBox->setEnabled(false);
            ui.degreesLineEdit->setEnabled(false);
            ui.minutesLineEdit->setEnabled(false);
        }
    }
}

void CompassConfig::autoDecClicked(bool enabled)
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
        return;
    // ROTATION_NONE
    QLOG_DEBUG() << "setCompassAPMOnBoard ROTATION_NONE";
    QGCUASParamManager* pm = m_uas->getParamManager();
    if (pm->getParameterValue(1, "COMPASS_ORIENT").toInt() != COMPASS_ORIENT_NONE){
        pm->setParameter(1,"COMPASS_ORIENT",COMPASS_ORIENT_NONE);
    } else {
        updateCompassSelection();
    }

}

void CompassConfig::setCompassPX4OnBoard()
{
    if (!m_uas)
        return;
    // FMUv1 & FMUv2 is None
    QLOG_DEBUG() << "setCompassPX4OnBoard None 0Deg";
    QGCUASParamManager* pm = m_uas->getParamManager();
    if (pm->getParameterValue(1, "COMPASS_ORIENT").toInt() != COMPASS_ORIENT_NONE){
        pm->setParameter(1,"COMPASS_ORIENT", COMPASS_ORIENT_NONE);
    } else {
        updateCompassSelection();
    }

}

void CompassConfig::setCompass3DRGPS()
{
    if (!m_uas)
        return;
    // ROTATION_ROLL_180
    QLOG_DEBUG() << "setCompass3DRGPS ROLL_180";
    QGCUASParamManager* pm = m_uas->getParamManager();
    if (pm->getParameterValue(1, "COMPASS_ORIENT").toInt() != COMPASS_ORIENT_ROLL_180){
        pm->setParameter(1,"COMPASS_ORIENT", COMPASS_ORIENT_ROLL_180);
    } else {
        updateCompassSelection();
    }

}

void CompassConfig::liveCalibrationClicked()
{
    QLOG_DEBUG() << "live Calibration Started";
    if (!m_uas) {
        showNullMAVErrorMessageBox();
        return;
    }

    QMessageBox::information(this,tr("Live Compass calibration"),
                             tr("Data will be collected for 60 seconds, Please click ok and move the apm around all axes"));

    QGCUASParamManager* pm = m_uas->getParamManager();
    // Initialiase to zero
    pm->setParameter(1,"COMPASS_OFS_X", QVariant(static_cast<float>(0.0f)));
    pm->setParameter(1,"COMPASS_OFS_Y", QVariant(static_cast<float>(0.0f)));
    pm->setParameter(1,"COMPASS_OFS_Z", QVariant(static_cast<float>(0.0f)));

    pm->setParameter(1,"COMPASS_OFS2_X", QVariant(static_cast<float>(0.0f)));
    pm->setParameter(1,"COMPASS_OFS2_Y", QVariant(static_cast<float>(0.0f)));
    pm->setParameter(1,"COMPASS_OFS2_Z", QVariant(static_cast<float>(0.0f)));

    pm->setParameter(1,"COMPASS_OFS3_X", QVariant(static_cast<float>(0.0f)));
    pm->setParameter(1,"COMPASS_OFS3_Y", QVariant(static_cast<float>(0.0f)));
    pm->setParameter(1,"COMPASS_OFS3_Z", QVariant(static_cast<float>(0.0f)));

    startDataCollection();
}

void CompassConfig::startDataCollection()
{
    connect(m_uas, SIGNAL(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)),
                this, SLOT(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)));
    connect(m_uas, SIGNAL(scaledImu2MessageUpdate(UASInterface*,mavlink_scaled_imu2_t)),
                this, SLOT(scaledImu2MessageUpdate(UASInterface*,mavlink_scaled_imu2_t)));
    m_uas->enableRawSensorDataTransmission(10);

    QGCUASParamManager* pm = m_uas->getParamManager();

    if (pm->getParameterNames(1).contains("COMPASS_OFS2_X"))
    {
        m_haveSecondCompass = true;
    }

    m_progressDialog = new QProgressDialog(tr("Compass calibration in progress. Please rotate your craft around all its axes for 60 seconds."),
                                           tr("Cancel"), 0, 60, this);
    connect(m_progressDialog, SIGNAL(canceled()), this, SLOT(cancelCompassCalibration()));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(progressCounter()));
    m_timer->start(1); // second counting progress timer
}

void CompassConfig::progressCounter()
{

    int newValue = m_progressDialog->value()+1;
    m_progressDialog->setValue(newValue);
    if (newValue < 60) {
        m_timer->start(1000);
    } else {
        finishCompassCalibration();
    }
}

void CompassConfig::cancelCompassCalibration()
{
    QLOG_INFO() << "cancelCompassCalibration";
    m_uas->enableRawSensorDataTransmission(2);
    disconnect(m_uas, SIGNAL(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)),
                this, SLOT(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)));
    disconnect(m_uas, SIGNAL(scaledImu2MessageUpdate(UASInterface*,mavlink_scaled_imu2_t)),
                this, SLOT(scaledImu2MessageUpdate(UASInterface*,mavlink_scaled_imu2_t)));
    cleanup();
}

void CompassConfig::cleanup()
{
    if (m_timer) m_timer->stop();
    delete m_timer;
    m_rawImuList.clear();
    m_scaledImu2List.clear();
    delete m_progressDialog;
}

void CompassConfig::finishCompassCalibration()
{
    QLOG_INFO() << "finishCompassCalibration with compass 1:" << m_rawImuList.count() << " data points";
    QLOG_INFO() << "finishCompassCalibration with compass 2:" << m_scaledImu2List.count() << " data points";
    disconnect(m_uas, SIGNAL(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)),
                this, SLOT(rawImuMessageUpdate(UASInterface*,mavlink_raw_imu_t)));
    disconnect(m_uas, SIGNAL(scaledImu2MessageUpdate(UASInterface*,mavlink_scaled_imu2_t)),
                this, SLOT(scaledImu2MessageUpdate(UASInterface*,mavlink_scaled_imu2_t)));
    m_uas->enableRawSensorDataTransmission(2);
    m_timer->stop();

    // Now calulate the offsets

    if (m_rawImuList.count() < 10) {
        QLOG_ERROR() << "Not enough data points for calculation:" ;
        QMessageBox::warning(this, tr("Compass Calibration Failed"), tr("Not enough data points to calibrate the compass."));
        return;
    }

    // Calculate and send the update message
    alglib::real_1d_array* answerCompass1 = leastSq(&m_rawImuList);
    alglib::real_1d_array* answerCompass2 = NULL;
    if(m_haveSecondCompass && m_scaledImu2List.count() > 10){
        answerCompass2 = leastSq(&m_scaledImu2List);
    }

    saveOffsets(answerCompass1, answerCompass2);
    delete answerCompass1;
    delete answerCompass2;
}

void CompassConfig::saveOffsets(alglib::real_1d_array* offset, alglib::real_1d_array* offset2)
{
    alglib::real_1d_array& ofs = *offset;

    float xOffset = static_cast<float>(ofs[0]);
    float yOffset = static_cast<float>(ofs[1]);
    float zOffset = static_cast<float>(ofs[2]);

    QLOG_INFO() << "New Compass Offset to be set are: " << xOffset
                << ", " <<  yOffset
                << ", " <<  zOffset;

    QGCUASParamManager* paramMgr = m_uas->getParamManager();

    paramMgr->setParameter(1, "COMPASS_LEARN", 0);
    // set values
    m_uas->executeCommand(MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS, 1, MAV_SENSOR_OFFSET_MAGNETOMETER,
                          xOffset, yOffset, zOffset, 0, 0, 0,
                          MAV_COMP_ID_ALL);
    if (m_compatibilityMode){
        paramMgr->setParameter(1, "COMPASS_OFS_X", xOffset);
        paramMgr->setParameter(1, "COMPASS_OFS_Y", yOffset);
        paramMgr->setParameter(1, "COMPASS_OFS_Z", zOffset);
    }
    QVariant deviceId;
    paramMgr->getParameterValue(1, "COMPASS_DEV_ID", deviceId);
    QString message = tr("New offsets (Compass 1) are \n\nx:") + QString::number(xOffset,'f',3)
                        + " y:" + QString::number(yOffset,'f',3) + " z:" + QString::number(zOffset,'f',3)
                        + " dev id:" + deviceId.toString();

    if (m_haveSecondCompass) {
        alglib::real_1d_array& ofs2 = *offset2;

        float xOffset2 = static_cast<float>(ofs2[0]);
        float yOffset2 = static_cast<float>(ofs2[1]);
        float zOffset2 = static_cast<float>(ofs2[2]);

        QLOG_INFO() << "New Compass 2 Offset to be set are: " << xOffset2
                    << ", " <<  yOffset2
                    << ", " <<  zOffset2;

        m_uas->executeCommand(MAV_CMD_PREFLIGHT_SET_SENSOR_OFFSETS, 1, MAV_SENSOR_OFFSET_MAGNETOMETER2,
                              xOffset2, yOffset2, zOffset2, 0, 0, 0,
                              MAV_COMP_ID_ALL);
        if (m_compatibilityMode){
            paramMgr->setParameter(1, "COMPASS_OFS2_X", xOffset2);
            paramMgr->setParameter(1, "COMPASS_OFS2_Y", yOffset2);
            paramMgr->setParameter(1, "COMPASS_OFS2_Z", zOffset2);
        }

        paramMgr->getParameterValue(1, "COMPASS_DEV_ID2", deviceId);
        message.append(tr("\n\nNew offsets (Compass 2) are \n\nx:") + QString::number(xOffset2,'f',3)
                       + " y:" + QString::number(yOffset2,'f',3) + " z:" + QString::number(zOffset2,'f',3)
                       + " dev id:" + deviceId.toString());
    }

    cleanup();

    QMessageBox::information(this, tr("New Compass Offsets"), message + tr("\n\nThese have been saved for you."));
}


void CompassConfig::rawImuMessageUpdate(UASInterface* uas, mavlink_raw_imu_t rawImu)
{
    if (m_uas == uas ){
        QLOG_DEBUG() << "RAW IMU x:" << rawImu.xmag << " y:" << rawImu.ymag << " z:" << rawImu.zmag;

        if (m_oldxmag != rawImu.xmag &&
            m_oldymag != rawImu.ymag &&
            m_oldzmag != rawImu.zmag)
        {
            RawImuTuple value;
            value.magX = rawImu.xmag;
            value.magY = rawImu.ymag;
            value.magZ = rawImu.zmag;

            m_rawImuList.append(value);

            m_oldxmag = rawImu.xmag;
            m_oldymag = rawImu.ymag;
            m_oldzmag = rawImu.zmag;
        }
    }
}

void CompassConfig::scaledImu2MessageUpdate(UASInterface* uas, mavlink_scaled_imu2_t scaledImu)
{
    if (m_uas == uas){
        QLOG_DEBUG() << "SCALED IMU2 x:" << scaledImu.xmag << " y:" << scaledImu.ymag << " z:" << scaledImu.zmag;

        if (m_old2xmag != scaledImu.xmag &&
            m_old2ymag != scaledImu.ymag &&
            m_old2zmag != scaledImu.zmag)
        {
            RawImuTuple value;
            value.magX = scaledImu.xmag;
            value.magY = scaledImu.ymag;
            value.magZ = scaledImu.zmag;

            m_scaledImu2List.append(value);

            m_old2xmag = scaledImu.xmag;
            m_old2ymag = scaledImu.ymag;
            m_old2zmag = scaledImu.zmag;
        }
    }
}

/// <summary>
/// Does the least sq adjustment to find the center of the sphere
/// </summary>
/// <param name="data">list of x,y,z data</param>
/// <returns>offsets</returns>
///

alglib::real_1d_array* CompassConfig::leastSq(QVector<RawImuTuple> *data)
{
    {
    using namespace alglib;

        real_1d_array* x = new real_1d_array("[0.0 , 0.0 , 0.0 , 0.0]");
        double epsg = 0.0000000001;
        double epsf = 0;
        double epsx = 0;
        int maxits = 0;
        alglib::minlmstate state;
        alglib::minlmreport rep;

        alglib::minlmcreatev(data->count(), *x, 100.0f,  state);
        alglib::minlmsetcond(state, epsg, epsf, epsx, maxits);
        alglib::minlmoptimize(state, &CompassConfig::sphere_error, NULL, data);
        alglib::minlmresults(state, *x, rep);

        QLOG_INFO() << "rep.terminationType" << rep.terminationtype;
//        QLOG_DEBUG() << "alglib" << alglib::ap::format(x, 2));

        return x;

    }
}

void CompassConfig::sphere_error(const alglib::real_1d_array &xi, alglib::real_1d_array &fi, void *obj)
{
    double xofs = xi[0];
    double yofs = xi[1];
    double zofs = xi[2];
    double r = xi[3];
    int a = 0;

    QVector<RawImuTuple>& rawImuVector = *reinterpret_cast<QVector<RawImuTuple>*>(obj);

    for(int count = 0; count < rawImuVector.count() ; count ++)
        {
            RawImuTuple d = rawImuVector[count];
            double x = d.magX;
            double y = d.magY;
            double z = d.magZ;
            double err = r - sqrt(pow((x + xofs), 2) + pow((y + yofs), 2) + pow((z + zofs), 2));
            fi[a] = err;
            a++;
        }
}

void CompassConfig::showCompassMotorCalibrationDialog()
{
    CompassMotorCalibrationDialog *dialog = new CompassMotorCalibrationDialog();
    if(dialog->exec() == QDialog::Accepted){
        // This modal, as you cannot do anything else while doing a compassMot
        QLOG_DEBUG() << "Compass Mot Success!";
    } else {
        QLOG_DEBUG() << "Compass Mot Cancelled!";
    }

}
