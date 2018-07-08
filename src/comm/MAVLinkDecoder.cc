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


#include "MAVLinkDecoder.h"
#include "QGC.h"
#include <QDataStream>
#include "LinkManager.h"
#include "UASManager.h"
#include "UASInterface.h"

MAVLinkDecoder::MAVLinkDecoder(QObject *parent):
    QObject(parent),
    m_localDecode(false),
    mp_uas(nullptr)
{
    QLOG_DEBUG() << "Create MAVLinkDecoder: " << this;

    // copy message description into hashmap for fast access
    QVector<mavlink_message_info_t> mavlinkMsg = MAVLINK_MESSAGE_INFO;
    for(const auto &typeInfo : mavlinkMsg)
    {
        if(!messageInfo.contains(typeInfo.msgid))
        {
            messageInfo.insert(typeInfo.msgid, typeInfo);
            m_messageNameToID.insert(typeInfo.name, typeInfo.msgid);
        }
        else
        {
            QLOG_WARN() << "Detected 2 Mavlink messages with same ID - decoding will not work properly!";
        }
    }

    // Allow system status
//    messageFilter.insert(MAVLINK_MSG_ID_HEARTBEAT, false);
//    messageFilter.insert(MAVLINK_MSG_ID_SYS_STATUS, false);
//    messageFilter.insert(MAVLINK_MSG_ID_STATUSTEXT, false);
    messageFilter.insert(MAVLINK_MSG_ID_COMMAND_LONG, false);
    messageFilter.insert(MAVLINK_MSG_ID_COMMAND_ACK, false);
    messageFilter.insert(MAVLINK_MSG_ID_PARAM_SET, false);
    messageFilter.insert(MAVLINK_MSG_ID_PARAM_VALUE, false);
    messageFilter.insert(MAVLINK_MSG_ID_MISSION_ITEM, false);
    messageFilter.insert(MAVLINK_MSG_ID_MISSION_COUNT, false);
    messageFilter.insert(MAVLINK_MSG_ID_MISSION_ACK, false);
    messageFilter.insert(MAVLINK_MSG_ID_DATA_STREAM, false);
    messageFilter.insert(MAVLINK_MSG_ID_GPS_STATUS, false);
    #ifdef MAVLINK_MSG_ID_ENCAPSULATED_DATA
    messageFilter.insert(MAVLINK_MSG_ID_ENCAPSULATED_DATA, false);
    #endif
    #ifdef MAVLINK_MSG_ID_DATA_TRANSMISSION_HANDSHAKE
    messageFilter.insert(MAVLINK_MSG_ID_DATA_TRANSMISSION_HANDSHAKE, false);
    #endif
    #ifdef MAVLINK_MSG_ID_EXTENDED_MESSAGE
    messageFilter.insert(MAVLINK_MSG_ID_EXTENDED_MESSAGE, false);
    #endif

    textMessageFilter.insert(MAVLINK_MSG_ID_DEBUG, false);
    textMessageFilter.insert(MAVLINK_MSG_ID_DEBUG_VECT, false);
    textMessageFilter.insert(MAVLINK_MSG_ID_NAMED_VALUE_FLOAT, false);
    textMessageFilter.insert(MAVLINK_MSG_ID_NAMED_VALUE_INT, false);
//    textMessageFilter.insert(MAVLINK_MSG_ID_HIGHRES_IMU, false);

}

MAVLinkDecoder::~MAVLinkDecoder()
{
    QLOG_DEBUG() << "Destroy MAVLinkDecoder: " << this;
}

mavlink_field_info_t MAVLinkDecoder::getFieldInfo(const QString &msgname, const QString &fieldname) const
{
    const auto iter = m_messageNameToID.find(msgname);
    if(iter != m_messageNameToID.end())
    {
        const mavlink_message_info_t &typeInfo = messageInfo[*iter];
        for (quint32 i = 0; i < typeInfo.num_fields; ++i)
        {
            if (fieldname == typeInfo.fields[i].name)
            {
                return typeInfo.fields[i];
            }
        }
    }
    QLOG_INFO() << "No Mavlink field info found for " << msgname << ":" << fieldname;
    return mavlink_field_info_t();
}

QString MAVLinkDecoder::getMessageName(quint32 msgid) const
{
    const auto iter = messageInfo.find(msgid);
    if(iter != messageInfo.end())
    {
        return iter->name;
    }
    QLOG_INFO() << "No Mavlink nessage name found for ID:" << msgid;
    return QString();
}

