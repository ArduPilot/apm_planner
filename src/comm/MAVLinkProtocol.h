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
 *   @author QGROUNDCONTROL PROJECT - This code has GPLv3+ snippets from QGROUNDCONTROL, (c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 */


#ifndef NEW_MAVLINKPARSER_H
#define NEW_MAVLINKPARSER_H

#include <QThread>
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include <QByteArray>
#include "LinkInterface.h"
#include <QFile>
#include "QGC.h"
#include <QDataStream>
#include "UASInterface.h"
//#include "MAVLinkDecoder.h"
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
private:
    QMap<quint64,mavlink_message_t> m_mavlinkMsgBuffer;
    void handleMessage(quint64 timeid,LinkInterface *link);
    bool m_isOnline;
    int getSystemId() { return 252; }
    int getComponentId() { return 1; }
    bool m_loggingEnabled;
    QFile *m_logfile;

    bool m_throwAwayGCSPackets;
    LinkManager *m_connectionManager;
    bool versionMismatchIgnore;
    QMap<int,qint64> totalReceiveCounter;
    QMap<int,qint64> currReceiveCounter;
    QMap<int,QMap<int,uint8_t> > lastIndex;
    QMap<int,qint64> totalLossCounter;
    QMap<int,qint64> currLossCounter;
    bool m_enable_version_check;

signals:
    void protocolStatusMessage(const QString& title, const QString& message);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
    void textMessageReceived(int uasid, int componentid, int severity, const QString& text);
    void receiveLossChanged(int id,float value);
    void messageReceived(LinkInterface *link,mavlink_message_t message);

    /**
     * @brief Emitted if a new radio status packet received
     *
     * @param rxerrors receive errors
     * @param fixed count of error corrected packets
     * @param rssi local signal strength in dBm
     * @param remrssi remote signal strength in dBm
     * @param txbuf how full the tx buffer is as a percentage
     * @param noise background noise level
     * @param remnoise remote background noise level
     */
    void radioStatusChanged(LinkInterface* link, unsigned rxerrors, unsigned fixed, int rssi, int remrssi,
    unsigned txbuf, unsigned noise, unsigned remnoise);

public slots:
    void receiveBytes(LinkInterface* link, QByteArray b);
};

#endif // NEW_MAVLINKPARSER_H
