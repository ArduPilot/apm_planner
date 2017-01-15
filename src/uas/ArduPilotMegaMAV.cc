
/*=====================================================================
QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
(c) 2013 APMPLANNER PROJECT <http://www.ardupliot.com>
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
 *   @brief Implementation of class ArduPilotMegaMAV Uas Object
 *   @author Lorenz Meier
 *   @author Bill Bonney
 */

#include "ArduPilotMegaMAV.h"
#include "logging.h"
#include "GAudioOutput.h"
#include "LinkManager.h"

#ifndef MAVLINK_MSG_ID_MOUNT_CONFIGURE
#include "ardupilotmega/mavlink_msg_mount_configure.h"
#endif

#ifndef MAVLINK_MSG_ID_MOUNT_CONTROL
#include "ardupilotmega/mavlink_msg_mount_control.h"
#endif

#include <QString>
#include <QDir>
#include <QDesktopServices>
#include <QSettings>
#include <QSqlRecord>


ArduPilotMegaMAV::ArduPilotMegaMAV(MAVLinkProtocol* mavlink, int id) :
    UAS(mavlink, id),
    m_severityCompatibilityMode(false)
{
    //This does not seem to work. Manually request each stream type at a specified rate.
    // Ask for all streams at 4 Hz
    //enableAllDataTransmission(4);
    txReqTimer = new QTimer(this);
    connect(txReqTimer,SIGNAL(timeout()),this,SLOT(RequestAllDataStreams()));

    QTimer::singleShot(5000,this,SLOT(RequestAllDataStreams())); //Send an initial TX request in 5 seconds.

    txReqTimer->start(10000); //Resend the TX requests every 10 seconds.

    connect(this,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(this,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));

    connect(this, SIGNAL(textMessageReceived(int,int,int,QString)),
            this, SLOT(textMessageReceived(int,int,int,QString)));

    connect(this, SIGNAL(heartbeatTimeout(bool,uint)),
            this, SLOT(heartbeatTimeout(bool,uint)));

    loadSettings();
}

void ArduPilotMegaMAV::loadSettings()
{
    QSettings settings;
    settings.beginGroup("ARDUPILOT");
//    m_severityCompatibilityMode = settings.value("STATUSTEXT_COMPAT_MODE",false).toBool();
    settings.endGroup();
}

void ArduPilotMegaMAV::saveSettings()
{
    QSettings settings;
    settings.beginGroup("ARDUPILOT");
//    settings.setValue("STATUSTEXT_COMPAT_MODE",m_severityCompatibilityMode);
    settings.endGroup();
    settings.sync();
}

void ArduPilotMegaMAV::RequestAllDataStreams()
{
    QLOG_TRACE() << "APM:RequestAllDataRates";
    QSettings settings;
    settings.sync();
    settings.beginGroup("DATA_RATES");
    enableExtendedSystemStatusTransmission(settings.value("EXT_SYS_STATUS",2).toInt());

    enablePositionTransmission(settings.value("POSITION",3).toInt());

    enableExtra1Transmission(settings.value("EXTRA1",10).toInt());

    enableExtra2Transmission(settings.value("EXTRA2",10).toInt());

    enableExtra3Transmission(settings.value("EXTRA3",2).toInt());

    enableRawSensorDataTransmission(settings.value("RAW_SENSOR_DATA",2).toInt());

    enableRCChannelDataTransmission(settings.value("RC_CHANNEL_DATA",2).toInt());
    settings.endGroup();
}

void ArduPilotMegaMAV::uasConnected()
{
    QLOG_INFO() << "ArduPilotMegaMAV APM Connected";
    QTimer::singleShot(500,this,SLOT(RequestAllDataStreams())); //Send an initial TX request in 0.5 seconds.
    createNewMAVLinkLog(type);
    LinkManager::instance()->startLogging();
}

void ArduPilotMegaMAV::uasDisconnected()
{
    QLOG_INFO() << "ArduPilotMegaMAV APM disconnected";
    //if (mavlink)
   // {
   //     mavlink->stopLogging();
   // }
}

void ArduPilotMegaMAV::createNewMAVLinkLog(uint8_t type)
{
    QString subDir;

    // This creates a log in subdir based on the vehicle
    // first detected. When connecting to multiple vehicles
    // it will not log message based on each specific.
    switch(type) {
    case MAV_TYPE_TRICOPTER:
        subDir = "/tricopter/";
        break;

    case MAV_TYPE_QUADROTOR:
        subDir = "/quadcopter/";
        break;

    case MAV_TYPE_HEXAROTOR:
        subDir = "/hexcopter/";
        break;

    case MAV_TYPE_OCTOROTOR:
        subDir = "/octocopter/";
        break;

    case MAV_TYPE_GROUND_ROVER:
        subDir = "/rover/";
        break;

    case MAV_TYPE_HELICOPTER:
        subDir = "/helicopter/";
        break;

    case MAV_TYPE_FIXED_WING:
        subDir = "/plane/";
        break;

    default:
        subDir = "/";
    }
    LinkManager::instance()->setLogSubDirectory(subDir);
}

