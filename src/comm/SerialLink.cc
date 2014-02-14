/*=====================================================================
======================================================================*/
/**
 * @file
 *   @brief Cross-platform support for serial ports
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#include "QsLog.h"
#include "SerialLink.h"
#include "LinkManager.h"
#include "QGC.h"
#include "UASInterface.h"

#include <QTimer>

#include <QSettings>
#include <QMutexLocker>
//#include <QtSerialPort/QSerialPort>
//#include <QtSerialPort/QSerialPortInfo>

#include <qserialport.h>
#include <qserialportinfo.h>
#include <MG.h>

SerialLink::SerialLink() :
    m_bytesRead(0),
    m_port(NULL),
    m_baud(QSerialPort::Baud115200),
    m_dataBits(QSerialPort::Data8),
    m_flowControl(QSerialPort::NoFlowControl),
    m_stopBits(QSerialPort::OneStop),
    m_parity(QSerialPort::NoParity),
    m_portName(""),
    m_stopp(false),
    m_reqReset(false)
{
    QLOG_INFO() << "create SerialLink: Load Previous Settings ";

    loadSettings();
    m_id = getNextLinkId();

    if (m_portName.length() == 0) {
        // Create a new serial link
        getCurrentPorts();
        if (!m_ports.isEmpty())
            m_portName = m_ports.first().trimmed();
        else
            m_portName = "No Devices";
    }

    QLOG_INFO() <<  m_portName << m_baud << m_flowControl
             << m_parity << m_dataBits << m_stopBits;

}
void SerialLink::requestReset()
{
    QMutexLocker locker(&this->m_stoppMutex);
    m_reqReset = true;
}

SerialLink::~SerialLink()
{
    disconnect();
    writeSettings();
    QLOG_INFO() << "Serial Link destroyed";
    if(m_port) delete m_port;
    m_port = NULL;
}

QList<QString> SerialLink::getCurrentPorts()
{
    m_ports.clear();

    QList<QSerialPortInfo> portList =  QSerialPortInfo::availablePorts();

    if( portList.count() == 0){
        QLOG_INFO() << "No Ports Found" << m_ports;
    }

    foreach (const QSerialPortInfo &info, portList)
    {
        QLOG_TRACE() << "PortName    : " << info.portName()
                     << "Description : " << info.description();
        QLOG_TRACE() << "Manufacturer: " << info.manufacturer();

        m_ports.append(info.portName());
    }
    return m_ports;
}

void SerialLink::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();
    if (settings.contains("SERIALLINK_COMM_PORT"))
    {
        m_portName = settings.value("SERIALLINK_COMM_PORT").toString();
        m_baud = settings.value("SERIALLINK_COMM_BAUD").toInt();
        m_parity = settings.value("SERIALLINK_COMM_PARITY").toInt();
        m_stopBits = settings.value("SERIALLINK_COMM_STOPBITS").toInt();
        m_dataBits = settings.value("SERIALLINK_COMM_DATABITS").toInt();
        m_flowControl = settings.value("SERIALLINK_COMM_FLOW_CONTROL").toInt();
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
}

void SerialLink::writeSettings()
{
    // Store settings
    QSettings settings;
    settings.setValue("SERIALLINK_COMM_PORT", getPortName());
    settings.setValue("SERIALLINK_COMM_BAUD", getBaudRateType());
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


/**
 * @brief Runs the thread
 *
 **/
