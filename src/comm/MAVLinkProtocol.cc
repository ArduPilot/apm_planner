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


#include "MAVLinkProtocol.h"
#include "LinkManager.h"

#include <cstring>
#include <QDataStream>

MAVLinkProtocol::MAVLinkProtocol():
    m_isOnline(true),
    m_loggingEnabled(false),
    m_throwAwayGCSPackets(false),
    m_connectionManager(nullptr),
    versionMismatchIgnore(false),
    m_enable_version_check(false)
{
}

MAVLinkProtocol::~MAVLinkProtocol()
{
    stopLogging();
}

void MAVLinkProtocol::sendMessage(mavlink_message_t msg)
{
    Q_UNUSED(msg);
}

void MAVLinkProtocol::receiveBytes(LinkInterface* link, const QByteArray &dataBytes)
{
    static int nonmavlinkCount = 0;
    static int radioVersionMismatchCount = 0;
    static bool decodedFirstPacket = false;
    static bool checkedUserNonMavlink = false;
    static bool warnedUserNonMavlink = false;

    mavlink_message_t message;
    memset(&message, 0, sizeof(mavlink_message_t));
    mavlink_status_t status;

    // Cache the link ID for common use.
    quint8 linkId = static_cast<quint8>(link->getId());

    for(const auto &data : dataBytes)
    {
        unsigned int decodeState = mavlink_parse_char(MAVLINK_COMM_0, static_cast<quint8>(data), &message, &status);

        if (decodeState == 0 && !decodedFirstPacket)
        {
            nonmavlinkCount++;
            if (nonmavlinkCount > 2000 && !warnedUserNonMavlink)
            {
                //2000 bytes with no mavlink message. Are we connected to a mavlink capable device?
                if (!checkedUserNonMavlink)
                {
                    link->requestReset();
                    nonmavlinkCount=0;
                    checkedUserNonMavlink = true;
                }
                else
                {
                    warnedUserNonMavlink = true;
                    emit protocolStatusMessage("MAVLink Baud Rate or Version Mismatch", "Please check if the baud rates of APM Planner and your autopilot are the same.");
                }
            }
        }

        if (decodeState == 1)
        {
            mavlink_status_t* mavlinkStatus = mavlink_get_channel_status(MAVLINK_COMM_0);
            if (!decodedFirstPacket)
            {
                decodedFirstPacket = true;

                if (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1)
                {
                    QLOG_INFO() << "First Mavlink message is version 1.0. Using mavlink 1.0 and ask for mavlink 2.0 capability";
                    mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;

                    // Request AUTOPILOT_VERSION message to check if vehicle is mavlink 2.0 capable
                    mavlink_command_long_t command;
                    mavlink_message_t commandMessage;
                    uint8_t sendbuffer[MAVLINK_MAX_PACKET_LEN];
                    command.command = MAV_CMD_REQUEST_AUTOPILOT_CAPABILITIES;
                    command.param1 = 1.0f;

                    mavlink_msg_command_long_encode(message.sysid, message.compid, &commandMessage, &command);
                    // Write message into buffer, prepending start sign
                    int len = mavlink_msg_to_send_buffer(sendbuffer, &commandMessage);
                    link->writeBytes(reinterpret_cast<const char*>(sendbuffer), len);
                    return;
                }
                else
                {
                    QLOG_INFO() << "First Mavlink message is version 2.0. Using Mavlink 2.0 for communication";
                    mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
                }
            }

            // Check if we are receiving mavlink 2.0 while sending mavlink 1.0
            if (!(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1) && (mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1))
            {
                QLOG_DEBUG() << "Switching outbound to mavlink 2.0 due to incoming mavlink 2.0 packet:" << mavlinkStatus << linkId << mavlinkStatus->flags;
                mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
            }

            if(message.msgid == MAVLINK_MSG_ID_AUTOPILOT_VERSION)
            {
                mavlink_autopilot_version_t version;
                mavlink_msg_autopilot_version_decode(&message, &version);
                if(version.capabilities & MAV_PROTOCOL_CAPABILITY_MAVLINK2)
                {
                    QLOG_INFO() << "Vehicle reports mavlink 2.0 capability. Using Mavlink 2.0 for communication";
                    mavlinkStatus->flags &= ~MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
                }
                else
                {
                    QLOG_INFO() << "Vehicle reports mavlink 1.0 capability. Using Mavlink 1.0 for communication";
                    mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
                }
            }

            else if(message.msgid == MAVLINK_MSG_ID_PING)
            {
                // process ping requests (tgt_system and tgt_comp must be zero)
                mavlink_ping_t ping;
                mavlink_msg_ping_decode(&message, &ping);
                if(!ping.target_system && !ping.target_component && m_isOnline)
                {
                    mavlink_message_t msg;
                    mavlink_msg_ping_pack(s_SystemID, s_ComponentID, &msg, ping.time_usec, ping.seq, message.sysid, message.compid);
                    sendMessage(msg);
                }
            }

            else if(message.msgid == MAVLINK_MSG_ID_RADIO_STATUS)
            {
                // process telemetry status message
                mavlink_radio_status_t rstatus;
                mavlink_msg_radio_status_decode(&message, &rstatus);
                int rssi = rstatus.rssi;
                int remrssi = rstatus.remrssi;
                // 3DR Si1k radio needs rssi fields to be converted to dBm
                if (message.sysid == '3' && message.compid == 'D')
                {
                    /* Per the Si1K datasheet figure 23.25 and SI AN474 code
                     * samples the relationship between the RSSI register
                     * and received power is as follows:
                     *
                     *                       10
                     * inputPower = rssi * ------ 127
                     *                       19
                     *
                     * Additionally limit to the only realistic range [-120,0] dBm
                     */
                    rssi    = qMin(qMax(qRound(static_cast<qreal>(rssi)    / 1.9 - 127.0), - 120), 0);
                    remrssi = qMin(qMax(qRound(static_cast<qreal>(remrssi) / 1.9 - 127.0), - 120), 0);
                }
                else
                {
                    rssi    = static_cast<qint8>(rstatus.rssi);
                    remrssi = static_cast<qint8>(rstatus.remrssi);
                }                
            }

            // Detect if we are talking to an old radio not supporting v2
            else if (message.msgid == MAVLINK_MSG_ID_RADIO_STATUS)
            {
                if ((mavlinkStatus->flags & MAVLINK_STATUS_FLAG_IN_MAVLINK1)
                        && !(mavlinkStatus->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1))
                {

                    radioVersionMismatchCount++;
                }
            }

            if (radioVersionMismatchCount == 5)
            {
                // Warn the user if the radio continues to send v1 while the link uses v2
                emit protocolStatusMessage(tr("MAVLink Protocol"), tr("Detected radio still using MAVLink v1.0 on a link with MAVLink v2.0 enabled. Please upgrade the radio firmware."));
                // Ensure the warning can't get stuck
                radioVersionMismatchCount++;
                // Flick link back to v1
                QLOG_DEBUG() << "Switching outbound to mavlink 1.0 due to incoming mavlink 1.0 packet:" << mavlinkStatus << linkId << mavlinkStatus->flags;
                mavlinkStatus->flags |= MAVLINK_STATUS_FLAG_OUT_MAVLINK1;
            }

            // Log data
            if (m_loggingEnabled && !m_ScopedLogfilePtr.isNull())
            {
                quint64 time = QGC::groundTimeUsecs();
                uint8_t buffer[MAVLINK_MAX_PACKET_LEN];

                QDataStream outStream(m_ScopedLogfilePtr.data());
                outStream.setByteOrder(QDataStream::BigEndian);
                outStream << time; // write time stamp

                // write decoded message into buffer and buffer to disk
                int len = mavlink_msg_to_send_buffer(&buffer[0], &message);
                int bytesWritten = outStream.writeRawData(reinterpret_cast<const char*>(&buffer[0]), len);

                if(bytesWritten != len)
                {
                    emit protocolStatusMessage(tr("MAVLink Logging failed"),
                                               tr("Could not write to file %1, disabling logging.")
                                               .arg(m_ScopedLogfilePtr->fileName()));
                    // Stop logging
                    stopLogging();
                }
            }

            if (m_isOnline)
            {
                 handleMessage(link, message);
            }
        }
    }
}

