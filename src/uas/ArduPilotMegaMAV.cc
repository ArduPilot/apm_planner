
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

CustomMode::CustomMode()
{
}

CustomMode::CustomMode(int aMode)
{
    m_mode = aMode;
}

int CustomMode::modeAsInt()
{
    return m_mode;
}

QString CustomMode::operator <<(int aMode)
{
    return QString::number(aMode);
}

QString CustomMode::colorForMode(int aMode)
{
    const uint numberOfKmlColors = 16;
    const QString kmlColors[] = {"FFFF00FF"
        , "FF00FF00"
        , "FFFF0000"
        , "FFFF2323"
        , "FFFFCE00"
        , "FF00CEFF"
        , "FF009900"
        , "FF33FFCC"
        , "FF0000FF"
        , "FFFFAAAA"
        , "FFABABAB"
        , "FF99FF33"
        , "FF66CC99"
        , "FFCC3300"
        , "FF0066FF"};

    if ((sizeof(kmlColors)/sizeof(const char*)) > aMode*numberOfKmlColors ){
        QLOG_ERROR() << "ColorForMode: not enough colors, so wrapping to 1st color";
        aMode -= numberOfKmlColors;
    }
    return kmlColors[aMode];
}

ApmPlane::ApmPlane(planeMode aMode) : CustomMode(aMode)
{
}

ApmPlane::planeMode ApmPlane::mode()
{
    return static_cast<ApmPlane::planeMode>(m_mode);
}

QString ApmPlane::stringForMode(int aMode)
{
    switch(static_cast<planeMode>(aMode)) {
    case MANUAL:
        return "Manual";
        break;
    case CIRCLE:
        return "Circle";
        break;
    case STABILIZE:
        return "Stabilize";
        break;
    case TRAINING:
        return "Training";
        break;
    case FLY_BY_WIRE_A:
        return "FBW A";
        break;
    case FLY_BY_WIRE_B:
        return "FBW B";
        break;
    case AUTO:
        return "Auto";
        break;
    case RTL:
        return "RTL";
        break;
    case LOITER:
        return "Loiter";
        break;
    case GUIDED:
        return "Guided";
        break;
    case INITIALIZING:
        return "Initializing";
        break;
    case ACRO:
        return "Acro";
        break;
    case CRUISE:
        return "Cruise";
        break;
    case AUTOTUNE:
        return "Auto Tune";
        break;
    case RESERVED_9:
    case RESERVED_13:
    case RESERVED_14:
        return "Reserved";
    default:
        return QString().sprintf("Mode (%d)", aMode);
    }
}

ApmCopter::ApmCopter(copterMode aMode) : CustomMode(aMode)
{
}

ApmCopter::copterMode ApmCopter::mode()
{
    return static_cast<ApmCopter::copterMode>(m_mode);
}

QString ApmCopter::stringForMode(int aMode) {
    switch(static_cast<copterMode>(aMode)) {
    case STABILIZE:
        return "Stabilize";
        break;
    case ACRO:
        return "Acro";
        break;
    case ALT_HOLD:
        return "Alt Hold";
        break;
    case AUTO:
        return "Auto";
        break;
    case GUIDED:
        return "Guided";
        break;
    case LOITER:
        return "Loiter";
        break;
    case RTL:
        return "RTL";
        break;
    case CIRCLE:
        return "Circle";
        break;
    case POSITION:
        return QString().sprintf("Position (%d)", aMode);
        break;
    case LAND:
        return "Land";
        break;
    case OF_LOITER:
        return "OF Loiter";
        break;
    case DRIFT:
        return "Drift";
        break;
    case SPORT:
        return "Sport";
        break;
    case RESERVED_12:
        return "Reserved";
        break;
    case POS_HOLD:
        return "Pos Hold";
        break;
    case AUTOTUNE:
        return "Autotune";
        break;
    case FLIP:
        return "Flip";
        break;
    case BRAKE:
        return "Brake";
        break;
    default:
        return QString().sprintf("Mode (%d)", aMode);
    }
}

ApmRover::ApmRover(roverMode aMode) : CustomMode(aMode)
{
}

ApmRover::roverMode ApmRover::mode()
{
    return static_cast<ApmRover::roverMode>(m_mode);
}

