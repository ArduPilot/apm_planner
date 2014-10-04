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

#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>
#include <MG.h>
//These three defines specify which threading method is used
//ONLY have one of these active at a time.

//SERIALLINK_THREAD is the origonal method, where the entire creation, use, and destruction of the
//QSerialPort is done inside a run() loop. This is the method which has caused driver and other odd issues.
//#define SERIALLINK_THREAD

//SERIALLINK_NOTHREAD does not use threads at all, but instead uses the current (UI) event loop to handle
//all serial port access, via the readyRead signal.
//#define SERIALLINK_NOTHREAD

//SERIALLINK_HYBRIDTHREAD uses a combonation of the two styles. This uses a run() loop for the creation, use, and
//destruction like the first method, but it calls exec() inside run(), and allows the threads event loop to handle
//all serial port access. This is the most likely candidate for being implemented permanently.
#define SERIALLINK_HYBRIDTHREAD

SerialLink::SerialLink() :
    m_bytesRead(0),
    m_port(NULL),
    m_isConnected(false),
    m_baud(QSerialPort::Baud115200),
    m_dataBits(QSerialPort::Data8),
    m_flowControl(QSerialPort::NoFlowControl),
    m_stopBits(QSerialPort::OneStop),
    m_parity(QSerialPort::NoParity),
    m_portName(""),
    m_stopp(false),
    m_reqReset(false),
    m_timeoutTimer(NULL),
    m_timeoutsEnabled(true)
{
    QLOG_INFO() << "create SerialLink: Load Previous Settings ";
    qRegisterMetaType<QSerialPort::SerialPortError>("QSerialPort::SerialPortError");
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
    this->wait(1000);
    if (this->isRunning())
    {
        this->terminate();
    }
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

bool SerialLink::waitForPort(QString name,int timeoutmilliseconds,bool toexist)
{
    int timeout = 0;
    while (timeout < timeoutmilliseconds)
    {
        bool here = false;
        foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
        {
            if (name == info.portName())
            {
                if (toexist)
                {
                    return true;
                }
                else
                {
                    here = true;
                }
            }
        }
        if (!toexist && !here)
        {
            return true;
        }
        msleep(100);
        timeout += 100;
    }
    return false;
}

QString SerialLink::findTypeFromPort(QString portname)
{
    foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        if (m_portName == info.portName())
        {
            if (info.description().toLower().contains("mega") && info.description().contains("2560"))
            {
                return "apm";
            }
            else if (info.description().toLower().contains("px4"))
            {
                return "px4";
            }
            else
            {
                QLOG_DEBUG() << "Unknown type found on port" << portname << "description:" << info.description();
                return "other";
            }
            break;
        }
    }
    return "unknown";
}

/**
 * @brief Runs the thread
 *
 **/
void SerialLink::run()
{
    // Initialize the connection
    //
    m_connectedType = findTypeFromPort(m_portName);
    if (!hardwareConnect(m_connectedType))
    {
        return;
    }
    m_isConnected = true;
    emit connected();
    emit connected(true);
    emit connected(this);
    if (m_useEventLoop)
    {
        m_timeoutTimer = new QTimer();
        QObject::connect(m_timeoutTimer,SIGNAL(timeout()),this,SLOT(timeoutTimerTimeout()),Qt::DirectConnection);
        //m_timeoutTimer->moveToThread(this); //This allows the timeoutTimerTimeout() to create/destroy m_port as needed.
        m_triedDtrReset = false;
        m_triedRebootReset = false;
        m_timeoutCounter = 0;
        m_timeoutExtendCounter = 0;
        m_timeoutTimer->start(500);

        QObject::connect(m_port,SIGNAL(readyRead()),this,SLOT(portReadyRead()),Qt::DirectConnection);
        exec();
        QLOG_DEBUG() << "Closing out of syeaherial link thread";
        if (m_port)
        {
            if (m_port->isOpen())
            {
                m_port->close();
            }
            delete m_port;
            m_port = NULL;
        }
        return;
    }



    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    qint64 initialmsecs = QDateTime::currentMSecsSinceEpoch();
    quint64 bytes = 0;
    bool triedreset = false;
    bool triedDTR = false;
    qint64 timeout = 5000;

    // Qt way to make clear what a while(1) loop does
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
            }
        } else {
            //QLOG_TRACE() << "Wait write response timeout %1" << QTime::currentTime().toString();
        }
