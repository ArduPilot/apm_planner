
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
#include "QsLog.h"
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
    m_severityCompatibilityMode = settings.value("STATUSTEXT_COMPAT_MODE",false).toBool();
    settings.endGroup();
}

void ArduPilotMegaMAV::saveSettings()
{
    QSettings settings;
    settings.beginGroup("ARDUPILOT");
    settings.setValue("STATUSTEXT_COMPAT_MODE",m_severityCompatibilityMode);
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
            QString text = QString(b);
            int severity = mavlink_msg_statustext_get_severity(&message);
            QLOG_INFO() << "STATUS TEXT:" << severity << ":" << text;

            if (text.contains(APM_COPTER_REXP) || text.contains(APM_PLANE_REXP)
                    || text.contains(APM_ROVER_REXP)) {
                QLOG_DEBUG() << "APM Version String detected:" << text;
                // Process Version and keep.

                emit versionDetected(text);
            }

            // Check is older APM and reset severity to correct MAVLINK spec.
            if (m_severityCompatibilityMode) {
                 // Older APM detected, translate severity to MAVLink Standard severity
                 // SEVERITY_LOW     =1 MAV_SEVERITY_WARNING = 4
                 // SEVERITY_MEDIUM  =2 MAV_SEVERITY_ALERT   = 1
                 // SEVERITY_HIGH    =3 MAV_SEVERITY_CRITICAL= 2
                 switch (severity) {
                     case 1: /*gcs_severity::SEVERITY_LOW:*/
                         severity = MAV_SEVERITY_WARNING;
                         break;
                     case 2: /*gcs_severity::SEVERITY_MEDIUM*/
                         severity = MAV_SEVERITY_ALERT;
                         break;
                     case 3: /*gcs_severity::SEVERITY_HIGH:*/
                         severity = MAV_SEVERITY_CRITICAL;
                         break;
                     default:
                         severity = MAV_SEVERITY_INFO;
                         break;
                 }
                 // repack message for further down the stack.s
                 mavlink_msg_statustext_pack(message.sysid,message.compid,&message,severity,b.data());

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
    ModeMessage mode(0, 0, custom_mode, 0);

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

const QString MessageBase::timeFieldName("TimeUS");

MessageBase::MessageBase() : m_Index(0), m_TimeStamp(0)
{}

MessageBase::MessageBase(const quint64 index, const quint64 timeStamp, const QString &name, const QColor &color) :
    m_Index(index),
    m_TimeStamp(timeStamp),
    m_TypeName(name),
    m_Color(color)
{}

quint64 MessageBase::getIndex() const
{
    return m_Index;
}

quint64 MessageBase::getTimeStamp() const
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

ErrorMessage::ErrorMessage(const quint64 index, const quint64 timeStamp, const quint8 subSys, const quint8 errCode) :
    MessageBase(index, timeStamp, TypeName, QColor(150,0,0)),
    m_SubSys(subSys),
    m_ErrorCode(errCode)
{}

quint8 ErrorMessage::getSubsystemCode() const
{
    return m_SubSys;
}

quint8 ErrorMessage::getErrorCode() const
{
    return m_ErrorCode;
}

bool ErrorMessage::setFromSqlRecord(const QSqlRecord &record)
{
    bool rc1 = false;
    bool rc2 = false;
    bool rc3 = false;

    if(record.value(0).isValid())
    {
        m_Index = static_cast<quint64>(record.value(0).toLongLong());
        rc1 = true;
    }
    if (record.contains(timeFieldName))
    {
        m_TimeStamp = static_cast<quint64>(record.value(timeFieldName).toLongLong());
        // TimeStamp does not influence the returncode as its optional
    }
    if (record.contains("Subsys"))
    {
        m_SubSys = static_cast<quint8>(record.value("Subsys").toInt());
        rc2 = true;
    }
    if (record.contains("ECode"))
    {
        m_ErrorCode = static_cast<quint8>(record.value("ECode").toInt());
        rc3 = true;
    }

    return rc1 && rc2 && rc3;
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

ModeMessage::ModeMessage() : m_Mode(0), m_ModeNum(0)
{
    // Set up base class vars for this message
    m_TypeName = TypeName;
    m_Color    = QColor(50,125,0);
}

ModeMessage::ModeMessage(const quint64 index, const quint64 timeStamp, const qint8 mode, const quint8 modeNum) :
    MessageBase(index, timeStamp, TypeName, QColor(50,125,0)),
    m_Mode(mode),
    m_ModeNum(modeNum)
{}

qint8 ModeMessage::getMode() const
{
    return m_Mode;
}

quint8 ModeMessage::getModeNum() const
{
    return m_ModeNum;
}

bool ModeMessage::setFromSqlRecord(const QSqlRecord &record)
{
    bool rc1 = false;
    bool rc2 = false;

    if(record.value(0).isValid())
    {
        m_Index = static_cast<quint64>(record.value(0).toLongLong());
        rc1 = true;
    }
    if (record.contains(timeFieldName))
    {
        m_TimeStamp = static_cast<quint64>(record.value(timeFieldName).toLongLong());
        // TimeStamp does not influence the returncode as its optional
    }
    if (record.contains("Mode"))
    {
        m_Mode = static_cast<quint8>(record.value("Mode").toInt());
        rc2 = true;
    }
    if (record.contains("ModeNum"))
    {
        m_ModeNum = static_cast<quint8>(record.value("ModeNum").toInt());
       // ModeNum does not influence the returncode as its optional
    }

    return rc1 && rc2;
}

QString ModeMessage::toString() const
{
    QString output;
    QTextStream outputStream(&output);

    outputStream << " Mode:" << m_Mode << " ModeNum:" << m_ModeNum;
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

EventMessage::EventMessage(const quint64 index, const quint64 timeStamp, const quint8 eventID) :
    MessageBase(index, timeStamp, TypeName, QColor(0,0,125)),
    m_EventID(eventID)
{}

quint8 EventMessage::getEventID() const
{
    return m_EventID;
}

bool EventMessage::setFromSqlRecord(const QSqlRecord &record)
{
    bool rc1 = false;
    bool rc2 = false;

    if(record.value(0).isValid())
    {
        m_Index = static_cast<quint64>(record.value(0).toLongLong());
        rc1 = true;
    }
    if (record.contains(timeFieldName))
    {
        m_TimeStamp = static_cast<quint64>(record.value(timeFieldName).toLongLong());
        // TimeStamp does not influence the returncode as its optional
    }
    if (record.contains("Id"))
    {
        m_EventID = static_cast<quint8>(record.value("Id").toInt());
        rc2 = true;
    }

    return rc1 && rc2;
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

MsgMessage::MsgMessage(const quint64 index, const quint64 timeStamp, const QString &message) :
    MessageBase(index, timeStamp, TypeName, QColor(0,0,0)),
    m_Message(message)
{}

bool MsgMessage::setFromSqlRecord(const QSqlRecord &record)
{
    bool rc1 = false;
    bool rc2 = false;

    if(record.value(0).isValid())
    {
        m_Index = static_cast<quint64>(record.value(0).toLongLong());
        rc1 = true;
    }
    if (record.contains(timeFieldName))
    {
        m_TimeStamp = static_cast<quint64>(record.value(timeFieldName).toLongLong());
        // TimeStamp does not influence the returncode as its optional
    }
    if (record.contains("Message"))
    {
        m_Message = record.value("Message").toString();
        rc2 = true;
    }

    return rc1 && rc2;
}

QString MsgMessage::toString() const
{
    return m_Message;
}

//********

MessageBase::Ptr MessageFactory::getMessageOfType(const QString &type)
{
    if (type == ErrorMessage::TypeName)
    {
        return MessageBase::Ptr(new ErrorMessage());
    }
    else if (type == ModeMessage::TypeName)
    {
        return MessageBase::Ptr(new ModeMessage());
    }
    else if (type == EventMessage::TypeName)
    {
        return MessageBase::Ptr(new EventMessage());
    }
    else if (type == MsgMessage::TypeName)
    {
        return MessageBase::Ptr(new MsgMessage());
    }
    QLOG_WARN() << "MessageFactory::getMessageOfType: No message of type '" << type << "' could be created";
    return MessageBase::Ptr();
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
    // last verification 24.01.2016

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
        ModeMessage tmpMsg(0, 0, message.getErrorCode(), 0);
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

    default:
        outputStream << "SubSys:" << message.getErrorCode() << " ECode:" << message.getErrorCode();
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
    // last verification 24.01.2016

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
    default:
        outputStream << "Unknown Mode:" << message.getMode();
        break;
    }
    return output;
}

QString Copter::MessageFormatter::format(const EventMessage &message)
{
    // Interpretation taken from
    // Ardupilot/ArduCopter/defines.h
    // last verification 24.01.2016

    QString output;
    QTextStream outputStream(&output);

    switch(message.getEventID())
    {
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
        outputStream << "Add WP";
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
        outputStream << "Motor interlock enable";
        break;
    case 57:
        outputStream << "Motor interlock disable";
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