void SerialLink::run()
{
    // Initialize the connection
    if (!hardwareConnect())
    {
        //Need to error out here.
        emit communicationError(getName(),"Error connecting: " + m_port->errorString());
        disconnect(); // This tidies up and sends the necessary signals
        return;
    }

    // Qt way to make clear what a while(1) loop does
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    qint64 initialmsecs = QDateTime::currentMSecsSinceEpoch();
    quint64 bytes = 0;
    bool triedreset = false;
    bool triedDTR = false;
    qint64 timeout = 5000;
    //While we're connected, find the serial port info we're on
    QString description = "X";
    foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        if (m_port->portName() == info.portName())
        {
            description = info.description();
            break;
        }
    }
    if (description.contains("mega") && description.contains("2560"))
    {
        QLOG_DEBUG() << "Connected to an APM, with description:" << description;
    }
    else
    {
        QLOG_DEBUG() << "Connected to a NON-APM or 3DR Radio with description:" << description;
    }
    forever
    {
        {
            QMutexLocker locker(&this->m_stoppMutex);
            if(m_stopp)
            {
                m_stopp = false;
                break; // exit the thread
            }

            if (m_reqReset)
            {
                m_reqReset = false;
                communicationUpdate(getName(),"Reset requested via DTR signal");
                m_port->setDataTerminalReady(true);
                msleep(250);
                m_port->setDataTerminalReady(false);
            }
        }

        if (m_transmitBuffer.length() > 0) {
            QMutexLocker writeLocker(&m_writeMutex);
            int numWritten = m_port->write(m_transmitBuffer);
            bool txError = m_port->waitForBytesWritten(-1);
            if ((txError) || (numWritten == -1)){
                QLOG_TRACE() << "TX Error!";
            }
            m_transmitBuffer =  m_transmitBuffer.remove(0, numWritten);
        } else {
            //QLOG_TRACE() << "Wait write response timeout %1" << QTime::currentTime().toString();
        }

        bool error = m_port->waitForReadyRead(10);

        if(error) { // Waits for 1/2 second [TODO][BB] lower to SerialLink::poll_interval?
            QByteArray readData = m_port->readAll();
            while (m_port->waitForReadyRead(10))
                readData += m_port->readAll();
            if (readData.length() > 0) {
                emit bytesReceived(this, readData);
                //QLOG_TRACE() << "rx of length " << QString::number(readData.length());

                m_bytesRead += readData.length();
                m_bitsReceivedTotal += readData.length() * 8;
            }
        } else {
            //QLOG_TRACE() << "Wait write response timeout %1" << QTime::currentTime().toString();
        }

        if (bytes != m_bytesRead) // i.e things are good and data is being read.
        {
            bytes = m_bytesRead;
            msecs = QDateTime::currentMSecsSinceEpoch();
        }
        else
        {
            /*
                MLC - The entire timeout code block has been disabled for the time being.
                There needs to be more discussion about when and how to do resets, as it is
                inherently unsafe that we can reset PX4 via software at any time (even in flight!!!)
                Possibly query the user to be sure?
            */


            if (QDateTime::currentMSecsSinceEpoch() - msecs > timeout)
            {
                //It's been 10 seconds since the last data came in. Reset and try again
                msecs = QDateTime::currentMSecsSinceEpoch();
                if (msecs - initialmsecs > 25000)
                {
                    //After initial 25 seconds, timeouts are increased to 30 seconds.
                    //This prevents temporary silences from things like calibration commands
                    //from screwing things up. In all reality, timeouts should be enabled/disabled
                    //for events like that on a case by case basis.
                    //TODO ^^
                    timeout = 30000;
                }
                if (!triedDTR && triedreset)
                {
                    if (description.contains("mega") && description.contains("2560"))
                    {
                        triedDTR = true;
                        communicationUpdate(getName(),"No data to receive on COM port. Attempting to reset via DTR signal");
                        QLOG_TRACE() << "No data!!! Attempting reset via DTR.";
                        m_port->setDataTerminalReady(true);
                        msleep(250);
                        m_port->setDataTerminalReady(false);
                    }
                }
                else if (!triedreset)
                {
                    if (description.contains("mega") && description.contains("2560"))
                    {
                        QLOG_DEBUG() << "No data!!! Attempting reset via reboot command.";
                        communicationUpdate(getName(),"No data to receive on COM port. Assuming possible terminal mode, attempting to reset via \"reboot\" command");
                        m_port->write("reboot\r\n",8);
                        triedreset = true;
                    }
                }
                else
                {
                    communicationUpdate(getName(),"No data to receive on COM port....");
                    QLOG_DEBUG() << "No data!!!";
                }
            }
        }
        MG::SLEEP::msleep(SerialLink::poll_interval);
    } // end of forever
    
    {
        QMutexLocker locker(&this->m_stoppMutex);
        if (m_port) { // [TODO][BB] Not sure we need to close the port here
            QLOG_DEBUG() << "Closing Port #"<< __LINE__ << m_port->portName();

            m_port->close();
            delete m_port;
            m_port = NULL;
        }
    }

    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    QLOG_DEBUG() << "Serial link ended, closing out";

}

