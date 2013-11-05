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

/**
 * @file
 *   @brief Terminal Console display View.
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 * Influenced from Qt examples by :-
 * Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
 * Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
 *
 */

#include "TerminalConsole.h"
#include "QsLog.h"
#include "SerialSettingsDialog.h"
#include "ui_TerminalConsole.h"
#include "Console.h"
#include "configuration.h"


#include <QSettings>
#include <QStatusBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTimer>
#include <qserialportinfo.h>
#include <qserialport.h>
#include <QPointer>

TerminalConsole::TerminalConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TerminalConsole)
{
    ui->setupUi(this);

    // create the cosole and add it to the centralwidget
    m_console = new Console;
    m_console->setEnabled(false);

    m_statusBar = new QStatusBar;

    QLayout* layout = ui->terminalGroupBox->layout();
    layout->addWidget(m_console);
    layout->addWidget(m_statusBar);

    m_serial = new QSerialPort(this);
    m_settingsDialog = new SettingsDialog;

    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->settingsButton->setEnabled(true);


    addBaudComboBoxConfig();
    fillPortsInfo(*ui->linkComboBox);

    loadSettings();

    initConnections();

    //Keep refreshing the serial port list
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(populateSerialPorts()));
}

void TerminalConsole::addBaudComboBoxConfig()
{
    ui->baudComboBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui->baudComboBox->addItem(QLatin1String("57600"), QSerialPort::Baud57600);
    ui->baudComboBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui->baudComboBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->baudComboBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui->baudComboBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
}

void TerminalConsole::fillPortsInfo(QComboBox &comboBox)
{
    QLOG_DEBUG() << "fillPortsInfo ";
    QString current = comboBox.itemText(comboBox.currentIndex());
    disconnect(&comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
    comboBox.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        int found = comboBox.findData(list);
        if (found == -1) {
            QLOG_INFO() << "Inserting " << list.first();
            comboBox.insertItem(0,list[0], list);
        } else {
            // Do nothing as the port is already listed
        }
    }
    for (int i=0;i<comboBox.count();i++)
    {
        if (comboBox.itemText(i) == current)
        {
            comboBox.setCurrentIndex(i);
            setLink(comboBox.currentIndex());
            connect(&comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
            return;
        }
    }
    connect(&comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
    if (current == "") {
        setLink(comboBox.currentIndex());
    }
}

void TerminalConsole::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    // Start refresh Timer
    m_timer->start(2000);
}

void TerminalConsole::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    // Stop the port list refeshing
    m_timer->stop();

}

TerminalConsole::~TerminalConsole()
{
    delete m_serial;
    delete m_console;
    delete m_statusBar;
    delete m_settingsDialog;
    delete ui;
}

void TerminalConsole::populateSerialPorts()
{
    QLOG_TRACE() << "populateSerialPorts";
    fillPortsInfo(*ui->linkComboBox);
}

void TerminalConsole::openSerialPort()
{
    openSerialPort(m_settings);
}

void TerminalConsole::openSerialPort(const SerialSettings &settings)
{
    m_serial->setPortName(settings.name);
    if (m_serial->open(QIODevice::ReadWrite)) {
        if (m_serial->setBaudRate(settings.baudRate)
                && m_serial->setDataBits(settings.dataBits)
                && m_serial->setParity(settings.parity)
                && m_serial->setStopBits(settings.stopBits)
                && m_serial->setFlowControl(settings.flowControl)) {

            m_console->setEnabled(true);
            m_console->setLocalEchoEnabled(false);
            ui->connectButton->setEnabled(false);
            ui->disconnectButton->setEnabled(true);
            ui->settingsButton->setEnabled(false);
            m_statusBar->showMessage(tr("Connected to %1 : baud %2")
                                       .arg(settings.name).arg(QString::number(settings.baudRate)));
            QLOG_INFO() << "Open Terminal Console Serial Port";
            writeSettings(); // Save last successful connection

            sendResetCommand();
            m_timer->stop();
        } else {
            m_serial->close();
            QString errorMessage = m_serial->errorString()
                    + tr("\nPlease ensure you have disconnected from the UAS, before connecting using terminal mode.");
            QMessageBox::critical(this, tr("Error"), errorMessage);

            m_statusBar->showMessage(tr("Open error:") + errorMessage);
        }
    } else {
        QString errorMessage = m_serial->errorString()
                + tr("\nPlease ensure you have disconnected from the UAS, before connecting using terminal mode.");
        QMessageBox::critical(this, tr("Error"), errorMessage);

        m_statusBar->showMessage(tr("Configure error: ") + errorMessage);
        QLOG_ERROR() << "ERROR: Cannot open" << settings.name << "baud" << settings.baudRate;
    }
}