void MAVLinkProtocol::handleMessage(LinkInterface *link, const mavlink_message_t &message)
{
    // ORDER MATTERS HERE!
    // If the matching UAS object does not yet exist, it has to be created
    // before emitting the packetReceived signal

    Q_ASSERT_X(m_connectionManager != nullptr, "MAVLinkProtocol::receiveBytes", " error:m_connectionManager == nullptr");
    UASInterface* uas = m_connectionManager->getUas(message.sysid);

    // Check and (if necessary) create UAS object
    if ((uas == nullptr) && (message.msgid == MAVLINK_MSG_ID_HEARTBEAT))
    {
        // ORDER MATTERS HERE!
        // The UAS object has first to be created and connected,
        // only then the rest of the application can be made aware
        // of its existence, as it only then can send and receive
        // it's first messages.

        // Check if the UAS has the same id like this system
        if (message.sysid == s_SystemID)
        {
            if (m_throwAwayGCSPackets)
            {
                //If replaying, we have to assume that it's just hearing ground control traffic
                return;
            }
            emit protocolStatusMessage(tr("SYSTEM ID CONFLICT!"), tr("Warning: A second system is using the same system id (%1)").arg(s_SystemID));
        }

        // Create a new UAS based on the heartbeat received
        // Todo dynamically load plugin at run-time for MAV
        // WIKISEARCH:AUTOPILOT_TYPE_INSTANTIATION

        // First create new UAS object
        // Decode heartbeat message
        mavlink_heartbeat_t heartbeat;
        // Reset version field to 0
        heartbeat.mavlink_version = 0;
        mavlink_msg_heartbeat_decode(&message, &heartbeat);

        // Check if the UAS has a different protocol version
        // TODO Check if this is still needed!
        if (m_enable_version_check && (heartbeat.mavlink_version != MAVLINK_VERSION))
        {
            // Bring up dialog to inform user
            if (!versionMismatchIgnore)
            {
                emit protocolStatusMessage(tr("The MAVLink protocol version on the MAV and APM Planner mismatch!"),
                                           tr("It is unsafe to use different MAVLink versions. APM Planner therefore refuses to connect to system %1, which sends MAVLink version %2 (APM Planner uses version %3).").arg(message.sysid).arg(heartbeat.mavlink_version).arg(MAVLINK_VERSION));
                versionMismatchIgnore = true;
            }

            // Ignore this message and continue gracefully
            return;
        }

        // Create a new UAS object
        uas = m_connectionManager->createUAS(this,link,message.sysid,&heartbeat);
    }

    // Only count message if UAS exists for this message
    if (uas != nullptr)
    {

        // Increase receive counter
        quint8 linkId = static_cast<quint8>(link->getId());
        totalReceiveCounter[linkId]++;
        currReceiveCounter[linkId]++;

        // Update last message sequence ID
        quint8 expectedSequence = 0;
        if (lastIndex.contains(message.sysid))
        {
            if (lastIndex.value(message.sysid).contains(message.compid))
            {
                //Sequence is uint8 type -> next value after 255 is 0. We do expect the overrun here!
                expectedSequence = lastIndex[message.sysid][message.compid] + 1;
            }
            else
            {
                lastIndex[message.sysid].insert(message.compid, message.seq);
                expectedSequence = message.seq;
            }
        }
        else
        {
            lastIndex.insert(message.sysid,QMap<int, quint8>());
            lastIndex[message.sysid].insert(message.compid, message.seq);
            expectedSequence = message.seq;
        }

        // Make some noise if a message was skipped
        //QLOG_DEBUG() << "SYSID" << message.sysid << "COMPID" << message.compid << "MSGID" << message.msgid << "EXPECTED INDEX:" << expectedIndex << "SEQ" << message.seq;
        if (message.seq != expectedSequence)
        {
            // Determine how many messages were skipped accounting for 0-wraparound
            int16_t lostMessages = message.seq - expectedSequence;
            if (lostMessages < 0)
            {
                // Usually, this happens in the case of an out-of order packet
                lostMessages = 0;
            }
            else
            {
                // TODO Console generates excessive load at high loss rates, needs better GUI visualization
                //QLOG_DEBUG() << QString("Lost %1 messages for comp %4: expected sequence ID %2 but received %3.").arg(lostMessages).arg(expectedIndex).arg(message.seq).arg(message.compid);
            }
            totalLossCounter[linkId] += lostMessages;
            currLossCounter[linkId] += lostMessages;
        }

        // Update the last sequence ID
        lastIndex[message.sysid][message.compid] = message.seq;

        // Update on every 32th packet
        if (totalReceiveCounter[linkId] % 32 == 0)
        {
            // Calculate new receive loss ratio
            float receiveLoss = (double)currLossCounter[linkId]/(double)(currReceiveCounter[linkId]+currLossCounter[linkId]);
            receiveLoss *= 100.0f;
            currLossCounter[linkId] = 0;
            currReceiveCounter[linkId] = 0;
            emit receiveLossChanged(message.sysid, receiveLoss);
        }

        // The packet is emitted as a whole, as it is only 255 - 261 bytes short
        // kind of inefficient, but no issue for a groundstation pc.
        // It buys as reentrancy for the whole code over all threads
        emit messageReceived(link, message);
    }
}

