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
#include "LogConsole.h"
#include "MainWindow.h"

#include <kmlcreator.h>
#include <QSettings>
#include <QStatusBar>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QComboBox>
#include <QTimer>
#include <qserialportinfo.h>
#include <qserialport.h>
#include <QPointer>
#include <QFileDialog>

TerminalConsole::TerminalConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TerminalConsole),
    m_windowVisible(false)
{
    ui->setupUi(this);

    // create the cosole and add it to the centralwidget
    m_console = new Console;
    m_console->setEnabled(false);

    m_logConsole = new LogConsole;
    m_logConsole->setVisible(false);
    m_logConsole->setEnabled(false);

    m_statusBar = new QStatusBar;

    QLayout* layout = ui->terminalGroupBox->layout();
    layout->addWidget(m_console);
    layout->addWidget(m_statusBar);
    layout->addWidget(m_logConsole);

    m_serial = new QSerialPort(this);
    m_settingsDialog = new SettingsDialog;

    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->settingsButton->setEnabled(true);
    ui->logsButton->setEnabled(false);

    loadSettings();
    addBaudComboBoxConfig();
    fillPortsInfo(*ui->linkComboBox);

    initConnections();

    //Keep refreshing the serial port list
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(populateSerialPorts()));

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    if (UASManager::instance()->getActiveUAS())
    {
        activeUASSet(UASManager::instance()->getActiveUAS());
    }
}

void TerminalConsole::activeUASSet(UASInterface *uas)
{
    if(m_uas) {
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    }
    m_uas = uas;
    if (uas) {
        connect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        connect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
        uasConnected();
    }
}

void TerminalConsole::uasConnected()
{
    ui->connectButton->setEnabled(false);
    if(m_windowVisible){
        MainWindow::instance()->toolBar().disableConnectWidget(false);
    }
}

void TerminalConsole::uasDisconnected()
{
    ui->connectButton->setEnabled(true);
    if(m_windowVisible){
        MainWindow::instance()->toolBar().disableConnectWidget(true);
    }
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
    // QLOG_DEBUG() << "fillPortsInfo ";
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
            // QLOG_INFO() << "Inserting " << list.first();
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
        comboBox.setCurrentIndex(comboBox.findText(m_settings.name));
        ui->baudComboBox->setCurrentIndex(ui->baudComboBox->findText(QString::number(m_settings.baudRate)));
    }
}

void TerminalConsole::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    // Start refresh Timer
    m_timer.start(2000);
    if (!m_uas) MainWindow::instance()->toolBar().disableConnectWidget(true);
    m_windowVisible = true;
}

void TerminalConsole::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    // Stop the port list refeshing
    m_timer.stop();
    MainWindow::instance()->toolBar().disableConnectWidget(false);
    m_windowVisible = false;
}