/**
 * This function is called by MAVLink once a complete, uncorrupted (CRC check valid)
 * mavlink packet is received.
 *
 * @param link Hardware link the message came from (e.g. /dev/ttyUSB0 or UDP port).
 *             messages can be sent back to the system via this link
 * @param message MAVLink message, as received from the MAVLink protocol stack
 */
void ArduPilotMegaMAV::receiveMessage(LinkInterface* link, mavlink_message_t message)
{
    // Let UAS handle the default message set
    //qDebug() << "Message type:" << message.sysid << message.msgid;

    if (message.sysid == uasId) {
        // Handle your special messages
        switch (message.msgid) {
        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            //QLOG_DEBUG() << "ARDUPILOT RECEIVED HEARTBEAT";
            break;
        }
        case MAVLINK_MSG_ID_STATUSTEXT:
        {
            QByteArray b;
            b.resize(MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN+1);
            mavlink_msg_statustext_get_text(&message, b.data());
            // Ensure NUL-termination
            b[b.length()-1] = '\0';
            QString messageText = QString(b);
            int severity = mavlink_msg_statustext_get_severity(&message);
            QLOG_INFO() << "STATUS TEXT:" << severity << ":" << messageText;

            if (!messageText.contains(APM_SOLO_REXP)) {
                if (messageText.contains(APM_COPTER_REXP) || messageText.contains(APM_PLANE_REXP)
                        || messageText.contains(APM_ROVER_REXP)) {
                    QLOG_DEBUG() << "APM Version String detected:" << messageText;
                    m_firmwareVersion.parseVersion(messageText);
                    // Process Version and keep.
                    m_severityCompatibilityMode = _isTextSeverityAdjustmentNeeded(m_firmwareVersion);

                    emit versionDetected(messageText);
                }
            }

            // Check is older APM and reset severity to correct MAVLINK spec.
            if (m_severityCompatibilityMode) {
                adjustSeverity(&message);
            }

        } break;
        default:
            //QLOG_DEBUG() << "\nARDUPILOT RECEIVED MESSAGE WITH ID" << message.msgid;
            break;
        }
    }

    // default Bea
    UAS::receiveMessage(link, message);
}

void ArduPilotMegaMAV::adjustSeverity(mavlink_message_t* message) const
{
    // lets make QGC happy with right severity values
    mavlink_statustext_t statusText;
    mavlink_msg_statustext_decode(message, &statusText);

    // Older APM detected, translate severity to MAVLink Standard severity
    // SEVERITY_LOW     =1 MAV_SEVERITY_WARNING = 4
    // SEVERITY_MEDIUM  =2 MAV_SEVERITY_ALERT   = 1
    // SEVERITY_HIGH    =3 MAV_SEVERITY_CRITICAL= 2
    // SEVERITY_USER_RESPONSE =5 MAV_SEVERITY_CRITICAL= 2

    switch(statusText.severity) {
    case 1:     /* gcs_severity::SEVERITY_LOW according to old codes */
        statusText.severity = MAV_SEVERITY_WARNING;
        break;
    case 2:     /* gcs_severity::SEVERITY_MEDIUM according to old codes  */
        statusText.severity = MAV_SEVERITY_ALERT;
        break;
    case 3:     /* gcs_severity::SEVERITY_HIGH  according to old codes */
        statusText.severity = MAV_SEVERITY_CRITICAL;
        break;
    case 5: /*gcs_severity::SEVERITY_USER_RESPONSE according to old codes*/
        statusText.severity = MAV_SEVERITY_CRITICAL;
        break;
    default:
        statusText.severity = MAV_SEVERITY_INFO;
    }

    mavlink_msg_statustext_encode(message->sysid, message->compid, message, &statusText);
}

bool ArduPilotMegaMAV::_isTextSeverityAdjustmentNeeded(const APMFirmwareVersion& firmwareVersion)
{
    if (!firmwareVersion.isValid()) {
        return false;
    }

    bool adjustmentNeeded = false;
    if (firmwareVersion.vehicleType().contains(APM_COPTER_REXP)) {
        if (firmwareVersion < APMFirmwareVersion(MIN_COPTER_VERSION_WITH_CORRECT_SEVERITY_MSGS)) {
            adjustmentNeeded = true;
        }
    } else if (firmwareVersion.vehicleType().contains(APM_PLANE_REXP)) {
        if (firmwareVersion < APMFirmwareVersion(MIN_PLANE_VERSION_WITH_CORRECT_SEVERITY_MSGS)) {
            adjustmentNeeded = true;
        }
    } else if (firmwareVersion.vehicleType().contains(APM_ROVER_REXP)) {
        if (firmwareVersion < APMFirmwareVersion(MIN_ROVER_VERSION_WITH_CORRECT_SEVERITY_MSGS)) {
            adjustmentNeeded = true;
        }
    } else if (firmwareVersion.vehicleType().contains(APM_SUB_REXP)) {
        if (firmwareVersion < APMFirmwareVersion(MIN_SUB_VERSION_WITH_CORRECT_SEVERITY_MSGS)) {
            adjustmentNeeded = true;
        }
    }

    return adjustmentNeeded;
}