QList<QString> MAVLinkDecoder::getFieldList(const QString &msgname) const
{
    QList<QString> retval;
    const auto iter = m_messageNameToID.find(msgname);
    if(iter != m_messageNameToID.end())
    {
        for (unsigned int j=0;j<messageInfo[*iter].num_fields;j++)
        {
            retval.append(messageInfo[*iter].fields[j].name);
        }
    }
    return retval;
}

void MAVLinkDecoder::sendMessage(mavlink_message_t msg)
{
    Q_UNUSED(msg);
}

void MAVLinkDecoder::decodeMessage(const mavlink_message_t &message)
{
    m_localDecode = true;
    receiveMessage(nullptr, message);
}

void MAVLinkDecoder::receiveMessage(LinkInterface* link, mavlink_message_t message)
{
    Q_UNUSED(link);
    const auto p_messageInfo = messageInfo.find(message.msgid);
    if(p_messageInfo == messageInfo.end())
    {
        return;
    }

    // Handle time sync message
#ifndef ENABLE_DEBUG_DATALOG_PARSING
    if (message.msgid == MAVLINK_MSG_ID_LOG_DATA)
    {
        return;
    }
#endif
    if (message.msgid == MAVLINK_MSG_ID_SYSTEM_TIME)
    {
        mavlink_system_time_t timebase;
        mavlink_msg_system_time_decode(&message, &timebase);
        onboardTimeOffset[message.sysid] = (timebase.time_unix_usec+500)/1000 - timebase.time_boot_ms;
        onboardToGCSUnixTimeOffsetAndDelay[message.sysid] = static_cast<qint64>(QGC::groundTimeMilliseconds() - (timebase.time_unix_usec+500)/1000);
    }
    else
    {
        // See if first value is a time value
        quint64 time = 0;
        quint8 fieldid = 0;
        quint8 *p_payload = reinterpret_cast<uint8_t*>(&message.payload64[0]);
        if (QString(p_messageInfo->fields[fieldid].name) == QString("time_boot_ms") && p_messageInfo->fields[fieldid].type == MAVLINK_TYPE_UINT32_T)
        {
            time = *(reinterpret_cast<quint32*>(p_payload + p_messageInfo->fields[fieldid].wire_offset));

            QPair<QString,QVariant> fieldval;
            fieldval.first = QString("M%1:%2.%3")
                             .arg(message.sysid)
                             .arg(p_messageInfo->name)
                             .arg(p_messageInfo->fields[fieldid].name);
            fieldval.second = time;
            emit valueChanged(message.sysid, fieldval.first, "uint32_t", fieldval.second, 0);
        }
        else if (QString(p_messageInfo->fields[fieldid].name).contains("usec") && p_messageInfo->fields[fieldid].type == MAVLINK_TYPE_UINT64_T)
        {
            time = *(reinterpret_cast<quint64*>(p_payload + p_messageInfo->fields[fieldid].wire_offset));
            time = (time+500)/1000; // Scale to milliseconds, round up/down correctly

            QPair<QString,QVariant> fieldval;
            fieldval.first = QString("M%1:%2.%3")
                             .arg(message.sysid)
                             .arg(p_messageInfo->name)
                             .arg(p_messageInfo->fields[fieldid].name);
            fieldval.second = *((quint64*)(p_payload + p_messageInfo->fields[fieldid].wire_offset));
            emit valueChanged(message.sysid, fieldval.first, "uint64_t", fieldval.second, 0);
        }
        else
        {
            // First value is not time, send out value 0
            emitFieldValue(&message, fieldid, getUnixTimeFromMs(message.sysid, 0));
        }

        // Align time to global time
        time = getUnixTimeFromMs(message.sysid, time);

        // do we have an active UAS? Check only if not local decoding
        if(!m_localDecode)
        {
            mp_uas = UASManager::instance()->getUASForId(message.sysid);
        }
        else
        {
            mp_uas = nullptr;
        }

        // Store component ID
        if (!m_componentID.contains(message.msgid))
        {
            m_componentID[message.msgid] = message.compid;
        }
        else
        {
            // Got this message already
            if (m_componentID[message.msgid] != message.compid)
            {
                m_componentMulti[message.msgid] = true;
            }
        }

        // Send out field values from 1..n
        for (int i = 1; i < static_cast<int>(p_messageInfo->num_fields); ++i)
        {
            emitFieldValue(&message, i, time);
        }
    }
}