void SerialLink::writeBytes(const char* data, qint64 size)
{
    if(m_port && m_port->isOpen()) {
        QLOG_TRACE() << "writeBytes" << m_portName << "attempting to tx " << size << "bytes.";

        QByteArray byteArray(data, size);
        {
            QMutexLocker writeLocker(&m_writeMutex);
            m_transmitBuffer.append(byteArray);
        }

        // Increase write counter
        m_bitsSentTotal += size * 8;

        // Extra debug logging
        QLOG_TRACE() << QByteArray(data,size);
    } else {
        disconnect();
        // Error occured
        emit communicationError(getName(), tr("Could not send data - link %1 is disconnected!").arg(getName()));
    }
}

/**
 * @brief Read a number of bytes from the interface.
 *
 * @param data Pointer to the data byte array to write the bytes to
 * @param maxLength The maximum number of bytes to write
 **/
void SerialLink::readBytes()
{
    m_dataMutex.lock();
    if(m_port && m_port->isOpen()) {
        const qint64 maxLength = 2048;
        char data[maxLength];
        qint64 numBytes = m_port->bytesAvailable();
        QLOG_TRACE() << "numBytes: " << numBytes;

        if(numBytes > 0) {
            /* Read as much data in buffer as possible without overflow */
            if(maxLength < numBytes) numBytes = maxLength;

            m_port->read(data, numBytes);
            QByteArray b(data, numBytes);
            emit bytesReceived(this, b);

            QLOG_TRACE() << "SerialLink::readBytes()" << &std::hex << data;
            //            int i;
            //            for (i=0; i<numBytes; i++){
            //                unsigned int v=data[i];
            //
            //                fprintf(stderr,"%02x ", v);
            //            }
            //            fprintf(stderr,"\n");
            m_bitsReceivedTotal += numBytes * 8;
        }
    }
    m_dataMutex.unlock();
}


/**
 * @brief Get the number of bytes to read.
 *
 * @return The number of bytes to read
 **/
qint64 SerialLink::bytesAvailable()
{
    QLOG_TRACE() << "Serial Link bytes available";
    if (m_port) {
        return m_port->bytesAvailable();
    } else {
        return 0;
    }
}

/**
 * @brief Disconnect the connection.
 *
 * @return True if connection has been disconnected, false if connection couldn't be disconnected.
 **/
bool SerialLink::disconnect()
{
    QLOG_INFO() << "SerialLink::disconnect";
    if (m_port) {
        QLOG_INFO() << m_port->portName();
    }

    if (isRunning())
    {
        QLOG_INFO() << "running so disconnect" << m_port->portName();
        {
            QMutexLocker locker(&m_stoppMutex);
            m_stopp = true;
        }
        this->wait(1000);
        if (this->isRunning())
        {
            this->terminate();
        }
        // [TODO] these signals are also emitted from RUN()
        // are these even required?
        emit disconnected();
        emit connected(false);
        emit disconnected(this);
        return true;
    }
    // [TODO]
    // Should we emit the disconncted signals to keep the states
    // in order. ie. if disconned is called the UI maybe out of sync
    // and a emit disconnect here could rectify this
    QLOG_INFO() << "SerialLink already disconnected";
    return true;
}

/**
 * @brief Connect the connection.
 *
 * @return True if connection has been established, false if connection couldn't be established.
 **/
bool SerialLink::connect()
{   
    if (isRunning())
        disconnect();
    {
        QMutexLocker locker(&this->m_stoppMutex);
        m_stopp = false;
    }

    start(LowPriority);
    return true;
}

/**
 * @brief This function is called indirectly by the connect() call.
 *
 * The connect() function starts the thread and indirectly calls this method.
 *
 * @return True if the connection could be established, false otherwise
 * @see connect() For the right function to establish the connection.
 **/