#ifndef Q_OS_WIN
        if (m_port->error() != QSerialPort::NoError && m_port->error() != QSerialPort::UnknownError)
        {
            //Serial port has gone bad???
            qDebug() << "Serial port error:" << m_port->errorString();
            QLOG_DEBUG() << "Serial port has bad things happening!!!" << m_port->errorString();
            break;
        }
#endif

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
                    if (m_connectedType.contains("apm"))
                    {
                        triedDTR = true;
                        communicationUpdate(getName(),"No data to receive on COM port. Attempting to reset via DTR signal");
                        QLOG_TRACE() << "No data!!! Attempting reset via DTR.";
                        m_port->setDataTerminalReady(true);
                        msleep(250);
                        m_port->setDataTerminalReady(false);
                    }
                    triedDTR = true;
                }
                else if (!triedreset)
                {
                    if (m_connectedType.contains("apm"))
                    {
                        QLOG_DEBUG() << "No data!!! Attempting reset via reboot command.";
                        communicationUpdate(getName(),"No data to receive on COM port. Assuming possible terminal mode, attempting to reset via \"reboot\" command");
                        m_port->write("reboot\r\n",8);
                        triedreset = true;
                    }
                    else if (m_connectedType.contains("px4"))
                    {
                        QLOG_DEBUG() << "No Data!!! Attempting reboot via reboot command";
                        communicationUpdate(getName(),"No data to receive on COM port. Assuming possible terminal mode, attempting to reset via \"reboot\" command");
                        m_port->write("reboot\r\n",8);
                        m_port->waitForBytesWritten(1);
                        m_port->close();
                        delete m_port;
                        waitForPort(m_portName,10000,false);
                        QLOG_DEBUG() << "Waiting for device" << m_portName;
                        if (!waitForPort(m_portName,10000,true))
                        {
                            //Timeout waiting for device
                            QLOG_DEBUG() << "Timout Waiting for device";

                        }
                        QLOG_DEBUG() << "Attempting connection to " << m_portName;
                        if (!hardwareConnect(m_connectedType))
                        {
                            QLOG_DEBUG() << "Failure to connect on reboot";
                            //Bad
                        }
                        QLOG_DEBUG() << "Succesfully reconected";
                        msleep(500);

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
    m_isConnected = false;
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
        //m_port->write(byteArray);
#ifdef SERIALLINK_THREAD
        {
            QMutexLocker writeLocker(&m_writeMutex);
            m_transmitBuffer.append(byteArray);
        }
#endif
#ifdef SERIALLINK_NOTHREAD
        m_port->write(byteArray);

#endif
#ifdef SERIALLINK_HYBRIDTHREAD
        m_port->write(byteArray);
#endif
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

            QLOG_TRACE() << "SerialLink::readBytes()" << &hex << data;
            //            int i;
            //            for (i=0; i<numBytes; i++){
            //                unsigned int v=data[i];
            //
            //                fprintf(stderr,"%02x ", v);
            //            }
            //            fprintf(stderr,"\n");
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
#ifdef SERIALLINK_THREAD
    return disconnectPureThreaded();
#endif
#ifdef SERIALLINK_NOTHREAD
    return disconnectNoThreaded();
#endif
#ifdef SERIALLINK_HYBRIDTHREAD
    return disconnectPartialThreaded();
#endif
}

bool SerialLink::connectPureThreaded()
{
    m_useEventLoop = false;
    start(LowPriority);
    return true;
}
bool SerialLink::connectPartialThreaded()
{
    m_useEventLoop = true;
    m_stopp = false;
    start(LowPriority);
    return true;
}
bool SerialLink::connectNoThreaded()
{
    m_connectedType = findTypeFromPort(m_portName);
    if (!hardwareConnect(m_connectedType))
    {
        return false;
    }
    QObject::connect(m_port,SIGNAL(readyRead()),this,SLOT(portReadyRead()));
    QObject::connect(m_port,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(linkError(QSerialPort::SerialPortError)));
    m_isConnected = true;
    emit connected();
    emit connected(true);
    emit connected(this);

    m_triedDtrReset = false;
    m_triedRebootReset = false;
    m_timeoutCounter = 0;
    m_timeoutExtendCounter = 0;
    m_timeoutTimer = new QTimer(this);
    QObject::connect(m_timeoutTimer,SIGNAL(timeout()),this,SLOT(timeoutTimerTimeout()));
    m_timeoutTimer->start(500);
    return true;
}
bool SerialLink::disconnectNoThreaded()
{
    if (m_timeoutTimer)
    {
        m_timeoutTimer->stop();
        delete m_timeoutTimer;
        m_timeoutTimer=NULL;
    }
    QLOG_INFO() << "SerialLink::disconnect";
    if (m_port) {
        QLOG_INFO() << m_port->portName();
        m_port->close();
        delete m_port;
        m_port = NULL;
    }
    m_isConnected = false;
    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    return true;
}

bool SerialLink::disconnectPartialThreaded()
{
    if (isRunning())
    {
        m_stopp = true;
        if (m_port)
        {
            QLOG_INFO() << "running so disconnect" << m_port->portName();
        }
        quit();
        wait(500); //TODO: May not be required, more testing.
    }
    QLOG_INFO() << "SerialLink already disconnected";
    if (m_timeoutTimer)
    {
        m_timeoutTimer->stop();
        delete m_timeoutTimer;
        m_timeoutTimer=NULL;
    }
    m_isConnected = false;
    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    return true;
}

bool SerialLink::disconnectPureThreaded()
{
    if (isRunning())
    {
        if (m_port) QLOG_INFO() << "running so disconnect" << m_port->portName();
        {
            QMutexLocker locker(&m_stoppMutex);
            m_stopp = true;
        }
        return true;
    }
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
    if (isConnected())
    {
        return false; //Already connected
    }
#ifdef SERIALLINK_THREAD
    return connectPureThreaded();
#endif
#ifdef SERIALLINK_NOTHREAD
    return connectNoThreaded();
#endif
#ifdef SERIALLINK_HYBRIDTHREAD
    return connectPartialThreaded();
#endif

}
void SerialLink::timeoutTimerTimeout()
{
    if (m_stopp)
    {
        return;
    }
    m_timeoutCounter++;
    m_timeoutExtendCounter++;
    if (m_timeoutExtendCounter == 40) //20 seconds initially
    {
        m_timeoutTimer->stop();
        m_timeoutTimer->start(1500); //Increase to every 1.5 seconds, increasing the total timeout to 30 seconds
    }
    if (m_timeoutCounter > 10) //5 seconds
    {
        if (!m_timeoutsEnabled)
        {
            return;
        }
        m_timeoutCounter = 0;
        if (!m_triedDtrReset && !m_triedRebootReset)
        {
            m_triedRebootReset = true;
            if (m_connectedType == "px4")
            {
                QLOG_DEBUG() << "No Data!!! Attempting reboot via reboot command";
                m_timeoutTimer->stop();
                communicationUpdate(getName(),"No data to receive on COM port. Assuming possible terminal mode, attempting to reset via \"reboot\" command");
                m_port->write("reboot\r\n",8);
                m_port->waitForBytesWritten(1);
                m_port->close();
                delete m_port;
                emit disconnected();
                emit connected(false);
                emit disconnected(this);
                waitForPort(m_portName,10000,false);
                if (m_stopp)
                {
                    return;
                }
                QLOG_DEBUG() << "Waiting for device" << m_portName;
                if (!waitForPort(m_portName,10000,true))
                {
                    //Timeout waiting for device
                    QLOG_DEBUG() << "Timout Waiting for device";

                }
                if (m_stopp)
                {
                    return;
                }
                QLOG_DEBUG() << "Attempting connection to " << m_portName;
                if (!hardwareConnect(m_connectedType))
                {
                    QLOG_DEBUG() << "Failure to connect on reboot";
                    //Bad
                }
                if (m_stopp)
                {
                    return;
                }
                QObject::connect(m_port,SIGNAL(readyRead()),this,SLOT(portReadyRead()),Qt::DirectConnection);
                QObject::connect(m_port,SIGNAL(error(QSerialPort::SerialPortError)),this,SLOT(linkError(QSerialPort::SerialPortError)));
                QLOG_DEBUG() << "Succesfully reconected";
                emit connected();
                emit connected(true);
                emit connected(this);
                m_timeoutExtendCounter = 0;

                m_timeoutTimer->start(500);
            }
            else
            {
                QLOG_DEBUG() << "No data!!! Attempting reset via reboot command.";
                communicationUpdate(getName(),"No data to receive on COM port. Assuming possible terminal mode, attempting to reset via \"reboot\" command");
                m_port->write("reboot\r\n",8);
            }
        }
        else if (!m_triedDtrReset)
        {
            m_triedDtrReset = true;
            if (m_connectedType != "px4")
            {
                communicationUpdate(getName(),"No data to receive on COM port. Attempting to reset via DTR signal");
                QLOG_DEBUG() << "No data!!! Attempting reset via DTR.";
                m_port->setDataTerminalReady(true);
                msleep(250);
                m_port->setDataTerminalReady(false);
            }
        }
        else
        {
            QLOG_DEBUG() << "No Data!! Tried reboot and DTR to no avail";
        }
    }
}

void SerialLink::portReadyRead()
{
    m_timeoutCounter = 0;
    QByteArray readData = m_port->readAll();
    if (readData.length() > 0) {
        emit bytesReceived(this, readData);
        m_bytesRead += readData.length();
    }
}

/**
 * @brief This function is called indirectly by the connect() call.
 *
 * The connect() function starts the thread and indirectly calls this method.
 *
 * @return True if the connection could be established, false otherwise
 * @see connect() For the right function to establish the connection.
 **/
bool SerialLink::hardwareConnect(QString type)
{
    QLOG_INFO() << "SerialLink: hardwareConnect to " << m_portName << "with type" << type;
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
        emit communicationError(getName(),"Error opening port: " + m_port->errorString());
        return false; // couldn't create serial port.
    }
    m_port->setSettingsRestoredOnClose(false);

    int tries = 0;
    while (tries++ < 3 && !m_port->isOpen())
    {
        if (!m_port->open(QIODevice::ReadWrite))
        {
            QLOG_DEBUG() << "Failed in attempt to open port, trying again...";
            QLOG_DEBUG() << "Error:" << m_port->errorString();
        }
    }
    if (!m_port->isOpen())
    {
        QLOG_DEBUG() << "Unable to open port:" << m_port->errorString();
        emit communicationUpdate(getName(),"Error opening port: " + m_port->errorString());
        emit communicationError(getName(),"Error opening port: " + m_port->errorString());
        delete m_port;
        m_port = NULL;
        return false; // couldn't open serial port
    }
    QLOG_DEBUG() << "Port opened!";

    emit communicationUpdate(getName(),"Opened port!");

    // Need to configure the port



    //Don't set baud rate/bits on PX4, it ignores the message anyway, and can potentially cause lockups
    if (!type.contains("px4"))
    {
        QLOG_DEBUG() << "Setting baud rate to:" << m_baud;
        if (!m_port->setBaudRate(m_baud)){
            QLOG_ERROR() << "Failed to set Baud Rate" << m_baud;
            emit communicationError(getName(),"Error setting baud rate to: " + QString::number(m_baud) + " :" + m_port->errorString());
            m_port->close();
            delete m_port;
            m_port = NULL;
            return false;
        }
        QLOG_DEBUG() << "Setting data bits to:" << m_dataBits;
        if(!m_port->setDataBits(static_cast<QSerialPort::DataBits>(m_dataBits))){
            QLOG_ERROR() << "Failed to set data bits Rate:" << m_dataBits;
            emit communicationError(getName(),"Error setting data bits to: " + QString::number(m_dataBits) + " :" + m_port->errorString());
            m_port->close();
            delete m_port;
            m_port = NULL;
            return false;
        }
        QLOG_DEBUG() << "Setting flow control to:" << m_flowControl;
        if(!m_port->setFlowControl(static_cast<QSerialPort::FlowControl>(m_flowControl))){
            QLOG_ERROR() << "Failed to set flow control:" << m_flowControl;
            emit communicationError(getName(),"Error setting flow control to: " + QString::number(m_flowControl) + " :" + m_port->errorString());
            m_port->close();
            delete m_port;
            m_port = NULL;
            return false;
        }
        QLOG_DEBUG() << "Setting stop bits to:" << m_stopBits;
        if(!m_port->setStopBits(static_cast<QSerialPort::StopBits>(m_stopBits))){
            QLOG_ERROR() << "Failed to set stop bits" << m_stopBits;
            emit communicationError(getName(),"Error setting stop bits to: " + QString::number(m_stopBits) + " :" + m_port->errorString());
            m_port->close();
            delete m_port;
            m_port = NULL;
            return false;
        }
        QLOG_DEBUG() << "Setting parity to :" << m_parity;
        if(!m_port->setParity(static_cast<QSerialPort::Parity>(m_parity))){
            QLOG_ERROR() << "Failed to set parity" << m_parity;
            emit communicationError(getName(),"Error setting parity to: " + QString::number(m_parity) + " :" + m_port->errorString());
            m_port->close();
            delete m_port;
            m_port = NULL;
            return false;
        }
    }


    QLOG_DEBUG() << "CONNECTED LINK: "<< m_portName << "with settings" << m_port->portName()
             << getBaudRate() << getDataBits() << getParityType() << getStopBits();

    writeSettings();

    return true; // successful connection
}

