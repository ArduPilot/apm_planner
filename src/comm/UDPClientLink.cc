/*=====================================================================

APM Planner 2.0 Open Source Ground Control Station

(c) 2015 APMPLANNER2 PROJECT <http://www.diydrones.com>

This file is part of the APMPLANNER2 project

    APMPLANNER2 is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APMPLANNER2 is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APMPLANNER2. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of UDP Client connection for unmanned vehicles
 *
 */

#include "QsLog.h"
#include "UDPClientLink.h"
#include "LinkManager.h"
#include "QGC.h"

#include <QTimer>
#include <QList>
#include <QMutexLocker>
#include <iostream>
#include <QHostInfo>

UDPClientLink::UDPClientLink(QHostAddress host, quint16 port) :
    _targetHost(host),
    _port(port),
    _packetsReceived(false)
{
    // Set unique ID and add link to the list of links
    _linkId = getNextLinkId();
    setName(tr("UDP Client (%1:%2)").arg(_targetHost.toString()).arg(_port));
    QLOG_INFO() << "UDP Created " << _name;

    QObject::connect(&_socket, SIGNAL(readyRead()), this, SLOT(readBytes()));

    QObject::connect(&_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(_socketError(QAbstractSocket::SocketError)));
    QObject::connect(&_socket, SIGNAL(disconnected()), this, SLOT(_socketDisconnected()));
}

UDPClientLink::~UDPClientLink()
{
    disconnect();
	this->deleteLater();
}

/**
 * @brief Runs the thread
 *
 **/
void UDPClientLink::run()
{
	exec();
}

void UDPClientLink::setAddress(QHostAddress host)
{
    bool reconnect = false;
    if(isConnected())
	{
		disconnect();
		reconnect = true;
	}
    _targetHost = host;
    setName(tr("UDP Client (%1:%2)").arg(_targetHost.toString()).arg(_port));
    emit linkChanged(this);

    if(reconnect)
	{
		connect();
	}
}

void UDPClientLink::setPort(int port)
{
    bool reconnect = false;
    if(isConnected())
	{
		disconnect();
		reconnect = true;
	}
    _port = port;
    setName(tr("UDP Client (%1:%2)").arg(_targetHost.toString()).arg(_port));
    emit linkChanged(this);

    if(reconnect)
	{
		connect();
	}
}

void UDPClientLink::writeBytes(const char* data, qint64 size)
{
    if (!_socket.isOpen())
        return;

//    _socket.writeDatagram(data,size,_targetHost,_port);
    _socket.write(data, size);

    // Log the amount and time written out for future data rate calculations.
    QMutexLocker dataRateLocker(&dataRateMutex);
    logDataRateToBuffer(outDataWriteAmounts, outDataWriteTimes, &outDataIndex, size, QDateTime::currentMSecsSinceEpoch());
}

/**
 * @brief Read a number of bytes from the interface.
 *
 * @param data Pointer to the data byte array to write the bytes to
 * @param maxLength The maximum number of bytes to write
 **/
void UDPClientLink::readBytes()
{
    while (_socket.bytesAvailable())
    {
        _packetsReceived = true;
        QByteArray datagram;
        datagram.resize(_socket.bytesAvailable());

        _socket.read(datagram.data(), datagram.size());

        emit bytesReceived(this, datagram);

        // Log this data reception for this timestep
        QMutexLocker dataRateLocker(&dataRateMutex);
        logDataRateToBuffer(inDataWriteAmounts, inDataWriteTimes, &inDataIndex, datagram.length(), QDateTime::currentMSecsSinceEpoch());
    }
}


/**
 * @brief Get the number of bytes to read.
 *
 * @return The number of bytes to read
 **/
qint64 UDPClientLink::bytesAvailable()
{
    return _socket.bytesAvailable();
}

/**
 * @brief Disconnect the connection.
 *
 * @return True if connection has been disconnected, false if connection couldn't be disconnected.
 **/
bool UDPClientLink::disconnect()
{
    QLOG_INFO() << "UDP disconnect";
    quit();
    wait();

    _socket.close();
    _packetsReceived = true;

    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    emit linkChanged(this);
    return false;
}

/**
 * @brief Connect the connection.
 *
 * @return True if connection has been established, false if connection couldn't be established.
 **/
bool UDPClientLink::connect()
{
    if (_socket.isOpen())
            disconnect();
    QLOG_INFO() << "UDPClientLink::UDP connect " << _targetHost.toString() << ":" << _port;
    bool connected = _hardwareConnect();
    if (connected){
        start(NormalPriority);
    }
    return connected;
}

bool UDPClientLink::_hardwareConnect()
{

    if (!_targetHost.isNull() && _port!=0) {
        QLOG_INFO() << "Connected UDP Client socket:" << _targetHost.toString();
        _socket.connectToHost(_targetHost,_port);
        _socket.write(QByteArray("HELLO")); // Force Trigger connection.
        emit connected(true);
        emit connected(this);
        emit connected();
        QTimer::singleShot(5000, this, SLOT(_sendTriggerMessage()));
        return true;
    } else {
        QLOG_ERROR() << "connect failed! " << _targetHost.toString() << ":" << _port
                     << " err:" << _socket.error() << ": " << _socket.errorString();
        emit connected(false);
        emit disconnected(this);
        emit disconnected();
    }
    return false;
}

void UDPClientLink::_sendTriggerMessage()
{
    if (!_packetsReceived){
        QLOG_DEBUG() << "Send UDP Client HELLO" << _targetHost.toString();
        _socket.write("HELLO");
        QTimer::singleShot(5000, this, SLOT(_sendTriggerMessage()));
    }
}

void UDPClientLink::_socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit communicationError(getName(), "UDP Client error on socket: " + _socket.errorString());
}

void UDPClientLink::_socketDisconnected()
{
    qDebug() << _name << ": disconnected";
    _socket.close();

    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    emit linkChanged(this);
}
/**
 * @brief Check if connection is active.
 *
 * @return True if link is connected, false otherwise.
 **/
bool UDPClientLink::isConnected() const
{
    return _socket.isOpen();
}

int UDPClientLink::getId() const
{
    return _linkId;
}

QHostAddress UDPClientLink::getHostAddress() const
{
    return _targetHost;
}

quint16 UDPClientLink::getPort() const
{
    return _port;
}

QString UDPClientLink::getName() const
{
    return _name;
}

QString UDPClientLink::getShortName() const
{
    return _targetHost.toString();
}

QString UDPClientLink::getDetail() const
{
    return QString::number(_port);
}

void UDPClientLink::setName(QString name)
{
    _name = name;
    emit nameChanged(_name);
    emit linkChanged(this);
}


qint64 UDPClientLink::getConnectionSpeed() const
{
    return 54000000; // 54 Mbit
}

qint64 UDPClientLink::getCurrentInDataRate() const
{
    return 0;
}

qint64 UDPClientLink::getCurrentOutDataRate() const
{
    return 0;
}