bool SerialLink::hardwareConnect()
{
    QLOG_INFO() << "SerialLink: hardwareConnect to " << m_portName;
    if(m_port)
    {
        QLOG_INFO() << "SerialLink:" << QString::number((long)this, 16) << "closing port before connecting";
        m_port->close();
        delete m_port;
        m_port = NULL;
    }

    m_port = new QSerialPort(m_portName);

    if (m_port == NULL)
    {
        emit communicationUpdate(getName(),"Error opening port: " + m_port->errorString());
        return false; // couldn't create serial port.
    }

    QObject::connect(m_port,SIGNAL(aboutToClose()),this,SIGNAL(disconnected()));
    m_connectionStartTime = MG::TIME::getGroundTimeNow();

    if (!m_port->open(QIODevice::ReadWrite))
    {
        emit communicationUpdate(getName(),"Error opening port: " + m_port->errorString());
        m_port->close();
        return false; // couldn't open serial port
    }

    emit communicationUpdate(getName(),"Opened port!");

    // Need to configure the port
    QLOG_DEBUG() << "Setting baud rate to:" << m_baud;
    if (!m_port->setBaudRate(m_baud)){
        QLOG_ERROR() << "Failed to set Baud Rate" << m_baud;
        disconnect();
        return false;

    }
    QLOG_DEBUG() << "Setting data bits to:" << m_dataBits;
    if(!m_port->setDataBits(static_cast<QSerialPort::DataBits>(m_dataBits))){
        QLOG_ERROR() << "Failed to set data bits Rate:" << m_dataBits;
        disconnect();
        return false;

    }
    QLOG_DEBUG() << "Setting flow control to:" << m_flowControl;
    if(!m_port->setFlowControl(static_cast<QSerialPort::FlowControl>(m_flowControl))){
        QLOG_ERROR() << "Failed to set flow control:" << m_flowControl;
        disconnect();
        return false;

    }
    QLOG_DEBUG() << "Setting stop bits to:" << m_stopBits;
    if(!m_port->setStopBits(static_cast<QSerialPort::StopBits>(m_stopBits))){
        QLOG_ERROR() << "Failed to set stop bits" << m_stopBits;
        disconnect();
        return false;

    }
    QLOG_DEBUG() << "Setting parity to :" << m_parity;
    if(!m_port->setParity(static_cast<QSerialPort::Parity>(m_parity))){
        QLOG_ERROR() << "Failed to set parity" << m_parity;
        disconnect();
        return false;

    }

    emit connected();
    emit connected(true);
    emit connected(this);

    QLOG_DEBUG() << "CONNECTING LINK: "<< m_portName << "with settings" << m_port->portName()
             << getBaudRate() << getDataBits() << getParityType() << getStopBits();

    writeSettings();

    return true; // successful connection
}

void SerialLink::linkError(QSerialPort::SerialPortError error)
{
    QLOG_ERROR() << error;
}


/**
 * @brief Check if connection is active.
 *
 * @return True if link is connected, false otherwise.
 **/
bool SerialLink::isConnected()
{

    if (m_port) {
        bool isConnected = m_port->isOpen();
        QLOG_TRACE() << "SerialLink #" << __LINE__ << ":"<<  m_port->portName()
                     << " isConnected =" << QString::number(isConnected);
        return isConnected;
    } else {
        QLOG_TRACE() << "SerialLink #" << __LINE__ << ":" <<  m_portName
                     << " isConnected = false";
        return false;
    }
}

int SerialLink::getId()
{
    return m_id;
}

QString SerialLink::getName()
{
    return m_portName;
}

/**
  * This function maps baud rate constants to numerical equivalents.
  * It relies on the mapping given in qportsettings.h from the QSerialPort library.
  */
qint64 SerialLink::getNominalDataRate()
{
    return m_baud;
}

qint64 SerialLink::getTotalUpstream()
{
    m_statisticsMutex.lock();
    return m_bitsSentTotal / ((MG::TIME::getGroundTimeNow() - m_connectionStartTime) / 1000);
    m_statisticsMutex.unlock();
}

qint64 SerialLink::getCurrentUpstream()
{
    return 0; // TODO
}

qint64 SerialLink::getMaxUpstream()
{
    return 0; // TODO
}

qint64 SerialLink::getBitsSent()
{
    return m_bitsSentTotal;
}

qint64 SerialLink::getBitsReceived()
{
    return m_bitsReceivedTotal;
}

qint64 SerialLink::getTotalDownstream()
{
    m_statisticsMutex.lock();
    return m_bitsReceivedTotal / ((MG::TIME::getGroundTimeNow() - m_connectionStartTime) / 1000);
    m_statisticsMutex.unlock();
}

qint64 SerialLink::getCurrentDownstream()
{
    return 0; // TODO
}

qint64 SerialLink::getMaxDownstream()
{
    return 0; // TODO
}

bool SerialLink::isFullDuplex()
{
    /* Serial connections are always half duplex */
    return false;
}

int SerialLink::getLinkQuality()
{
    /* This feature is not supported with this interface */
    return -1;
}

QString SerialLink::getPortName()
{
    return m_portName;
}