TerminalConsole::~TerminalConsole()
{
    delete m_serial;
    if (!m_console.isNull()) delete m_console;
    delete m_statusBar;
    delete m_settingsDialog;
    delete m_logConsole;
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

void TerminalConsole::logsButtonClicked() {
    bool visible = m_logConsole->isVisible();

    if(visible) {
        m_logConsole->setVisible(false);
        m_logConsole->setEnabled(false);
        m_logConsole->setSerial(0);
        logConsoleHidden();
        m_logConsole->onShow(false);
    }
    else {
        m_logConsole->setVisible(true);
        m_logConsole->setEnabled(true);
        m_logConsole->setSerial(m_serial);
        logConsoleShown();
        m_logConsole->onShow(true);
    }
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
            m_timer.stop();
            ui->logsButton->setEnabled(true);
        } else {
            m_serial->close();
            QString errorMessage = m_serial->errorString()
                    + tr("\nPlease ensure you have disconnected from the UAS, before connecting using terminal mode.");
            QMessageBox::critical(this, tr("Error"), errorMessage);

            m_statusBar->showMessage(tr("Open error:") + errorMessage);
            ui->logsButton->setEnabled(false);
        }
    } else {
        QString errorMessage = m_serial->errorString()
                + tr("\nPlease ensure you have disconnected from the UAS, before connecting using terminal mode.");

#ifdef Q_OS_LINUX
        if(m_serial->errorString().contains("busy"))
        {
            errorMessage = tr("ERROR: Port ") + m_serial->portName() + tr(" is locked by an external process. Try uninstalling \"modemmanager\" or run: \"sudo lsof /dev/") + m_serial->portName() + tr("\" to determine the interfering application.");
        }
#endif

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
    m_timer.start(2000); //re-start port scanning
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

    m_console->putData(data);

    // On reset, send the break sequence and display help
    if (data.contains("ENTER 3")) {
        m_serial->write("\r\r\r");
        m_serial->waitForBytesWritten(10);
        m_serial->write("HELP\r");
    }
    if (m_serial->error() != QSerialPort::NoError && m_serial->error() != QSerialPort::UnknownError)
    {
        //Serial port has gone bad???
        QLOG_DEBUG() << "Serial port has bad things happening!!!" << m_serial->errorString();
       // break;
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
    connect(ui->logsButton, SIGNAL(released()), this, SLOT(logsButtonClicked()));

    connect(ui->baudComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBaudRate(int)));
    connect(ui->linkComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setLink(int)));
    connect(ui->logToKmlButton, SIGNAL(released()), this, SLOT(logToKmlClicked()));

    // Serial Port Connections
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(m_console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));

    connect(m_logConsole, SIGNAL(statusMessage(QString)), this, SLOT(logConsoleStatusMessage(QString)));
    connect(m_logConsole, SIGNAL(activityStart()), this, SLOT(logConsoleActivityStart()));
    connect(m_logConsole, SIGNAL(activityStop()), this, SLOT(logConsoleActivityStop()));
}

void TerminalConsole::logToKmlClicked() {
    QString filename = QFileDialog::getOpenFileName(this, "Open Log File", QGC::logDirectory(), tr("Log Files (*.log)"));
    QApplication::processEvents(); // Helps clear dialog from screen

    if(filename.length() > 0) {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString kmlFile(filename);
            kmlFile.replace(".log", ".kml");
            kml::KMLCreator kml;

            kml.start(kmlFile);
            QTextStream in(&file);
            while(!in.atEnd()) {
                QString line = in.readLine();
                kml.processLine(line);
            }

            QString generated = kml.finish(true);
            file.close();

            QString msg = QString("Generated %1.").arg(generated);
            QMessageBox::information(this, "Log to KML", msg);
        }
        else {
            QString msg = QString("Unable to open %1.").arg(filename);
            QMessageBox::warning(this, "Log to KML", msg, QMessageBox::Ok);
        }
    }
}

void TerminalConsole::logConsoleShown() {
    disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void TerminalConsole::logConsoleHidden() {
    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
}

void TerminalConsole::logConsoleStatusMessage(QString msg) {
    m_statusBar->showMessage(msg);
}

void TerminalConsole::logConsoleActivityStart() {
    ui->logsButton->setEnabled(false);
}

void TerminalConsole::logConsoleActivityStop() {
    ui->logsButton->setEnabled(true);
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
//    QLOG_INFO() << "Changed Link to:" << m_settings.name;

}

void TerminalConsole::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
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
    QSettings settings;
    settings.setValue("TERMINALCONSOLE_COMM_PORT", m_settings.name);
    settings.setValue("TERMINALCONSOLE_COMM_BAUD", m_settings.baudRate);
    settings.setValue("TERMINALCONSOLE_COMM_PARITY", m_settings.parity);
    settings.setValue("TERMINALCONSOLE_COMM_STOPBITS", m_settings.stopBits);
    settings.setValue("TERMINALCONSOLE_COMM_DATABITS", m_settings.dataBits);
    settings.setValue("TERMINALCONSOLE_COMM_FLOW_CONTROL", m_settings.flowControl);
    settings.sync();
}