void ArduPilotMegaMAV::setMountConfigure(unsigned char mode, bool stabilize_roll,bool stabilize_pitch,bool stabilize_yaw)
{
    //Only supported by APM
    mavlink_message_t msg;
    mavlink_msg_mount_configure_pack(255,1,&msg,this->uasId,1,mode,stabilize_roll,stabilize_pitch,stabilize_yaw);
    sendMessage(msg);
}
void ArduPilotMegaMAV::setMountControl(double pa,double pb,double pc,bool islatlong)
{
    mavlink_message_t msg;
    if (islatlong)
    {
        mavlink_msg_mount_control_pack(255,1,&msg,this->uasId,1,pa*10000000.0,pb*10000000.0,pc*10000000.0,0);
    }
    else
    {
        mavlink_msg_mount_control_pack(255,1,&msg,this->uasId,1,pa,pb,pc,0);
    }
    sendMessage(msg);
}

void ArduPilotMegaMAV::armSystem()
{
    QLOG_INFO() << "APM ARM System";
    mavlink_message_t msg;

    mavlink_msg_command_long_pack(getSystemId(),
                                  getComponentId(),
                                  &msg,
                                  getUASID(),                    // uint8_t target_system,
                                  MAV_COMP_ID_PRIMARY,    // uint8_t target_component
                                  MAV_CMD_COMPONENT_ARM_DISARM,    // uint16_t command,
                                  1,                // uint8_t confirmation,
                                  1.0,             // float param1,
                                  0,                // float param2,
                                  0,                // float param3,
                                  0,                // float param4,
                                  0,                // float param5,
                                  0,                // float param6,
                                  0                 // float param7)
                                  );
    sendMessage(msg);
}

void ArduPilotMegaMAV::disarmSystem()
{
    QLOG_INFO() << "APM DISARM System";
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(getSystemId(),
                                  getComponentId(),
                                  &msg,
                                  getUASID(),                    // uint8_t target_system,
                                  MAV_COMP_ID_PRIMARY,    // uint8_t target_component
                                  MAV_CMD_COMPONENT_ARM_DISARM,    // uint16_t command,
                                  1,                // uint8_t confirmation,
                                  0.0,             // float param1,
                                  0,                // float param2,
                                  0,                // float param3,
                                  0,                // float param4,
                                  0,                // float param5,
                                  0,                // float param6,
                                  0                 // float param7)
                                  );
    sendMessage(msg);
}

QString ArduPilotMegaMAV::getCustomModeText()
{
    QLOG_DEBUG() << "APM: getCustomModeText()";
    QString customModeString;
    ModeMessage mode(0, 0, custom_mode, 0, 0);

    if (isFixedWing()){
        customModeString = Plane::MessageFormatter::format(mode);

    } else if (isMultirotor()){
        customModeString = Copter::MessageFormatter::format(mode);

    } else if (isGroundRover()){
        customModeString = Rover::MessageFormatter::format(mode);

    } else if (getSystemType() == MAV_TYPE_ANTENNA_TRACKER ){
        customModeString = tr("Ant Tracker");

    } else {
        QLOG_WARN() << "APM: Unsupported System Type " << getSystemType();
            customModeString = tr("APM UNKOWN");
    }
    return customModeString;
}

QString ArduPilotMegaMAV::getCustomModeAudioText()
{
    QLOG_DEBUG() << "APM: getCustomModeAudioText()";

    QString returnString = tr("and mode is ");
    return returnString + getCustomModeText();
}

void ArduPilotMegaMAV::textMessageReceived(int /*uasid*/, int /*componentid*/, int severity, QString text)
{
    QLOG_DEBUG() << "APM: Text Message rx'd" << text;
    if (text.startsWith("PreArm:")) {
        // Speak the PreArm warning
        QString audioString = "Pre-arm check:" + text.remove("PreArm:");
        GAudioOutput::instance()->say(audioString, severity);
    } else if (text.startsWith("Arm:")){
        QString audioString = "Arm check:" + text.remove("Arm:");
        GAudioOutput::instance()->say(audioString, severity);
    }
}

void ArduPilotMegaMAV::heartbeatTimeout(bool timeout, unsigned int /*ms*/)
{
    if(timeout == false) {
        QLOG_DEBUG() << "Send requestall data streams when heartbeat restarts";
        // Request data, as this means we have reconnected
        // Send an request in .5 seconds.
        QTimer::singleShot(500,this,SLOT(RequestAllDataStreams()));
    }
}

void ArduPilotMegaMAV::playCustomModeChangedAudioMessage()
{
    QString phrase;

    phrase = "Mode changed to " + getCustomModeText() + " for system " + QString::number(getUASID());
    QLOG_DEBUG() << "APM say:" << phrase;
    GAudioOutput::instance()->say(phrase.toLower());
}

