/*=====================================================================

APM_PLANNER Open Source Ground Control Station

(c) 2013, Bill Bonney <billbonney@communistech.com>

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

#include "RadioFlashWizard.h"
#include "ui_RadioFlashWizard.h"

#include "QsLog.h"
#include "QGC.h"

#include <QDir>
#include <QProcess>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>

static const QString FirmwareBaseURL = "./sik_uploader/firmware/";
static const QString FirmwareVersionURL = "1.9";

RadioFlashWizard::RadioFlashWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RadioFlashWizard)
{
    ui->setupUi(this);

    connect(ui->flashPushButton, SIGNAL(clicked()), this, SLOT(flashRadio()));
    connect(this, SIGNAL(finished(int)),SLOT(accept()));
    connect(this, SIGNAL(currentIdChanged(int)), this, SLOT(selectPage(int)));

    // Set up Serial Port Baud Rate selection
    ui->baudrateComboBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui->baudrateComboBox->addItem(QLatin1String("57600"), QSerialPort::Baud57600);
    ui->baudrateComboBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->baudrateComboBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->baudrateComboBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    ui->baudrateComboBox->addItem(QLatin1String("9600"), QSerialPort::Baud4800);
    ui->baudrateComboBox->addItem(QLatin1String("9600"), QSerialPort::Baud2400);
    ui->baudrateComboBox->addItem(QLatin1String("9600"), QSerialPort::Baud1200);

    ui->baudrateComboBox->setCurrentIndex(ui->baudrateComboBox->findData(QSerialPort::Baud57600)); // Select 57600 default
}

RadioFlashWizard::~RadioFlashWizard()
{
    delete m_updateProcess;
    m_updateProcess = NULL;

    delete ui;
}

void RadioFlashWizard::accept()
{
    this->done(0);
}

void RadioFlashWizard::selectPage(int index)
{
    switch (index) {
    case 0:

        break;
    case 1:
        if (ui->standardRadio->isChecked() || ui->rfd900aRadioButton->isChecked()
                || ui->rfd900uRadioButton->isChecked()){
            portSelectionShown();
        } else {
            QMessageBox::warning(this,"Error","Please select a radio type.");
            back();
        }

        break;
    default:
        break;
    }
}

void RadioFlashWizard::portSelectionShown()
{
    ui->deviceComboBox->clear();
    QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();

    foreach (const QSerialPortInfo& info, serialPortInfoList) {
        ui->deviceComboBox->addItem(info.portName(),info.systemLocation());
    }
}

void RadioFlashWizard::flashRadio()
{
    QLOG_DEBUG() << "Flash Radio:";
    ui->plainTextEdit->insertPlainText("\nFlashing with firmware\n");
    ui->plainTextEdit->insertPlainText("./sik_uploader/firmware/1.9/radio~hm_trp.ihx\n");

    QStringList argumentsList;
    argumentsList << "./sik_uploader/sik_uploader.py";
    argumentsList << "--port" << ui->deviceComboBox->currentData().toString();
    argumentsList << "--baudrate" << ui->baudrateComboBox->currentText();
    argumentsList << "./sik_uploader/firmware/1.9/radio~hm_trp.ihx";

    QLOG_DEBUG() << "commands: python " << argumentsList;

    m_updateProcess = new QProcess();
    connect(m_updateProcess, SIGNAL(readyReadStandardOutput()), SLOT(standardOutputReady()));
    connect(m_updateProcess, SIGNAL(readyReadStandardError()), SLOT(standardErrorReady()));
    connect(m_updateProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            SLOT(processFinished(int)));

    QLOG_DEBUG() << "App dir:" << QCoreApplication::applicationDirPath();
    m_updateProcess->setWorkingDirectory(QCoreApplication::applicationDirPath());

    QLOG_DEBUG() << "Working dir:" << m_updateProcess->workingDirectory();

    m_updateProcess->start("python", argumentsList);
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(processTimeout()));
    m_timer.start(100);
}

void RadioFlashWizard::processTimeout()
{
    if(!m_updateProcess)
        return;
    if(m_updateProcess->bytesAvailable()>0){
        QString output = m_updateProcess->readAll();
        ui->plainTextEdit->insertPlainText(output);
    } else {
        ui->plainTextEdit->insertPlainText(".");
    }
}

void RadioFlashWizard::standardOutputReady()
{
    QString output = m_updateProcess->readAllStandardOutput();
    ui->plainTextEdit->insertPlainText(output);
    ui->plainTextEdit->update();
}

void RadioFlashWizard::standardErrorReady()
{
    QString error = m_updateProcess->readAllStandardError();
    ui->plainTextEdit->insertPlainText(error);
    ui->plainTextEdit->update();
}

void RadioFlashWizard::processFinished(int exitCode)
{
    QLOG_DEBUG() << "Flash Update Process Finished: " << exitCode;
    ui->plainTextEdit->insertPlainText(QString("\nComplete %1").arg(exitCode));
    m_timer.stop();
    m_updateProcess->deleteLater();
    m_updateProcess = NULL;
}

QString RadioFlashWizard::getFirmwareImageName(int index)
{
    QStringList imageList;
    imageList << "radio~hm_trp.ihx";
    imageList << "RFD_SiK_V1.9_rfd900a.ihx";
    imageList << "RFD_SiK_V1.9_rfd900u.ihx";
    return FirmwareBaseURL + FirmwareVersionURL + "/" + imageList[index];
}

