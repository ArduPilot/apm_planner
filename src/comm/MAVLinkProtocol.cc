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

MAVLinkProtocol::MAVLinkProtocol():
    m_isOnline(true),
    m_loggingEnabled(false),
    m_logfile(NULL),
    m_connectionManager(NULL)
{
}

MAVLinkProtocol::~MAVLinkProtocol()
{
    stopLogging();
    m_connectionManager = NULL;
}

void MAVLinkProtocol::sendMessage(mavlink_message_t msg)
{
    Q_UNUSED(msg);
}

void MAVLinkProtocol::receiveBytes(LinkInterface* link, QByteArray b)
{
    mavlink_message_t message;
    mavlink_status_t status;

    // Cache the link ID for common use.
    int linkId = link->getId();

    static int mavlink09Count = 0;
    static int nonmavlinkCount = 0;
    static bool decodedFirstPacket = false;
    static bool warnedUser = false;
    static bool checkedUserNonMavlink = false;
    static bool warnedUserNonMavlink = false;

    // FIXME: Add check for if link->getId() >= MAVLINK_COMM_NUM_BUFFERS
    for (int position = 0; position < b.size(); position++) {
        unsigned int decodeState = mavlink_parse_char(linkId, (uint8_t)(b[position]), &message, &status);

        if ((uint8_t)b[position] == 0x55) mavlink09Count++;
        if ((mavlink09Count > 100) && !decodedFirstPacket && !warnedUser)
        {
            warnedUser = true;
            // Obviously the user tries to use a 0.9 autopilot
            // with QGroundControl built for version 1.0
            emit protocolStatusMessage("MAVLink Version or Baud Rate Mismatch", "Your MAVLink device seems to use the deprecated version 0.9, while APM Planner only supports version 1.0+. Please upgrade the MAVLink version of your autopilot. If your autopilot is using version 1.0, check if the baud rates of APM Planner and your autopilot are the same.");
        }

        if (decodeState == 0 && !decodedFirstPacket)
        {
            nonmavlinkCount++;
            if (nonmavlinkCount > 2000 && !warnedUserNonMavlink)
            {
                //500 bytes with no mavlink message. Are we connected to a mavlink capable device?
                if (!checkedUserNonMavlink)
                {
                    link->requestReset();
                    nonmavlinkCount=0;
                    checkedUserNonMavlink = true;
                }
                else
                {
                    warnedUserNonMavlink = true;
                    emit protocolStatusMessage("MAVLink Baud Rate Mismatch", "Please check if the baud rates of APM Planner and your autopilot are the same.");
                }
            }
        }
        if (decodeState == 1)
        {
            decodedFirstPacket = true;

            if(message.msgid == MAVLINK_MSG_ID_PING)
            {
                // process ping requests (tgt_system and tgt_comp must be zero)
                mavlink_ping_t ping;
                mavlink_msg_ping_decode(&message, &ping);
                if(!ping.target_system && !ping.target_component && m_isOnline)
                {
                    mavlink_message_t msg;
                    mavlink_msg_ping_pack(getSystemId(), getComponentId(), &msg, ping.time_usec, ping.seq, message.sysid, message.compid);
                    sendMessage(msg);
                }
            }

            if(message.msgid == MAVLINK_MSG_ID_RADIO_STATUS)
            {
                // process telemetry status message
                mavlink_radio_status_t rstatus;
                mavlink_msg_radio_status_decode(&message, &rstatus);
                int rssi = rstatus.rssi,
                    remrssi = rstatus.remrssi;
                // 3DR Si1k radio needs rssi fields to be converted to dBm
                if (message.sysid == '3' && message.compid == 'D') {
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

                } else {
                    rssi = (int8_t) rstatus.rssi;
                    remrssi = (int8_t) rstatus.remrssi;
                }                
            }

            // Log data
            if (m_loggingEnabled && m_logfile)
            {
                quint64 time = QGC::groundTimeUsecs();

                QDataStream outStream(m_logfile);
                outStream.setByteOrder(QDataStream::BigEndian);
                outStream << time; // write time stamp
                // write headers, payload (incs CRC)
                int bytesWritten = outStream.writeRawData((const char*)&message.magic,
                                     static_cast<uint>(MAVLINK_NUM_NON_PAYLOAD_BYTES + message.len));

                if(bytesWritten != (MAVLINK_NUM_NON_PAYLOAD_BYTES + message.len))
                {
                    emit protocolStatusMessage(tr("MAVLink Logging failed"),
                                               tr("Could not write to file %1, disabling logging.")
                                               .arg(m_logfile->fileName()));
                    // Stop logging
                    stopLogging();
                }
            }
            quint64 time = QGC::groundTimeUsecs();
            m_mavlinkMsgBuffer[time] = message;
            if (m_isOnline)
            {
                handleMessage(time,link);
            }
        }
    }
}
void MAVLinkProtocol::handleMessage(quint64 timeid,LinkInterface *link)
{
    mavlink_message_t message = m_mavlinkMsgBuffer.value(timeid);
    unsigned int linkId = link->getId();
    // ORDER MATTERS HERE!
    // If the matching UAS object does not yet exist, it has to be created
    // before emitting the packetReceived signal

    //UASInterface* uas = UASManager::instance()->getUASForId(message.sysid);
    Q_ASSERT_X(m_connectionManager != NULL, "MAVLinkProtocol::receiveBytes", " error:m_connectionManager == NULL");
    UASInterface* uas = m_connectionManager->getUas(message.sysid);
    //qDebug() << "MAVLinkProtocol::receiveBytes" << uas;

    // Check and (if necessary) create UAS object
    if (uas == NULL && message.msgid == MAVLINK_MSG_ID_HEARTBEAT)
    {
        // ORDER MATTERS HERE!
        // The UAS object has first to be created and connected,
        // only then the rest of the application can be made aware
        // of its existence, as it only then can send and receive
        // it's first messages.

        // Check if the UAS has the same id like this system
        if (message.sysid == getSystemId())
        {
            if (m_throwAwayGCSPackets)
            {
                //If replaying, we have to assume that it's just hearing ground control traffic
                return;
            }
            emit protocolStatusMessage(tr("SYSTEM ID CONFLICT!"), tr("Warning: A second system is using the same system id (%1)").arg(getSystemId()));
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
        uas = m_connectionManager->createUAS(this,link,message.sysid,&heartbeat); //QGCMAVLinkUASFactory::createUAS(this, link, message.sysid, &heartbeat);
    }

    // Only count message if UAS exists for this message
    if (uas != NULL)
    {

        // Increase receive counter
        totalReceiveCounter[linkId]++;
        currReceiveCounter[linkId]++;

        // Update last message sequence ID
        uint8_t expectedIndex;
        if (lastIndex.contains(message.sysid))
        {
            if (lastIndex.value(message.sysid).contains(message.compid))
            {
                if (lastIndex.value(message.sysid).value(message.compid) == static_cast<uint8_t>(-1))
                {
                    lastIndex[message.sysid][message.compid] = message.seq;
                    expectedIndex = message.seq;
                }
                else
                {
                    expectedIndex = lastIndex[message.sysid][message.compid] + 1;
                }
            }
            else
            {
                lastIndex[message.sysid].insert(message.compid,message.seq);
                expectedIndex = message.seq;
            }
        }
        else
        {
            lastIndex.insert(message.sysid,QMap<int,uint8_t>());
            lastIndex[message.sysid].insert(message.compid,message.seq);
            expectedIndex = message.seq;
        }

        // Make some noise if a message was skipped
        //QLOG_DEBUG() << "SYSID" << message.sysid << "COMPID" << message.compid << "MSGID" << message.msgid << "EXPECTED INDEX:" << expectedIndex << "SEQ" << message.seq;
        if (message.seq != expectedIndex)
        {
            // Determine how many messages were skipped accounting for 0-wraparound
            int16_t lostMessages = message.seq - expectedIndex;
            if (lostMessages < 0)
            {
                // Usually, this happens in the case of an out-of order packet
                lostMessages = 0;
            }
            else
            {
                // Console generates excessive load at high loss rates, needs better GUI visualization
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
            // Calculate new loss ratio
            // Receive loss
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

        // Multiplex message if enabled
        //if (m_multiplexingEnabled)
        //{
            // Get all links connected to this unit
            //QList<LinkInterface*> links = LinkManager::instance()->getLinksForProtocol(this);

            // Emit message on all links that are currently connected
            //foreach (LinkInterface* currLink, links)
            //{
                // Only forward this message to the other links,
                // not the link the message was received on
             //   if (currLink != link) sendMessage(currLink, message, message.sysid, message.compid);
            //}
        //}
    }
}

void MAVLinkProtocol::stopLogging()
{
    if (m_logfile && m_logfile->isOpen()){
        QLOG_DEBUG() << "Stop MAVLink logging" << m_logfile->fileName();
        // Close the current open file
        m_logfile->close();
        delete m_logfile;
        m_logfile = NULL;
    }
    m_loggingEnabled = false;
}

bool MAVLinkProtocol::startLogging(const QString& filename)
{
    if (m_logfile && m_logfile->isOpen())
    {
        return true;
    }
    stopLogging();
    QLOG_DEBUG() << "Start MAVLink logging" << filename;

    Q_ASSERT_X(m_logfile == NULL, "startLogging", "m_logFile == NULL");

    m_logfile = new QFile(filename);
    if (m_logfile->open(QIODevice::WriteOnly | QIODevice::Append)){
         m_loggingEnabled = true;

    } else {
        emit protocolStatusMessage(tr("Started MAVLink logging"),
                                   tr("FAILED: MAVLink cannot start logging to.").arg(m_logfile->fileName()));
        m_loggingEnabled = false;
        delete m_logfile;
        m_logfile = NULL;
    }
    //emit loggingChanged(m_loggingEnabled);
    return m_loggingEnabled; // reflects if logging started or not.
}