void ArduPilotMegaMAV::playArmStateChangedAudioMessage(bool armedState)
{
    QString armedPhrase("disarmed");

    if (armedState){
        armedPhrase = "armed";
    }

    QLOG_DEBUG() << "APM say:" << armedPhrase;
    GAudioOutput::instance()->say(QString("system %1 is %2").arg(QString::number(getUASID()),armedPhrase));
}


//******************* Classes for Sepcial message handling **************************

MessageBase::MessageBase() : m_Index(0), m_TimeStamp(0)
{}

MessageBase::MessageBase(const quint32 index, const double timeStamp, const QString &name, const QColor &color) :
    m_Index(index),
    m_TimeStamp(timeStamp),
    m_TypeName(name),
    m_Color(color)
{}

quint32 MessageBase::getIndex() const
{
    return m_Index;
}

double MessageBase::getTimeStamp() const
{
    return m_TimeStamp;
}

QString MessageBase::typeName() const
{
    return m_TypeName;
}

QColor MessageBase::typeColor() const
{
    return m_Color;
}

//********

const QString ErrorMessage::TypeName("ERR");

ErrorMessage::ErrorMessage() : m_SubSys(0), m_ErrorCode(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(150,0,0);
}

ErrorMessage::ErrorMessage(const QString &TimeFieldName) : m_SubSys(0), m_ErrorCode(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(150,0,0);
    m_TimeFieldName = TimeFieldName;
}

ErrorMessage::ErrorMessage(const quint32 index, const double timeStamp, const quint32 subSys, const quint32 errCode) :
    MessageBase(index, timeStamp, TypeName, QColor(150,0,0)),
    m_SubSys(subSys),
    m_ErrorCode(errCode)
{}

quint32 ErrorMessage::getSubsystemCode() const
{
    return m_SubSys;
}

quint32 ErrorMessage::getErrorCode() const
{
    return m_ErrorCode;
}

bool ErrorMessage::setFromNameValuePairList(const QList<NameValuePair> &values, const double timeDivider)
{
    bool rc1 = false;
    bool rc2 = false;
    bool rc3 = false;
    bool rc4 = false;

    for(int i = 0; i < values.size(); ++i)
    {
        if(values.at(i).first == "Index")
        {
            m_Index = values.at(i).second.toUInt();
            rc1 = true;
        }
        else if(values.at(i).first == m_TimeFieldName)
        {
            m_TimeStamp = values.at(i).second.toDouble();
            m_TimeStamp /= timeDivider;
            rc2 = true;
        }
        else if(values.at(i).first == "ECode")
        {
            m_ErrorCode = values.at(i).second.toUInt();
            rc3 = true;
        }
        else if(values.at(i).first == "Subsys")
        {
            m_SubSys = values.at(i).second.toUInt();
            rc4 = true;
        }
    }
    return rc1 && rc2 && rc3 && rc4;
}

QString ErrorMessage::toString() const
{
    QString output;
    QTextStream outputStream(&output);

    outputStream << " Subsystem:" << m_SubSys << " Errorcode:" << m_ErrorCode;
    return output;
}

//********

const QString ModeMessage::TypeName("MODE");

ModeMessage::ModeMessage() : m_Mode(0), m_ModeNum(0), m_Reason(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(50,125,0);
}

ModeMessage::ModeMessage(const QString &TimeFieldName) : m_Mode(0), m_ModeNum(0), m_Reason(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(50,125,0);
    m_TimeFieldName = TimeFieldName;
}

ModeMessage::ModeMessage(const quint32 index, const double timeStamp, const quint32 mode, const quint32 modeNum, const quint32 reason) :
    MessageBase(index, timeStamp, TypeName, QColor(50,125,0)),
    m_Mode(mode),
    m_ModeNum(modeNum),
    m_Reason(reason)
{}

quint32 ModeMessage::getMode() const
{
    return m_Mode;
}

quint32 ModeMessage::getModeNum() const
{
    return m_ModeNum;
}

quint32 ModeMessage::getReason() const
{
    return m_Reason;
}

bool ModeMessage::setFromNameValuePairList(const QList<NameValuePair> &values, const double timeDivider)
{
    bool rc1 = false;
    bool rc2 = false;
    bool rc3 = false;

    for(int i = 0; i < values.size(); ++i)
    {
        if(values.at(i).first == "Index")
        {
            m_Index = values.at(i).second.toUInt();
            rc2 = true;
        }
        else if(values.at(i).first == m_TimeFieldName)
        {
            m_TimeStamp = values.at(i).second.toDouble();
            m_TimeStamp /= timeDivider;
            rc3 = true;
        }
        else if(values.at(i).first == "Mode")
        {
            m_Mode = values.at(i).second.toUInt();
            rc1 = true;
        }
        else if(values.at(i).first == "ModeNum")
        {
            m_ModeNum = values.at(i).second.toUInt();
            // ModeNum does not influence the returncode as its optional
        }
        else if(values.at(i).first == "Rsn")
        {
            m_Reason = values.at(i).second.toUInt();
            // Reason does not influence the returncode as its optional. Came with AC 3.4
        }
    }
    return rc1 && rc2 && rc3;
}


