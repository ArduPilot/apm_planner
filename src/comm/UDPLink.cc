/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009 - 2011 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of UDP connection (server) for unmanned vehicles
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */
#include <QtGlobal>

#include <QTimer>
#include <QList>
#include <QMutexLocker>
#include <QtNetwork/qhostaddress.h>
#include <iostream>
#include <QHostInfo>

#include "QsLog.h"
#include "UDPLink.h"
#include "LinkManager.h"
#include "QGC.h"


UDPLink::UDPLink(QHostAddress host, quint16 port) :
    socket(NULL),
    connectState(false),
    _running(false)
{
    this->host = host;
    this->port = port;
    // Set unique ID and add link to the list of links
    this->id = getNextLinkId();
    this->name = tr("UDP Link (port:%1)").arg(this->port);
    emit nameChanged(this->name);
    QLOG_INFO() << "UDP Created " << name;
}

UDPLink::~UDPLink()
{
    // Tell the thread to exit
    _running = false;

    // Wait for it to exit
    wait();
    while(_outQueue.count() > 0) {
        delete _outQueue.dequeue();
    }
    this->deleteLater();
}

/**
 * @brief Runs the thread
 *
 **/
void UDPLink::run()
{
    forever {
        if (!isConnected() && !host.isNull() && port != 0 ) {
            hardwareConnect();
            msleep(50);
            continue;
        }

        if (_shouldRestartConnection) {
            _shouldRestartConnection = false;
            hardwareConnect();
            msleep(50);
            continue;
        }

        if (!socket) {
            msleep(50);
            continue;
        }

        bool loop = socket->hasPendingDatagrams();
        if(loop) {
            readBytes();
        }

        //-- Loop right away if busy
        if((_dequeBytes() || loop) && _running)
            continue;

        if (!_running) {
            socket->deleteLater();
            socket = NULL;
            connectState = false;
            emit disconnected();
            emit connected(false);
            emit disconnected(this);
            QLOG_INFO() << "UDPLink:" << "Terminando a thread:";
            break;
        }

        //-- Settle down (it gets here if there is nothing to read or write)
        msleep(50);
    }
}

void UDPLink::setAddress(QHostAddress host)
{
    this->host = host;
    emit linkChanged(this);
    _shouldRestartConnection = true;
}

void UDPLink::setPort(int port)
{
    this->port = port;
    this->name = tr("UDP Link (port:%1)").arg(this->port);
    emit nameChanged(this->name);
    emit linkChanged(this);
    _shouldRestartConnection = true;
}

/**
 * @param host Hostname in standard formatting, e.g. localhost:14551 or 192.168.1.1:14551
 */
void UDPLink::addHost(const QString& host)
{
    QLOG_INFO() << "UDP:" << "ADDING HOST:" << host;
    if (host.contains(":"))
    {
        QLOG_DEBUG() << "HOST: " << host.split(":").first();
        QHostInfo info = QHostInfo::fromName(host.split(":").first());
        if (info.error() == QHostInfo::NoError)
        {
            // Add host
            QList<QHostAddress> hostAddresses = info.addresses();
            QHostAddress address;
            for (int i = 0; i < hostAddresses.size(); i++)
            {
                // Exclude loopback IPv4 and all IPv6 addresses
                if (!hostAddresses.at(i).toString().contains(":"))
                {
                    address = hostAddresses.at(i);
                }
            }
            hosts.append(address);
            QLOG_DEBUG() << "Address:" << address.toString();
            // Set port according to user input
            ports.append(host.split(":").last().toInt());
        }
    }
    else
    {
        QHostInfo info = QHostInfo::fromName(host);
        if (info.error() == QHostInfo::NoError)
        {
            // Add host
            hosts.append(info.addresses().first());
            // Set port according to default (this port)
            ports.append(port);
        }
    }
    emit linkChanged(this);
        _shouldRestartConnection = true;
}

void UDPLink::removeHost(const QString& hostname)
{
    QString host = hostname;
    if (host.contains(":")) host = host.split(":").first();
    host = host.trimmed();
    QHostInfo info = QHostInfo::fromName(host);
    QHostAddress address;
    QList<QHostAddress> hostAddresses = info.addresses();
    for (int i = 0; i < hostAddresses.size(); i++)
    {
        // Exclude loopback IPv4 and all IPv6 addresses
        if (!hostAddresses.at(i).toString().contains(":"))
        {
            address = hostAddresses.at(i);
        }
    }
    for (int i = 0; i < hosts.count(); ++i)
    {
        if (hosts.at(i) == address)
        {
            hosts.removeAt(i);
            ports.removeAt(i);
        }
    }
    _shouldRestartConnection = true;
}

void UDPLink::writeBytes(const char* data, qint64 size)
{
    if (!socket) {
        return;
    }
    QByteArray* qdata = new QByteArray(data, size);
    QMutexLocker lock(&_mutex);
    _outQueue.enqueue(qdata);
}

