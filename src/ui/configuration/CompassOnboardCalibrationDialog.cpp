#include "logging.h"
#include "UASInterface.h"
#include "UAS.h"
#include "UASManager.h"
#include "QGCUASParamManager.h"

#include <QtCore/qglobal.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QHBoxLayout>
#include <QMessageBox>

#include "CompassOnboardCalibrationDialog.h"
#include "ui_CompassOnboardCalibrationDialog.h"

using namespace QtDataVisualization;

QString magCalStatusToString(uint8_t status);

CompassOnboardCalibrationDialog::CompassOnboardCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompassOnboardCalibrationDialog)
{
    ui->setupUi(this);
    ui->progressBar->setValue(0);
    ui->statusLabel->setText("");

    // setupCustomPlot();
    addDataVisualization();

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    // connect(this, SIGNAL(about()), this, SLOT(rejected()));

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());

}

CompassOnboardCalibrationDialog::~CompassOnboardCalibrationDialog()
{
    delete ui;
}

void CompassOnboardCalibrationDialog::activeUASSet(UASInterface *uas)
{
    if (m_uasInterface){
        disconnect(ui->startButton, SIGNAL(clicked()), this, SLOT(startCalibration()));
        disconnect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopCalibration()));
        disconnect(
            m_uasInterface, SIGNAL(compassCalibrationProgress(UASInterface*,mavlink_mag_cal_progress_t&)),
            this, SLOT(compassCalibrationProgress(UASInterface*,mavlink_mag_cal_progress_t&))
        );
        disconnect(
            m_uasInterface, SIGNAL(compassCalibrationReport(UASInterface*,mavlink_mag_cal_report_t&)),
            this, SLOT(compassCalibrationReport(UASInterface*,mavlink_mag_cal_report_t&))
            );
        disconnect(
            m_uasInterface, SIGNAL(textMessageReceived(int,int,int,QString)),
            this, SLOT(textMessageReceived(int,int,int,QString))
        );
    }
    m_uasInterface = uas;

    if(m_uasInterface){
        connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startCalibration()));
        connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopCalibration()));
        connect(
            m_uasInterface, SIGNAL(compassCalibrationProgress(UASInterface*,mavlink_mag_cal_progress_t&)),
            this, SLOT(compassCalibrationProgress(UASInterface*,mavlink_mag_cal_progress_t&))
        );
        connect(
            m_uasInterface, SIGNAL(compassCalibrationReport(UASInterface*,mavlink_mag_cal_report_t&)),
            this, SLOT(compassCalibrationReport(UASInterface*,mavlink_mag_cal_report_t&))
        );
    }
}

void CompassOnboardCalibrationDialog::updateStatusMessage()
{
    QString statusMessage;

    foreach(mavlink_mag_cal_progress_t compass, m_compassCalibrationProgress) {
        statusMessage.append(
            QString("Compass %1 status %2 - %3\% ")
                .arg(
                    QString::number(compass.compass_id),
                    magCalStatusToString(compass.cal_status),
                    QString::number(compass.completion_pct)
                )
            );
    }

    ui->statusLabel->setText(statusMessage);
}

void CompassOnboardCalibrationDialog::compassCalibrationProgress(
    UASInterface *uas, mavlink_mag_cal_progress_t &mag_cal_progress)
{
    Q_UNUSED(uas);

    if (!m_uasInterface) {
        return; // no active UAS.
    }

    ui->progressBar->setValue(mag_cal_progress.completion_pct);

    m_compassCalibrationProgress[mag_cal_progress.compass_id] = mag_cal_progress;
    updateStatusMessage();

    QLOG_DEBUG() << "CAL: compass_id: " << mag_cal_progress.compass_id
                << " status: " << mag_cal_progress.cal_status
                << " direction_x: " << mag_cal_progress.direction_x
                << " direction_y: " << mag_cal_progress.direction_y
                << " direction_z: " << mag_cal_progress.direction_z;

    if (m_pointDataArray) {
        m_pointDataArray->append(QVector3D(mag_cal_progress.direction_x, mag_cal_progress.direction_y, mag_cal_progress.direction_z) );
    }

}