QString ModeMessage::toString() const
{
    QString output;
    QTextStream outputStream(&output);

    outputStream << " Mode:" << m_Mode << " ModeNum:" << m_ModeNum << " Reason:" << m_Reason;
    return output;
}

//********

const QString EventMessage::TypeName("EV");

EventMessage::EventMessage() : m_EventID(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(0,0,125);
}

EventMessage::EventMessage(const QString &TimeFieldName) : m_EventID(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(0,0,125);
    m_TimeFieldName = TimeFieldName;
}

EventMessage::EventMessage(const quint32 index, const double timeStamp, const quint32 eventID) :
    MessageBase(index, timeStamp, TypeName, QColor(0,0,125)),
    m_EventID(eventID)
{}

quint32 EventMessage::getEventID() const
{
    return m_EventID;
}

bool EventMessage::setFromNameValuePairList(const QList<NameValuePair> &values, const double timeDivider)
{
    bool rc1 = false;
    bool rc2 = false;
    bool rc3 = false;

    for(int i = 0; i < values.size(); ++i)
    {
        if(values.at(i).first == "Index")
        {
            m_Index = values.at(i).second.toUInt();
            rc1 = true;
        }
        else if(values.at(i).first == m_TimeFieldName)
        {
            m_TimeStamp = values.at(i).second.toUInt();
            m_TimeStamp /= timeDivider;
            rc2 = true;
        }
        else if(values.at(i).first == "Id")
        {
            m_EventID = values.at(i).second.toUInt();
            rc3 = true;
        }
    }
    return rc1 && rc2 && rc3;
}

QString EventMessage::toString() const
{
    QString output;
    QTextStream outputStream(&output);

    outputStream << " Event ID:" << m_EventID;
    return output;
}

//********

const QString MsgMessage::TypeName("MSG");

MsgMessage::MsgMessage()
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(0,0,0);
}

MsgMessage::MsgMessage(const QString &TimeFieldName)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(0,0,0);
    m_TimeFieldName = TimeFieldName;
}

MsgMessage::MsgMessage(const quint32 index, const double timeStamp, const QString &message) :
    MessageBase(index, timeStamp, TypeName, QColor(0,0,0)),
    m_Message(message)
{}

bool MsgMessage::setFromNameValuePairList(const QList<NameValuePair> &values, const double timeDivider)
{
    bool rc1 = false;
    bool rc2 = false;
    bool rc3 = false;

    for(int i = 0; i < values.size(); ++i)
    {
        if(values.at(i).first == "Index")
        {
            m_Index = values.at(i).second.toUInt();
            rc1 = true;
        }
        else if(values.at(i).first == m_TimeFieldName)
        {
            m_TimeStamp = values.at(i).second.toUInt();
            m_TimeStamp /= timeDivider;
            rc2 = true;
        }
        else if(values.at(i).first == "Message")
        {
            m_Message = values.at(i).second.toString();
            rc3 = true;
        }
    }
    return rc1 && rc2 && rc3;
}

QString MsgMessage::toString() const
{
    return m_Message;
}

//********

MessageBase::Ptr MessageFactory::CreateMessageOfType(const QString &type, const QList<QPair<QString, QVariant> > &values, const QString &timeFieldName, const double &timeDivider)
{
    MessageBase::Ptr messagePtr;

    if (type == ErrorMessage::TypeName)
    {
        messagePtr = MessageBase::Ptr(new ErrorMessage(timeFieldName));
    }
    else if (type == ModeMessage::TypeName)
    {
        messagePtr = MessageBase::Ptr(new ModeMessage(timeFieldName));
    }
    else if (type == EventMessage::TypeName)
    {
        messagePtr = MessageBase::Ptr(new EventMessage(timeFieldName));
    }
    else if (type == MsgMessage::TypeName)
    {
        messagePtr = MessageBase::Ptr(new MsgMessage(timeFieldName));
    }

    if(!messagePtr)
    {
        QLOG_WARN() << "MessageFactory::CreateMessageOfType: No message of type '" << type << "' could be created";
        return MessageBase::Ptr();
    }
    else
    {
        if(!messagePtr->setFromNameValuePairList(values, timeDivider))
        {
            QLOG_WARN() << "MessageFactory::CreateMessageOfType: Not all data could be read from variant list "
                        << "The data of type " << type << " might be corrupted.";
        }
        return messagePtr;
    }
}


//******** Message Formatters ********

QString Copter::MessageFormatter::format(MessageBase::Ptr &p_message)
{
    QString retval("Unknown Type");
    if (p_message)
    {
        if (p_message->typeName() == ErrorMessage::TypeName)
        {
            // can use the internal pointer here avoiding dynamic pointer cast with refcount increase
            retval = format(*dynamic_cast<ErrorMessage*>(p_message.data()));
        }
        else if (p_message->typeName() == ModeMessage::TypeName)
        {
            retval = format(*dynamic_cast<ModeMessage*>(p_message.data()));
        }
        else if (p_message->typeName() == EventMessage::TypeName)
        {
            retval = format(*dynamic_cast<EventMessage*>(p_message.data()));
        }
        else // The msgMessage does not need a formatter -> handled by else
        {
            retval = p_message->toString();
        }
    }
    else
    {
        QLOG_ERROR() << "CopterMessageFormatter::format() called with nullpointer";
    }
    return retval;
}

