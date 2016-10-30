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
 * @file BinLogParser.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 14 Jul 2016
 * @brief File providing header for the binary log parser
 */

#ifndef BINLOGPARSER_H
#define BINLOGPARSER_H

#include "ILogParser.h"
#include "IParserCallback.h"
#include "AP2DataPlot2DModel.h"

/**
 * @brief The BinLogParser class is a parser for binary ArduPilot
 *        logfiles aka flash logs
 */
class BinLogParser : public ILogParser
{
public:

    /**
     * @brief BinLogParser - CTOR
     * @param model - Pointer to a valid AP2DataPlot2DModel used for data storage
     * @param object - Pointer to a valid call back interface
     */
    explicit BinLogParser(AP2DataPlot2DModel *model, IParserCallback *object);

    /**
     * @brief ~BinLogParser - DTOR
     */
    virtual ~BinLogParser();

    /**
     * @brief parse method reads the logfile. Should be called with an
     *        own thread
     * @param logfile - The file which should be parsed
     * @return - Detailed status of the parsing
     */
    virtual AP2DataPlotStatus parse(QFile &logfile);

    /**
     * @brief stopParsing forces the parse method to return immediately.
     *        Shall be used for cancelling or stopping.
     */
    virtual void stopParsing();

private:

    static const int s_MinHeaderSize  = 5;       /// Minimal size to be able to start parsing
    static const int s_HeaderOffset  = 3;        /// byte offset after successful header parsing
    static const quint8 s_StartByte1 = 0xA3;     /// Startbyte 1 is always first byte in one message
    static const quint8 s_StartByte2 = 0x95;     /// Startbyte 2 is always second byte in one message

    static const quint8 s_FMTMessageType = 0x80; /// Type Id of the format (FMT) message
    static const int s_FMTNameSize   = 4;        /// Size of the name field in FMT message
    static const int s_FMTFormatSize = 16;       /// Size of the format field in FMT message
    static const int s_FMTLabelsSize = 64;       /// Size of the comma delimited names field in FMT message

    static const quint8 s_STRTMessageType = 0x0A; /// Type Id of the Start (STRT) message

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
    struct typeDescriptor
    {
        typeDescriptor();

        /**
         * @brief finalize sets the m_hasTimeStamp and the m_timeStampIndex by trying to
         *        find the Timestamp name in m_labels
         * @param timeStamp - time stamp to search for.
         */
        void finalize(const timeStampType &timeStamp);

        /**
         * @brief addTimeStampField adds a timestamp field to the descriptor.
         * @param timestamp - time stamp type which should be added
         */
        void addTimeStampField(const timeStampType &timestamp);

        /**
         * @brief replaceLabelName replaces a special label in m_labels
         * @param oldName - name string to search for
         * @param newName - the new name to replace the old one
         */
        void replaceLabelName(const QString &oldName, const QString newName);

        QString getLabelAtIndex(int index) const;
        bool hasNoTimestamp() const;
        bool isValid() const;

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
    AP2DataPlot2DModel *m_dataModel;        /// Pointer to the datamodel for storing the data

    QByteArray m_dataBlock;                 /// Data buffer for parsing.
    int m_dataPos;                          /// bytecounter for running through the data packet.
    quint8 m_messageType;                   /// Holding type of the actual message.
    quint64 m_MessageCounter;               /// Simple counter showing number of message wich is currently parsed

    bool m_stop;                            /// Flag indicating to stop parsing
    MAV_TYPE m_loadedLogType;               /// Mav type of the log - will be populated during parsing
    AP2DataPlotStatus m_logLoadingState;    /// State of the parser
    int m_timeErrorCount;                   /// Counter for time errors used to avoid log flooding

    QHash<int, typeDescriptor> m_typeToDescriptorMap;   /// hashMap storing a format descriptor for every message type

    QList<timeStampType> m_possibleTimestamps;            /// List of possible timestamps. Filled in CTOR
    timeStampType m_activeTimestamp;                      /// the detected timestamp used for parsing
    QList<typeDescriptor> m_descriptorForDeferredStorage; /// temp list for storing descriptors without a timestamp field
    quint64 m_lastValidTimeStamp;                         /// Contains always the last valid timestamp


    /**
     * @brief headerIsValid checks the first 2 start bytes
     *        and extracts the message type which is stored in m_messageType.
     *
     * @return true - header is valid, false otherwise
     */
    bool headerIsValid();

    /**
     * @brief parseFMTMessage parses a FMT message into a typeDescriptor
     *        and removes the parsed data from the input array
     * @param desc typeDescriptor to be filled
     * @return true - on success, false - not enough data to parse the message
     */
    bool parseFMTMessage(typeDescriptor &desc);

    /**
     * @brief storeDescriptor validates the descriptor adds a time stamp field
     *        if needed and stores it in the datamodel
     * @param desc typeDescriptor to store
     * @return true - success, false - datamodel failure
     */
    bool storeDescriptor(typeDescriptor desc);

    /**
     * @brief extendedStoreDescriptor just calls storeDescriptor on all elements
     *        hold in m_descriptorForDeferredStorage, then clears the container
     *        and stores the descriptor passed
     * @param desc typeDescriptor to be stored
     * @return true - success, false - datamodel failure
     */
    bool extendedStoreDescriptor(const typeDescriptor &desc);

    /**
     * @brief parseDataByDescriptor parses the data like described in the
     *        descriptor which is referenced by m_messageType. After the
     *        parsing the used data is removed from the data array
     * @param NameValuePairList - conatiner for the paresed data
     * @return
     */
    bool parseDataByDescriptor(QList<NameValuePair> &NameValuePairList, const typeDescriptor &desc);

    /**
     * @brief storeNameValuePairList stores the NameValuePairList into the
     *        datamodel. Adds a timestamp if needed
     * @param NameValuePairList to be stored
     * @return true - success, false - datamodel failure
     */
    bool storeNameValuePairList(QList<NameValuePair> &NameValuePairList, const typeDescriptor &desc);

    /**
     * @brief checkForValidTimestamp verifies whether the descriptor has a
     *        time stamp field. If the descriptor has one it sets the internal
     *        m_activeTimestamp. All descriptors which have no time stamp are
     *        stored until a valid time stamp was detected.
     * @param desc - the typeDescriptor to check
     */
    void checkForValidTimestamp(typeDescriptor &desc);

    /**
     * @brief readTimeStamp reads the current timestamp from the value pair list
     * @param valuepairlist - value pair list to read the timestamp from
     * @param timeStampIndex - Index in list where the timestamp resides
     */
    void readTimeStamp(QList<NameValuePair> &valuepairlist, const int timeStampIndex);

    /**
     * @brief detectMavType tries to detect the MAV type from the data in a
     *        value pair list.
     * @param valuepairlist - value pair list to do the check on
     */
    void detectMavType(const QList<NameValuePair> &valuepairlist);

};

#endif // BINLOGPARSER_H
