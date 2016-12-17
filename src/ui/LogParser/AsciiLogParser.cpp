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
 * @file AsciiLogParser.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @author Michael Carpenter <malcom2073@gmail.com>
 * @date 09 Okt 2016
 * @brief File providing implementation for the ascii log parser
 */

#include "AsciiLogParser.h"
#include "logging.h"


bool AsciiLogParser::asciiDescriptor::isValid() const
{
    // Special handling for FMT messages as they are corrupt in some logs. This is not a real
    // problem as the FMT is parsed by a fixed scheme at the moment.
    if(m_name == AsciiLogParser::s_FMTMessageName)
    {
        if(m_format.size() != m_labels.size())
        {
            QLOG_WARN() << "asciiDescriptor::valid() Corrupt FMT descriptor found - known bug in some logs - "
                        << "trying to ignore...";
        }
        return (m_ID != 0xFF) && (m_length > 0) && (m_name.size() > 0) &&
               (m_format.size() > 0) && (m_labels.size() > 0);
    }
    // STRT message has also a special behaviour as it has no data fields in older logs.
    else if(m_name == AsciiLogParser::s_STRTMessageName)
    {
        if(m_format.size() == 0 && m_length == 3)
        {
            QLOG_WARN() << "asciiDescriptor::valid() Corrupt STRT descriptor found - known bug in some logs - "
                        << "trying to ignore...";
        }
        return (m_ID != 0xFF) && (m_length > 0) && (m_name.size() > 0) &&
               (m_format.size() == m_labels.size());
    }
    else
    {
        return (m_ID != 0xFF) && (m_length > 0) && (m_name.size() > 0) &&
               (m_format.size() > 0) && (m_format.size() == m_labels.size());
    }
}

//*****************************************

const QString AsciiLogParser::s_FMTMessageName = "FMT";
const QString AsciiLogParser::s_STRTMessageName = "STRT";

AsciiLogParser::AsciiLogParser(LogdataStorage::Ptr storagePtr, IParserCallback *object) :
    LogParserBase (storagePtr, object),
    m_noMessageBytes(0)
{
    QLOG_DEBUG() << "AsciiLogParser::AsciiLogParser - CTOR";
}

AsciiLogParser::~AsciiLogParser()
{
    QLOG_DEBUG() << "AsciiLogParser::AsciiLogParser - DTOR";
}

AP2DataPlotStatus AsciiLogParser::parse(QFile &logfile)
{
    QLOG_DEBUG() << "AsciiLogParser::parse:" << logfile.fileName();

    if(!m_dataStoragePtr || !m_callbackObject)
    {
        QLOG_ERROR() << "AsciiLogParser::parse - No valid datamodel or callback object - parsing stopped";
        return m_logLoadingState;
    }

    while (!logfile.atEnd() && !m_stop)
    {
        m_callbackObject->onProgress(logfile.pos(),logfile.size());
        QString line = logfile.readLine();
        line = line.remove(QChar('\n'));
        line = line.remove(QChar('\r'));
        m_tokensToParse.clear();
        m_tokensToParse = line.split(QChar(s_TokenSeperator));

        if(m_tokensToParse.size() > 0)
        {
            if(m_tokensToParse.at(s_TypeIndex) == s_FMTMessageName)
            {
                asciiDescriptor descriptor;
                if(parseFMTMessage(descriptor))
                {
                    if(descriptor.m_name == "GPS" && !descriptor.m_labels.contains("GPSTimeMS"))
                    {
                        // Special handling for "GPS" messages that have a "TimeMS"
                        // timestamp but scaling and value does not mach all other time stamps
                        descriptor.replaceLabelName("TimeMS", "GPSTimeMS");
                    }
                    if(m_activeTimestamp.valid())
                    {
                        descriptor.finalize(m_activeTimestamp);
                        if(!extendedStoreDescriptor(descriptor))
                        {
                            return m_logLoadingState;
                        }
                    }
                    else
                    {
                        checkForValidTimestamp(descriptor);
                        m_descriptorForDeferredStorage.push_back(descriptor);
                    }
                }
                else
                {
                    // Parsing of FMT failed - all data of this line is dropped
                    // error message is generated by parseFMTMessage
                    m_noMessageBytes += line.size();
                }
            }
            // Data packet
            else if(m_nameToDescriptorMap.contains(m_tokensToParse.at(0)))
            {
                QList<NameValuePair> NameValuePairList;
                asciiDescriptor descriptor = m_nameToDescriptorMap.value(m_tokensToParse.at(0));
                m_tokensToParse.pop_front();    // remove the already parsed token

                if(parseDataByDescriptor(NameValuePairList, descriptor))
                {
                    if(NameValuePairList.size() >= 1)   // need at least one element
                    {
                        if(!storeNameValuePairList(NameValuePairList, descriptor))
                        {
                            return m_logLoadingState;
                        }
                        if(m_loadedLogType == MAV_TYPE_GENERIC)
                        {
                            detectMavType(NameValuePairList);
                        }
                    }
                    else
                    {
                        QLOG_DEBUG() << "AsciiLogParser::parse():No values within data message " + descriptor.m_name;
                        m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter),
                                                          "No values within data message " + descriptor.m_name);
                        m_noMessageBytes += line.size();
                    }
                }
                else
                {
                    // Parsing of data failed - all data of this line is dropped
                    // error message is generated by parseDataByDescriptor
                    m_noMessageBytes += line.size();
                }
            }
            else
            {
                QLOG_DEBUG() << "AsciiLogParser::parse():Read data without having a valid format descriptor - Message type is " + m_tokensToParse.at(0);
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter),
                                                  "Read data without having a valid format descriptor - "
                                                  "Message type is " + m_tokensToParse.at(0));
                m_noMessageBytes += line.size();
            }
        }
        else
        {
            QLOG_DEBUG() << "AsciiLogParser::parse(): No tokens found in line: " << line;
            m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), "No data for parsing found.");
            m_noMessageBytes += line.size();
        }
    }

    if (m_noMessageBytes > 0)
    {
        QLOG_DEBUG() << "AsciiLogParser::parse(): Non packet bytes found in log file. " << m_noMessageBytes << " bytes filtered out. This may be a corrupt log";
        m_logLoadingState.setNoMessageBytes(m_noMessageBytes);
    }

    m_dataStoragePtr->setTimeStamp(m_activeTimestamp.m_name, m_activeTimestamp.m_divisor);
    return m_logLoadingState;
}

