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
 * @file TlogParser.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 22 Okt 2016
 * @brief File providing header for the tlog log parser
 */

#ifndef TLOGPARSER_H
#define TLOGPARSER_H

#include "ILogParser.h"
#include "IParserCallback.h"
#include "LogParserBase.h"
#include "MAVLinkDecoder.h"
#include "LogdataStorage.h"

/**
 * @brief The TlogParser class is a parser for tlog ArduPilot
 *        logfiles (.tlog extension).
 */
class TlogParser : public QObject, public LogParserBase
{
    Q_OBJECT
public:
    /**
     * @brief TlogParser - CTOR
     * @param storagePtr - Pointer to a valid LogdataStorage used for data storage
     * @param object - Pointer to a valid call back interface
     */
    explicit TlogParser(LogdataStorage::Ptr storagePtr, IParserCallback *object);

    /**
     * @brief ~TlogParser - DTOR
     */
    virtual ~TlogParser() override;

    /**
     * @brief parse method reads the logfile. Should be called with an
     *        own thread
     * @param logfile - The file which should be parsed
     * @return - Detailed status of the parsing
     */
    virtual AP2DataPlotStatus parse(QFile &logfile) override;

    /**
     * @brief newValue - Callback for the mavlink decoder - called for every decoded value
     * @param uasId - UAS ID
     * @param name  - Name of the value
     * @param unit  - Unit / data type
     * @param value - the value itself
     * @param msec  - some sort of timestamp
     */
    void newValue(int uasId, const QString &name, const QString &unit, const QVariant &value, quint64 msec);

    /**
     * @brief newTextValue - Callback for the mavlink decoder - called for every decoded string
     * @param uasId         - UAS ID
     * @param componentId   - Component ID
     * @param severity      - severity
     * @param text          - The text
     */
    void newTextValue(int uasId, int componentId, int severity, const QString &text);

private:

    /**
     * @brief The tlogDescriptor class provides a specialized typeDescriptor
     *        with an own isValid method.
     */
    class tlogDescriptor : public typeDescriptor
    {
    public:
        virtual bool isValid() const override;
    };

    QHash<QString, tlogDescriptor> m_nameToDescriptorMap;   /// hashMap storing a format descriptor for every message type

    QByteArray m_dataBlock;                 /// Data buffer for parsing.

    QScopedPointer<MAVLinkDecoder> m_mavDecoderPtr;     /// pointer to mavlink decoder

    quint8 m_lastModeVal;       /// holds the current mode used to detect changes

    QList<NameValuePair> *mp_ReceiveData;  /// pointer to the data structure for receiving data

    quint8 m_GCSMavID = QGC::MavlinkID();  /// sys id of Ground station

    /**
     * @brief addMissingDescriptors adds the missing type descriptors to the
     *        database. tlogs do not have a message for MODE or MSG messages
     *        but their meaning can be extracted from other messages. descriptors
     *        for MODE and MSG are added to the DB.
     */
    void addMissingDescriptors();

    /**
     * @brief parseDescriptor extracts the descriptor data from tlog messages.
     *        It reads its data direcly from the mavlink decoder.
     * @param desc - The descriptor is filled.
     * @return - true - success, false - data could not be parsed
     */
    bool parseDescriptor(tlogDescriptor &desc);

    /**
     * @brief extractDataFields extracts the datafields of a descriptor. it is a helper
     *        used by parseDescriptor method. Maily handles the differences between single
     *        and array data.
     * @param desc - The tlog descriptor to add the datatfiled info
     * @param fieldInfo - the mavlink fieldinfo descriptor where the data is extracted from
     * @param format - the format type for the field
     * @param size - the size of the data field in bytes
     */
    void extractDescriptorDataFields(tlogDescriptor &desc, const mavlink_field_info_t &fieldInfo, const QString &format, int size);

    /**
     * @brief storeDescriptor validates the descriptor adds a time stamp field
     *        if needed and stores it in the datamodel
     * @param desc tlogDescriptor to store
     * @return true - success, false - datamodel failure
     */
    bool storeDescriptor(tlogDescriptor desc);

    /**
     * @brief decodeData - decodes data from a mavlink message to a name value
     *        pair list.
     * @param mavlinkMessage - the message to decode
     * @param NameValuePairList - the parsing result as list of NameValuePair
     * @return true - success, false otherwise or no result
     */
    bool decodeData(const mavlink_message_t &mavlinkMessage, QList<NameValuePair> &NameValuePairList);

    /**
     * @brief extractModeMessage - extracts the data needed for a MODE message from
     *        a NameValuePair list of a tlog HEARTBEAT message and stores it in the datamodel.
     * @param NameValuePairList - input data of a tlog HEARTBEAT message
     * @return - true success, false datamodel failure
     */
    bool extractModeMessage(const QList<NameValuePair> &NameValuePairList);

    /**
     * @brief extractMsgMessage extracts the data needed for a MSG message from
     *        a NameValuePair list of a tlog STATUSTEXT message and stores it in the datamodel.
     * @param NameValuePairList - input data of a tlog STATUSTEXT message
     * @return - true success, false datamodel failure
     */
    bool extractMsgMessage(const QList<NameValuePair> &NameValuePairList);

};

#endif // TLOGPARSER_H
