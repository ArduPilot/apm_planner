/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2016 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file TlogParser.cpp.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @author Michael Carpenter <malcom2073@gmail.com>
 * @date 22 Okt 2016
 * @brief File providing implementation for the tlog log parser
 */


#include "TlogParser.h"
#include "logging.h"


bool TlogParser::tlogDescriptor::isValid() const
{
    return (m_ID != 0xFF) && (m_length > 0) && (m_name.size() > 0) &&
           (m_format.size() > 0) && (m_format.size() == m_labels.size());
}

//*****************************************

TlogParser::TlogParser(LogdataStorage::Ptr storagePtr, IParserCallback *object) :
    LogParserBase (storagePtr, object),
    m_mavDecoderPtr(new MAVLinkDecoder()),
    m_lastModeVal(255)
{
    QLOG_DEBUG() << "TlogParser::TlogParser - CTOR";
}

TlogParser::~TlogParser()
{
    QLOG_DEBUG() << "TlogParser::TlogParser - DTOR";
}

AP2DataPlotStatus TlogParser::parse(QFile &logfile)
{
    QLOG_DEBUG() << "TlogParser::parse:" << logfile.fileName();

    if(!m_dataStoragePtr || !m_callbackObject)
    {
        QLOG_ERROR() << "TlogParser::parse - No valid datamodel or callback object - parsing stopped";
        return m_logLoadingState;
    }

    // tlogs always have this timestamp
    m_activeTimestamp = timeStampType("time_boot_ms", 1000.0);

    // tlogs do not provide special messages like MODE or MSG. As we can reconstruct the data
    // from other messages we add those descriptors artificially to the DB
    addMissingDescriptors();

    int emptyMessages = 0;

    while(!logfile.atEnd() && !m_stop)
    {
        mavlink_message_t mavlinkMessage;
        mavlink_status_t mavlinkStatus;

        m_callbackObject->onProgress(logfile.pos(),logfile.size());
        m_dataBlock = logfile.read(8192);

        for (int i = 0; i < m_dataBlock.size(); ++i)
        {
            unsigned int decodeState = mavlink_parse_char(14, static_cast<uint8_t>(m_dataBlock[i]), &mavlinkMessage, &mavlinkStatus);
            if (decodeState == MAVLINK_FRAMING_OK)
            {
                tlogDescriptor descriptor;
                descriptor.m_name = m_mavDecoderPtr->getMessageName(mavlinkMessage.msgid);
                if ((descriptor.m_name != "EMPTY") && (mavlinkMessage.sysid != s_GCSType))
                {
                    descriptor.m_ID = mavlinkMessage.msgid;
                    if(!m_nameToDescriptorMap.contains(descriptor.m_name))
                    {
                        if(parseDescriptor(descriptor))
                        {
                            descriptor.finalize(m_activeTimestamp);
                            if(!storeDescriptor(descriptor))
                            {
                                return m_logLoadingState;
                            }
                        }
                    }

                    // Read packet data - if there is something
                    QList<NameValuePair> NameValuePairList;
                    if(decodeData(mavlinkMessage, NameValuePairList))
                    {
                        if(!storeNameValuePairList(NameValuePairList, m_nameToDescriptorMap.value(descriptor.m_name)))
                        {
                            return m_logLoadingState;
                        }

                        // Special message handling - Heartbeat
                        if(mavlinkMessage.msgid == MAVLINK_MSG_ID_HEARTBEAT)
                        {
                            // extract mode message from tlog data
                            if(!extractModeMessage(NameValuePairList))
                            {
                                return m_logLoadingState;
                            }
                            // detect mav type
                            if(m_loadedLogType == MAV_TYPE_GENERIC)
                            {
                                detectMavType(NameValuePairList);
                            }

                        }
                        // Special message handling - Statustext
                        else if(mavlinkMessage.msgid == MAVLINK_MSG_ID_STATUSTEXT)
                        {
                            // Create a MsgMessage from STATUSTEXT
                            if(!extractMsgMessage(NameValuePairList))
                            {
                                return m_logLoadingState;
                            }
                        }
                    }
                }
                else
                {
                   emptyMessages++;
                }
            }
            else if(decodeState == MAVLINK_FRAMING_BAD_CRC)
            {
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), "Bad CRC");
            }
        }
    }

    if(emptyMessages != 0) // Did we have messages named "EMPTY" ?
    {
        m_logLoadingState.corruptDataRead(0, "Found " + QString::number(emptyMessages) +" 'EMPTY' messages wich could not be processed");
    }

    m_dataStoragePtr->setTimeStamp(m_activeTimestamp.m_name, m_activeTimestamp.m_divisor);
    return m_logLoadingState;
}