QString ApmRover::stringForMode(int aMode) {
    switch(static_cast<roverMode>(aMode)) {
    case MANUAL:
        return "Manual";
        break;
    case LEARNING:
        return "Learning";
        break;
    case STEERING:
        return "Steering";
        break;
    case HOLD:
        return "Hold";
        break;
    case AUTO:
        return "Auto";
        break;
    case RTL:
        return "RTL";
        break;
    case GUIDED:
        return "Guided";
        break;
    case INITIALIZING:
        return "Initializing";
        break;
    case RESERVED_1:
    case RESERVED_5:
    case RESERVED_6:
    case RESERVED_7:
    case RESERVED_8:
    case RESERVED_9:
    case RESERVED_12:
    case RESERVED_13:
    case RESERVED_14:
    default:
        return QString().sprintf("Mode (%d)", aMode);
    }
}

ArduPilotMegaMAV::ArduPilotMegaMAV(MAVLinkProtocol* mavlink, int id) :
    UAS(mavlink, id)//,
    // place other initializers here
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
    UAS::receiveMessage(link, message);

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
                emit versionDetected(text);
            }

        } break;
        default:
            //QLOG_DEBUG() << "\nARDUPILOT RECEIVED MESSAGE WITH ID" << message.msgid;
            break;
        }
    }
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

    if (isFixedWing()){
        customModeString = ApmPlane::stringForMode(custom_mode);

    } else if (isMultirotor()){
        customModeString = ApmCopter::stringForMode(custom_mode);

    } else if (isGroundRover()){
        customModeString = ApmRover::stringForMode(custom_mode);

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

QString ArduPilotMegaMAV::getNameFromEventId(int ecode)
{
    QString ecodestring = "";
    if (ecode == 10)
    {
        ecodestring = "Armed";
    }
    else if (ecode == 11)
    {
        ecodestring = "Disrmed";
    }
    else if (ecode == 15)
    {
        ecodestring = "Auto-Armed";
    }
    else if (ecode == 16)
    {
        ecodestring = "Takeoff";
    }
    else if (ecode == 17)
    {
        ecodestring = "Land Complete Maybe";
    }
    else if (ecode == 18)
    {
        ecodestring = "Land Complete";
    }
    else if (ecode == 19)
    {
        ecodestring = "Lost GPS";
    }
    else if (ecode == 25)
    {
        ecodestring = "Home Set";
    }
    else if (ecode == 28)
    {
        ecodestring = "Not Landed";
    }
    else if (ecode == 30)
    {
        ecodestring = "Autotune Initialized";
    }
    else if (ecode == 31)
    {
        ecodestring = "Autotune Off";
    }
    else if (ecode == 32)
    {
        ecodestring = "Autotune Restart";
    }
    else if (ecode == 33)
    {
        ecodestring = "Autotune Success";
    }
    else if (ecode == 34)
    {
        ecodestring = "Autotune Failed";
    }
    else if (ecode == 35)
    {
        ecodestring = "Autotune Reached Limit";
    }
    else if (ecode == 36)
    {
        ecodestring = "Autotune Pilot Testing";
    }
    else if (ecode == 37)
    {
        ecodestring = "Autotune Saved Gains";
    }
    else if (ecode == 38)
    {
        ecodestring = "Save Trim";
    }
    else if (ecode == 39)
    {
        ecodestring = "Add WP";
    }
    else if (ecode == 40)
    {
        ecodestring = "WP Clear Mission RTL";
    }
    else if (ecode == 41)
    {
        ecodestring = "Fence Enable";
    }
    else if (ecode == 42)
    {
        ecodestring = "Fence Disable";
    }
    else if (ecode == 49)
    {
        ecodestring = "Parachute Disabled";
    }
    else if (ecode == 50)
    {
        ecodestring = "Parachute Enabled";
    }
    else if (ecode == 51)
    {
        ecodestring = "Parachute Released";
    }
    else
    {
        return "Event: " + QString::number(ecode);
    }
    return ecodestring;

}

//******************* Classes for error handling **************************

ErrorType::ErrorType() : SubSys(0), ErrorCode(0)
{}

bool ErrorType::operator != (const ErrorType &rhs)
{
    return ((this->SubSys != rhs.SubSys) || (this->ErrorCode != rhs.ErrorCode));
}

quint8 ErrorType::getSubsystemCode()
{
    return SubSys;
}

quint8 ErrorType::getErrorCode()
{
    return ErrorCode;
}

bool ErrorType::setFromSqlRecord(const QSqlRecord &record)
{
    bool returnCode = true;

    if (record.contains("Subsys"))
    {
        SubSys = static_cast<quint8>(record.value("Subsys").toString().toShort());
    }
    else
    {
        returnCode = false;
    }

    if (record.contains("ECode"))
    {
        ErrorCode = static_cast<quint8>(record.value("ECode").toString().toShort());
    }
    else
    {
        returnCode = false;
    }

    return returnCode;
}

QString ErrorType::toString() const
{
    QString output;
    QTextStream QTStream(&output);

    QTStream << " Subsystem:" << SubSys << " Errorcode:" << ErrorCode;
    return output;
}

QString CopterErrorTypeFormatter::format(ErrorType &code)
{
    // SubSys ans ErrorCode interpretation was taken from
    // Ardupilot/ArduCopter/defines.h

    QString output;
    QTextStream QTStream(&output);

    bool EcodeUsed = false;

    switch (code.getSubsystemCode())
    {
    case 1:
        QTStream << "Main:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Ins-Delay";
            EcodeUsed = true;
        }
        break;

    case 2:
        QTStream << "Radio:";
        if (code.getErrorCode() == 2)
        {
            QTStream << "Late Frame detected";
            EcodeUsed = true;
        }
        break;

    case 3:
        QTStream << "Compass:";
        if (code.getErrorCode() == 2)
        {
            QTStream << "Failed to read data";
            EcodeUsed = true;
        }
        break;

    case 4:
        QTStream << "OptFlow:";
        break;

    case 5:
        QTStream << "FS-Radio:";
        break;

    case 6:
        QTStream << "FS-Batt:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 7:
        QTStream << "FS-GPS:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 8:
        QTStream << "FS-GCS:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 9:
        QTStream << "FS-Fence:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 10:
        QTStream << "Flight-Mode:";
        break;

    case 11:
        QTStream << "GPS:";
        break;

    case 12:
        QTStream << "Crash-Check:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Crash Detected";
            EcodeUsed = true;
        }
        else if (code.getErrorCode() == 2)
        {
            QTStream << "Control Lost";
            EcodeUsed = true;
        }
        break;

    case 13:
        QTStream << "FLIP:";
        if (code.getErrorCode() == 2)
        {
            QTStream << "Abandoned";
            EcodeUsed = true;
        }
        break;

    case 14:
        QTStream << "Autotune:";
        break;

    case 15:
        QTStream << "Parachute:";
        if (code.getErrorCode() == 2)
        {
            QTStream << "Too low to eject";
            EcodeUsed = true;
        }
        else if (code.getErrorCode() == 3)
        {
            QTStream << "Copter Landed";
            EcodeUsed = true;
        }
        break;

    case 16:
        QTStream << "EKF-Check:";
        if (code.getErrorCode() == 2)
        {
            QTStream << "Bad Variance detected";
            EcodeUsed = true;
        }
        break;

    case 17:
        QTStream << "FS-EKF-INAV:";
        if (code.getErrorCode() == 1)
        {
            QTStream << "Detected";
            EcodeUsed = true;
        }
        break;

    case 18:
        QTStream << "Baro:";
        if (code.getErrorCode() == 2)
        {
            QTStream << "Glitch detected";
            EcodeUsed = true;
        }
        break;

    case 19:
        QTStream << "CPU:";
        break;

    default:
        QTStream << "SubSys:" << code.getErrorCode() << " ECode:" << code.getErrorCode();
        EcodeUsed = true;
        break;

    }

    if (!EcodeUsed)
    {
        switch (code.getErrorCode())
        {
        case 0:
            QTStream << "Everything OK!";
            break;

        case 1:
            QTStream << "Failed to init";
            break;

        case 4:
            QTStream << "Is Unhealthy";
            break;

        default:
            QTStream << "Unknown ErrorCode(" << code.getErrorCode() << ")";
            break;
        }
    }

    return output;
}
