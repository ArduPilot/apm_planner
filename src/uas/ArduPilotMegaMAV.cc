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

#ifndef MAVLINK_MSG_ID_MOUNT_CONFIGURE
#include "ardupilotmega/mavlink_msg_mount_configure.h"
#endif

#ifndef MAVLINK_MSG_ID_MOUNT_CONTROL
#include "ardupilotmega/mavlink_msg_mount_control.h"
#endif

#include <QString>
#include <QDir>
#include <QDesktopServices>

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


ApmPlane::ApmPlane(planeMode aMode)
{
    CustomMode(static_cast<int>(aMode));
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
    case RESERVED_4:
    case RESERVED_7:
    case RESERVED_8:
    case RESERVED_9:
    case RESERVED_13:
    case RESERVED_14:
        return "Reserved";
    default:
        return "Undefined";
    }
}

ApmCopter::ApmCopter(copterMode aMode)
{
     CustomMode(static_cast<int>(aMode));
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
        return "Position";
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
    case HYBRID:
        return "Hybrid Loiter";
        break;
    default:
        return "Undefined";
    }
}

ApmRover::ApmRover(roverMode aMode)
{
     CustomMode(static_cast<int>(aMode));
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
        return "Reserved";
    default:
        return "Undefined";
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
    enableExtendedSystemStatusTransmission(2);

    enablePositionTransmission(3);

    enableExtra1Transmission(10);

    enableExtra2Transmission(10);

    enableExtra3Transmission(2);

    enableRawSensorDataTransmission(2);

    enableRCChannelDataTransmission(2);
}

void ArduPilotMegaMAV::uasConnected()
{
    QLOG_INFO() << "ArduPilotMegaMAV APM Connected";
    QTimer::singleShot(500,this,SLOT(RequestAllDataStreams())); //Send an initial TX request in 0.5 seconds.
    createNewMAVLinkLog(type);
}

void ArduPilotMegaMAV::uasDisconnected()
{
    QLOG_INFO() << "ArduPilotMegaMAV APM disconnected";
    mavlink->stopLogging();
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

    QString logFileName =  QGC::MAVLinkLogDirectory() + QGC::fileNameAsTime();
    QLOG_DEBUG() << "start new MAVLink Log:" << logFileName;
    mavlink->startLogging(logFileName);
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

            if (text.startsWith("ArduCopter") || text.startsWith("ArduPlane")
                    || text.startsWith("ArduRover")) {
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
    mavlink_msg_command_long_pack(mavlink->getSystemId(),
                                  mavlink->getComponentId(),
                                  &msg,
                                  getUASID(),                    // uint8_t target_system,
                                  MAV_COMP_ID_SYSTEM_CONTROL,    // uint8_t target_component
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
    mavlink_msg_command_long_pack(mavlink->getSystemId(),
                                  mavlink->getComponentId(),
                                  &msg,
                                  getUASID(),                    // uint8_t target_system,
                                  MAV_COMP_ID_SYSTEM_CONTROL,    // uint8_t target_component
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
        QString audioString = "Please press and hold safety switch";
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