void MAVLinkProtocol::stopLogging()
{
    if (!m_ScopedLogfilePtr.isNull() && m_ScopedLogfilePtr->isOpen())
    {
        QLOG_DEBUG() << "Stop MAVLink logging" << m_ScopedLogfilePtr->fileName();
        // Close the current open file
        m_ScopedLogfilePtr->close();
        m_ScopedLogfilePtr.reset();
    }
    m_loggingEnabled = false;
}

bool MAVLinkProtocol::startLogging(const QString& filename)
{
    if (!m_ScopedLogfilePtr.isNull() && m_ScopedLogfilePtr->isOpen())
    {
        return true;
    }
    stopLogging();
    QLOG_DEBUG() << "Start MAVLink logging" << filename;

    m_ScopedLogfilePtr.reset(new QFile(filename));
    if (m_ScopedLogfilePtr->open(QIODevice::WriteOnly | QIODevice::Append))
    {
         m_loggingEnabled = true;
    }
    else
    {
        emit protocolStatusMessage(tr("Started MAVLink logging"),
                                   tr("FAILED: MAVLink cannot start logging to.").arg(m_ScopedLogfilePtr->fileName()));
        m_loggingEnabled = false;
        m_ScopedLogfilePtr.reset();
    }
    return m_loggingEnabled; // reflects if logging started or not.
}
