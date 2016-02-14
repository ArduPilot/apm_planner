/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief APM UAS specilization Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 */

#ifndef ARDUPILOTMEGAMAV_H
#define ARDUPILOTMEGAMAV_H

#include "UAS.h"
#include <QString>
#include <QSqlDatabase>

class ArduPilotMegaMAV : public UAS
{
    Q_OBJECT
public:
    ArduPilotMegaMAV(MAVLinkProtocol* mavlink, int id = 0);
    /** @brief Set camera mount stabilization modes */
    void setMountConfigure(unsigned char mode, bool stabilize_roll,bool stabilize_pitch,bool stabilize_yaw);
    /** @brief Set camera mount control */
    void setMountControl(double pa,double pb,double pc,bool islatlong);

    QString getCustomModeText();
    QString getCustomModeAudioText();
    void playCustomModeChangedAudioMessage();
    void playArmStateChangedAudioMessage(bool armedState) ;

    void loadSettings();
    void saveSettings();

signals:
    void versionDetected(QString versionString);

public slots:
    /** @brief Receive a MAVLink message from this MAV */
    void receiveMessage(LinkInterface* link, mavlink_message_t message);
    void RequestAllDataStreams();

    // Overides from UAS virtual interface
    virtual void armSystem();
    virtual void disarmSystem();

    // UAS Interface
    void textMessageReceived(int uasid, int componentid, int severity, QString text);
    void heartbeatTimeout(bool timeout, unsigned int ms);

private slots:
    void uasConnected();
    void uasDisconnected();

private:
    void createNewMAVLinkLog(uint8_t type);

private:
    QTimer *txReqTimer;
    bool m_severityCompatibilityMode;
};


/**
 * @brief Base Class for all message types
 */
class MessageBase
{
public:

    static const QString timeFieldName;

    typedef QSharedPointer<MessageBase> Ptr;

    MessageBase();

    /**
     * @brief MessageBase constructor for setting all params
     * @param index - Index of this message
     * @param timeStamp - Time stamp of this message
     * @param name - name of this message, used to identify type
     * @param color - color associated with this message type
     */
    MessageBase(const quint64 index, const quint64 timeStamp, const QString &name, const QColor &color);

    virtual ~MessageBase(){}

    /**
     * @brief Getter for the index of this message
     * @return The index
     */
    virtual quint64 getIndex() const;

    /**
     * @brief Getter for the Time stamp of this message.
     * @attention This value can countain 0 as not all logs
     *            support a time stamp.
     * @return  the time stamp or 0 if not supported by log
     */
    virtual quint64 getTimeStamp() const;

    /**
     * @brief Reads an QSqlRecord and sets the internal data.
     *        See derived types.
     * @param record[in] - Filled QSqlRecord
     * @return true - all Fields could be read
     *         false - not all data could be read
     */
    virtual bool setFromSqlRecord(const QSqlRecord &record) = 0;

    /**
     * @brief Converts the ErrorCode into an uninterpreted string.
     *        Uinterpreted means it prints ErrorCode and SubSystem.
     * @return The uninterpreted Qstring
     */
    virtual QString toString() const = 0;

    /**
     * @brief typeName returns the message type name.
     * @return Type name string
     */
    virtual QString typeName() const;

    /**
     * @brief typeColor returns an QColor object with the color associated
     *        with the typ of the Message.
     * @return Color for this type
     */
    virtual QColor typeColor() const;

protected:

    quint64 m_Index;        /// DB Index of this message
    quint64 m_TimeStamp;    /// Timestamp of this message
    QString m_TypeName;     /// Name of this message
    QColor  m_Color;        /// Color associated with this message
};

/**
 * @brief Class for making it easier to handle the errorcodes.
 *        This class implements everything which is needed to
 *        handle MAV Errors.
 */
class ErrorMessage : public MessageBase
{
public:

    static const QString TypeName;   /// Name of this message is 'ERR'

    ErrorMessage();

    /**
     * @brief ErrorMessage Constructor for setting all internals
     * @param index - Index of this message
     * @param timeStamp - Time stamp of this message
     * @param subSys - Subsys who emitted this error
     * @param errCode - Errorcode emmitted by subsys
     */
    ErrorMessage(const quint64 index, const quint64 timeStamp, const quint8 subSys, const quint8 errCode);

    /**
     * @brief Getter for the Subsystem ID which emitted the error
     * @return Subsystem ID
     */
    quint8 getSubsystemCode() const;

    /**
     * @brief Getter for the Errorcode emitted by the subsystem
     * @return Errorcode
     */
    quint8 getErrorCode() const;

