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
#include "LogParserBase.h"
#include "LogdataStorage.h"

/**
 * @brief The BinLogParser class is a parser for binary ArduPilot
 *        logfiles aka flash logs
 */
class BinLogParser : public LogParserBase
{
public:

    /**
     * @brief BinLogParser - CTOR
     * @param storagePtr - Pointer to a valid LogdataStorage used for data storage
     * @param object - Pointer to a valid call back interface
     */
    explicit BinLogParser(LogdataStorage::Ptr storagePtr, IParserCallback *object);

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

private:

    static const quint8 s_FMTMessageType = 0x80; /// Type Id of the format (FMT) message
    static const quint8 s_STRTMessageType = 0x0A; /// Type Id of the Start (STRT) message

    static const int s_MinHeaderSize  = 5;       /// Minimal size to be able to start parsing
    static const int s_HeaderOffset  = 3;        /// byte offset after successful header parsing
    static const quint8 s_StartByte1 = 0xA3;     /// Startbyte 1 is always first byte in one message
    static const quint8 s_StartByte2 = 0x95;     /// Startbyte 2 is always second byte in one message

    static const int s_FMTNameSize   = 4;        /// Size of the name field in FMT message
    static const int s_FMTFormatSize = 16;       /// Size of the format field in FMT message
    static const int s_FMTLabelsSize = 64;       /// Size of the comma delimited names field in FMT message


    /**
     * @brief The binDescriptor class provides a specialized typeDescriptor
     *        with an own isValid method.
     */
    class binDescriptor : public typeDescriptor
    {
    public:
        virtual bool isValid() const;
    };

    QByteArray m_dataBlock;                 /// Data buffer for parsing.
    int m_dataPos;                          /// bytecounter for running through the data packet.
    quint8 m_messageType;                   /// Holding type of the actual message.

    QHash<int, binDescriptor> m_typeToDescriptorMap;   /// hashMap storing a format descriptor for every message type

    QList<binDescriptor> m_descriptorForDeferredStorage; /// temp list for storing descriptors without a timestamp field

    /**
     * @brief headerIsValid checks the first 2 start bytes
     *        and extracts the message type which is stored in m_messageType.
     *
     * @return true - header is valid, false otherwise
     */
    bool headerIsValid();

    /**
     * @brief parseFMTMessage parses a FMT message into a binDescriptor
     *        and removes the parsed data from the input array
     * @param desc binDescriptor to be filled
     * @return true - on success, false - not enough data to parse the message
     */
    bool parseFMTMessage(binDescriptor &desc);

    /**
     * @brief storeDescriptor validates the descriptor adds a time stamp field
     *        if needed and stores it in the datamodel
     * @param desc binDescriptor to store
     * @return true - success, false - datamodel failure
     */
    bool storeDescriptor(binDescriptor desc);

    /**
     * @brief extendedStoreDescriptor just calls storeDescriptor on all elements
     *        hold in m_descriptorForDeferredStorage, then clears the container
     *        and stores the descriptor passed
     * @param desc binDescriptor to be stored
     * @return true - success, false - datamodel failure
     */
    bool extendedStoreDescriptor(const binDescriptor &desc);

    /**
     * @brief parseDataByDescriptor parses the data like described in the
     *        descriptor which is referenced by m_messageType. After the
     *        parsing the used data is removed from the data array
     * @param NameValuePairList - conatiner for the paresed data
     * @return
     */
    bool parseDataByDescriptor(QList<NameValuePair> &NameValuePairList, const binDescriptor &desc);
};

#endif // BINLOGPARSER_H
