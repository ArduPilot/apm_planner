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
 * @file LogParserBase.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 11 Okt 2016
 * @brief File providing implementation for the log parser base
 */

#include "LogParserBase.h"
#include "logging.h"

LogParserBase::typeDescriptor::typeDescriptor() : m_ID(s_InvalidID), m_length(0), m_hasTimeStamp(false), m_timeStampIndex(0)
{}

void LogParserBase::typeDescriptor::finalize(const timeStampType &timeStamp)
{
    int tempIndex = m_labels.indexOf(timeStamp.m_name);

    if(tempIndex != -1)
    {
        m_hasTimeStamp = true;
        m_timeStampIndex = tempIndex;
    }
}

void LogParserBase::typeDescriptor::addTimeStampField(const LogParserBase::timeStampType &timeStamp)
{
    // Add timestamp name to label list
    m_labels.push_front(timeStamp.m_name);
    // Add timestamp format code to format string
    m_format.prepend('Q');
    // and increase the length by 8 bytes ('Q' is a quint_64)
    m_length += 8;

    m_hasTimeStamp = true;
    m_timeStampIndex = 0;
}

void LogParserBase::typeDescriptor::replaceLabelName(const QString &oldName, const QString &newName)
{
    int tempIndex = m_labels.indexOf(oldName);

    if(tempIndex != -1)
    {
        m_labels[tempIndex] = newName;
    }
}

QString LogParserBase::typeDescriptor::getLabelAtIndex(int index) const
{
    return index < m_labels.size() ? m_labels.at(index) : QString("NoLabel");
}

bool LogParserBase::typeDescriptor::hasNoTimestamp() const
{
    return !m_hasTimeStamp;
}

bool LogParserBase::typeDescriptor::isValid() const
{
    return true;    // Default implementation should be overloaded
}

//*****************************************

LogParserBase::LogParserBase(LogdataStorage::Ptr storagePtr, IParserCallback *object):
    m_callbackObject(object),
    m_dataStoragePtr(storagePtr),
    m_stop(false),
    m_MessageCounter(0),
    m_loadedLogType(MAV_TYPE_GENERIC),
    m_idUnitMessage(typeDescriptor::s_InvalidID),
    m_idMultMessage(typeDescriptor::s_InvalidID),
    m_idFMTUMessage(typeDescriptor::s_InvalidID),
    m_hasUnitData(false),
    m_timeErrorCount(0),
    m_highestTimestamp(0),
    m_timestampOffset(0)
{
    QLOG_DEBUG() << "LogParserBase::LogParserBase - CTOR";
    if(!m_dataStoragePtr)
    {
        QLOG_ERROR() << "LogParserBase::LogParserBase - No valid datamodel - parsing will not be possible";
    }
    if(m_callbackObject == nullptr)
    {
        QLOG_ERROR() << "LogParserBase::LogParserBase - No valid callback object - parsing will not be possible";
    }

    // logs can have different timestamps
    m_possibleTimestamps.push_back(timeStampType("TimeUS", 1000000.0));
    m_possibleTimestamps.push_back(timeStampType("TimeMS", 1000.0));
    m_possibleTimestamps.push_back(timeStampType("time_boot_ms", 1000.0));
}

LogParserBase::~LogParserBase()
{
    QLOG_DEBUG() << "LogParserBase::LogParserBase - DTOR";
}

void LogParserBase::stopParsing()
{
    QLOG_DEBUG() << "LogParserBase::stopParsing";
    m_stop = true;
}

void LogParserBase::checkForValidTimestamp(typeDescriptor &desc)
{
    for(const auto &timeStamp: m_possibleTimestamps)
    {
        if (desc.m_labels.contains(timeStamp.m_name))
        {
            m_activeTimestamp = timeStamp;
            desc.finalize(m_activeTimestamp);
            break;
        }
    }
}