QString Copter::MessageFormatter::format(const ErrorMessage &message)
{
    // SubSys ans ErrorCode interpretation was taken from
    // Ardupilot/ArduCopter/defines.h
    // last verification 17.01.2017

    QString output;
    QTextStream outputStream(&output);

    bool EcodeUsed = false;

    switch (message.getSubsystemCode())
    {
    case 1:
        outputStream << "Main:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Ins-Delay";
            EcodeUsed = true;
        }
        break;

    case 2:
        outputStream << "Radio:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Late Frame detected";
            EcodeUsed = true;
        }
        break;

    case 3:
        outputStream << "Compass:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Failed to read data";
            EcodeUsed = true;
        }
        break;

    case 4:
        outputStream << "OptFlow:";
        break;

    case 5:
        outputStream << "FS-Radio:";
        break;

    case 6:
        outputStream << "FS-Batt:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 7:
        outputStream << "FS-GPS:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 8:
        outputStream << "FS-GCS:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 9:
        outputStream << "FS-Fence:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 10:
    {
        ModeMessage tmpMsg(0, 0, message.getErrorCode(), 0, 0);
        outputStream << "Flight-Mode "  << Copter::MessageFormatter::format(tmpMsg) <<" refused.";
        EcodeUsed = true;
        break;
    }

    case 11:
        outputStream << "GPS:";
        break;

    case 12:
        outputStream << "Crash-Check:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Crash Detected";
            EcodeUsed = true;
        }
        else if (message.getErrorCode() == 2)
        {
            outputStream << "Control Lost";
            EcodeUsed = true;
        }
        break;

    case 13:
        outputStream << "FLIP:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Abandoned";
            EcodeUsed = true;
        }
        break;

    case 14:
        outputStream << "Autotune:";
        break;

    case 15:
        outputStream << "Parachute:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Too low to eject";
            EcodeUsed = true;
        }
        else if (message.getErrorCode() == 3)
        {
            outputStream << "Copter Landed";
            EcodeUsed = true;
        }
        break;

    case 16:
        outputStream << "EKF-Check:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Bad Variance detected";
            EcodeUsed = true;
        }
        break;

    case 17:
        outputStream << "FS-EKF-INAV:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 18:
        outputStream << "Baro:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Glitch detected";
            EcodeUsed = true;
        }
        break;

    case 19:
        outputStream << "CPU:";
        break;

    case 20:
        outputStream << "FS-ADSB:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 21:
        outputStream << "Terrain:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Missing Terrain Data";
            EcodeUsed = true;
        }
        break;

    case 22:
        outputStream << "Navigation:";
        if (message.getErrorCode() == 2)
        {
            outputStream << "Failed to set destination";
            EcodeUsed = true;
        }
        else if (message.getErrorCode() == 3)
        {
            outputStream << "Restarted RTL";
            EcodeUsed = true;
        }
        else if (message.getErrorCode() == 4)
        {
            outputStream << "Failed Circle init";
            EcodeUsed = true;
        }
        else if (message.getErrorCode() == 5)
        {
            outputStream << "Destination outside fence";
            EcodeUsed = true;
        }
        break;

    case 23:
        outputStream << "FS-Terrain:";
        if (message.getErrorCode() == 1)
        {
            outputStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 24:
        outputStream << "EKF primary:";
        break;

    default:
        outputStream << "SubSys:" << message.getSubsystemCode() << " ECode:" << message.getErrorCode();
        EcodeUsed = true;
        break;

    }

    if (!EcodeUsed)
    {
        switch (message.getErrorCode())
        {
        case 0:
            outputStream << "Everything OK!";
            break;

        case 1:
            outputStream << "Failed to init";
            break;

        case 4:
            outputStream << "Is Unhealthy";
            break;

        default:
            outputStream << "Unknown ErrorCode(" << message.getErrorCode() << ")";
            break;
        }
    }

    return output;
}