void TlogParser::addMissingDescriptors()
{
    // Tlog does not contain MODE messages the mode information ins transmitted in
    // a heartbeat message. So we create the datatype for MODE here and put it into data model
    tlogDescriptor descriptor;
    descriptor.m_ID = 0;
    descriptor.m_name = ModeMessage::TypeName;
    descriptor.m_format = QString("QMBZ");
    descriptor.m_length = 8 + 1 + 1 + 64;     // Q:8byte M:1byte B:1byte Z:64byte
    descriptor.m_labels.push_back(m_activeTimestamp.m_name);
    descriptor.m_labels.push_back(QString("Mode"));
    descriptor.m_labels.push_back(QString("ModeNum"));
    descriptor.m_labels.push_back(QString("Info"));
    descriptor.finalize(m_activeTimestamp);

    storeDescriptor(descriptor);

    // Tlog does not contain MSG messages. The information is gathered from STATUSTEXT tlog
    // messages. So we create the datatype for MSG here and put it into data model.
    descriptor = tlogDescriptor();
    descriptor.m_ID = 0;
    descriptor.m_name = MsgMessage::TypeName;
    descriptor.m_format = QString("QZZ");
    descriptor.m_length = 8 + 64 + 64;                   // Q:8byte Z:64byte Z:64byte
    descriptor.m_labels.push_back(m_activeTimestamp.m_name);
    descriptor.m_labels.push_back(QString("Message"));
    descriptor.m_labels.push_back(QString("Info"));
    descriptor.finalize(m_activeTimestamp);

    storeDescriptor(descriptor);
}

bool TlogParser::parseDescriptor(tlogDescriptor &desc)
{
    QList<QString> fieldnames = m_mavDecoderPtr->getFieldList(desc.m_name);
    for (int i = 0; i < fieldnames.size(); ++i)
    {
        mavlink_field_info_t fieldinfo = m_mavDecoderPtr->getFieldInfo(desc.m_name, fieldnames.at(i));
        desc.m_labels.push_back(QString(fieldinfo.name));
        switch (fieldinfo.type)
        {
            case MAVLINK_TYPE_CHAR:
            {
                if (fieldinfo.array_length == 0)
                {
                    desc.m_format += "b";   // it is a single byte
                    desc.m_length += 1;
                }
                else
                {
                    desc.m_format += "Z";   // everything else is a string
                    desc.m_length += 64;
                }
            }
            break;
            case MAVLINK_TYPE_UINT8_T:
            {
                desc.m_format += "B";
                desc.m_length += 1;
            }
            break;
            case MAVLINK_TYPE_INT8_T:
            {
                desc.m_format += "b";
                desc.m_length += 1;
            }
            break;
            case MAVLINK_TYPE_UINT16_T:
            {
                desc.m_format += "H";
                desc.m_length += 2;
            }
            break;
            case MAVLINK_TYPE_INT16_T:
            {
                desc.m_format += "h";
                desc.m_length += 2;
            }
            break;
            case MAVLINK_TYPE_UINT32_T:
            {
                desc.m_format += "I";
                desc.m_length += 4;
            }
                break;
            case MAVLINK_TYPE_INT32_T:
            {
                desc.m_format += "i";
                desc.m_length += 4;
            }
            break;
            case MAVLINK_TYPE_FLOAT:
            {
                desc.m_format += "f";
                desc.m_length += 4;
            }
            break;
            case MAVLINK_TYPE_DOUBLE:
            {
                desc.m_format += "d";
                desc.m_length += 8;
            }
            break;
            case MAVLINK_TYPE_UINT64_T:
            {
                desc.m_format += "Q";
                desc.m_length += 8;
            }
            break;
            case MAVLINK_TYPE_INT64_T:
            {
                desc.m_format += "q";
                desc.m_length += 8;
            }
            break;
            default:
            {
                QLOG_ERROR() << "TlogParser::parseDescriptor:Unknown data type:" << QString::number(fieldinfo.type);
                m_logLoadingState.corruptFMTRead(static_cast<int>(m_MessageCounter),
                                                 desc.m_name + " data: Unknown data type:" + QString::number(fieldinfo.type));
                return false;
            }
        }
    }
    return true;
}

