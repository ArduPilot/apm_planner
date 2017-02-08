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
 * @file AP2DataPlotStatus.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 21 sept 2016
 * @brief File providing header for the AP2DataPlotStatus
 */


#ifndef AP2DATAPLOTSTATUS_H
#define AP2DATAPLOTSTATUS_H

#include <QString>
#include <QVector>

#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

/**
 * @brief The AP2DataPlotStatus class is a helper class desinged as status type for
 *        the log parsing.
 *        It contains the final state of parsing as well as all error strings inserted
 *        with the corruptDataRead(), corruptFMTRead() etc. methods during the parsing
 *        process.
 */
class AP2DataPlotStatus
{
public:

    /**
     * @brief The parsingState enum
     *        All possible parsing states
     */
    enum parsingState
    {
        OK,                 /// Perfect result
        FmtError,           /// Corrupt Format description.
        TruncationError,    /// The log was truncated due to errors @ the end
        TimeError,          /// The log contains corrupt time data
        DataError           /// Data can be corrupted or incomplete
    };

    /**
     * @brief AP2DataPlotStatus CTOR
     */
    AP2DataPlotStatus();

    /**
     * @brief validDataRead
     *        Shall be called if a logline was read successful. Used to
     *        determine if the error(s) are only at the end of the log.
     *        Should be inline due to the high calling frequency.
     */
    inline void validDataRead()
    {
        // Rows with time errors will stored too, so they have to handeled like
        // the OK ones.
        if (!((m_lastParsingState == OK)||(m_lastParsingState == TimeError)))
        {
            // insert entry with state OK to mark data is ok.
            m_errors.push_back(errorEntry());
            m_lastParsingState = OK;
            // When here we know we had an error and now data is OK again
            // Set to data error as we cannot predict whats wrong
            m_globalState = DataError;
        }
    }

    /**
     * @brief corruptDataRead
     *        Shall be called when ever an error occurs while parsing
     *        a data package.
     *
     * @param index - The log index the error occured
     * @param errorMessage - Error message describing the error reason
     */
    void corruptDataRead(const int index, const QString &errorMessage);

    /**
     * @brief corruptFMTRead
     *        Shall be called when ever an error occurs while parsing
     *        a format package.
     *
     * @param index - The log index the error occured
     * @param errorMessage - Error message describing the error reason
     */
    void corruptFMTRead(const int index, const QString &errorMessage);

    /**
     * @brief corruptTimeRead
     *        Shall be called when ever a time error occurs while parsing
     *        any data.
     *
     * @param index - The log index the error occured
     * @param errorMessage - Error message describing the error reason
     */
    void corruptTimeRead(const int index, const QString &errorMessage);

    /**
     * @brief setNoMessageBytes
     *        Stores the number of bytes which could not be parsed and adds
     *        them to the output
     *
     * @param noMessageBytes - number of bytes which could not be parsed
     */
    void setNoMessageBytes(const int noMessageBytes);

    /**
     * @brief setMavType
     *        Set the MAV_TYPE of the loaded log.
     *
     * @param type - MAV_TYPE
     */
    void setMavType(const MAV_TYPE type);

    /**
     * @brief getMavType
     *        Gets the stored MAV_TYPE.
     *
     * @return The stored MAV_TYPE
     */
    MAV_TYPE getMavType() const;

    /**
     * @brief getParsingState
     *        Delivers the final state of the log parsing. The value
     *        is only valid if parsing is finished.
     *
     * @return - The parsing state - @see parsingState
     */
    parsingState getParsingState() const;

    /**
     * @brief getErrorOverview
     *        Creates an overview of errors occured. Type and number are listed
     *
     *  @return - String containing overview data
     */
    QString getErrorOverview() const;

    /**
     * @brief getDetailedErrorText
     *        Creates a text containing all errormessages inserted during
     *        parsing. One line for each error.
     *
     * @return - multi line string with all error messages.
     */
    QString getDetailedErrorText() const;

private:
    /**
     * @brief The errorEntry struct
     *        holds all data describing the error
     */
    struct errorEntry
    {
        parsingState m_state;
        int m_index;
        QString m_errortext;

        errorEntry() : m_state(OK), m_index(0){}
        errorEntry(const parsingState state, const int index, const QString &text) :
                   m_state(state), m_index(index), m_errortext(text) {}
    };

    parsingState m_lastParsingState;        /// The internal parsing state since last call
    parsingState m_globalState;             /// Reflecting the overall parsing state
    QVector<errorEntry> m_errors;           /// For storing all error entries
    MAV_TYPE     m_loadedLogType;           /// Type of the loaded log (QUAD, PLANE, ROVER...)
    int m_noMessageBytes;                   /// Shall store the No Message Bytes of the log
};


#endif // AP2DATAPLOTSTATUS_H