void TerminalConsole::closeSerialPort()
{
    m_serial->close();
    m_console->setEnabled(false);
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->settingsButton->setEnabled(true);
    m_statusBar->showMessage(tr("Disconnected"));
    m_timer->start(2000); //re-start port scanning
}

void TerminalConsole::sendResetCommand()
{
    if (m_serial->isOpen()) {
        m_serial->setDataTerminalReady(true);
        m_serial->waitForBytesWritten(250);
        m_serial->setDataTerminalReady(false);
    }
}

void TerminalConsole::writeData(const QByteArray &data)
{
//    QLOG_TRACE() << "writeData:" << data;
    m_serial->write(data);
}

void TerminalConsole::readData()
{
    QByteArray data = m_serial->readAll();
//    QLOG_TRACE() << "readData:" << data;
    m_console->putData(data);

    // On reset, send the break sequence and display help
    if (data.contains("ENTER 3")) {
        m_serial->write("\r\r\r");
        m_serial->waitForBytesWritten(10);
        m_serial->write("HELP\r");
    }
}

void TerminalConsole::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), m_serial->errorString());
        closeSerialPort();
    }
}

void TerminalConsole::initConnections()
{
    // Ui Connections
    connect(ui->connectButton, SIGNAL(released()), this, SLOT(openSerialPort()));
    connect(ui->disconnectButton, SIGNAL(released()), this, SLOT(closeSerialPort()));
    connect(ui->settingsButton, SIGNAL(released()), m_settingsDialog, SLOT(show()));
    connect(ui->clearButton, SIGNAL(released()), m_console, SLOT(clear()));

    connect(ui->baudComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBaudRate(int)));
    connect(ui->linkComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setLink(int)));

    // Serial Port Connections
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
}

void TerminalConsole::setBaudRate(int index)
{
    m_settings.baudRate = static_cast<QSerialPort::BaudRate>(
                ui->baudComboBox->itemData(index).toInt());
    QLOG_INFO() << "Changed Baud to:" << m_settings.baudRate;

}

void TerminalConsole::setLink(int index)
{
    if (index == -1)
    {
        return;
    }
    //m_settings.name = ui->linkComboBox->currentText();
    m_settings.name = ui->linkComboBox->itemData(index).toStringList()[0];
    QLOG_INFO() << "Changed Link to:" << m_settings.name;

}

void TerminalConsole::loadSettings()
{
    // Load defaults from settings
    QSettings settings(QGC::COMPANYNAME, QGC::APPNAME);
    settings.sync();
    if (settings.contains("TERMINALCONSOLE_COMM_PORT"))
    {
        m_settings.name = settings.value("TERMINALCONSOLE_COMM_PORT").toString();
        m_settings.baudRate = settings.value("TERMINALCONSOLE_COMM_BAUD").toInt();
        m_settings.parity = static_cast<QSerialPort::Parity>
                (settings.value("TERMINALCONSOLE_COMM_PARITY").toInt());
        m_settings.stopBits = static_cast<QSerialPort::StopBits>
                (settings.value("TERMINALCONSOLE_COMM_STOPBITS").toInt());
        m_settings.dataBits = static_cast<QSerialPort::DataBits>
                (settings.value("TERMINALCONSOLE_COMM_DATABITS").toInt());
        m_settings.flowControl = static_cast<QSerialPort::FlowControl>
                (settings.value("TERMINALCONSOLE_COMM_FLOW_CONTROL").toInt());
    } else {
        // init the structure
    }
}

void TerminalConsole::writeSettings()
{
    // Store settings
    QSettings settings(QGC::COMPANYNAME, QGC::APPNAME);
    settings.setValue("TERMINALCONSOLE_COMM_PORT", m_settings.name);
    settings.setValue("TERMINALCONSOLE_COMM_BAUD", m_settings.baudRate);
    settings.setValue("TERMINALCONSOLE_COMM_PARITY", m_settings.parity);
    settings.setValue("TERMINALCONSOLE_COMM_STOPBITS", m_settings.stopBits);
    settings.setValue("TERMINALCONSOLE_COMM_DATABITS", m_settings.dataBits);
    settings.setValue("TERMINALCONSOLE_COMM_FLOW_CONTROL", m_settings.flowControl);
    settings.sync();
}



