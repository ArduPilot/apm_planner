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
 *   @brief MAVLinkDecoder
 *          This class decodes value fields from incoming mavlink_message_t packets
 *          It emits valueChanged, which is passed up to the UAS class to emit to the UI
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author QGROUNDCONTROL PROJECT - This code has GPLv3+ snippets from QGROUNDCONTROL, (c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 */


#ifndef NEW_MAVLINKDECODER_H
#define NEW_MAVLINKDECODER_H

#include "QsLog.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include "LinkInterface.h"

#include <QObject>
#include <QThread>
#include <QFile>
#include <QMap>
#include <QVector>

class ConnectionManager;

class MAVLinkDecoder : public QObject
{
    Q_OBJECT
public:
    MAVLinkDecoder(QObject *parent=0);
    ~MAVLinkDecoder();

    void passManager(ConnectionManager *manager) { m_connectionManager = manager; }
    mavlink_field_info_t getFieldInfo(QString msgname,QString fieldname);
    QList<QString> getFieldList(QString msgname);
    QString getMessageName(uint8_t msgid);
    quint64 getUnixTimeFromMs(int systemID, quint64 time);

signals:
    void protocolStatusMessage(const QString& title, const QString& message);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
    void textMessageReceived(int uasid, int componentid, int severity, const QString& text);
    void receiveLossChanged(int id,float value);

public slots:
    QList<QPair<QString,QVariant> > receiveMessage(LinkInterface* link, mavlink_message_t message);
    void sendMessage(mavlink_message_t msg);
    QPair<QString,QVariant> emitFieldValue(mavlink_message_t* msg, int fieldid, quint64 time);

private:
    int getSystemId() { return 252; }
    int getComponentId() { return 1; }

private:
    bool m_loggingEnabled;
    QFile *m_logfile;
    ConnectionManager *m_connectionManager;
    bool m_throwAwayGCSPackets;
    bool m_enable_version_check;
    bool versionMismatchIgnore;
    QMap<int,qint64> totalReceiveCounter;
    QMap<int,qint64> currReceiveCounter;
    QMap<int,QMap<int,uint8_t> > lastIndex;
    QMap<int,qint64> totalLossCounter;
    QMap<int,qint64> currLossCounter;
    bool m_multiplexingEnabled;

    QMap<int,int> componentID;
    QMap<int,bool> componentMulti;
    QMap<uint16_t, bool> messageFilter;               ///< Message/field names not to emit
    QMap<uint16_t, bool> textMessageFilter;           ///< Message/field names not to emit in text mode
    mavlink_message_t receivedMessages[256];    ///< Available / known messages
    mavlink_message_info_t messageInfo[256];    ///< Message information
    QMap<int,quint64> onboardTimeOffset;
    QMap<int,quint64> firstOnboardTime;
    QMap<int,quint64> onboardToGCSUnixTimeOffsetAndDelay;
};

#endif // NEW_MAVLINKDECODER_H