bool AsciiLogParser::parseFMTMessage(asciiDescriptor &desc)
{
    if(m_tokensToParse.size() >= s_MinFmtTokens)
    {
        desc.m_ID = static_cast<quint8>(m_tokensToParse.at(s_IDIndex).trimmed().toUInt());
        desc.m_length = m_tokensToParse[s_LengthIndex].trimmed().toInt();
        desc.m_name   = m_tokensToParse[s_NameIndex].trimmed();
        desc.m_format = m_tokensToParse[s_FormatIndex].trimmed();

        for(int i = s_MinFmtTokens; i < m_tokensToParse.size(); ++i)
        {
            QString tempStr = m_tokensToParse.at(i).trimmed();
            if(tempStr.size() > 0)
            {
                desc.m_labels += tempStr;
            }
        }
        return true;
    }
    else
    {
        QLOG_DEBUG() << "AsciiLogParser::parseFMTMessage: Not enough tokens in FMT line - line dropped";
        m_logLoadingState.corruptFMTRead(static_cast<int>(m_MessageCounter), "Not enough tokens in FMT line");
        return false;
    }
}

bool AsciiLogParser::storeDescriptor(asciiDescriptor desc)
{
    if(desc.isValid())
    {
        if(!m_nameToDescriptorMap.contains(desc.m_name))
        {
            m_nameToDescriptorMap.insert(desc.m_name, desc);

            if(desc.m_name != s_FMTMessageName)   // the descriptor for the FMT message itself shall not be stored in DB
            {
                if(desc.hasNoTimestamp())
                {
                    desc.addTimeStampField(m_activeTimestamp);
                }

                m_dataStoragePtr->addDataType(desc.m_name, desc.m_ID, desc.m_length, desc.m_format, desc.m_labels, desc.m_timeStampIndex);
            }
        }
        else
        {
           QLOG_WARN() << "AsciiLogParser::storeDescriptor(): m_typeToDescriptorMap already contains descriptor with ID "
                       << desc.m_ID << " ignoring the new one";
           m_logLoadingState.corruptFMTRead(static_cast<int>(m_MessageCounter), desc.m_name + " format data: Doubled entry found. Using the first one.");
        }
    }
    else
    {
        QLOG_WARN() << "AsciiLogParser::storeDescriptor(): Invalid type descriptor found for type " << desc.m_ID << ":" << desc.m_name;
        m_logLoadingState.corruptFMTRead(static_cast<int>(m_MessageCounter), desc.m_name + " format data: Corrupt or missing. Message type is:0x" +
                                       QString::number(desc.m_ID, 16));
    }
    return true;
}


bool AsciiLogParser::extendedStoreDescriptor(const asciiDescriptor &desc)
{
    bool rc = true;
    if(m_descriptorForDeferredStorage.size() > 0)
    {
        foreach (const asciiDescriptor &descriptor, m_descriptorForDeferredStorage)
        {
            bool localRc = storeDescriptor(descriptor);
            rc = rc && localRc;
        }
        m_descriptorForDeferredStorage.clear();
    }
    if (rc)
    {
        rc = storeDescriptor(desc);
    }
    return rc;
}

