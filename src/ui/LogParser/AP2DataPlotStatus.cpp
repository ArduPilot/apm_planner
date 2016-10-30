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
 * @file AP2DataPlotStatus.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 21 sept 2016
 * @brief File providing implementation for the AP2DataPlotStatus
 */

#include "AP2DataPlotStatus.h"
#include <QTextStream>

AP2DataPlotStatus::AP2DataPlotStatus() :
    m_lastParsingState(OK),
    m_globalState(OK),
    m_loadedLogType(MAV_TYPE_GENERIC),
    m_noMessageBytes(0)
{}

void AP2DataPlotStatus::corruptDataRead(const int index, const QString &errorMessage)
{
    // When here we just know that we have an error but not if it is at the end
    // or in the middle of the logfile. So we set truncation error. Data error will
    // be set as soon as valid data is read again.
    m_globalState = m_globalState == OK ? TruncationError : m_globalState;
    m_lastParsingState = DataError;
    errorEntry entry(m_lastParsingState, index, errorMessage);
    m_errors.push_back(entry);
}

void AP2DataPlotStatus::corruptFMTRead(const int index, const QString &errorMessage)
{
    m_globalState = m_globalState == OK ? FmtError : m_globalState;
    m_lastParsingState = FmtError;
    errorEntry entry(m_lastParsingState, index, errorMessage);
    m_errors.push_back(entry);
}

void AP2DataPlotStatus::corruptTimeRead(const int index, const QString &errorMessage)
{
    m_globalState = m_globalState == OK ? TimeError : m_globalState;
    m_lastParsingState = TimeError;
    errorEntry entry(m_lastParsingState, index, errorMessage);
    m_errors.push_back(entry);
}

void AP2DataPlotStatus::setNoMessageBytes(const int noMessageBytes)
{
    m_noMessageBytes = noMessageBytes;
}

void AP2DataPlotStatus::setMavType(const MAV_TYPE type)
{
    m_loadedLogType = type;
}

MAV_TYPE AP2DataPlotStatus::getMavType() const
{
    return m_loadedLogType;
}

AP2DataPlotStatus::parsingState AP2DataPlotStatus::getParsingState() const
{
    return m_globalState;
}


QString AP2DataPlotStatus::getErrorOverview() const
{
    int timeErrorCount = 0;
    int dataErrorCount = 0;
    int fmtErrorCount  = 0;
    int unknownErrorCount = 0;
    QString out;
    QTextStream outStream(&out);

    foreach (const errorEntry &entry, m_errors)
    {
        switch (entry.m_state)
        {
        case OK:
            break;
        case FmtError:
            fmtErrorCount++;
            break;
        case TimeError:
            timeErrorCount++;
            break;
        case DataError:
            dataErrorCount++;
            break;
        default:
            unknownErrorCount++;
            break;
        }
    }

    if (fmtErrorCount     > 0) outStream << fmtErrorCount << " format corruptions" << endl;
    if (timeErrorCount    > 0) outStream << timeErrorCount << " time corruptions" << endl;
    if (dataErrorCount    > 0) outStream << dataErrorCount << " data corruptions" << endl;
    if (unknownErrorCount > 0) outStream << unknownErrorCount << " unspecific corruptions" << endl;
    if (m_noMessageBytes  > 0) outStream << m_noMessageBytes << " Bytes were dropped" << endl;

    return out;
}

QString AP2DataPlotStatus::getDetailedErrorText() const
{
    QString out;
    QTextStream outStream(&out);
    int errorCount = 0;

    foreach (const errorEntry &entry, m_errors)
    {
        if (entry.m_state != OK)    // Only if a error
        {
            outStream << "Logline " << entry.m_index << ": " << entry.m_errortext << endl;
            errorCount++;
        }
    }
    outStream << endl << " There were " << errorCount << " errors during log parsing." << endl;
    if(m_noMessageBytes > 0)
    {
        outStream << m_noMessageBytes << " bytes were dropped during parsing." << endl;
    }
    return out;
}
