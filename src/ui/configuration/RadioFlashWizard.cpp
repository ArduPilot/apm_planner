#include "RadioFlashWizard.h"
#include "ui_RadioFlashWizard.h"

#include "QsLog.h"

#include <QDir>
#include <QProcess>
#include <QSerialPort>
#include <QSerialPortInfo>

RadioFlashWizard::RadioFlashWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RadioFlashWizard)
{
    ui->setupUi(this);

    connect(ui->flashPushButton, SIGNAL(clicked()), this, SLOT(flashRadio()));
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

}

void RadioFlashWizard::selectPage(int index)
{
    switch (index) {
    case 0:

        break;
    case 1:
        portSelectionShown();
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
    QStringList argumentsList;

    argumentsList << "./sik_uploader/sik_uploader.py";
    argumentsList << "--port" << ui->deviceComboBox->currentData().toString();
    argumentsList << "--baudrate" << ui->baudrateComboBox->currentText();
    argumentsList << "./sik_uploader/firmware/1.9/radio~hm_trp.ihx";

    QLOG_DEBUG() << "commands: python " << argumentsList;

    m_updateProcess = new QProcess();
    connect(m_updateProcess, SIGNAL(readyReadStandardOutput()), SLOT(standardOutputReady()));
    connect(m_updateProcess, SIGNAL(finished(int,QProcess::ExitStatus)),
            SLOT(processFinished(int)));

    QLOG_DEBUG() << "Working dir:" << m_updateProcess->workingDirectory();
    m_updateProcess->execute("python", argumentsList);
}

void RadioFlashWizard::standardOutputReady()
{
    QString output = m_updateProcess->readAllStandardOutput();
    ui->plainTextEdit->insertPlainText(output);
}

void RadioFlashWizard::processFinished(int exitCode)
{
    QLOG_DEBUG() << "Flash Update Process Finished: " << exitCode;
    m_updateProcess->deleteLater();
    m_updateProcess = NULL;
}