// We should replace the accessors below with one to get the QSerialPort

int SerialLink::getBaudRate()
{
    return getNominalDataRate();
}

int SerialLink::getBaudRateType()
{
    return m_baud;
}

int SerialLink::getFlowType()
{
    return m_flowControl;
}

int SerialLink::getParityType()
{
    return m_parity;
}

int SerialLink::getDataBitsType()
{
    return m_dataBits;
}

int SerialLink::getStopBitsType()
{
    return m_stopBits;
}

int SerialLink::getDataBits()
{
    return m_dataBits;
}

int SerialLink::getStopBits()
{
    return m_stopBits;
}

bool SerialLink::setPortName(QString portName)
{
    QLOG_INFO() << "current portName " << m_portName;
    QLOG_INFO() << "setPortName to " << portName;

    if(portName.length() == 0)
        return false; // [TODO] temporary fox to stop wiping out retreived serial portname.

    if (portName != m_portName) {
        m_portName = portName;
        emit nameChanged(m_portName); // [TODO] maybe we can eliminate this

        if (m_portBaudMap.contains(m_portName))
        {
            setBaudRate(m_portBaudMap[m_portName]);
        }
        emit updateLink(this);
        return true;
    }
    return false;
}


bool SerialLink::setBaudRateType(int rateIndex)
{
    // These minimum and maximum baud rates were based on those enumerated in qserialport.h
    const int minBaud = (int)QSerialPort::Baud1200;
    const int maxBaud = (int)QSerialPort::Baud115200;

    if (rateIndex >= minBaud && rateIndex <= maxBaud)
    {
        m_baud = rateIndex;
        emit updateLink(this);
        return true;
    }

    return false;
}

bool SerialLink::setBaudRateString(const QString& rate)
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

bool SerialLink::setBaudRate(int rate)
{
    if (rate != m_baud) {
        m_baud = rate;
        m_portBaudMap[m_portName] = rate; //Update baud rate for that port in the map.
        emit updateLink(this);
    }
    return true;
}

bool SerialLink::setFlowType(int flow)
{
    if (flow != m_flowControl) {
        m_flowControl = static_cast<QSerialPort::FlowControl>(flow);
        emit updateLink(this);
    }
    return true;
}

bool SerialLink::setParityType(int parity)
{
    if (parity != m_parity) {
        m_parity = static_cast<QSerialPort::Parity>(parity);
        emit updateLink(this);
    }
    return true;
}


bool SerialLink::setDataBits(int dataBits)
{
    if (dataBits != m_dataBits) {
        m_dataBits = static_cast<QSerialPort::DataBits>(dataBits);
        emit updateLink(this);
    }
    return true;
}

bool SerialLink::setStopBits(int stopBits)
{
    if (stopBits != m_stopBits) {
        m_stopBits = static_cast<QSerialPort::StopBits>(stopBits);
        emit updateLink(this);
    }
    return true;
}

bool SerialLink::setDataBitsType(int dataBits)
{
    if (dataBits != m_dataBits) {
        m_dataBits = static_cast<QSerialPort::DataBits>(dataBits);
        emit updateLink(this);
    }
    return true;
}

bool SerialLink::setStopBitsType(int stopBits)
{
    if (stopBits != m_stopBits) {
        m_stopBits = static_cast<QSerialPort::StopBits>(stopBits);
        emit updateLink(this);
    }
    return true;
}

const QList<SerialLink*> SerialLink::getSerialLinks(LinkManager *linkManager)
{
    if(!linkManager)
        return QList<SerialLink*>();

    QList<LinkInterface*> list = linkManager->instance()->getLinks();
    QList<SerialLink*> serialLinklist;
    foreach( LinkInterface* link, list)  {
        SerialLink* serialLink = dynamic_cast<SerialLink*>(link);
        if (serialLink) {
                serialLinklist.append(serialLink);
            }
        };

    return serialLinklist;
}

const QList<SerialLink*> SerialLink::getSerialLinks(UASInterface *uas)
{
    if(!uas)
        return QList<SerialLink*>();

    QList<LinkInterface*>* list = uas->getLinks();
    QList<SerialLink*> serialLinklist;
    foreach( LinkInterface* link, *list)  {
        SerialLink* serialLink = dynamic_cast<SerialLink*>(link);
        if (serialLink) {
                serialLinklist.append(serialLink);
            }
        };

    return serialLinklist;
}