bool LogParserBase::storeNameValuePairList(QList<NameValuePair> &NameValuePairList, const typeDescriptor &desc)
{
    bool wasNotRepaired = true;
    // Verify data matches descriptor - simple size check
    if(NameValuePairList.size() != desc.m_labels.size())
    {
        QLOG_WARN() << "Number of received values does not match number defined in type. Type:"
                    << desc.m_name << " Expected:" << desc.m_labels.size() << " got:"
                    << NameValuePairList.size() << ". Repairing message.";
        m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter),
                                          "Number of received values does not match number defined in type. Type:"
                                          + desc.m_name + " Expected:" + QString::number(desc.m_labels.size()) + " got:"
                                          + QString::number(NameValuePairList.size()) + ". Repairing message.");

        repairMessage(NameValuePairList, desc);
        wasNotRepaired = false;
    }

    // Set or read timestamp
    if(desc.hasNoTimestamp())
    {
        NameValuePairList.prepend(NameValuePair(m_activeTimestamp.m_name, m_highestTimestamp));
    }
    else
    {
        handleTimeStamp(NameValuePairList, desc);
    }

    if(!m_dataStoragePtr->addDataRow(desc.m_name, NameValuePairList))
    {
        QLOG_WARN() << m_dataStoragePtr->getError();
        m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter), m_dataStoragePtr->getError());
        return false;
    }

    if(wasNotRepaired)
    {
        m_logLoadingState.validDataRead();
    }
    m_MessageCounter++;
    return true;
}

bool LogParserBase::extendedStoreNameValuePairList(QList<NameValuePair> &NameValuePairList, const typeDescriptor &desc)
{
    bool retCode = true;
    // Store unit related information
    if(desc.m_ID == m_idUnitMessage)
    {
        // Unit data contains unit ID on index 1 and Unit Name on index 2
        quint8 id = static_cast<quint8>(NameValuePairList[1].second.toUInt());
        m_dataStoragePtr->addUnitData(id, NameValuePairList[2].second.toString());
    }
    else if(desc.m_ID == m_idMultMessage)
    {
        // Multiplier data contains unit ID on index 1 and the multiplier on index 2
        quint8 id = static_cast<quint8>(NameValuePairList[1].second.toUInt());
        double multi = NameValuePairList[2].second.toDouble();
        // ID 45 and ID 63 are multipliers which should not be used eg. unknown
        if((id == 45) || (id == 63))
        {
            multi = qQNaN();    // we mark them with an NaN wich is easy to detect.
        }
        m_dataStoragePtr->addMultiplierData(id, multi);
    }
    else if(desc.m_ID == m_idFMTUMessage)
    {
        QByteArray multiplierField(NameValuePairList[3].second.toByteArray());
        QByteArray unitField(NameValuePairList[2].second.toByteArray());
        // number of elements in multiplier & unit should be the same
        if(multiplierField.size() == unitField.size())
        {
            m_dataStoragePtr->addMsgToUnitAndMultiplierData(NameValuePairList[1].second.toUInt(), multiplierField, unitField);
            m_hasUnitData = true;
        }
        else
        {
            QLOG_WARN() << "Unit and multiplier info have size mismatch. Number of elements should be the same. "
                        << "Data of type " << NameValuePairList[1].second.toUInt() << " will have no scaling nor Unit info";

            m_logLoadingState.corruptDataRead(static_cast<int>(m_MessageCounter),
                              "Unit and multiplier info have size mismatch. Number of elements should be the same. Data of type "
                              + QString::number(NameValuePairList[1].second.toUInt()) + " will have no scaling nor Unit info");
            retCode = false;
        }
    }
    else
    {
        retCode = storeNameValuePairList(NameValuePairList, desc);
    }

    return retCode;
}

void LogParserBase::handleTimeStamp(QList<NameValuePair> &valuepairlist, const typeDescriptor &desc)
{
    // read time stamp value, add time offset of prepending flight (if there was one), and store it back.
    // Due to this we always have a increasing time value
    quint64 tempVal = static_cast<quint64>(valuepairlist.at(desc.m_timeStampIndex).second.toULongLong());
    tempVal += m_timestampOffset;
    valuepairlist[desc.m_timeStampIndex].second = tempVal;

    if(!m_lastValidTimePerType.contains(desc.m_name))
    {
        m_lastValidTimePerType.insert(desc.m_name, 0);
    }
    // store highest
    m_highestTimestamp = m_highestTimestamp < tempVal ? tempVal : m_highestTimestamp;

    // check if time is increasing
    if (tempVal >= m_lastValidTimePerType[desc.m_name])
    {
        m_lastValidTimePerType[desc.m_name] = tempVal;
    }
    // All time jumps < 60 sec shall be treated as error
    else if(tempVal >= m_lastValidTimePerType[desc.m_name] - 60 * m_activeTimestamp.m_divisor)
    {
        if(m_timeErrorCount < 50)
        {
            QLOG_WARN() << "Corrupt data read: Time for " << desc.m_name << " is not increasing! Last valid time stamp:"
                        << QString::number(m_lastValidTimePerType[desc.m_name]) << " actual read time stamp is:"
                        << QString::number(tempVal);

            ++m_timeErrorCount;
        }
        else if(m_timeErrorCount < 51)
        {
            QLOG_WARN() << "Supressing further time is not increasing messages....";
            ++m_timeErrorCount;
        }
        m_logLoadingState.corruptTimeRead(static_cast<int>(m_MessageCounter), "Log time for " + desc.m_name +
                                          " is not increasing! Last Time:" + QString::number(m_lastValidTimePerType[desc.m_name]) +
                                          " new Time:" + QString::number(tempVal));
        // if not increasing set to last valid value
        valuepairlist[desc.m_timeStampIndex].second = m_lastValidTimePerType[desc.m_name];
    }
    else
    {
        QLOG_INFO() << "Log Start over detected - setting time offset to " << m_highestTimestamp;
        m_timestampOffset = m_highestTimestamp;
        tempVal += m_timestampOffset;
        m_highestTimestamp = tempVal;

        m_lastValidTimePerType[desc.m_name] = tempVal;
        valuepairlist[desc.m_timeStampIndex].second = tempVal;
    }
}