void SerialLink::linkError(QSerialPort::SerialPortError error)
{
    qDebug() << "Error in serial port!" << error;
    QLOG_ERROR() << error;
}


/**
 * @brief Check if connection is active.
 *
 * @return True if link is connected, false otherwise.
 **/
bool SerialLink::isConnected() const
{
    return m_isConnected;
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

int SerialLink::getId() const
{
    return m_id;
}

QString SerialLink::getName() const
{
    return m_portName;
}

/**
  * This function maps baud rate constants to numerical equivalents.
  * It relies on the mapping given in qportsettings.h from the QSerialPort library.
  */
qint64 SerialLink::getConnectionSpeed() const
{
    qint64 dataRate;
    switch (m_baud)
    {
        case QSerialPort::Baud1200:
            dataRate = 1200;
            break;
        case QSerialPort::Baud2400:
            dataRate = 2400;
            break;
        case QSerialPort::Baud4800:
            dataRate = 4800;
            break;
        case QSerialPort::Baud9600:
            dataRate = 9600;
            break;
        case QSerialPort::Baud19200:
            dataRate = 19200;
            break;
        case QSerialPort::Baud38400:
            dataRate = 38400;
            break;
        case QSerialPort::Baud57600:
            dataRate = 57600;
            break;
        case QSerialPort::Baud115200:
            dataRate = 115200;
            break;
            // Otherwise do nothing.
        case QSerialPort::UnknownBaud:
        default:
            dataRate = -1;
            break;
    }
    return dataRate;
}

QString SerialLink::getPortName() const
{
    return m_portName;
}

// We should replace the accessors below with one to get the QSerialPort

int SerialLink::getBaudRate() const
{
    return getConnectionSpeed();
}

int SerialLink::getBaudRateType() const
{
    return m_baud;
}

int SerialLink::getFlowType() const
{
    return m_flowControl;
}

int SerialLink::getParityType() const
{
    return m_parity;
}

int SerialLink::getDataBitsType() const
{
    return m_dataBits;
}

int SerialLink::getStopBitsType() const
{
    return m_stopBits;
}

int SerialLink::getDataBits() const
{
    return m_dataBits;
}

int SerialLink::getStopBits() const
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

    /*QList<LinkInterface*> list = linkManager->instance()->getLinks();
    QList<SerialLink*> serialLinklist;
    foreach( LinkInterface* link, list)  {
        SerialLink* serialLink = dynamic_cast<SerialLink*>(link);
        if (serialLink) {
                serialLinklist.append(serialLink);
            }
        };

    return serialLinklist;*/
    return QList<SerialLink*>();
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
void SerialLink::disableTimeouts()
{
    m_timeoutsEnabled = false;
}

void SerialLink::enableTimeouts()
{
    m_timeoutsEnabled = true;
}