    /**
     * @brief Reads an QSqlRecord and sets the internal data.
     *        The record must contain an Index in colum 0 and the
     *        colums "Subsys" and "ECode" in order to get a positive
     *        return value. If the record conatins also a "TimeUS"
     *        field the internal time stamp will be filled too.
     *
     * @param record[in] - Filled QSqlRecord
     * @return true - all Fields could be read
     *         false - not all data could be read
     */
    virtual bool setFromSqlRecord(const QSqlRecord &record);

    /**
     * @brief Converts the ErrorCode into an uninterpreted string.
     *        Uinterpreted means it prints ErrorCode and SubSystem.
     * @return The uninterpreted Qstring
     */
    virtual QString toString() const;

private:

    quint8 m_SubSys;        /// Subsystem signaling the error
    quint8 m_ErrorCode;     /// Errorcode of the Subsystem
};


/**
 * @brief Class for making it easier to handle the mode messages.
 *        This class implements everything which is needed to
 *        handle MAV Mode messages.
 */
class ModeMessage : public MessageBase
{
public:
    static const QString TypeName;   /// Name of this message is 'MODE'

    ModeMessage();

    /**
     * @brief ModeMessage Costructor for setting all internals
     * @param index - Index of this message
     * @param timeStamp - Time stamp of this message
     * @param mode - Mode of this message
     * @param modeNum - Mode Num of this message (not used)
     */
    ModeMessage(const quint64 index, const quint64 timeStamp, const qint8 mode, const quint8 modeNum);

    /**
     * @brief Getter for the Mode of this message
     * @return Mode ID
     */
    qint8 getMode() const;

    /**
     * @brief Getter for the ModeNum of this message
     * @return ModeNum ID
     */
    quint8 getModeNum() const;

    /**
     * @brief Reads an QSqlRecord and sets the internal data.
     *        The record must contain an Index in colum 0 and the
     *        colums "Mode" and "ModeNum" in order to get a positive
     *        return value. If the record conatins also a "TimeUS"
     *        field the internal time stamp will be filled too.
     *
     * @param record[in] - Filled QSqlRecord
     * @return true - all Fields could be read
     *         false - not all data could be read
     */
    virtual bool setFromSqlRecord(const QSqlRecord &record);

    /**
     * @brief Converts the ModeMessage into an uninterpreted string.
     *        Uinterpreted means it prints Mode ID and ModNum ID.
     * @return The uninterpreted Qstring
     */
    virtual QString toString() const;

private:

    qint8 m_Mode;        /// Subsystem signaling the error
    quint8 m_ModeNum;    /// Errorcode of the Subsystem
};

/**
 * @brief Class for making it easier to handle the event messages.
 *        This class implements everything which is needed to
 *        handle MAV EV messages.
 */
class EventMessage : public MessageBase
{
public:

    static const QString TypeName;   /// Name of this message is 'EV'

    EventMessage();

    /**
     * @brief EventMessage Constructor for setting all internals
     * @param index - Index of this message
     * @param timeStamp - Time stamp of this message
     * @param eventID - Event ID of this message
     */
    EventMessage(const quint64 index, const quint64 timeStamp, const quint8 eventID);

    /**
     * @brief Getter for the Event ID of this message
     * @return Event ID
     */
    quint8 getEventID() const;

    /**
     * @brief Reads an QSqlRecord and sets the internal data.
     *        The record must contain an Index in colum 0 and the
     *        colums "Id" in order to get a positive return value.
     *        If the record conatins also a "TimeUS"
     *        field the internal time stamp will be filled too.
     *
     * @param record[in] - Filled QSqlRecord
     * @return true - all Fields could be read
     *         false - not all data could be read
     */
    virtual bool setFromSqlRecord(const QSqlRecord &record);

    /**
     * @brief Converts the ModeMessage into an uninterpreted string.
     *        Uinterpreted means it prints Mode ID and ModNum ID.
     * @return The uninterpreted Qstring
     */
    virtual QString toString() const;

private:

     quint8 m_EventID;    /// EventID
};

/**
 * @brief Class for making it easier to handle the Msg messages.
 *        This class implements everything which is needed to
 *        handle MAV MSG messages.
 *        This class has no getter - use toString method instead.
 */
class MsgMessage : public MessageBase
{
public:

    static const QString TypeName;   /// Name of this message is 'MSG'

    MsgMessage();

    /**
     * @brief MsgMessage Constructor for setting all internals
     * @param index - Index of this message
     * @param timeStamp - Time stamp of this message
     * @param eventID - Event ID of this message
     */
    MsgMessage(const quint64 index, const quint64 timeStamp, const QString &message);