QString Copter::MessageFormatter::format(const ModeMessage &message)
{
    // Interpretation taken from
    // Ardupilot/ArduCopter/defines.h
    // last verification 17.01.2017

    QString output;
    QTextStream outputStream(&output);

    switch (message.getMode())
    {
    case Copter::STABILIZE:
        outputStream << "Stabilize";
        break;
    case Copter::ACRO:
        outputStream << "Acro";
        break;
    case Copter::ALT_HOLD:
        outputStream << "Alt Hold";
        break;
    case Copter::AUTO:
        outputStream << "Auto";
        break;
    case Copter::GUIDED:
        outputStream << "Guided";
        break;
    case Copter::LOITER:
        outputStream << "Loiter";
        break;
    case Copter::RTL:
        outputStream << "RTL";
        break;
    case Copter::CIRCLE:
        outputStream << "Circle";
        break;
    case Copter::LAND:
        outputStream << "Land";
        break;
    case Copter::DRIFT:
        outputStream << "Drift";
        break;
    case Copter::SPORT:
        outputStream << "Sport";
        break;
    case Copter::FLIP:
        outputStream << "Flip";
        break;
    case Copter::AUTOTUNE:
        outputStream << "Auto Tune";
        break;
    case Copter::POS_HOLD:
        outputStream << "Pos Hold";
        break;
    case Copter::BRAKE:
        outputStream << "Brake";
        break;
    case Copter::THROW:
        outputStream << "Throw";
        break;
    case Copter::AVOID_ADSB:
        outputStream << "Avoid-ADSB";
        break;
    case Copter::GUIDED_NOGPS:
        outputStream << "Guided no GPS";
        break;
    default:
        outputStream << "Unknown Mode:" << message.getMode();
        break;
    }

    // only if we have a valid reason
    if (message.getReason() != 0)
    {
        outputStream << endl << "by " ;

        switch (message.getReason())
        {
        case 1:
            outputStream << "radio";
            break;
        case 2:
            outputStream << "GCS cmd";
            break;
        case 3:
            outputStream << "radio FS";
            break;
        case 4:
            outputStream << "battery FS";
            break;
        case 5:
            outputStream << "GCS FS";
            break;
        case 6:
            outputStream << "EKF FS";
            break;
        case 7:
            outputStream << "GPS Glitch";
            break;
        case 8:
            outputStream << "mission end";
            break;
        case 9:
            outputStream << "throttle land escape";
            break;
        case 10:
            outputStream << "fence breach";
            break;
        case 11:
            outputStream << "terrain FS";
            break;
        case 12:
            outputStream << "brake timeout";
            break;
        case 13:
            outputStream << "Flip complete";
            break;
        case 14:
            outputStream << "Avoidance";
            break;
        case 15:
            outputStream << "Avoidance recovery";
            break;
        case 16:
            outputStream << "Throw complete";
            break;
        default:
            outputStream << "unknown reason:" << message.getReason();
            break;
        }
    }

    return output;
}

QString Copter::MessageFormatter::format(const EventMessage &message)
{
    // Interpretation taken from
    // Ardupilot/ArduCopter/defines.h
    // last verification 17.01.2017

    QString output;
    QTextStream outputStream(&output);

    switch(message.getEventID())
    {
    case 7:
        outputStream << "AP-State";
        break;
    case 8:
        outputStream << "System time set";
        break;
    case 9:
        outputStream << "Init simple bearing";
        break;
    case 10:
        outputStream << "Armed";
        break;
    case 11:
        outputStream << "Disarmed";
        break;
    case 15:
        outputStream << "Auto-Armed";
        break;
    case 16:
        outputStream << "Takeoff";
        break;
    case 17:
        outputStream << "Land Complete Maybe";
        break;
    case 18:
        outputStream << "Land Complete";
        break;
    case 19:
        outputStream << "Lost GPS";
        break;
    case 21:
        outputStream << "Flip Start";
        break;
    case 22:
        outputStream << "Flip End";
        break;
    case 25:
        outputStream << "Home Set";
        break;
    case 26:
        outputStream << "Simple Mode ON";
        break;
    case 27:
        outputStream << "Simple Mode OFF";
        break;
    case 28:
        outputStream << "Not Landed";
        break;
    case 29:
        outputStream << "SuperSimple Mode ON";
        break;
    case 30:
        outputStream << "Autotune Initialized";
        break;
    case 31:
        outputStream << "Autotune Off";
        break;
    case 32:
        outputStream << "Autotune Restart";
        break;
    case 33:
        outputStream << "Autotune Success";
        break;
    case 34:
        outputStream << "Autotune Failed";
        break;
    case 35:
        outputStream << "Autotune Reached Limit";
        break;
    case 36:
        outputStream << "Autotune Pilot Testing";
        break;
    case 37:
        outputStream << "Autotune Saved Gains";
        break;
    case 38:
        outputStream << "Save Trim";
        break;
    case 39:
        outputStream << "Save/Add WP";
        break;
    case 40:
        outputStream << "WP Clear Mission RTL";
        break;
    case 41:
        outputStream << "Fence enable";
        break;
    case 42:
        outputStream << "Fence disable";
        break;
    case 43:
        outputStream << "Acro Trainer disabled";
        break;
    case 44:
        outputStream << "Acro Trainer leveling";
        break;
    case 45:
        outputStream << "Acro Trainer limited";
        break;
    case 46:
        outputStream << "EPM grab";
        break;
    case 47:
        outputStream << "EPM realease";
        break;
    case 48:
        outputStream << "EPM neutral";      // Deprecated
        break;
    case 49:
        outputStream << "Parachute disabled";
        break;
    case 50:
        outputStream << "Parachute enabled";
        break;
    case 51:
        outputStream << "Parachute released";
        break;
    case 52:
        outputStream << "Landing gear delpoyed";
        break;
    case 53:
        outputStream << "Landing gear retracted";
        break;
    case 54:
        outputStream << "Motor emergency stop";
        break;
    case 55:
        outputStream << "Motor emergency stop clear";
        break;
    case 56:
        outputStream << "Motor interlock disabled";
        break;
    case 57:
        outputStream << "Motor interlock enabled";
        break;
    case 58:
        outputStream << "Motor runup complete";         // heli only
        break;
    case 59:
        outputStream << "Motor speed below critical";   // heli only
        break;
    case 60:
        outputStream << "EKF alt reset";
        break;
    case 61:
        outputStream << "Land cancelled by pilot";
        break;
    case 62:
        outputStream << "EKF yaw reset";
        break;
    case 63:
        outputStream << "Avoidance ADSB enable";
        break;
    case 64:
        outputStream << "Avoidance ADSB disable";
        break;
    case 65:
        outputStream << "Avoidance proximity enable";
        break;
    case 66:
        outputStream << "Avoidance proximity disable";
        break;
    default:
        outputStream << "Unknown Event: " << message.getEventID();
        break;
    }

    return output;
}