void LogParserBase::detectMavType(const QList<NameValuePair> &valuepairlist)
{
    // Name field in bin logs is not always on same Index. So first search for the right position.
    int nameIndex = 0;
    for (int i = 0; i < valuepairlist.size(); ++i)
    {
        if (valuepairlist[i].first == "Name")
        {
            nameIndex = i;
            break;
        }
        // in tlogs the mav type can be directly read
        if (valuepairlist[i].first == "type")   // "type" field holds MAV_TYPE
        {
            m_loadedLogType = static_cast<MAV_TYPE>(valuepairlist[i].second.toInt());
            break;
        }
    }

    // check again - avoid overwrite in case of tlog file
    if(m_loadedLogType == MAV_TYPE_GENERIC)
    {
        if (valuepairlist[nameIndex].second == "RATE_RLL_P" || valuepairlist[nameIndex].second == "H_SWASH_PLATE"
                || valuepairlist[nameIndex].second == "ATC_RAT_RLL_P" ) // ATC_RAT_RLL_P Used in AC3.4+
        {
            m_loadedLogType = MAV_TYPE_QUADROTOR;
        }
        else if (valuepairlist[nameIndex].second == "PTCH2SRV_P")
        {
            m_loadedLogType = MAV_TYPE_FIXED_WING;
        }
        else if (valuepairlist[nameIndex].second == "SKID_STEER_OUT")
        {
            m_loadedLogType = MAV_TYPE_GROUND_ROVER;
        }
    }

    if(m_loadedLogType != MAV_TYPE_GENERIC)
    {
        m_logLoadingState.setMavType(m_loadedLogType);
    }
}

bool LogParserBase::repairMessage(QList<NameValuePair> &NameValuePairList, const typeDescriptor &descriptor)
{
    QList<NameValuePair> originalList(NameValuePairList);
    NameValuePairList.clear();
    // reconstruct message based on descriptor
    for(const QString &label: descriptor.m_labels)
    {
        bool found = false;
        for(QList<NameValuePair>::Iterator iter = originalList.begin(); iter != originalList.end(); ++iter)
        {
            if(label == iter->first) // does this value exist in original message?
            {
                NameValuePairList.append(*iter);
                originalList.erase(iter);
                found = true;
                break;
            }
        }

        if(!found)
        {
            // this value is missing in message add one with value 0
            NameValuePair pair(label, QVariant(0));
            NameValuePairList.append(pair);
        }
    }
    return true;
}

quint64 LogParserBase::nextValidTimestamp()
{
    return m_highestTimestamp - m_timestampOffset;
}

void LogParserBase::specialDescriptorHandling(typeDescriptor &desc)
{
    if(desc.m_name == "GPS" && !desc.m_labels.contains("GPSTimeMS"))
    {
        // Special handling for "GPS" messages that have a "TimeMS"
        // timestamp but scaling and value does not mach all other time stamps
        desc.replaceLabelName("TimeMS", "GPSTimeMS");
    }
    // Store IDs of UNIT, MULT and FMTU messages to use them later
    else if(desc.m_name == "UNIT")
    {
        m_idUnitMessage = desc.m_ID;
    }
    else if(desc.m_name == "MULT")
    {
        m_idMultMessage = desc.m_ID;
    }
    else if(desc.m_name == "FMTU")
    {
        m_idFMTUMessage = desc.m_ID;
    }
}
