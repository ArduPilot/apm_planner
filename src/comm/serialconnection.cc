/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 *   @brief LinkManager
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author QGROUNDCONTROL PROJECT - This code has GPLv3+ snippets from QGROUNDCONTROL, (c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 */


#include "serialconnection.h"
#include "QsLog.h"
#include <QtSerialPort/qserialportinfo.h>
#include <QSettings>
#include <QStringList>
#include <QTimer>
SerialConnection::SerialConnection() : SerialLinkInterface(),
    m_port(0),
    m_isConnected(false),
    m_retryCount(0),
    m_timeoutsEnabled(true),
    m_timeoutMessageSent(false)
{
    QLOG_DEBUG() << "Create Serial Connection:" << this;
    m_linkId = getNextLinkId();

    loadSettings();

    if (m_portName.length() == 0) {
        // Create a new serial link
        getCurrentPorts();
        if (!m_portList.isEmpty())
            m_portName = m_portList.first().trimmed();
        else
            m_portName = "No Devices";
    }
    m_timeoutTimer = new QTimer(this);
    QObject::connect(m_timeoutTimer,SIGNAL(timeout()),this,SLOT(timeoutTimerTick()));
    m_timeoutTimer->start(500);

    QLOG_INFO() <<  m_portName << m_baud;
}

SerialConnection::~SerialConnection()
{
    QLOG_DEBUG() << "Destroy Serial Connection:" << this;
}

void SerialConnection::connectionDestroyed(QObject *object)
{
    QLOG_DEBUG() << "serial connection: object destroyed:" << object;
}

void SerialConnection::portError(QSerialPort::SerialPortError serialPortError)
{
    QLOG_ERROR() << "serial connection: error " << serialPortError;

    switch(serialPortError){
    case QSerialPort::ReadError: // Required for commands when the port is open.
    case QSerialPort::WriteError:
    case QSerialPort::DeviceNotFoundError:
        {
            //Windows triggers this when the port disappears
            //Break out if we're not connected, could be a genuine error.
            if (!m_isConnected)
            {
                break;
            }
        }
    case QSerialPort::ResourceError:
        disconnect();
        break;
    case QSerialPort::NotOpen:
    case QSerialPort::OpenError:
    default:
        ;// Do nothing
    }
}

void SerialConnection::timeoutTimerTick()
{
    if (!m_isConnected || !m_timeoutsEnabled)
    {
        //Don't care if we're not connected
        return;
    }
    if (QDateTime::currentMSecsSinceEpoch() > (m_lastTimeoutMessage + SERIAL_TIMEOUT_MILLISECONDS))
    {
        if (m_timeoutsEnabled && !m_timeoutMessageSent)
        {
            m_timeoutMessageSent = true;
            emit timeoutTriggered(this);
        }
    }
}

bool SerialConnection::setPortName(QString portName)
{
    m_portName = portName;
    emit linkChanged(this);
    writeSettings();
    return true;
}

bool SerialConnection::setBaudRate(int baud)
{
    m_baud = baud;
    emit linkChanged(this);
    writeSettings();
    return true;
}
QList<QString> SerialConnection::getCurrentPorts()
{
    m_portList.clear();

    QList<QSerialPortInfo> portList =  QSerialPortInfo::availablePorts();

    if( portList.count() == 0){
        QLOG_INFO() << "No Ports Found" << m_portList;
    }

    foreach (const QSerialPortInfo &info, portList)
    {
        QLOG_TRACE() << "PortName    : " << info.portName()
                     << "Description : " << info.description();
        QLOG_TRACE() << "Manufacturer: " << info.manufacturer();

        m_portList.append(info.portName());
    }
    return m_portList;
}
QString SerialConnection::getPortName() const
{
    return m_portName;
}
int SerialConnection::getBaudRate() const
{
    return m_baud;
}
int SerialConnection::getDataBits() const
{
    return 0;
}
int SerialConnection::getStopBits() const
{
    return 0;
}
void SerialConnection::requestReset()
{

}
int SerialConnection::getBaudRateType() const
{
    return 0;
}
int SerialConnection::getFlowType() const
{
    return 0;
}
int SerialConnection::getParityType() const
{
    return 0;
}
int SerialConnection::getDataBitsType() const
{
    return 0;
}
int SerialConnection::getStopBitsType() const
{
    return 0;
}
bool SerialConnection::setBaudRateType(int rateIndex)
{
    Q_UNUSED(rateIndex);
    return true;
}
bool SerialConnection::setFlowType(int flow)
{
    Q_UNUSED(flow);
    return true;
}
bool SerialConnection::setParityType(int parity)
{
    Q_UNUSED(parity);
    return true;
}
bool SerialConnection::setDataBitsType(int dataBits)
{
    Q_UNUSED(dataBits);
    return true;
}
bool SerialConnection::setStopBitsType(int stopBits)
{
    Q_UNUSED(stopBits);
    return true;
}
void SerialConnection::loadSettings()
{
    QSettings settings;
    settings.sync();
    if (settings.contains("SERIALLINK_COMM_PORT"))
    {
        m_portName = settings.value("SERIALLINK_COMM_PORT").toString();
        m_baud = settings.value("SERIALLINK_COMM_BAUD",115200).toInt();
        if (m_baud < 0 || m_baud > 12500000)
        {
            //Bad baud rate.
            m_baud = 115200;
        }
        //m_parity = settings.value("SERIALLINK_COMM_PARITY").toInt();
        //m_stopBits = settings.value("SERIALLINK_COMM_STOPBITS").toInt();
        //m_dataBits = settings.value("SERIALLINK_COMM_DATABITS").toInt();
        //m_flowControl = settings.value("SERIALLINK_COMM_FLOW_CONTROL").toInt();
        QString portbaudmap = settings.value("SERIALLINK_COMM_PORTMAP").toString();
        QStringList portbaudsplit = portbaudmap.split(",");
        foreach (QString portbaud,portbaudsplit)
        {
            if (portbaud.split(":").size() == 2)
            {
                m_portBaudMap[portbaud.split(":")[0]] = portbaud.split(":")[1].toInt();
            }
        }
        if (m_portBaudMap.size() == 0)
        {
            m_portBaudMap[m_portName] = m_baud;
        }
    }
    else
    {
        m_baud = 115200;
    }
    emit linkChanged(this);
}
void SerialConnection::writeSettings()
{
    QSettings settings;
    settings.setValue("SERIALLINK_COMM_PORT", getPortName());
    settings.setValue("SERIALLINK_COMM_BAUD", getBaudRate());
    settings.setValue("SERIALLINK_COMM_PARITY", getParityType());
    settings.setValue("SERIALLINK_COMM_STOPBITS", getStopBits());
    settings.setValue("SERIALLINK_COMM_DATABITS", getDataBits());
    settings.setValue("SERIALLINK_COMM_FLOW_CONTROL", getFlowType());
    QString portbaudmap = "";
    for (QMap<QString,int>::const_iterator i=m_portBaudMap.constBegin();i!=m_portBaudMap.constEnd();i++)
    {
        portbaudmap += i.key() + ":" + QString::number(i.value()) + ",";
    }
    portbaudmap = portbaudmap.mid(0,portbaudmap.length()-1); //Remove the last comma (,)
    settings.setValue("SERIALLINK_COMM_PORTMAP",portbaudmap);
    settings.sync();
}

bool SerialConnection::connect()
{
    QLOG_DEBUG() << "SerialConnection::connect()";
    if (m_port)
    {
        //Port already exists
        disconnect();
    }
    m_port = new QSerialPort();
    QObject::connect(m_port,SIGNAL(readyRead()),this,SLOT(readyRead()));
    QObject::connect(m_port, SIGNAL(destroyed(QObject*)),this,SLOT(connectionDestroyed(QObject*)));
    QObject::connect(m_port, SIGNAL(error(QSerialPort::SerialPortError)),
                     this, SLOT(portError(QSerialPort::SerialPortError)), Qt::UniqueConnection);

#ifdef Q_OS_MACX
    // temp fix Qt5.4.1 issue on OSX
    // http://code.qt.io/cgit/qt/qtserialport.git/commit/?id=687dfa9312c1ef4894c32a1966b8ac968110b71e
    m_port->setPortName("/dev/cu." + m_portName);
#else
    m_port->setPortName(m_portName);
#endif

    if (!m_port->open(QIODevice::ReadWrite))
    {
        if (m_retryCount++ > 1)
        {
            m_retryCount = 0;
            emit error(this,"Error opening port: " + m_port->errorString());
            QLOG_ERROR() << "Error opening port" << m_port->errorString();
            m_port->deleteLater();
            m_port = 0;
            return false;
        }
        QLOG_ERROR() << "Error opening port" << m_port->errorString() << "trying again...";
        QTimer::singleShot(1000,this, SLOT(connect()));
        return false;
    }
    if (!m_port->setBaudRate(m_baud))
    {
        //Unable to set baud rate.
        emit error(this,"Unable to set baud rate: " + m_port->errorString());
        m_port->close();
        delete m_port;
        m_port = 0;
        return false;
    }
    if (!m_port->setParity(QSerialPort::NoParity))
    {
        emit error(this,"Unable to set parity rate: " + m_port->errorString());
        m_port->close();
        delete m_port;
        m_port = 0;
        return false;
    }
    if (!m_port->setDataBits(QSerialPort::Data8))
    {
        emit error(this,"Unable to set databits: " + m_port->errorString());
        m_port->close();
        delete m_port;
        m_port = 0;
        return false;
    }
    if (!m_port->setFlowControl(QSerialPort::NoFlowControl))
    {
        emit error(this,"Unable to set flow control: " + m_port->errorString());
        m_port->close();
        delete m_port;
        m_port = 0;
        return false;
    }
    if (!m_port->setStopBits(QSerialPort::OneStop))
    {
        emit error(this,"Unable to set stop bits: " + m_port->errorString());
        m_port->close();
        delete m_port;
        m_port = 0;
        return false;
    }
    m_lastTimeoutMessage = QDateTime::currentMSecsSinceEpoch();
    m_isConnected = true;
    m_timeoutMessageSent = false;
    emit connected();
    emit connected(true);
    emit connected(this);
    m_retryCount = 0;
    return true;
}
void SerialConnection::readyRead()
{
    if (!m_port)
    {
        //This shouldn't happen
    }
    m_lastTimeoutMessage = QDateTime::currentMSecsSinceEpoch();
    QByteArray bytes = m_port->readAll();
    emit bytesReceived(this,bytes);
}

void SerialConnection::disableTimeouts()
{
    m_timeoutsEnabled = false;
}

void SerialConnection::enableTimeouts()
{
    m_timeoutsEnabled = true;
}

int SerialConnection::getId() const
{
    return m_linkId;
}

QString SerialConnection::getName() const
{
    return m_portName;
}

QString SerialConnection::getShortName() const
{
    return m_portName;
}

QString SerialConnection::getDetail() const
{
    return QString::number(m_baud);
}

bool SerialConnection::isConnected() const
{
    return m_isConnected;
}

qint64 SerialConnection::getConnectionSpeed() const
{
    return m_baud;
}

bool SerialConnection::disconnect()
{
    QLOG_DEBUG() << "SerialConnection::disconnect()" << m_port;
    if (m_port)
    {
        m_port->close();
        m_port->deleteLater();
        m_port = 0;
        m_isConnected = false;
        emit disconnected();
        emit connected(false);
        emit disconnected(this);
        return true;
    }
    else
    {
        return false;
    }
}

qint64 SerialConnection::bytesAvailable()
{
    return 0;
}

void SerialConnection::writeBytes(const char* buf,qint64 size)
{
    if (m_port)
    {
        int error = m_port->write(buf,size);
        if (error == -1) {
            QLOG_DEBUG() << "serial connecton: write error = " << error;
        }
    }
}

void SerialConnection::readBytes()
{
    QLOG_DEBUG() << "serial connection: read bytes";
}

bool SerialConnection::setBaudRateString(QString rate)
{
    bool ok;
    int intrate = rate.toInt(&ok);
    if (!ok) {
        emit linkChanged(this);
        return false;
    }
    emit linkChanged(this);
    return setBaudRate(intrate);

}
