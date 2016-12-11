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
 * @file LogParserBase.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 11 okt 2016
 * @brief File providing header for the log parser base class
 */

#ifndef LOGPARSERBASE_H
#define LOGPARSERBASE_H

#include "ILogParser.h"
#include "IParserCallback.h"
#include "LogdataStorage.h"

/**
 * @brief The LogParserBase class provides a base class for all log
 *        parsers.
 */
class LogParserBase : public ILogParser
{
public:
    /**
     * @brief LogParserBase - CTOR
     * @param storagePtr - Pointer to a valid LogdataStorage used for data storage
     * @param object - Pointer to a valid call back interface
     */
    explicit LogParserBase(LogdataStorage::Ptr storagePtr, IParserCallback *object);

    /**
     * @brief ~LogParserBase - DTOR
     */
    virtual ~LogParserBase();

    /**
     * @brief stopParsing forces the parse method to return immediately.
     *        Shall be used for cancelling or stopping.
     */
    virtual void stopParsing();

protected:

    /**
     * @brief The timeStampType struct
     *        Used to hold the name and the scaling of a time stamp.
     */
    struct timeStampType
    {
        QString m_name;     /// Name of the time stamp
        double  m_divisor;  /// Divisor to scale time stamp to seconds

        timeStampType() : m_divisor(0.0) {}
        timeStampType(const QString &name, const double divisor) : m_name(name), m_divisor(divisor) {}
        bool valid()
        {
            return m_name.size() != 0;
        }
    };

    /**
     * @brief The typeDescriptor struct
     *        Used to hold all data needed to describe a message type
     */
    class typeDescriptor
    {
    public:

        typeDescriptor();

        virtual ~typeDescriptor() {}

        /**
         * @brief finalize sets the m_hasTimeStamp and the m_timeStampIndex by trying to
         *        find the Timestamp name in m_labels
         * @param timeStamp - time stamp to search for.
         */
        virtual void finalize(const timeStampType &timeStamp);

        /**
         * @brief addTimeStampField adds a timestamp field to the descriptor.
         * @param timestamp - time stamp type which should be added
         */
        virtual void addTimeStampField(const timeStampType &timestamp);

        /**
         * @brief replaceLabelName replaces a special label in m_labels
         * @param oldName - name string to search for
         * @param newName - the new name to replace the old one
         */
        virtual void replaceLabelName(const QString &oldName, const QString newName);

        virtual QString getLabelAtIndex(int index) const;
        virtual bool hasNoTimestamp() const;
        virtual bool isValid() const;

        quint8 m_ID;            /// ID of the message - mainly used for validation
        int m_length;           /// Length of the message
        QString m_name;         /// Name of the message
        QString m_format;       /// Format string like "QbbI"
        QStringList m_labels;   /// List of labels for each value in message (colums).
        bool m_hasTimeStamp;    /// true if descriptor has valid Timestamp.
        int m_timeStampIndex;   /// contains the index pointing to the time stamp field.
    };

    typedef QPair<QString, QVariant> NameValuePair;          /// Type holding Lablestring and its value

    IParserCallback *m_callbackObject;      /// Pointer to callback interface.
    LogdataStorage::Ptr m_dataStoragePtr;   /// Pointer to the datamodel for storing the data

    bool m_stop;                            /// Flag indicating to stop parsing
    quint64 m_MessageCounter;               /// Simple counter showing number of message wich is currently parsed
    int m_timeErrorCount;                   /// Counter for time errors used to avoid log flooding

    MAV_TYPE m_loadedLogType;               /// Mav type of the log - will be populated during parsing
    AP2DataPlotStatus m_logLoadingState;    /// State of the parser

    QList<timeStampType> m_possibleTimestamps;            /// List of possible timestamps. Filled in CTOR
    timeStampType m_activeTimestamp;                      /// the detected timestamp used for parsing
    QHash<QString, quint64> m_lastValidTimePerType;       /// Contains the last valid timestamp for each type (which have a timestamp)
    quint64 m_highestTimestamp;                           /// Contains always the biggest timestamp


    /**
     * @brief checkForValidTimestamp verifies whether the descriptor has a
     *        time stamp field. If the descriptor has one it sets the internal
     *        m_activeTimestamp.
     * @param desc - the binDescriptor to check
     */
    void checkForValidTimestamp(typeDescriptor &desc);

    /**
     * @brief storeNameValuePairList stores the NameValuePairList into the
     *        datamodel. Adds a timestamp if needed
     * @param NameValuePairList to be stored
     * @return true - success, false - datamodel failure
     */
    bool storeNameValuePairList(QList<NameValuePair> &NameValuePairList, const typeDescriptor &desc);

    /**
     * @brief readTimeStamp reads the current timestamp from the value pair list
     * @param valuepairlist - value pair list to read the timestamp from
     * @param timeStampIndex - Index in list where the timestamp resides
     */
    void readTimeStamp(QList<NameValuePair> &valuepairlist, const typeDescriptor &desc);

    /**
     * @brief detectMavType tries to detect the MAV type from the data in a
     *        value pair list.
     * @param valuepairlist - value pair list to do the check on
     */
    void detectMavType(const QList<NameValuePair> &valuepairlist);

    /**
     * @brief repairMessage tries to repair a message to match the descriptor.
     *        Missing fields will be added and set to 0. If there are too many fields
     *        they are cut.
     * @param NameValuePairList - list that needs repair.
     * @param descriptor - descriptor describing the message type.
     * @return true - message is ok, false otherwise.
     */
    bool repairMessage(QList<NameValuePair> &NameValuePairList, const typeDescriptor &desc);

};

#endif // LOGPARSERBASE_H