void CompassOnboardCalibrationDialog::compassCalibrationReport(
    UASInterface *uas, mavlink_mag_cal_report_t &mag_cal_report)
{
    if (!m_uasInterface) {
        return; // no active UAS.
    }

    if (m_compassCalibrationComplete[mag_cal_report.compass_id]) {
        QLOG_DEBUG() << "Already reported compass: " << mag_cal_report.compass_id;
        return;
    }

    if (mag_cal_report.cal_status == MAG_CAL_SUCCESS) {
        QMessageBox::information(
            this, "Calibration Complete",
            QString("SUCCESS: Stored\n Compass:%1 fitness: %2")
                .arg(QString::number(mag_cal_report.compass_id),QString::number(mag_cal_report.fitness))
            );
        m_compassCalibrationComplete[mag_cal_report.compass_id] = true;
        m_compassCalibrationProgress.remove(mag_cal_report.compass_id);
        uas->acceptOnboardCompassCalibration(mag_cal_report.compass_id);
        ui->progressBar->setValue(100);
        updateStatusMessage();
    } else {
        QMessageBox::information(
            this, "Calibration Complete",
            QString("FAILED: Compass:%1 fitness: %2")
                .arg(QString::number(mag_cal_report.compass_id),QString::number(mag_cal_report.fitness))

            );
        m_compassCalibrationComplete[mag_cal_report.compass_id] = true;
        m_compassCalibrationProgress.remove(mag_cal_report.compass_id);
        uas->acceptOnboardCompassCalibration(mag_cal_report.compass_id);
        ui->progressBar->setValue(100);
        updateStatusMessage();
    }
    QString status = QString("compass_id %1 status %2\n")
                         .arg(mag_cal_report.compass_id)
                         .arg(magCalStatusToString(mag_cal_report.cal_status));
    QLOG_INFO() << "COMPASS_CALIBRATION: " << status;
}

void CompassOnboardCalibrationDialog::textMessageReceived(int uasid, int componentid,
                                                        int severity, QString text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);
    Q_UNUSED(severity);
    Q_UNUSED(text);
}

void CompassOnboardCalibrationDialog::startCalibration()
{
    if (!m_uasInterface) {
        return; // no active UAS.
    }

    QLOG_DEBUG() << "COMPASS_ONBOARD_CAL: starting compass calibration";

    int ok = QMessageBox::information(
        this,
        "Compass Onboard Calibration",
        tr("Please make rotate the vehicle around its 3-axes X,Y,Z\n"
           "At 100% calibration will be complete."),
           QMessageBox::Ok,
           QMessageBox::Cancel
        );
    if (ok == QMessageBox::Cancel){
        QTimer::singleShot(100, this, SLOT(cancelCalibration()));
    } else {
        m_uasInterface->startOnboardCompassCalibration();
        delete m_pointDataArray;
        m_pointDataArray = new QScatterDataArray;
    }
}

void CompassOnboardCalibrationDialog::stopCalibration()
{
    if (!m_uasInterface) {
        return; // no active UAS.
    }
    QLOG_DEBUG() << "COMPASS_ONBOARD_CAL: stop compass calibration";

    cancelCalibration();
}

void CompassOnboardCalibrationDialog::okButtonClicked()
{
    int calibrationComplete = false;
    for (size_t i = 0; i < sizeof(m_compassCalibrationComplete); i++) {
        if (m_compassCalibrationComplete[i]) {
            calibrationComplete = true;
            break;
        }
    }
    if (m_uasInterface && calibrationComplete) {
        QLOG_DEBUG() << "COMPASS_ONBOARD_CAL: accepted = retrieving new values";
        QMessageBox::information(this, "Success!",
            QString("Compass Calibration Updated\nPlease reboot vehicle!"));
    }
    accept();
}

void CompassOnboardCalibrationDialog::cancelCalibration()
{
    if (m_uasInterface){
        ui->statusLabel->setText("Calibration Cancelled.");
        m_uasInterface->cancelOnboardCompassCalibration();
    }
}

void CompassOnboardCalibrationDialog::closeEvent(QCloseEvent *)
{
    cancelCalibration();
}

void CompassOnboardCalibrationDialog::addDataVisualization()
{
    Q3DScatter* graph = new Q3DScatter();
    QWidget* container = QWidget::createWindowContainer(graph);
    ui->graphHorizontalLayout->addWidget(container);

    m_proxy = new QScatterDataProxy();
    m_series = new QScatter3DSeries(m_proxy);

    QScatterDataArray *m_pointDataArray = new QScatterDataArray;

    m_proxy->resetArray(m_pointDataArray);
    graph->addSeries(m_series);
}

QString magCalStatusToString(uint8_t status)
{
    switch (status) {
    case MAG_CAL_NOT_STARTED:
        return "Not Started";
    case MAG_CAL_WAITING_TO_START:
        return "Waiting";
    case MAG_CAL_RUNNING_STEP_ONE:
        return "Step One";
    case MAG_CAL_RUNNING_STEP_TWO:
        return "Step Two";
    case MAG_CAL_SUCCESS:
        return "Success";
    case MAG_CAL_FAILED:
        return "Failed";
    case MAG_CAL_BAD_ORIENTATION:
        return "Bad Orientation";
    case MAG_CAL_BAD_RADIUS:
        return "Bad Radius";
    case MAG_CAL_STATUS_ENUM_END:
    default:
        return "unknown";
    }
}

