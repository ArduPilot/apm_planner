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
#include "SerialSettingsDialog.h"
#include "Radio3DRConfig.h"
#include "configuration.h"

#include <QSettings>
#include <qserialportinfo.h>
#include <qserialport.h>
#include <QTimer>


Radio3DRConfig::Radio3DRConfig(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);

    m_serial = new QSerialPort(this);
    m_settingsDialog = new SettingsDialog;

    ui.settingsButton->setEnabled(true);

    addBaudComboBoxConfig();
    fillPortsInfo(*ui.linkPortComboBox);

    loadSettings();

    initConnections();

    //Keep refreshing the serial port list
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(populateSerialPorts()));
}

Radio3DRConfig::~Radio3DRConfig()
{
}

void Radio3DRConfig::addBaudComboBoxConfig()
{
    ui.baudPortComboBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    ui.baudPortComboBox->addItem(QLatin1String("57600"), QSerialPort::Baud57600);
    ui.baudPortComboBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    ui.baudPortComboBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui.baudPortComboBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    ui.baudPortComboBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
}

void Radio3DRConfig::fillPortsInfo(QComboBox &comboxBox)
{
    QLOG_DEBUG() << "3DR Radio fillPortsInfo ";
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        int found = comboxBox.findData(list);
        if (found == -1) {
            QLOG_INFO() << "Inserting " << list.first();
            comboxBox.insertItem(0,list[0], list);
        } else {
            // Do nothing as the port is already listed
        }
    }
}

void Radio3DRConfig::loadSettings()
{
    // Load defaults from settings
    QSettings settings(QGC::COMPANYNAME, QGC::APPNAME);
    settings.sync();
    if (settings.contains("3DRRADIO_COMM_PORT"))
    {
        m_settings.name = settings.value("3DRRADIO_COMM_PORT").toString();
        m_settings.baudRate = settings.value("3DRRADIO_COMM_BAUD").toInt();
        m_settings.parity = static_cast<QSerialPort::Parity>
                (settings.value("3DRRADIO_COMM_PARITY").toInt());
        m_settings.stopBits = static_cast<QSerialPort::StopBits>
                (settings.value("3DRRADIO_COMM_STOPBITS").toInt());
        m_settings.dataBits = static_cast<QSerialPort::DataBits>
                (settings.value("3DRRADIO_COMM_DATABITS").toInt());
        m_settings.flowControl = static_cast<QSerialPort::FlowControl>
                (settings.value("3DRRADIO_COMM_FLOW_CONTROL").toInt());
    } else {
        // init the structure
    }
}

void Radio3DRConfig::writeSettings()
{
    // Store settings
    QSettings settings(QGC::COMPANYNAME, QGC::APPNAME);
    settings.setValue("3DRRADIO_COMM_PORT", m_settings.name);
    settings.setValue("3DRRADIO_COMM_BAUD", m_settings.baudRate);
    settings.setValue("3DRRADIO_COMM_PARITY", m_settings.parity);
    settings.setValue("3DRRADIO_COMM_STOPBITS", m_settings.stopBits);
    settings.setValue("3DRRADIO_COMM_DATABITS", m_settings.dataBits);
    settings.setValue("3DRRADIO_COMM_FLOW_CONTROL", m_settings.flowControl);
    settings.sync();
}

void Radio3DRConfig::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    // Start refresh Timer
    m_timer->start(2000);
    loadSettings();
}

void Radio3DRConfig::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    // Stop the port list refeshing
    m_timer->stop();
    writeSettings();
}

void Radio3DRConfig::populateSerialPorts()
{
    QLOG_TRACE() << "populateSerialPorts";
    fillPortsInfo(*ui.linkPortComboBox);
}

void Radio3DRConfig::initConnections()
{
    // Ui Connections
    connect(ui.settingsButton, SIGNAL(released()), m_settingsDialog, SLOT(show()));

    connect(ui.baudPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBaudRate(int)));
    connect(ui.linkPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setLink(int)));

    // Serial Port Connections
    connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));

//    connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
//    connect(m_console, SIGNAL(getData(QByteArray)), this, SLOT(writeData(QByteArray)));
}

void Radio3DRConfig::setBaudRate(int index)
{
    m_settings.baudRate = static_cast<QSerialPort::BaudRate>(
                ui.baudPortComboBox->itemData(index).toInt());
    QLOG_INFO() << "Changed Baud to:" << m_settings.baudRate;

}

void Radio3DRConfig::setLink(int index)
{
    if (index == -1)
    {
        return;
    }
    m_settings.name = ui.linkPortComboBox->itemData(index).toStringList()[0];
    QLOG_INFO() << "Changed Link to:" << m_settings.name;

}
