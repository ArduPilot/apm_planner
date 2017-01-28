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
 * @file AsciiLogParser.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 09 Okt 2016
 * @brief File providing header for the ascii log parser
 */

#ifndef ASCIILOGPARSER_H
#define ASCIILOGPARSER_H

#include "ILogParser.h"
#include "IParserCallback.h"
#include "LogParserBase.h"
#include "LogdataStorage.h"

/**
 * @brief The AsciiLogParser class is a parser for ASCII ArduPilot
 *        logfiles (.log extension).
 */
class AsciiLogParser : public LogParserBase
{
public:

    static const QString s_FMTMessageName;      /// Name of the format (FMT) message
    static const QString s_STRTMessageName;     /// Name of the Start (STRT) message

    /**
     * @brief AsciiLogParser - CTOR
     * @param storagePtr - Pointer to a valid LogdataStorage used for data storage
     * @param object - Pointer to a valid call back interface
     */
    explicit AsciiLogParser(LogdataStorage::Ptr storagePtr, IParserCallback *object);

    /**
     * @brief ~AsciiLogParser - DTOR
     */
    virtual ~AsciiLogParser();

    /**
     * @brief parse method reads the logfile. Should be called with an
     *        own thread
     * @param logfile - The file which should be parsed
     * @return - Detailed status of the parsing
     */
    virtual AP2DataPlotStatus parse(QFile &logfile);


private:

    static const char s_TokenSeperator = ',';   /// Token seperator in log file

    static const int s_TypeIndex   = 0;         /// Index of message type
    static const int s_IDIndex     = 1;         /// Index of message ID
    static const int s_LengthIndex = 2;         /// Index of message length
    static const int s_NameIndex   = 3;         /// Index of message name
    static const int s_FormatIndex = 4;         /// Index of message format

    static const int s_MinFmtTokens = 5;        /// Min amount of tokens in a FMT line

    /**
     * @brief The asciiDescriptor class provides a specialized typeDescriptor
     *        with an own isValid method.
     */
    class asciiDescriptor : public typeDescriptor
    {
    public:
        virtual bool isValid() const;
    };

    QHash<QString, asciiDescriptor> m_nameToDescriptorMap;   /// hashMap storing a format descriptor for every message type

    QStringList m_tokensToParse;        /// Tokenized input data to parse
    int m_noMessageBytes;               /// Dropped bytes during parsing

    QList<asciiDescriptor> m_descriptorForDeferredStorage; /// temp list for storing descriptors without a timestamp field


    /**
     * @brief parseFMTMessage parses a FMT message into a asciiDescriptor.
     * @param desc asciiDescriptor to be filled
     * @return true - on success, false - parsing failure
     */
    bool parseFMTMessage(asciiDescriptor &desc);

    /**
     * @brief storeDescriptor validates the descriptor adds a time stamp field
     *        if needed and stores the descriptor in the datamodel
     * @param desc asciiDescriptor to store
     * @return true - success, false - datamodel failure
     */
    bool storeDescriptor(asciiDescriptor desc);

    /**
     * @brief extendedStoreDescriptor just calls storeDescriptor on all elements
     *        hold in m_descriptorForDeferredStorage, then clears the container
     *        and stores the descriptor passed
     * @param desc asciiDescriptor to be stored
     * @return true - success, false - datamodel failure
     */
    bool extendedStoreDescriptor(const asciiDescriptor &desc);

    /**
     * @brief parseDataByDescriptor parses the data like described in the
     *        descriptor which is referenced by the message name.
     * @param NameValuePairList - conatiner for the parsed data
     * @return true - success, false - parsing failed
     */
    bool parseDataByDescriptor(QList<NameValuePair> &NameValuePairList, const asciiDescriptor &desc);
};

#endif // ASCIILOGPARSER_H