bool TlogParser::storeDescriptor(tlogDescriptor desc)
{
    if(desc.isValid())
    {
        m_nameToDescriptorMap.insert(desc.m_name, desc);
        if(desc.hasNoTimestamp())
        {
            desc.addTimeStampField(m_activeTimestamp);
        }

        m_dataStoragePtr->addDataType(desc.m_name, desc.m_ID, desc.m_length, desc.m_format, desc.m_labels, desc.m_timeStampIndex);
    }
    else
    {
        QLOG_WARN() << "TlogParser::storeDescriptor(): Invalid type descriptor found for type" << desc.m_ID << ":" << desc.m_name;
        m_logLoadingState.corruptFMTRead(static_cast<int>(m_MessageCounter), desc.m_name +
                                         " format data: Corrupt or missing. Message type is:0x" + QString::number(desc.m_ID, 16));
    }
    return true;
}

bool TlogParser::decodeData(const mavlink_message_t &mavlinkMessage, QList<NameValuePair> &NameValuePairList)
{
    QList<NameValuePair> decodedMessage = m_mavDecoderPtr->receiveMessage(0, mavlinkMessage);
    if(decodedMessage.empty())
    {
        return false;   // No data
    }
    // Copy data
    for (int i = 0; i < decodedMessage.size(); ++i)
    {
        QStringList list = decodedMessage.at(i).first.split(".");
        if (list.size() >= 2)   // There must be at least 2 elements
        {
            NameValuePairList.append(NameValuePair(list[1], decodedMessage.at(i).second));
        }
        else
        {
            QLOG_WARN() << "Missing data type information. Message:" << decodedMessage.at(i).first <<  ":"
                        << decodedMessage.at(i).second.toString();
            m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter),
                                              "Missing data type information. Message:" + decodedMessage.at(i).first +
                                              ":" + decodedMessage.at(i).second.toString());
            return false;
        }
    }
    return true;    // everything is ok
}

bool TlogParser::extractModeMessage(const QList<NameValuePair> &NameValuePairList)
{
    // Tlog does not contain MODE messages the mode information ins transmitted in
    // a heartbeat message. So here we extract MODE data from heartbeat

    // Only if mode val has canged
    if (m_lastModeVal != static_cast<quint8>(NameValuePairList[1].second.toInt()))
    {
        QList<NameValuePair> modeValuePairlist;
        tlogDescriptor modeDesc = m_nameToDescriptorMap.value(ModeMessage::TypeName);
        // Extract MODE messages from heartbeat messages
        m_lastModeVal = static_cast<quint8>(NameValuePairList[1].second.toInt());

        modeValuePairlist.append(QPair<QString, QVariant>(modeDesc.m_labels[0], m_highestTimestamp));
        modeValuePairlist.append(QPair<QString, QVariant>(modeDesc.m_labels[1], m_lastModeVal));
        modeValuePairlist.append(QPair<QString, QVariant>(modeDesc.m_labels[2], m_lastModeVal));
        modeValuePairlist.append(QPair<QString, QVariant>(modeDesc.m_labels[3], "Generated from heartbeat"));

        if(!storeNameValuePairList(modeValuePairlist, modeDesc))
        {
            return false;
        }
    }

    return true;
}

bool TlogParser::extractMsgMessage(const QList<NameValuePair> &NameValuePairList)
{
    // Tlog does not contain MSG messages the MSG information ins transmitted in
    // a statustext message. So here we extract MSG data from statustext
    QList<NameValuePair> msgValuePairlist;
    tlogDescriptor msgDesc = m_nameToDescriptorMap.value(MsgMessage::TypeName);

    if((msgDesc.m_labels.size() >= 2) && (NameValuePairList.size() >= 2))
    {
        msgValuePairlist.append(QPair<QString, QVariant>(msgDesc.m_labels[0], m_highestTimestamp));
        msgValuePairlist.append(QPair<QString, QVariant>(msgDesc.m_labels[1], NameValuePairList[2].second));
        msgValuePairlist.append(QPair<QString, QVariant>(msgDesc.m_labels[2], "Generated from statustext"));
        if (!storeNameValuePairList(msgValuePairlist, msgDesc))
        {
            return false;
        }
    }
    else
    {
        m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), "MSG message extraction from statustext failed. Not enough elements.");
    }
    return true;
}