void MAVLinkDecoder::emitFieldValue(mavlink_message_t* msg, int fieldid, quint64 time)
{
    // check if we have data about the message format
    quint32 msgid = msg->msgid;
    const auto p_messageInfo = messageInfo.find(msgid);
    if ((messageFilter.contains(msgid)) || (p_messageInfo == messageInfo.end()))
    {
        return;
    }

    QString fieldName(p_messageInfo->fields[fieldid].name);
    QString fieldType;

    char *p_payload = _MAV_PAYLOAD_NON_CONST(msg);
    QString name('M' + QString::number(msg->sysid) + ':');

    if (m_componentMulti[msg->msgid] == true)
    {
        name.append('C' + QString::number(msg->compid) + ':');
    }

    // Debug vector messages
    if (msgid == MAVLINK_MSG_ID_DEBUG_VECT)
    {
        mavlink_debug_vect_t debug;
        mavlink_msg_debug_vect_decode(msg, &debug);
        char buf[11];
        strncpy(buf, debug.name, 10);
        buf[10] = '\0';
        name.append( QString("%1.%2").arg(buf).arg(fieldName) );
        time = getUnixTimeFromMs(msg->sysid, (debug.time_usec+500)/1000); // Scale to milliseconds, round up/down correctly
    }
    else if (msgid == MAVLINK_MSG_ID_DEBUG)
    {
        mavlink_debug_t debug;
        mavlink_msg_debug_decode(msg, &debug);
        name.append( QString("%1.%2").arg(QString("debug")).arg(debug.ind) );
        time = getUnixTimeFromMs(msg->sysid, debug.time_boot_ms);
    }
    else if (msgid == MAVLINK_MSG_ID_NAMED_VALUE_FLOAT)
    {
        mavlink_named_value_float_t debug;
        mavlink_msg_named_value_float_decode(msg, &debug);
        char buf[11];
        strncpy(buf, debug.name, 10);
        buf[10] = '\0';
        name.append(buf);
        time = getUnixTimeFromMs(msg->sysid, debug.time_boot_ms);
    }
    else if (msgid == MAVLINK_MSG_ID_NAMED_VALUE_INT)
    {
        mavlink_named_value_int_t debug;
        mavlink_msg_named_value_int_decode(msg, &debug);
        char buf[11];
        strncpy(buf, debug.name, 10);
        buf[10] = '\0';
        name.append(buf);
        time = getUnixTimeFromMs(msg->sysid, debug.time_boot_ms);
    }
    else
    {
        name.append(p_messageInfo->name);
        name.append('.' + fieldName);
    }

    switch (p_messageInfo->fields[fieldid].type)
    {
    case MAVLINK_TYPE_CHAR:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            char* str = reinterpret_cast<char*>(p_payload + p_messageInfo->fields[fieldid].wire_offset);
            // Enforce null termination
            str[p_messageInfo->fields[fieldid].array_length-1] = '\0';
            QString string(name + ": " + str);
            if (!textMessageFilter.contains(msgid))
            {
                emit textMessageReceived(msg->sysid, msg->compid, 0, string);
            }
        }
        else
        {
            // Single char
            char b = *(reinterpret_cast<char*>(p_payload + p_messageInfo->fields[fieldid].wire_offset));
            QString unit = QString("char[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, unit, b, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, unit, b, time);
            }
        }
        break;
    case MAVLINK_TYPE_UINT8_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            uint8_t* nums = reinterpret_cast<uint8_t *>(p_payload + p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("uint8_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            uint8_t u = *(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = "uint8_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, u, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, u, time);
            }
        }
        break;
    case MAVLINK_TYPE_INT8_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            int8_t* nums = reinterpret_cast<int8_t*>(p_payload + p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("int8_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            int8_t n = *(reinterpret_cast<int8_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "int8_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, n, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, n, time);
            }
        }
        break;
    case MAVLINK_TYPE_UINT16_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            uint16_t* nums = reinterpret_cast<uint16_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("uint16_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            uint16_t n = *(reinterpret_cast<uint16_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "uint16_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, n, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, n, time);
            }
        }
        break;
    case MAVLINK_TYPE_INT16_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            int16_t* nums = reinterpret_cast<int16_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("int16_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            int16_t n = *(reinterpret_cast<int16_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "int16_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, n, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, n, time);
            }
        }
        break;
    case MAVLINK_TYPE_UINT32_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            uint32_t* nums = reinterpret_cast<uint32_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("uint32_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            uint32_t n = *(reinterpret_cast<uint32_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "uint32_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, n, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, n, time);
            }
        }
        break;
    case MAVLINK_TYPE_INT32_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            int32_t* nums = reinterpret_cast<int32_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("int32_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            int32_t n = *(reinterpret_cast<int32_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "int32_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, n, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, n, time);
            }
        }
        break;
    case MAVLINK_TYPE_FLOAT:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            float* nums = reinterpret_cast<float*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("float[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            float f = *(reinterpret_cast<float*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "float";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, f, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, f, time);
            }
        }
        break;
    case MAVLINK_TYPE_DOUBLE:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            double* nums = reinterpret_cast<double*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("double[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, nums[j], time);
                }
            }
        }
        else
        {
            // Single value
            double f = *(reinterpret_cast<double*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "double";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, f, time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, f, time);
            }
        }
        break;
    case MAVLINK_TYPE_UINT64_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            uint64_t* nums = reinterpret_cast<uint64_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("uint64_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType,  static_cast<quint64>(nums[j]), time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, static_cast<quint64>(nums[j]), time);
                }
            }
        }
        else
        {
            // Single value
            qulonglong n = *(reinterpret_cast<uint64_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "uint64_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, static_cast<quint64>(n), time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, static_cast<quint64>(n), time);
            }
        }
        break;
    case MAVLINK_TYPE_INT64_T:
        if (p_messageInfo->fields[fieldid].array_length > 0)
        {
            int64_t* nums = reinterpret_cast<int64_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset);
            fieldType = QString("int64_t[%1]").arg(p_messageInfo->fields[fieldid].array_length);
            for (unsigned int j = 0; j < p_messageInfo->fields[fieldid].array_length; ++j)
            {
                if (mp_uas == nullptr)
                {
                    emit valueChanged(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, static_cast<quint64>(nums[j]), time);
                }
                else
                {
                    mp_uas->valueChangedRec(msg->sysid, QString("%1.%2").arg(name).arg(j), fieldType, static_cast<quint64>(nums[j]), time);
                }
            }
        }
        else
        {
            // Single value
            int64_t n = *(reinterpret_cast<int64_t*>(p_payload+p_messageInfo->fields[fieldid].wire_offset));
            fieldType = "int64_t";
            if (mp_uas == nullptr)
            {
                emit valueChanged(msg->sysid, name, fieldType, static_cast<quint64>(n), time);
            }
            else
            {
                mp_uas->valueChangedRec(msg->sysid, name, fieldType, static_cast<quint64>(n), time);
            }
        }
        break;
    default:
        QLOG_DEBUG() << "WARNING: UNKNOWN MAVLINK TYPE";
    }
}
quint64 MAVLinkDecoder::getUnixTimeFromMs(int systemID, quint64 time)
{
    quint64 ret = 0;
    if (time == 0)
    {
        ret = QGC::groundTimeMilliseconds() - onboardToGCSUnixTimeOffsetAndDelay.value(systemID,0);
    }
    // Check if time is smaller than 40 years,
    // assuming no system without Unix timestamp
    // runs longer than 40 years continuously without
    // reboot. In worst case this will add/subtract the
    // communication delay between GCS and MAV,
    // it will never alter the timestamp in a safety
    // critical way.
    //
    // Calculation:
    // 40 years
    // 365 days
    // 24 hours
    // 60 minutes
    // 60 seconds
    // 1000 milliseconds
    // 1000 microseconds
#ifndef _MSC_VER
    else if (time < 1261440000000LLU)
#else
    else if (time < 1261440000000)
#endif
    {
        if (!onboardTimeOffset.contains(systemID) || (time < firstOnboardTime.value(systemID,0)-100))
        {
            firstOnboardTime.insert(systemID,time);
            onboardTimeOffset.insert(systemID,QGC::groundTimeMilliseconds() - time);
        }
        if (time > firstOnboardTime.value(systemID,0))
        {
            firstOnboardTime.insert(systemID,time);
        }

        ret = time + onboardTimeOffset.value(systemID,0);
    }
    else
    {
        // Time is not zero and larger than 40 years -> has to be
        // a Unix epoch timestamp. Do nothing.
        ret = time;
    }
    return ret;
}