QString Plane::MessageFormatter::format(MessageBase::Ptr &p_message)
{
    QString retval("Unknown Type");
    if (p_message)
    {
        // Only mode message formatter is implemented for planes
        if (p_message->typeName() == ModeMessage::TypeName)
        {
            // can use the internal pointer here avoiding dynamic pointer cast with refcount increase
            retval = format(*dynamic_cast<ModeMessage*>(p_message.data()));
        }
        else
        {
            retval = p_message->toString();
        }
    }
    else
    {
        QLOG_ERROR() << "PlaneMessageFormatter::format() called with nullpointer";
    }
    return retval;
}

QString Plane::MessageFormatter::format(const ModeMessage &message)
{
    // Interpretation taken from
    // Ardupilot/ArduPlane/defines.h
    // last verification 24.01.2016

    QString output;
    QTextStream outputStream(&output);

    switch (message.getMode())
    {
    case Plane::MANUAL:
        outputStream << "Manual";
        break;
    case Plane::CIRCLE:
        outputStream << "Circle";
        break;
    case Plane::STABILIZE:
        outputStream << "Stabilize";
        break;
    case Plane::TRAINING:
        outputStream << "Training";
        break;
    case Plane::ACRO:
        outputStream << "Acro";
        break;
    case Plane::FLY_BY_WIRE_A:
        outputStream << "Fly by wire A";
        break;
    case Plane::FLY_BY_WIRE_B:
        outputStream << "Fly by wire B";
        break;
    case Plane::CRUISE:
        outputStream << "Cruise";
        break;
    case Plane::AUTOTUNE:
        outputStream << "Autotune";
        break;
    case Plane::LAND:
        outputStream << "Land";
        break;
    case Plane::AUTO:
        outputStream << "Auto";
        break;
    case Plane::RTL:
        outputStream << "RTL";
        break;
    case Plane::LOITER:
        outputStream << "Loiter";
        break;
    case Plane::GUIDED:
        outputStream << "Guided";
        break;
    case Plane::INITIALIZING:
        outputStream << "Initialising";
        break;
    case Plane::QSTABILIZE:
        outputStream << "QStabilize";
        break;
    case Plane::QHOVER:
        outputStream << "QHover";
        break;
    case Plane::QLOITER:
        outputStream << "QLoiter";
        break;
    case Plane::QLAND:
        outputStream << "QLand";
        break;
    case Plane::QRTL:
        outputStream << "QRTL";
        break;
    default:
        outputStream << "Unknown Mode:" << message.getMode();
        break;
    }
    return output;
}


QString Rover::MessageFormatter::format(MessageBase::Ptr &p_message)
{
    QString retval("Unknown Type");
    if (p_message)
    {
        // Only mode message formatter is implemented for rovers
        if (p_message->typeName() == ModeMessage::TypeName)
        {
            // can use the internal pointer here avoiding dynamic pointer cast with refcount increase
            retval = format(*dynamic_cast<ModeMessage*>(p_message.data()));
        }
        else
        {
            retval = p_message->toString();
        }
    }
    else
    {
        QLOG_ERROR() << "RoverMessageFormatter::format() called with nullpointer";
    }
    return retval;
}

QString Rover::MessageFormatter::format(const ModeMessage &message)
{
    // Interpretation taken from
    // Ardupilot/APMRover2/defines.h
    // last verification 24.01.2016

    QString output;
    QTextStream outputStream(&output);

    switch (message.getMode())
    {
    case Rover::MANUAL:
        outputStream << "Manual";
        break;
    case Rover::LEARNING:
        outputStream << "Learning";
        break;
    case Rover::STEERING:
        outputStream << "Steering";
        break;
    case Rover::HOLD:
        outputStream << "Hold";
        break;
    case Rover::AUTO:
        outputStream << "Auto";
        break;
    case Rover::RTL:
        outputStream << "RTL";
        break;
    case Rover::GUIDED:
        outputStream << "Guided";
        break;
    case Rover::INITIALIZING:
        outputStream << "Initialising";
        break;
    default:
        outputStream << "Unknown Mode:" << message.getMode();
        break;
    }
    return output;
}
