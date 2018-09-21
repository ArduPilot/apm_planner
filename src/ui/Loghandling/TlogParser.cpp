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
    return (m_ID != s_InvalidID) && (m_length > 0) && (m_name.size() > 0) &&
           (m_format.size() > 0) && (m_format.size() == m_labels.size());
}

//*****************************************

TlogParser::TlogParser(LogdataStorage::Ptr storagePtr, IParserCallback *object) :
    QObject(),
    LogParserBase (storagePtr, object),
    m_mavDecoderPtr(new MAVLinkDecoder()),
    m_lastModeVal(255),
    mp_ReceiveData(nullptr)
{
    QLOG_DEBUG() << "TlogParser::TlogParser - CTOR";
    // connect to the callbacks of the mavlink deoder - all data is delivered through them
    connect(m_mavDecoderPtr.data(), &MAVLinkDecoder::valueChanged, this, &TlogParser::newValue);
    connect(m_mavDecoderPtr.data(), &MAVLinkDecoder::textMessageReceived, this, &TlogParser::newTextValue);
}

TlogParser::~TlogParser()
{
    QLOG_DEBUG() << "TlogParser::TlogParser - DTOR";
    disconnect(m_mavDecoderPtr.data(), &MAVLinkDecoder::valueChanged, this, &TlogParser::newValue);
    disconnect(m_mavDecoderPtr.data(), &MAVLinkDecoder::textMessageReceived, this, &TlogParser::newTextValue);
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

        switch (fieldinfo.type)
        {
            case MAVLINK_TYPE_CHAR:
            {
                desc.m_labels.push_back(fieldinfo.name);
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
                 extractDescriptorDataFields(desc, fieldinfo, "B", 1);
            }
            break;
            case MAVLINK_TYPE_INT8_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "b", 1);
            }
            break;
            case MAVLINK_TYPE_UINT16_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "H", 2);
            }
            break;
            case MAVLINK_TYPE_INT16_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "h", 2);
            }
            break;
            case MAVLINK_TYPE_UINT32_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "I", 4);
            }
            break;
            case MAVLINK_TYPE_INT32_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "i", 4);
            }
            break;
            case MAVLINK_TYPE_FLOAT:
            {
                extractDescriptorDataFields(desc, fieldinfo, "f", 4);
            }
            break;
            case MAVLINK_TYPE_DOUBLE:
            {
                extractDescriptorDataFields(desc, fieldinfo, "d", 8);
            }
            break;
            case MAVLINK_TYPE_UINT64_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "Q", 8);
            }
            break;
            case MAVLINK_TYPE_INT64_T:
            {
                extractDescriptorDataFields(desc, fieldinfo, "q", 8);
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

void TlogParser::extractDescriptorDataFields(tlogDescriptor &desc, const mavlink_field_info_t &fieldInfo, const QString &format, int size)
{
    if(fieldInfo.array_length == 0)
    {
        // extract single value
        desc.m_labels.push_back(fieldInfo.name);
        desc.m_format += format;
        desc.m_length += size;
    }
    else
    {
        for (unsigned int i = 0; i < fieldInfo.array_length; ++i)
        {
            // extract array value
            QString name(fieldInfo.name);
            name.append('-');
            name.append(QString::number(i));
            desc.m_labels.push_back(name);
            desc.m_format += format;
            desc.m_length += size;
        }
    }
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
    bool success = true;
    mp_ReceiveData = &NameValuePairList;

    m_mavDecoderPtr->decodeMessage(mavlinkMessage);

    if(NameValuePairList.empty())
    {
        success = false;
    }

    mp_ReceiveData = nullptr;
    return success;
}

void TlogParser::newValue(int uasId, const QString &name, const QString &unit, const QVariant &value, quint64 msec)
{
    Q_UNUSED(uasId);
    Q_UNUSED(msec);
    Q_UNUSED(unit);

    // the name of the expected data looks like this:
    // "M1:RAW_IMU.time_usec" in standart case
    // "M1:BATTERY_STATUS.voltages.0" in case of an array

    if((mp_ReceiveData != nullptr) && value.isValid())
    {
        QStringList list = name.split(".");
        if(list.size() == 2)
        {   // handle standart data
            mp_ReceiveData->append(NameValuePair(list[1], value));
        }
        else if(list.size() == 3)
        {   // handle array data
            QString name(list[1] + "-" + list[2]);
            mp_ReceiveData->append(NameValuePair(name, value));
        }
        else
        {
            QLOG_WARN() << "Missing data type information. Message:" << name <<  ":"
                        << value.toString();
            m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter),
                                              "Missing data type information. Message:" + name +
                                              ":" + value.toString());
        }
    }
}

void TlogParser::newTextValue(int uasId, int componentId, int severity, const QString &text)
{
    Q_UNUSED(uasId);
    Q_UNUSED(componentId);
    Q_UNUSED(severity);

    // The date we are expecting looks like this:
    // M1:STATUSTEXT.text: ArduCopter V3.6-dev (12a53ed6)
    // M1:STATUSTEXT.text: Frame: QUAD

    if(mp_ReceiveData != nullptr && text.contains(':'))
    {
         // we remove the "M1:STATUSTEXT.text:" part
        int index = text.indexOf(':') + 1;
        index = text.indexOf(':', index) + 1;

        if(index != -1)
        {
            mp_ReceiveData->append(NameValuePair("text", text.mid(index)));
        }
    }
}

bool TlogParser::extractModeMessage(const QList<NameValuePair> &NameValuePairList)
{
    // Tlog does not contain MODE messages the mode information ins transmitted in
    // a heartbeat message. So here we extract MODE data from heartbeat

    // Only if mode val has canged
    if (m_lastModeVal != static_cast<quint8>(NameValuePairList[4].second.toInt()))
    {
        QList<NameValuePair> modeValuePairlist;
        tlogDescriptor modeDesc = m_nameToDescriptorMap.value(ModeMessage::TypeName);
        // Extract MODE messages from heartbeat messages
        m_lastModeVal = static_cast<quint8>(NameValuePairList[4].second.toInt());

        modeValuePairlist.append(QPair<QString, QVariant>(modeDesc.m_labels[0], nextValidTimestamp()));
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
        msgValuePairlist.append(QPair<QString, QVariant>(msgDesc.m_labels[0], nextValidTimestamp()));
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

