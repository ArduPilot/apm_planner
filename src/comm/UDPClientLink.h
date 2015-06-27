/*=====================================================================

APM PLANNER 2.0 Open Source Ground Control Station

(c) 2015 APMPLANNER PROJECT <http://www.diydrones.com>

This file is part of the QGROUNDCONTROL project

    APMPLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APMPLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APMPLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#ifndef UDPCLIENTLINK_H
#define UDPCLIENTLINK_H

#include <QString>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QUdpSocket>
#include <LinkInterface.h>
#include <configuration.h>

class UDPClientLink : public LinkInterface
{
    Q_OBJECT
    //Q_INTERFACES(UDPLinkInterface:LinkInterface)

public:
    UDPClientLink(QHostAddress host, quint16 port);

    ~UDPClientLink();
    void disableTimeouts() { }
    void enableTimeouts() { }
    void requestReset() { }

    bool isConnected() const;
    qint64 bytesAvailable();

    /**
     * @brief The human readable port name
     */
    QString getName() const;
    QString getShortName() const;
    QString getDetail() const;
    QHostAddress getHostAddress() const;
    quint16 getPort() const;

    // Extensive statistics for scientific purposes
    qint64 getConnectionSpeed() const;
    qint64 getCurrentInDataRate() const;
    qint64 getCurrentOutDataRate() const;

    void run();

    int getId() const;

    LinkType getLinkType() { return UDP_CLIENT_LINK; }

public slots:
    void setAddress(QHostAddress host);
    void setPort(int port);

    void readBytes();
    void writeBytes(const char* data, qint64 length);

    bool connect();
    bool disconnect();

protected slots:
    void _socketError(QAbstractSocket::SocketError socketError);
    void _socketDisconnected();

private slots:
    void _sendTriggerMessage();

private: // Helper Methods
    void setName(QString name);
    bool _hardwareConnect();
    void _resetName();

private:
    QString _name;
    QHostAddress _targetHost;
    quint16 _port;
    int _linkId;
    QUdpSocket _socket;
    bool _packetsReceived;

    QMutex _dataMutex;
};

#endif // UDPCLIENTLINK_H
