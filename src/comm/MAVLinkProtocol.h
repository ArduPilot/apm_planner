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
 *   @brief MAVLinkProtocol
 *          This class handles incoming mavlink_message_t packets.
 *          It will create a UAS class if one does not exist for a particular heartbeat systemid
 *          It will pass mavlink_message_t on to the UAS class for further parsing
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Arne Wischmann <wischmann-a@gmx.de>
 *   @author QGROUNDCONTROL PROJECT - This code has GPLv3+ snippets from QGROUNDCONTROL, (c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 */


#ifndef NEW_MAVLINKPARSER_H
#define NEW_MAVLINKPARSER_H

#include <mavlink_types.h>
extern mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS]; // defined in src/main.cc
#include <mavlink.h>

#include <QByteArray>
#include "LinkInterface.h"
#include <QFile>
#include "QGC.h"
#include "UASInterface.h"

class LinkManager;
class MAVLinkProtocol : public QObject
{
    Q_OBJECT

public:

    explicit MAVLinkProtocol();
    ~MAVLinkProtocol();

    void setConnectionManager(LinkManager *manager) { m_connectionManager = manager; }
    void sendMessage(mavlink_message_t msg);
    void stopLogging();
    bool startLogging(const QString& filename);
    bool loggingEnabled() { return m_loggingEnabled; }
    void setOnline(bool isonline) { m_isOnline = isonline; }

public slots:
    void receiveBytes(LinkInterface* link, const QByteArray &dataBytes);

private:
    void handleMessage(LinkInterface *link, const mavlink_message_t &message);

    quint8 m_systemID    = QGC::defaultMavlinkSystemId;
    quint8 m_componentID = QGC::defaultComponentId;

    bool m_isOnline = true;
    bool m_loggingEnabled = true;
    QScopedPointer<QFile>m_ScopedLogfilePtr;

    bool m_throwAwayGCSPackets = false;
    LinkManager *m_connectionManager = nullptr;
    bool versionMismatchIgnore = false;
    QMap<int, qint64> totalReceiveCounter;
    QMap<int, qint64> currReceiveCounter;
    QMap<int,QMap<int, quint8> > lastIndex;
    QMap<int, qint64> totalLossCounter;
    QMap<int, qint64> currLossCounter;
    bool m_enable_version_check = false;

signals:
    void protocolStatusMessage(const QString& title, const QString& message);
    void receiveLossChanged(int id,float value);
    void messageReceived(LinkInterface *link,mavlink_message_t message);
};

#endif // NEW_MAVLINKPARSER_H
