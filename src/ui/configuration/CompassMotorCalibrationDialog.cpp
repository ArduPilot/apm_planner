#include "QsLog.h"
#include "qcustomplot.h"
#include "UASInterface.h"
#include "UAS.h"
#include "UASManager.h"
#include "QGCUASParamManager.h"
#include "QMessageBox"

#include "CompassMotorCalibrationDialog.h"
#include "ui_CompassMotorCalibrationDialog.h"

CompassMotorCalibrationDialog::CompassMotorCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CompassMotorCalibrationDialog),
    m_uasInterface(NULL)
{
    ui->setupUi(this);

    QCustomPlot* customPlot = ui->customPlot;
    customPlot->addGraph();
    customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
    customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
    customPlot->xAxis->setLabel("Throttle (%)");
    customPlot->yAxis->setLabel("Interference (%)");
    customPlot->xAxis->setRange(0,100);
    customPlot->yAxis->setRange(0,100);

    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red)); // line color blue for first graph
    customPlot->graph(1)->setBrush(QBrush(QColor(0, 255, 0, 20)));
    customPlot->yAxis2->setVisible(true);
    customPlot->yAxis2->setLabel("Amps (A)");
    customPlot->xAxis2->setRange(0,100);
    customPlot->yAxis2->setRange(0,50);

    customPlot->replot();

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(okButtonClicked()));
    connect(this, SIGNAL(about), this, SLOT(rejected()));
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
}

CompassMotorCalibrationDialog::~CompassMotorCalibrationDialog()
{
    cancelCalibration();
    delete ui;
}

void CompassMotorCalibrationDialog::activeUASSet(UASInterface *uas)
{
    if (m_uasInterface){
        disconnect(ui->startButton, SIGNAL(clicked()), this, SLOT(startCalibration()));
        disconnect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopCalibration()));
        disconnect(m_uasInterface, SIGNAL(compassMotCalibration(mavlink_compassmot_status_t)),
                this, SLOT(compassMotCalibration(mavlink_compassmot_status_t)));
        disconnect(m_uasInterface, SIGNAL(textMessageReceived(int,int,int,QString)),
                this, SLOT(textMessageReceived(int,int,int,QString)));
    }
    m_uasInterface = uas;

    if(m_uasInterface){
        connect(ui->startButton, SIGNAL(clicked()), this, SLOT(startCalibration()));
        connect(ui->stopButton, SIGNAL(clicked()), this, SLOT(stopCalibration()));
        connect(m_uasInterface, SIGNAL(compassMotCalibration(mavlink_compassmot_status_t*)),
                this, SLOT(compassMotCalibration(mavlink_compassmot_status_t*)));
    }
}

void CompassMotorCalibrationDialog::compassMotCalibration(mavlink_compassmot_status_t *compassmot_status)
{
    if (!m_uasInterface)
        return; // no active UAS.

    QCustomPlot* customPlot = ui->customPlot;

    int index = compassmot_status->throttle/10;
    customPlot->graph(0)->addData(index, compassmot_status->current);
    customPlot->graph(1)->addData(index, compassmot_status->interference);
    customPlot->replot();

    x_scalar = compassmot_status->CompensationX;
    y_scalar = compassmot_status->CompensationY;
    z_scalar = compassmot_status->CompensationZ;

}

void CompassMotorCalibrationDialog::textMessageReceived(int uasid, int componentid,
                                                        int severity, QString text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);
    Q_UNUSED(severity);
    ui->messageTextEdit->insertPlainText(text +"\n");
}

void CompassMotorCalibrationDialog::startCalibration()
{
    if (!m_uasInterface)
        return; // no active UAS.
    QLOG_DEBUG() << "COMPASSMOT: starting compass mot calibration";
    m_uasInterface->startCompassMotCalibration();
    connect(m_uasInterface, SIGNAL(textMessageReceived(int,int,int,QString)),
            this, SLOT(textMessageReceived(int,int,int,QString)), Qt::UniqueConnection);
}

void CompassMotorCalibrationDialog::stopCalibration()
{
    if (!m_uasInterface)
        return; // no active UAS.
    QLOG_DEBUG() << "COMPASSMOT: stop compass mot calibration";
    disconnect(m_uasInterface, SIGNAL(textMessageReceived(int,int,int,QString)),
            this, SLOT(textMessageReceived(int,int,int,QString)));

    // Need to send ACK
    m_uasInterface->executeCommandAck(MAV_CMD_PREFLIGHT_CALIBRATION, MAV_RESULT_ACCEPTED);
    ui->messageTextEdit->insertPlainText("Calibration Stopped\n");
}

void CompassMotorCalibrationDialog::okButtonClicked()
{
    if(m_uasInterface){
        cancelCalibration();
        QLOG_DEBUG() << "COMPASSMOT: accepted = retrieving new values";
        m_uasInterface->requestParameter(1, "COMPASS_MOT_X");
        m_uasInterface->requestParameter(1, "COMPASS_MOT_Y");
        m_uasInterface->requestParameter(1, "COMPASS_MOT_Z");
        QMessageBox::information(this, "Sucess!",
                                 QString("New values have been stored\n X:%1 Y:%2 Z:%3")
                                 .arg(QString::number(x_scalar))
                                 .arg(QString::number(y_scalar))
                                 .arg(QString::number(z_scalar)));
    }
    accept();
}

void CompassMotorCalibrationDialog::cancelCalibration()
{
    if(m_uasInterface){
        m_uasInterface->executeCommandAck(MAV_CMD_PREFLIGHT_CALIBRATION, MAV_RESULT_FAILED);
    }
}

void CompassMotorCalibrationDialog::closeEvent(QCloseEvent *)
{
    cancelCalibration();
}