bool AsciiLogParser::parseDataByDescriptor(QList<NameValuePair> &NameValuePairList, const asciiDescriptor &desc)
{
    static QString intdef("bhi");   // unsigned 32 bit max types.
    static QString uintdef("BHI");  // signed 32 bit max types.
    static QString floatdef("cCeEfL");
    static QString chardef("nNZM");

    if(m_tokensToParse.size() != desc.m_format.size())
    {
        QLOG_WARN() << "AsciiLogParser::parseDataByDescriptor(): Not enough tokens for message of type "
                    << desc.m_ID << ":" << desc.m_name << ". Dropping line.";
        m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), "Not enough tokens to parse for message of type " +
                                          desc.m_name);
        return false;
    }

    for(int i = 0; i < desc.m_format.size(); ++i)
    {
        QString token = m_tokensToParse.at(i).trimmed();
        if(intdef.contains(desc.m_format.at(i)))
        {
            bool ok = false;
            qint32 value = token.toInt(&ok);
            if(ok)
            {
                NameValuePairList.append(NameValuePair(desc.getLabelAtIndex(i), value));
            }
            else
            {
                QLOG_DEBUG() << "AsciiLogParser::parseDataByDescriptor(): Failed to convert " << token
                             << " to an signed integer number.";
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), desc.m_name + " data: Failed to convert " +
                                                  token + " to an signed integer number.");
                return false; // we don't have to go further because if one token is missing we cannot store the message in DB
            }
        }
        else if(uintdef.contains(desc.m_format.at(i)))
        {
            bool ok = false;
            quint32 value = token.toUInt(&ok);
            if(ok)
            {
                NameValuePairList.append(NameValuePair(desc.getLabelAtIndex(i), value));
            }
            else
            {
                QLOG_DEBUG() << "AsciiLogParser::parseDataByDescriptor(): Failed to convert " << token
                             << " to an unsigned integer number.";
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), desc.m_name + " data: Failed to convert " +
                                                  token + " to an unsigned integer number.");
                return false; // we don't have to go further because if one token is missing we cannot store the message in DB
            }
        }
        else if(floatdef.contains(desc.m_format.at(i)))
        {
            bool ok = false;
            double value = token.toDouble(&ok);
            if(ok && !qIsInf(value) && !qIsNaN(value))
            {
                NameValuePairList.append(NameValuePair(desc.getLabelAtIndex(i), value));
            }
            else
            {
                QLOG_DEBUG() << "AsciiLogParser::parseDataByDescriptor(): Failed to convert " << token
                             << " to a float number.";
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), desc.m_name + " data: Failed to convert " +
                                                  token + " to a float number.");
                return false; // we don't have to go further because if one token is missing we cannot store the message in DB
            }
        }
        else if(chardef.contains(desc.m_format.at(i)))
        {
            NameValuePairList.append(NameValuePair(desc.getLabelAtIndex(i), token));
        }
        else if(desc.m_format.at(i) == 'Q')
        {
            bool ok = false;
            quint64 value = token.toULongLong(&ok);
            if(ok)
            {
                NameValuePairList.append(NameValuePair(desc.getLabelAtIndex(i), value));
            }
            else
            {
                QLOG_DEBUG() << "AsciiLogParser::parseDataByDescriptor(): Failed to convert " << token
                             << " to an unsigned int64 number.";
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), desc.m_name + " data: Failed to convert " +
                                                  token + " to an unsigned int64 number.");
                return false; // we don't have to go further because if one token is missing we cannot store the message in DB
            }
        }
        else if(desc.m_format.at(i) == 'q')
        {
            bool ok = false;
            qint64 value = token.toLongLong(&ok);
            if(ok)
            {
                NameValuePairList.append(NameValuePair(desc.getLabelAtIndex(i), value));
            }
            else
            {
                QLOG_DEBUG() << "AsciiLogParser::parseDataByDescriptor(): Failed to convert " << token
                             << " to a signed int64 number.";
                m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), desc.m_name + " data: Failed to convert " +
                                                  token + " to a signed int64 number.");
                return false; // we don't have to go further because if one token is missing we cannot store the message in DB
            }
        }
        else
        {
            QLOG_DEBUG() << "AsciiLogParser::parseDataByDescriptor(): Unknown format type - dropping data";
            m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), desc.m_name + " data: Failed to convert " +
                                              token + " cause the format type " + desc.m_format.at(i) + "is unknown");
            return false; // we don't have to go further because if one token is missing we cannot store the message in DB
        }
    }
    return true;
}
