#include "serialconnection.h"
#include "QsLog.h"
#include <qserialportinfo.h>
#include <QSettings>
#include <QStringList>
SerialConnection::SerialConnection(QObject *parent) : SerialLinkInterface()
{
    m_linkId = getNextLinkId();
    m_isConnected = false;
    loadSettings();

    if (m_portName.length() == 0) {
        // Create a new serial link
        getCurrentPorts();
        if (!m_portList.isEmpty())
            m_portName = m_portList.first().trimmed();
        else
            m_portName = "No Devices";
    }

    QLOG_INFO() <<  m_portName << m_baud;
}
bool SerialConnection::setPortName(QString portName)
{
    m_portName = portName;
    emit updateLink(this);
    writeSettings();
    return true;
}

bool SerialConnection::setBaudRate(int baud)
{
    m_baud = baud;
    emit updateLink(this);
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
    return true;
}
bool SerialConnection::setFlowType(int flow)
{
    return true;
}
bool SerialConnection::setParityType(int parity)
{
    return true;
}
bool SerialConnection::setDataBitsType(int dataBits)
{
    return true;
}
bool SerialConnection::setStopBitsType(int stopBits)
{
    return true;
}
void SerialConnection::loadSettings()
{
    QSettings settings;
    settings.sync();
    if (settings.contains("SERIALLINK_COMM_PORT"))
    {
        m_portName = settings.value("SERIALLINK_COMM_PORT").toString();
        m_baud = settings.value("SERIALLINK_COMM_BAUD").toInt();
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
    emit updateLink(this);
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
    m_port = new QSerialPort();
    QObject::connect(m_port,SIGNAL(readyRead()),this,SLOT(readyRead()));
    m_port->setPortName(m_portName);

    if (!m_port->open(QIODevice::ReadWrite))
    {
        QLOG_ERROR() << "Error opening port" << m_port->errorString();
    }
    m_port->setBaudRate(m_baud);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setStopBits(QSerialPort::OneStop);
    m_isConnected = true;
    emit connected();
    emit connected(true);
    emit connected(this);
    return true;
}
void SerialConnection::readyRead()
{
    QByteArray bytes = m_port->readAll();
    emit bytesReceived(this,bytes);
}

void SerialConnection::disableTimeouts()
{

}

void SerialConnection::enableTimeouts()
{

}

int SerialConnection::getId() const
{
    return m_linkId;
}

QString SerialConnection::getName() const
{
    return m_portName;
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
    m_port->close();
    m_port->deleteLater();
    m_port = 0;
    m_isConnected = false;
    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    return true;
}

qint64 SerialConnection::bytesAvailable()
{
    return 0;
}

void SerialConnection::writeBytes(const char* buf,qint64 size)
{
    m_port->write(buf,size);
}

void SerialConnection::readBytes()
{

}
bool SerialConnection::setBaudRateString(QString rate)
{
    bool ok;
    int intrate = rate.toInt(&ok);
    if (!ok) {
        emit updateLink(this);
        return false;
    }
    emit updateLink(this);
    return setBaudRate(intrate);

}