    /**
     * @brief Reads an QSqlRecord and sets the internal data.
     *        The record must contain an Index in colum 0 and the
     *        colum "Msg" in order to get a positive return value.
     *        If the record conatins also a "TimeUS"
     *        field the internal time stamp will be filled too.
     *
     * @param record[in] - Filled QSqlRecord
     * @return true - all Fields could be read
     *         false - not all data could be read
     */
    virtual bool setFromSqlRecord(const QSqlRecord &record);

    /**
     * @brief Converts the MsgMessage into an uninterpreted string.
     *        In this case there is nothing to interpret. The internal
     *        string is directly returned.
     * @return The uninterpreted Qstring
     */
    virtual QString toString() const;

private:

    QString m_Message; /// The 'message'

};

/**
 * @brief The MessageFactory class should be used to construct
 *        messages of every type by name.
 */
class MessageFactory
{
public:
    static MessageBase::Ptr getMessageOfType(const QString &type);
};

/**
 *  Namespace for all copter related stuff
 */
namespace Copter
{

/**
 * @brief The Mode enum holds all possible flying modes
 *        of a copter
 */
enum Mode
{
    STABILIZE   = 0,
    ACRO        = 1,
    ALT_HOLD    = 2,
    AUTO        = 3,
    GUIDED      = 4,
    LOITER      = 5,
    RTL         = 6,
    CIRCLE      = 7,
    RESERVED_8  = 8,
    LAND        = 9,
    OF_LOITER   = 10,
    DRIFT       = 11,
    RESERVED_12 = 12,
    SPORT       = 13,
    FLIP        = 14,
    AUTOTUNE    = 15,
    POS_HOLD    = 16,
    BRAKE       = 17,
    LAST_MODE           // This must always be the last entry
};

/**
 * @brief Helper class for creating an interpreted output of
 *        all messages generated by copter logs
 */
class MessageFormatter
{
public:
    static QString format(MessageBase::Ptr &p_message);

    static QString format(const ErrorMessage &message);

    static QString format(const ModeMessage &message);

    static QString format(const EventMessage &message);
};

} // namespace Copter

/**
 *  Namespace for all plane related stuff
 */
namespace Plane
{

/**
 * @brief The Mode enum holds all possible flying modes
 *        of a plane
 */
enum Mode
{
    MANUAL        = 0,
    CIRCLE        = 1,
    STABILIZE     = 2,
    TRAINING      = 3,
    ACRO          = 4,
    FLY_BY_WIRE_A = 5,
    FLY_BY_WIRE_B = 6,
    CRUISE        = 7,
    AUTOTUNE      = 8,
    LAND          = 9,
    AUTO          = 10,
    RTL           = 11,
    LOITER        = 12,
    RESERVED_13   = 13, // RESERVED FOR FUTURE USE
    RESERVED_14   = 14, // RESERVED FOR FUTURE USE
    GUIDED        = 15,
    INITIALIZING  = 16,
    QSTABILIZE    = 17,
    QHOVER        = 18,
    QLOITER       = 19,
    LAST_MODE           // This must always be the last entry
};

/**
 * @brief Helper class for creating an interpreted output of
 *        all messages generated by Plane logs
 */
class MessageFormatter
{
public:
    static QString format(MessageBase::Ptr &p_message);

    static QString format(const ModeMessage &message);
};

} // namespace Plane

/**
 *  Namespace for all rover related stuff
 */
namespace Rover
{

/**
 * @brief The Mode enum holds all possible driving Modes
 *        of a rover
 */
enum Mode
{
    MANUAL        = 0,
    RESERVED_1    = 1, // RESERVED FOR FUTURE USE
    LEARNING      = 2,
    STEERING      = 3,
    HOLD          = 4,
    RESERVED_5    = 5, // RESERVED FOR FUTURE USE
    RESERVED_6    = 6, // RESERVED FOR FUTURE USE
    RESERVED_7    = 7, // RESERVED FOR FUTURE USE
    RESERVED_8    = 8, // RESERVED FOR FUTURE USE
    RESERVED_9    = 9, // RESERVED FOR FUTURE USE
    AUTO          = 10,
    RTL           = 11,
    RESERVED_12   = 12, // RESERVED FOR FUTURE USE
    RESERVED_13   = 13, // RESERVED FOR FUTURE USE
    RESERVED_14   = 14, // RESERVED FOR FUTURE USE
    GUIDED        = 15,
    INITIALIZING  = 16,
    LAST_MODE           // This must always be the last entry
};


/**
 * @brief Helper class for creating an interpreted output of
 *        all messages generated by Rover logs
 */
class MessageFormatter
{
public:
    static QString format(MessageBase::Ptr &p_message);

    static QString format(const ModeMessage &message);
};

} // Namespace Rover

#endif // ARDUPILOTMAV_H