bool UDPLink::_dequeBytes()
{
    QMutexLocker lock(&_mutex);
    if(_outQueue.count() > 0) {
        QByteArray* qdata = _outQueue.dequeue();
        lock.unlock();
        _sendBytes(qdata->data(), qdata->size());
        delete qdata;
        lock.relock();
    }
    return (_outQueue.count() > 0);
}

void UDPLink::_sendBytes(const char* data, qint64 size)
{
    // Broadcast to all connected systems
    for (int h = 0; h < hosts.size(); h++)
    {
        QHostAddress currentHost = hosts.at(h);
        quint16 currentPort = ports.at(h);
//#define UDPLINK_DEBUG
#ifdef UDPLINK_DEBUG
        QString bytes;
        QString ascii;
        for (int i=0; i<size; i++)
        {
            unsigned char v = data[i];
            bytes.append(QString().sprintf("%02x ", v));
            if (data[i] > 31 && data[i] < 127)
            {
                ascii.append(data[i]);
            }
            else
            {
                ascii.append(219);
            }
        }
        QLOG_TRACE() << "Sent" << size << "bytes to" << currentHost.toString() << ":" << currentPort << "data:";
        QLOG_TRACE() << bytes;
        QLOG_TRACE() << "ASCII:" << ascii;
#endif
        socket->writeDatagram(data, size, currentHost, currentPort);

        // Log the amount and time written out for future data rate calculations.
        QMutexLocker dataRateLocker(&dataRateMutex);
        logDataRateToBuffer(outDataWriteAmounts, outDataWriteTimes, &outDataIndex, size, QDateTime::currentMSecsSinceEpoch());
    }
}

/**
 * @brief Read a number of bytes from the interface.
 *
 * @param data Pointer to the data byte array to write the bytes to
 * @param maxLength The maximum number of bytes to write
 **/
void UDPLink::readBytes()
{
    while (socket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // FIXME TODO Check if this method is better than retrieving the data by individual processes
        emit bytesReceived(this, datagram);

        // Log this data reception for this timestep
        QMutexLocker dataRateLocker(&dataRateMutex);
        logDataRateToBuffer(inDataWriteAmounts, inDataWriteTimes, &inDataIndex, datagram.length(), QDateTime::currentMSecsSinceEpoch());

#ifdef UDPLINK_DEBUG
        // Echo data for debugging purposes
        std::cerr << __FILE__ << __LINE__ << "Received datagram:" << std::endl;
//        int i;
//        for (i=0; i<s; i++)
//        {
//            unsigned int v=data[i];
//            fprintf(stderr,"%02x ", v);
//        }
//        std::cerr << std::endl;
#endif

        // Add host to broadcast list if not yet present
        if (!hosts.contains(sender))
        {
            hosts.append(sender);
            ports.append(senderPort);
            //        ports->insert(sender, senderPort);
        }
        else
        {
            int index = hosts.indexOf(sender);
            ports.replace(index, senderPort);
        }
        if(!_running)
            break;
    }
}


/**
 * @brief Get the number of bytes to read.
 *
 * @return The number of bytes to read
 **/
qint64 UDPLink::bytesAvailable()
{
    return socket->pendingDatagramSize();
}

/**
 * @brief Disconnect the connection.
 *
 * @return True if connection has been disconnected, false if connection couldn't be disconnected.
 **/
bool UDPLink::disconnect()
{
    QLOG_INFO() << "UDP disconnect";
    _running = false;
    return true;
}

/**
 * @brief Connect the connection.
 *
 * @return True if connection has been established, false if connection couldn't be established.
 **/
bool UDPLink::connect()
{
    QLOG_INFO() << "UDPLink::UDP connect " << host << ":" << port;
    start(NormalPriority);
    return true;
}

bool UDPLink::hardwareConnect(void)
{
    delete socket;
    QHostAddress host = QHostAddress::AnyIPv4;
    socket = new QUdpSocket();
    socket->setProxy(QNetworkProxy::NoProxy);
    connectState = socket->bind(host, port, QAbstractSocket::ReuseAddressHint);
    if (connectState) {
        emit connected();
        emit connected(true);
        emit connected(this);
    } else {
        emit disconnected();
        emit connected(false);
        emit communicationError("UDP Link Error", "Error binding UDP port");
    }
    _running = true;
    return connectState;
}

/**
 * @brief Check if connection is active.
 *
 * @return True if link is connected, false otherwise.
 **/
bool UDPLink::isConnected() const
{
    return connectState;
}

int UDPLink::getId() const
{
    return id;
}

QString UDPLink::getName() const
{
    return name;
}

QString UDPLink::getShortName() const
{
    return QString("UDP Link");
}

QString UDPLink::getDetail() const
{
    return QString::number(port);
}

void UDPLink::setName(QString name)
{
    this->name = name;
    emit nameChanged(this->name);
    emit linkChanged(this);
}


qint64 UDPLink::getConnectionSpeed() const
{
    return 54000000; // 54 Mbit
}

qint64 UDPLink::getCurrentInDataRate() const
{
    return 0;
}

qint64 UDPLink::getCurrentOutDataRate() const
{
    return 0;
}
