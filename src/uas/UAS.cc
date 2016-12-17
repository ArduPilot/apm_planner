 /*===================================================================
======================================================================*/

/**
 * @file
 *   @brief Represents one unmanned aerial vehicle
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#include "logging.h"
#include "UAS.h"
#include "LinkInterface.h"
#include "UASManager.h"
#include "QGC.h"
#include "GAudioOutput.h"
//#include "MAVLinkProtocol.h"
#include "QGCMAVLink.h"
#include "LinkManager.h"
#include "MainWindow.h"

#include <QList>
#include <QMessageBox>
#include <QTimer>
#include <QSettings>
#include <iostream>
#include <QDesktopServices>

#include <cmath>
#include <qmath.h>

#ifdef QGC_PROTOBUF_ENABLED
#include <google/protobuf/descriptor.h>
#endif


const double UAS::lipoFull = 4.2f;  ///< 100% charged voltage
const double UAS::lipoEmpty = 3.5f; ///< Discharged voltage


/**
* Gets the settings from the previous UAS (name, airframe, autopilot, battery specs)
* by calling readSettings. This means the new UAS will have the same settings 
* as the previous one created unless one calls deleteSettings in the code after
* creating the UAS. 
*/
UAS::UAS(MAVLinkProtocol* protocol, int id) : UASInterface(),
    uasId(id),
    links(new QList<LinkInterface*>()),
    unknownPackets(),
    commStatus(COMM_DISCONNECTED),
    receiveDropRate(0),
    sendDropRate(0),
    statusTimeout(new QTimer(this)),

    name(""),
    type(-1),
    airframe(-1),
    autopilot(-1),
    systemIsArmed(false),
    base_mode(-1),
    // custom_mode not initialized
    custom_mode(-1),
    status(-1),
    // shortModeText not initialized
    // shortStateText not initialized

    // actuatorValues not initialized
    // actuatorNames not initialized
    // motorValues not initialized
    // motorNames mnot initialized
    thrustSum(0),
    thrustMax(10),

    // batteryType not initialized
    // cells not initialized
    // fullVoltage not initialized
    // emptyVoltage not initialized
    startVoltage(-1.0),
    tickVoltage(10.5),
    lastTickVoltageValue(13.0),
    tickLowpassVoltage(12.0),
    warnVoltage(9.5),
    warnLevelPercent(20.0),
    currentVoltage(12.6),
    lpVoltage(12.0),
    currentCurrent(0.4),
    batteryRemainingEstimateEnabled(true),
    // chargeLevel not initialized
    // timeRemaining  not initialized
    lowBattAlarm(false),

    startTime(QGC::groundTimeMilliseconds()),
    onboardTimeOffset(0),

    manualControl(false),
    overrideRC(false),

    positionLock(false),
    isLocalPositionKnown(false),
    isGlobalPositionKnown(false),

    localX(0.0),
    localY(0.0),
    localZ(0.0),
    latitude(0.0),
    longitude(0.0),
    altitudeAMSL(0.0),
    altitudeRelative(0.0),

    globalEstimatorActive(false),
    latitude_gps(0.0),
    longitude_gps(0.0),
    altitude_gps(0.0),

    speedX(0.0),
    speedY(0.0),
    speedZ(0.0),

    nedPosGlobalOffset(0,0,0),
    nedAttGlobalOffset(0,0,0),

    #if defined(QGC_PROTOBUF_ENABLED) && defined(QGC_USE_PIXHAWK_MESSAGES)
    receivedOverlayTimestamp(0.0),
    receivedObstacleListTimestamp(0.0),
    receivedPathTimestamp(0.0),
    receivedPointCloudTimestamp(0.0),
    receivedRGBDImageTimestamp(0.0),
    #endif

    airSpeed(std::numeric_limits<double>::quiet_NaN()),
    groundSpeed(std::numeric_limits<double>::quiet_NaN()),
    waypointManager(this),

    attitudeKnown(false),
    attitudeStamped(false),
    lastAttitude(0),

    roll(0.0),
    pitch(0.0),
    yaw(0.0),

    blockHomePositionChanges(false),
    receivedMode(false),


    paramsOnceRequested(false),
    paramManager(NULL),

    simulation(0),

    // The protected members.
    connectionLost(false),
    lastVoltageWarning(0),
    lastNonNullTime(0),
    onboardTimeOffsetInvalidCount(0),
    hilEnabled(false),
    sensorHil(false),
    lastSendTimeGPS(0),
    lastSendTimeSensors(0)
{
    Q_UNUSED(protocol);

    for (unsigned int i = 0; i<255;++i)
    {
        componentID[i] = -1;
        componentMulti[i] = false;
    }
    
    color = UASInterface::getNextColor();
    setBatterySpecs(QString("9V,9.5V,12.6V"));
    connect(statusTimeout, SIGNAL(timeout()), this, SLOT(updateState()));
    connect(this, SIGNAL(systemSpecsChanged(int)), this, SLOT(writeSettings()));
    statusTimeout->start(500);
    readSettings(); 
    // Initial signals
    emit disarmed();
    emit armingChanged(false);  

    systemId = QGC::defaultSystemId;
    componentId = QGC::defaultComponentId;

    m_heartbeatsEnabled = MainWindow::instance()->heartbeatEnabled(); //Default to sending heartbeats
    QTimer *heartbeattimer = new QTimer(this);
    connect(heartbeattimer,SIGNAL(timeout()),this,SLOT(sendHeartbeat()));
    heartbeattimer->start(MAVLINK_HEARTBEAT_DEFAULT_RATE * 1000);

    m_parameterSendTimer.setInterval(20);
    connect(&m_parameterSendTimer, SIGNAL(timeout()), this, SLOT(requestNextParamFromQueue()));
}

/**
* Saves the settings of name, airframe, autopilot type and battery specifications
* by calling writeSettings.
*/
UAS::~UAS()
{
    writeSettings();
    delete links;
    delete statusTimeout;
    delete simulation;
}

/**
* Saves the settings of name, airframe, autopilot type and battery specifications
* for the next instantiation of UAS.
*/
void UAS::writeSettings()
{
    QSettings settings;
    settings.beginGroup(QString("MAV%1").arg(uasId));
    settings.setValue("NAME", this->name);
    settings.setValue("AIRFRAME", this->airframe);
    settings.setValue("AP_TYPE", this->autopilot);
    settings.setValue("BATTERY_SPECS", getBatterySpecs());
    settings.endGroup();
    settings.sync();
}

/**
* Reads in the settings: name, airframe, autopilot type, and battery specifications
* for the new UAS.
*/
void UAS::readSettings()
{
    QSettings settings;
    settings.beginGroup(QString("MAV%1").arg(uasId));
    setUASName(settings.value("NAME", this->name).toString());
    setAirframe(settings.value("AIRFRAME", this->airframe).toInt());
    setAutopilotType(autopilot = settings.value("AP_TYPE", this->autopilot).toInt());
    if (settings.contains("BATTERY_SPECS"))
    {
        setBatterySpecs(settings.value("BATTERY_SPECS").toString());
    }
    settings.endGroup();
}

/**
*  Deletes the settings origianally read into the UAS by readSettings.
*  This is in case one does not want the old values but would rather 
*  start with the values assigned by the constructor.
*/
void UAS::deleteSettings()
{
    this->name = "";
    this->airframe = QGC_AIRFRAME_GENERIC;
    this->autopilot = -1;
    setBatterySpecs(QString("9V,9.5V,12.6V"));
}

/**
* @ return the id of the uas
*/
int UAS::getUASID() const
{
    return uasId;
}

/**
* Update the heartbeat.
*/
void UAS::updateState()
{
    // Check if heartbeat timed out
    quint64 heartbeatInterval = QGC::groundTimeUsecs() - lastHeartbeat;
    if (!connectionLost && (heartbeatInterval > timeoutIntervalHeartbeat))
    {
        connectionLost = true;
        receivedMode = false;
        QString audiostring = QString("Link lost to system %1").arg(this->getUASID());
        GAudioOutput::instance()->say(audiostring.toLower());
    }

    // Update connection loss time on each iteration
    if (connectionLost && (heartbeatInterval > timeoutIntervalHeartbeat))
    {
        connectionLossTime = heartbeatInterval;
        emit heartbeatTimeout(true, heartbeatInterval/1000);
    }

    // Connection gained
    if (connectionLost && (heartbeatInterval < timeoutIntervalHeartbeat))
    {
        QString audiostring = QString("Link regained to system %1 after %2 seconds").arg(this->getUASID()).arg((int)(connectionLossTime/1000000));
        GAudioOutput::instance()->say(audiostring.toLower());
        connectionLost = false;
        connectionLossTime = 0;
        emit heartbeatTimeout(false, 0);
    }

    // Position lock is set by the MAVLink message handler
    // if no position lock is available, indicate an error
    if (positionLock)
    {
        positionLock = false;
    }
    else
    {
        if (((base_mode & MAV_MODE_FLAG_DECODE_POSITION_AUTO) || (base_mode & MAV_MODE_FLAG_DECODE_POSITION_GUIDED)) && positionLock)
        {
            GAudioOutput::instance()->notifyNegative();
        }
    }
}

/**
* If the acitve UAS (the UAS that was selected) is not the one that is currently
* active, then change the active UAS to the one that was selected.
*/
void UAS::setSelected()
{
    if (UASManager::instance()->getActiveUAS() != this)
    {
        UASManager::instance()->setActiveUAS(this);
        emit systemSelected(true);
    }
}

/**
* @return if the active UAS is the current UAS
**/
bool UAS::getSelected() const
{
    return (UASManager::instance()->getActiveUAS() == this);
}

QString UAS::statusName() const
{
    return QString("M%1:GCS Status.%2").arg(systemId);
}

QString UAS::statusMetric() const
{
    return QString("M%1:GCS Metric.%2").arg(systemId);
}

QString UAS::statusImperial() const
{
    return QString("M%1:GCS Imperial.%2").arg(systemId);
}

QString UAS::statusGPS() const
{
    return QString("M%1:GCS GPS.%2").arg(systemId);
}

void UAS::receiveMessage(LinkInterface* link, mavlink_message_t message)
{
    if (!link) return;
    if (!links->contains(link))
    {
        addLink(link);
        QLOG_TRACE() << __FILE__ << __LINE__ << "ADDED LINK!" << link->getName();
    }

    if (!components.contains(message.compid))
    {
        QString componentName;

        switch (message.compid)
        {
        case MAV_COMP_ID_ALL:
        {
            componentName = "ANONYMOUS";
            break;
        }
        case MAV_COMP_ID_IMU:
        {
            componentName = "IMU #1";
            break;
        }
        case MAV_COMP_ID_CAMERA:
        {
            componentName = "CAMERA";
            break;
        }
        case MAV_COMP_ID_MISSIONPLANNER:
        {
            componentName = "MISSIONPLANNER";
            break;
        }
        }

        components.insert(message.compid, componentName);
        emit componentCreated(uasId, message.compid, componentName);
    }

    //    QLOG_DEBUG() << "UAS RECEIVED from" << message.sysid << "component" << message.compid << "msg id" << message.msgid << "seq no" << message.seq;

    // Only accept messages from this system (condition 1)
    // and only then if a) attitudeStamped is disabled OR b) attitudeStamped is enabled
    // and we already got one attitude packet
    if (message.sysid == uasId && (!attitudeStamped || (attitudeStamped && (lastAttitude != 0)) || message.msgid == MAVLINK_MSG_ID_ATTITUDE))
    {
        QString uasState;
        QString stateDescription;

        bool multiComponentSourceDetected = false;
        bool wrongComponent = false;

        switch (message.compid)
        {
        case MAV_COMP_ID_IMU_2:
            // Prefer IMU 2 over IMU 1 (FIXME)
            componentID[message.msgid] = MAV_COMP_ID_IMU_2;
            break;
        default:
            // Do nothing
            break;
        }

        // Store component ID
        if (componentID[message.msgid] == -1)
        {
            // Prefer the first component
            componentID[message.msgid] = message.compid;
        }
        else
        {
            // Got this message already
            if (componentID[message.msgid] != message.compid)
            {
                componentMulti[message.msgid] = true;
                wrongComponent = true;
            }
        }

        if (componentMulti[message.msgid] == true) multiComponentSourceDetected = true;


        switch (message.msgid)
        {
        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            if (multiComponentSourceDetected && wrongComponent)
            {
                break;
            }
            lastHeartbeat = QGC::groundTimeUsecs();
            emit heartbeat(this);
            mavlink_heartbeat_t state;
            mavlink_msg_heartbeat_decode(&message, &state);
			
			// Send the base_mode and system_status values to the plotter. This uses the ground time
			// so the Ground Time checkbox must be ticked for these values to display
            quint64 time = getUnixTime();
			QString name = QString("M%1:HEARTBEAT.%2").arg(message.sysid);
			emit valueChanged(uasId, name.arg("base_mode"), "bits", state.base_mode, time);
			emit valueChanged(uasId, name.arg("custom_mode"), "bits", state.custom_mode, time);
			emit valueChanged(uasId, name.arg("system_status"), "-", state.system_status, time);
			
            // Set new type if it has changed
            if (this->type != state.type)
            {
                this->type = state.type;
                if (isFixedWing()) {
                    setAirframe(UASInterface::QGC_AIRFRAME_EASYSTAR);

                } else if (isMultirotor()) {
                    setAirframe(UASInterface::QGC_AIRFRAME_CHEETAH);

                } else if (isGroundRover()) {
                    setAirframe(UASInterface::QGC_AIRFRAME_HEXCOPTER);

                } else if (isHelicopter()) {
                    setAirframe(UASInterface::QGC_AIRFRAME_HELICOPTER);

                } else {
                    QLOG_DEBUG() << "Airframe is set to: " << type;
                    setAirframe(UASInterface::QGC_AIRFRAME_GENERIC);
                }
                this->autopilot = state.autopilot;
                emit systemTypeSet(this, type);
            }

            bool currentlyArmed = state.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY;

            if (systemIsArmed != currentlyArmed)
            {
                systemIsArmed = currentlyArmed;
                emit armingChanged(systemIsArmed);
                if (systemIsArmed)
                {
                    emit armed();
                }
                else
                {
                    emit disarmed();
                }
                playArmStateChangedAudioMessage(systemIsArmed);
            }

            bool customModeHasChanged = false;

            if ((state.system_status != static_cast<uint8_t>(this->status))) {
                QLOG_DEBUG() << "UAS: new system_status" << state.system_status;
                this->status = static_cast<uint8_t>(state.system_status);
                getStatusForCode((int)state.system_status, uasState, stateDescription);
                emit statusChanged(this, uasState, stateDescription);
                emit statusChanged(this->status);
                shortStateText = uasState;
            }

            receivedMode = true;
            if (base_mode != state.base_mode) {
                QLOG_DEBUG() << "UAS: new base mode " << state.base_mode;
                receivedMode = true;
                base_mode = state.base_mode;
                shortModeText = getShortModeTextFor(base_mode);
                emit modeChanged(this->getUASID(), shortModeText, "");
            }

            if (custom_mode != state.custom_mode) {
                QLOG_DEBUG() << "UAS: new custom mode " << state.custom_mode;
                customModeHasChanged = true;
                custom_mode = state.custom_mode;
                emit navModeChanged(uasId, state.custom_mode, getCustomModeText());
            }

            // AUDIO
            if (/*modeHasChanged || stateHasChanged || */customModeHasChanged){
                playCustomModeChangedAudioMessage(); // delegate audio to autopilot specializations
            }

            } break;
        case MAVLINK_MSG_ID_SYS_STATUS:
        {
            if (multiComponentSourceDetected && wrongComponent)
            {
                break;
            }
            mavlink_sys_status_t state;
            mavlink_msg_sys_status_decode(&message, &state);

            // Prepare for sending data to the realtime plotter, which is every field excluding onboard_control_sensors_present.
            quint64 time = getUnixTime();
            emit valueChanged(uasId, statusName().arg("Sensors Enabled"), "bits", state.onboard_control_sensors_enabled, time);
            emit valueChanged(uasId, statusName().arg("Sensors Health"), "bits", state.onboard_control_sensors_health, time);
            emit valueChanged(uasId, statusName().arg("Comms Errors"), "-", state.errors_comm, time);
            emit valueChanged(uasId, statusName().arg("Errors Count 1"), "-", state.errors_count1, time);
            emit valueChanged(uasId, statusName().arg("Errors Count 2"), "-", state.errors_count2, time);
            emit valueChanged(uasId, statusName().arg("Errors Count 3"), "-", state.errors_count3, time);
            emit valueChanged(uasId, statusName().arg("Errors Count 4"), "-", state.errors_count4, time);

			// Process CPU load.
            emit loadChanged(this,state.load/10.0);
            emit valueChanged(uasId, statusName().arg("CPU Load"), "%", state.load/10.0, time);

			// Battery charge/time remaining/voltage calculations
            currentVoltage = state.voltage_battery/1000.0;
            lpVoltage = filterVoltage(currentVoltage);
            tickLowpassVoltage = tickLowpassVoltage*0.8 + 0.2*currentVoltage;

            // We don't want to tick above the threshold
            if (tickLowpassVoltage > tickVoltage)
            {
                lastTickVoltageValue = tickLowpassVoltage;
            }

            if ((startVoltage > 0.0) && (tickLowpassVoltage < tickVoltage) && (fabs(lastTickVoltageValue - tickLowpassVoltage) > 0.1)
                    /* warn if lower than treshold */
                    && (lpVoltage < tickVoltage)
                    /* warn only if we have at least the voltage of an empty LiPo cell, else we're sampling something wrong */
                    && (currentVoltage > 3.3)
                    /* warn only if current voltage is really still lower by a reasonable amount */
                    && ((currentVoltage - 0.2) < tickVoltage)
                    /* warn only every 12 seconds */
                    && (QGC::groundTimeUsecs() - lastVoltageWarning) > 12000000)
            {
                GAudioOutput::instance()->say(QString("voltage warning: %1 volts").arg(lpVoltage, 0, 'f', 1, QChar(' ')));
                lastVoltageWarning = QGC::groundTimeUsecs();
                lastTickVoltageValue = tickLowpassVoltage;
            }

            if (startVoltage == -1.0 && currentVoltage > 0.1) startVoltage = currentVoltage;
            timeRemaining = calculateTimeRemaining();
            if (!batteryRemainingEstimateEnabled && chargeLevel != -1)
            {
                chargeLevel = state.battery_remaining;
            }

            emit batteryChanged(this, lpVoltage, currentCurrent, getChargeLevel(), timeRemaining);
            // emit voltageChanged(message.sysid, currentVoltage);

            emit valueChanged(uasId, statusName().arg("Battery"), "%", state.battery_remaining, time);
            emit valueChanged(uasId, statusName().arg("Voltage"), "V", state.voltage_battery/1000.0, time);

			// And if the battery current draw is measured, log that also.
			if (state.current_battery != -1)
			{
                currentCurrent = ((double)state.current_battery)/100.0;
                emit valueChanged(uasId, statusName().arg("Current"), "A", currentCurrent, time);
			}

            // LOW BATTERY ALARM
            if (lpVoltage < warnVoltage && (currentVoltage - 0.2) < warnVoltage && (currentVoltage > 3.3))
            {
                // An audio alarm. Does not generate any signals.
                startLowBattAlarm();
            }
            else
            {
                stopLowBattAlarm();
            }

            // control_sensors_enabled:
            // relevant bits: 11: attitude stabilization, 12: yaw position, 13: z/altitude control, 14: x/y position control
            emit attitudeControlEnabled(state.onboard_control_sensors_enabled & (1 << 11));
            emit positionYawControlEnabled(state.onboard_control_sensors_enabled & (1 << 12));
            emit positionZControlEnabled(state.onboard_control_sensors_enabled & (1 << 13));
            emit positionXYControlEnabled(state.onboard_control_sensors_enabled & (1 << 14));

			// Trigger drop rate updates as needed. Here we convert the incoming
			// drop_rate_comm value from 1/100 of a percent in a uint16 to a true
			// percentage as a float. We also cap the incoming value at 100% as defined
			// by the MAVLink specifications.
			if (state.drop_rate_comm > 10000)
			{
				state.drop_rate_comm = 10000;
			}
            emit dropRateChanged(this->getUASID(), state.drop_rate_comm/100.0);
            emit valueChanged(uasId, statusName().arg("Comms Drop Rate"), "%", state.drop_rate_comm/100.0, time);
		}
            break;
        case MAVLINK_MSG_ID_ATTITUDE:
        {
            mavlink_attitude_t attitude;
            mavlink_msg_attitude_decode(&message, &attitude);
            quint64 time = getUnixReferenceTime(attitude.time_boot_ms);

            emit attitudeChanged(this, message.compid, QGC::limitAngleToPMPIf(attitude.roll), QGC::limitAngleToPMPIf(attitude.pitch), QGC::limitAngleToPMPIf(attitude.yaw), time);

            if (!wrongComponent)
            {
                lastAttitude = time;
                setRoll(QGC::limitAngleToPMPIf(attitude.roll));
                setPitch(QGC::limitAngleToPMPIf(attitude.pitch));
                setYaw(QGC::limitAngleToPMPIf(attitude.yaw));


                attitudeKnown = true;
                emit attitudeChanged(this, getRoll(), getPitch(), getYaw(), time);
                emit attitudeRotationRatesChanged(uasId, attitude.rollspeed, attitude.pitchspeed, attitude.yawspeed, time);

                emit valueChanged(uasId,statusName().arg("Roll"),"deg",QVariant(getRoll() * (180.0/M_PI)),time);
                emit valueChanged(uasId,statusName().arg("Pitch"),"deg",QVariant(getPitch() * (180.0/M_PI)),time);
                emit valueChanged(uasId,statusName().arg("Yaw"),"deg",QVariant(getYaw() * (180.0/M_PI)),time);
            }
        }
            break;
        case MAVLINK_MSG_ID_ATTITUDE_QUATERNION:
        {
            mavlink_attitude_quaternion_t attitude;
            mavlink_msg_attitude_quaternion_decode(&message, &attitude);
            quint64 time = getUnixReferenceTime(attitude.time_boot_ms);

            double a = attitude.q1;
            double b = attitude.q2;
            double c = attitude.q3;
            double d = attitude.q4;

            double aSq = a * a;
            double bSq = b * b;
            double cSq = c * c;
            double dSq = d * d;
            float dcm[3][3];
            dcm[0][0] = aSq + bSq - cSq - dSq;
            dcm[0][1] = 2.0 * (b * c - a * d);
            dcm[0][2] = 2.0 * (a * c + b * d);
            dcm[1][0] = 2.0 * (b * c + a * d);
            dcm[1][1] = aSq - bSq + cSq - dSq;
            dcm[1][2] = 2.0 * (c * d - a * b);
            dcm[2][0] = 2.0 * (b * d - a * c);
            dcm[2][1] = 2.0 * (a * b + c * d);
            dcm[2][2] = aSq - bSq - cSq + dSq;

            float phi, theta, psi;
            theta = asin(-dcm[2][0]);

            if (fabs(theta - M_PI_2) < 1.0e-3f) {
                phi = 0.0f;
                psi = (atan2(dcm[1][2] - dcm[0][1],
                        dcm[0][2] + dcm[1][1]) + phi);

            } else if (fabs(theta + M_PI_2) < 1.0e-3f) {
                phi = 0.0f;
                psi = atan2f(dcm[1][2] - dcm[0][1],
                          dcm[0][2] + dcm[1][1] - phi);

            } else {
                phi = atan2f(dcm[2][1], dcm[2][2]);
                psi = atan2f(dcm[1][0], dcm[0][0]);
            }

            emit attitudeChanged(this, message.compid, QGC::limitAngleToPMPIf(phi),
                                 QGC::limitAngleToPMPIf(theta),
                                 QGC::limitAngleToPMPIf(psi), time);

            if (!wrongComponent)
            {
                lastAttitude = time;
                setRoll(QGC::limitAngleToPMPIf(phi));
                setPitch(QGC::limitAngleToPMPIf(theta));
                setYaw(QGC::limitAngleToPMPIf(psi));

                attitudeKnown = true;
                emit attitudeChanged(this, getRoll(), getPitch(), getYaw(), time);
                emit attitudeRotationRatesChanged(uasId, attitude.rollspeed, attitude.pitchspeed, attitude.yawspeed, time);
            }
        }
            break;
        case MAVLINK_MSG_ID_LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET:
        {
            mavlink_local_position_ned_system_global_offset_t offset;
            mavlink_msg_local_position_ned_system_global_offset_decode(&message, &offset);
            nedPosGlobalOffset.setX(offset.x);
            nedPosGlobalOffset.setY(offset.y);
            nedPosGlobalOffset.setZ(offset.z);
            nedAttGlobalOffset.setX(offset.roll);
            nedAttGlobalOffset.setY(offset.pitch);
            nedAttGlobalOffset.setZ(offset.yaw);
        }
            break;
        case MAVLINK_MSG_ID_HIL_CONTROLS:
        {
            mavlink_hil_controls_t hil;
            mavlink_msg_hil_controls_decode(&message, &hil);
            emit hilControlsChanged(hil.time_usec, hil.roll_ailerons, hil.pitch_elevator, hil.yaw_rudder, hil.throttle, hil.mode, hil.nav_mode);
        }
            break;
        case MAVLINK_MSG_ID_VFR_HUD:
        {
            mavlink_vfr_hud_t hud;
            mavlink_msg_vfr_hud_decode(&message, &hud);
            quint64 time = getUnixTime();
            // Display updated values
            emit thrustChanged(this, hud.throttle/100.0);

            if (!attitudeKnown)
            {
                setYaw(QGC::limitAngleToPMPId((((double)hud.heading)/180.0)*M_PI));
                emit attitudeChanged(this, getRoll(), getPitch(), getYaw(), time);
            }
//            setAltitudeAMSL(hud.alt);
//            setGroundSpeed(hud.groundspeed);
            if (!qIsNaN(hud.airspeed))
                setAirSpeed(hud.airspeed);

//            speedZ = -hud.climb;
//            if (!globalEstimatorActive)
//            emit altitudeChanged(this, altitudeAMSL, altitudeRelative, -speedZ, time);
//            emit speedChanged(this, groundSpeed, airSpeed, time);
        }
            break;
        case MAVLINK_MSG_ID_LOCAL_POSITION_NED:
            //std::cerr << std::endl;
            //std::cerr << "Decoded attitude message:" << " roll: " << std::dec << mavlink_msg_attitude_get_roll(message.payload) << " pitch: " << mavlink_msg_attitude_get_pitch(message.payload) << " yaw: " << mavlink_msg_attitude_get_yaw(message.payload) << std::endl;
        {
            mavlink_local_position_ned_t pos;
            mavlink_msg_local_position_ned_decode(&message, &pos);
            quint64 time = getUnixTime(pos.time_boot_ms);

            // Emit position always with component ID
            emit localPositionChanged(this, message.compid, pos.x, pos.y, pos.z, time);

            if (!wrongComponent)
            {
                setLocalX(pos.x);
                setLocalY(pos.y);
                setLocalZ(pos.z);

                speedX = pos.vx;
                speedY = pos.vy;
                speedZ = pos.vz;

                // Emit
                emit localPositionChanged(this, localX, localY, localZ, time);
                emit velocityChanged_NED(this, speedX, speedY, speedZ, time);

                // Set internal state
                if (!positionLock) {
                    // If position was not locked before, notify positive
                    GAudioOutput::instance()->notifyPositive();
                }
                positionLock = true;
                isLocalPositionKnown = true;
            }
        }
            break;
        case MAVLINK_MSG_ID_GLOBAL_VISION_POSITION_ESTIMATE:
        {
            mavlink_global_vision_position_estimate_t pos;
            mavlink_msg_global_vision_position_estimate_decode(&message, &pos);
            quint64 time = getUnixTime(pos.usec);
            emit localPositionChanged(this, message.compid, pos.x, pos.y, pos.z, time);
            emit attitudeChanged(this, message.compid, pos.roll, pos.pitch, pos.yaw, time);
        }
            break;
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
            //std::cerr << std::endl;
            //std::cerr << "Decoded attitude message:" << " roll: " << std::dec << mavlink_msg_attitude_get_roll(message.payload) << " pitch: " << mavlink_msg_attitude_get_pitch(message.payload) << " yaw: " << mavlink_msg_attitude_get_yaw(message.payload) << std::endl;
        {
            mavlink_global_position_int_t pos;
            mavlink_msg_global_position_int_decode(&message, &pos);

            quint64 time = getUnixTime();

            setLatitude(pos.lat/(double)1E7);
            setLongitude(pos.lon/(double)1E7);
            setAltitudeAMSL(pos.alt/1000.0);
            setAltitudeRelative(pos.relative_alt/1000.0);
			
            emit valueChanged(uasId,statusName().arg("Heading"),"degs",QVariant((double)pos.hdg),time);
            emit valueChanged(uasId,statusName().arg("Climb"),"m/s",QVariant((double)pos.vz / 100.0),time);

            globalEstimatorActive = true;

            speedX = pos.vx/100.0;
            speedY = pos.vy/100.0;
            speedZ = pos.vz/100.0;

            emit globalPositionChanged(this, getLatitude(), getLongitude(), getAltitudeAMSL(), time);
            emit altitudeChanged(this, altitudeAMSL, altitudeRelative, -speedZ, time);
            // We had some frame mess here, global and local axes were mixed.
            emit velocityChanged_NED(this, speedX, speedY, speedZ, time);

            setGroundSpeed(qSqrt(speedX*speedX+speedY*speedY));
            emit speedChanged(this, groundSpeed, airSpeed, time);

            // Set internal state
            if (!positionLock)
            {
                // If position was not locked before, notify positive
                GAudioOutput::instance()->notifyPositive();
            }
            positionLock = true;
            isGlobalPositionKnown = true;
            //TODO fix this hack for forwarding of global position for patch antenna tracking
            forwardMessage(message);
        }
            break;
        case MAVLINK_MSG_ID_GPS_RAW_INT:
        {
            mavlink_gps_raw_int_t pos;
            mavlink_msg_gps_raw_int_decode(&message, &pos);

            quint64 time = getUnixTime(pos.time_usec);

            emit gpsLocalizationChanged(this, pos.fix_type);
            // TODO: track localization state not only for gps but also for other loc. sources
            int loc_type = pos.fix_type;
            if (loc_type == 1)
            {
                loc_type = 0; 
            }
            emit localizationChanged(this, loc_type);
            setSatelliteCount(pos.satellites_visible);
            setGpsHdop(pos.eph/100.0);
            setGpsFix(pos.fix_type);
            setGPSVelocity(pos.vel/100.0);

            // emit raw GPS message
            setGPSLatitude(pos.lat/(double)1E7);
            setGPSLongitude(pos.lon/(double)1E7);
            setGPSAltitude(pos.alt/1000.0);
            emit valueChanged(this->uasId,statusGPS().arg("GPS COG"),"deg",QVariant(pos.cog/100.0),getUnixTime());

            if (pos.fix_type > 2)
            {
                positionLock = true;
                isGlobalPositionKnown = true;

                // If no GLOBAL_POSITION_INT messages ever received, use these raw GPS values instead.
                if (!globalEstimatorActive) {
                    setLatitude(latitude_gps);
                    setLongitude(longitude_gps);
                    setAltitudeAMSL(altitude_gps);
                    emit globalPositionChanged(this, getLatitude(), getLongitude(), getAltitudeAMSL(), time);
                    emit altitudeChanged(this, altitudeAMSL, altitudeRelative, -speedZ, time);
                
                    // Smaller than threshold and not NaN
                    if ((velocity_gps < 1000000) && !qIsNaN(velocity_gps) && !qIsInf(velocity_gps))
                    {
                        setGroundSpeed(velocity_gps);
                        emit speedChanged(this, groundSpeed, airSpeed, time);
                    }
                    else
                    {
                        emit textMessageReceived(uasId, message.compid, 255, QString("GCS ERROR: RECEIVED INVALID SPEED OF %1 m/s").arg(velocity_gps));
                    }
                }
            }
        }
            break;
        case MAVLINK_MSG_ID_GPS_STATUS:
        {
            mavlink_gps_status_t pos;
            mavlink_msg_gps_status_decode(&message, &pos);
            for(int i = 0; i < (int)pos.satellites_visible; i++)
            {
                emit gpsSatelliteStatusChanged(uasId, (unsigned char)pos.satellite_prn[i], (unsigned char)pos.satellite_elevation[i], (unsigned char)pos.satellite_azimuth[i], (unsigned char)pos.satellite_snr[i], static_cast<bool>(pos.satellite_used[i]));
            }
            setSatelliteCount(pos.satellites_visible);
        }
            break;
        case MAVLINK_MSG_ID_GPS_GLOBAL_ORIGIN:
        {
            mavlink_gps_global_origin_t pos;
            mavlink_msg_gps_global_origin_decode(&message, &pos);
            emit homePositionChanged(uasId, pos.latitude / 10000000.0, pos.longitude / 10000000.0, pos.altitude / 1000.0);
        }
            break;
        case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
        {
            mavlink_rc_channels_raw_t channels;
            mavlink_msg_rc_channels_raw_decode(&message, &channels);

            const unsigned int portWidth = 8; // XXX magic number

            emit remoteControlRSSIChanged(channels.rssi/255.0f);
            if (channels.chan1_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 0, channels.chan1_raw);
            if (channels.chan2_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 1, channels.chan2_raw);
            if (channels.chan3_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 2, channels.chan3_raw);
            if (channels.chan4_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 3, channels.chan4_raw);
            if (channels.chan5_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 4, channels.chan5_raw);
            if (channels.chan6_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 5, channels.chan6_raw);
            if (channels.chan7_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 6, channels.chan7_raw);
            if (channels.chan8_raw != UINT16_MAX)
                emit remoteControlChannelRawChanged(channels.port * portWidth + 7, channels.chan8_raw);
        }
            break;
        case MAVLINK_MSG_ID_RC_CHANNELS_SCALED:
        {
            mavlink_rc_channels_scaled_t channels;
            mavlink_msg_rc_channels_scaled_decode(&message, &channels);

            const unsigned int portWidth = 8; // XXX magic number

            emit remoteControlRSSIChanged(channels.rssi/255.0f);
            if (static_cast<uint16_t>(channels.chan1_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 0, channels.chan1_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan2_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 1, channels.chan2_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan3_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 2, channels.chan3_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan4_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 3, channels.chan4_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan5_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 4, channels.chan5_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan6_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 5, channels.chan6_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan7_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 6, channels.chan7_scaled/10000.0f);
            if (static_cast<uint16_t>(channels.chan8_scaled) != UINT16_MAX)
                emit remoteControlChannelScaledChanged(channels.port * portWidth + 7, channels.chan8_scaled/10000.0f);
        }
            break;
        case MAVLINK_MSG_ID_PARAM_VALUE:
        {
            mavlink_param_value_t rawValue;
            mavlink_msg_param_value_decode(&message, &rawValue);
            QByteArray bytes(rawValue.param_id, MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN);
            // Construct a string stopping at the first NUL (0) character, else copy the whole
            // byte array (max MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN, so safe)
            QString parameterName(bytes);
            mavlink_param_union_t paramVal;
            paramVal.param_float = rawValue.param_value;
            paramVal.type = rawValue.param_type;

            processParamValueMsg(message, parameterName,rawValue,paramVal);

        }
            break;
        case MAVLINK_MSG_ID_COMMAND_ACK:
        {
            mavlink_command_ack_t ack;
            mavlink_msg_command_ack_decode(&message, &ack);
            switch (ack.result)
            {
            case MAV_RESULT_ACCEPTED:
            {
                emit textMessageReceived(uasId, message.compid, 0, tr("SUCCESS: Executed CMD: %1").arg(ack.command));
            }
                break;
            case MAV_RESULT_TEMPORARILY_REJECTED:
            {
                emit textMessageReceived(uasId, message.compid, 0, tr("FAILURE: Temporarily rejected CMD: %1").arg(ack.command));
            }
                break;
            case MAV_RESULT_DENIED:
            {
                emit textMessageReceived(uasId, message.compid, 0, tr("FAILURE: Denied CMD: %1").arg(ack.command));
            }
                break;
            case MAV_RESULT_UNSUPPORTED:
            {
                emit textMessageReceived(uasId, message.compid, 0, tr("FAILURE: Unsupported CMD: %1").arg(ack.command));
            }
                break;
            case MAV_RESULT_FAILED:
            {
                emit textMessageReceived(uasId, message.compid, 0, tr("FAILURE: Failed CMD: %1").arg(ack.command));
            }
                break;
            }
        }
        case MAVLINK_MSG_ID_MISSION_COUNT:
        {
            mavlink_mission_count_t wpc;
            mavlink_msg_mission_count_decode(&message, &wpc);
            waypointManager.handleWaypointCount(message.sysid, message.compid, wpc.count);
        }
            break;

        case MAVLINK_MSG_ID_MISSION_ITEM:
        {
            mavlink_mission_item_t wp;
            mavlink_msg_mission_item_decode(&message, &wp);
            //QLOG_DEBUG() << "got waypoint (" << wp.seq << ") from ID " << message.sysid << " x=" << wp.x << " y=" << wp.y << " z=" << wp.z;
            waypointManager.handleWaypoint(message.sysid, message.compid, &wp);
        }
            break;

        case MAVLINK_MSG_ID_MISSION_ACK:
        {
            mavlink_mission_ack_t wpa;
            mavlink_msg_mission_ack_decode(&message, &wpa);
            waypointManager.handleWaypointAck(message.sysid, message.compid, &wpa);
        }
            break;

        case MAVLINK_MSG_ID_MISSION_REQUEST:
        {
            mavlink_mission_request_t wpr;
            mavlink_msg_mission_request_decode(&message, &wpr);
            waypointManager.handleWaypointRequest(message.sysid, message.compid, &wpr);
        }
            break;

        case MAVLINK_MSG_ID_MISSION_ITEM_REACHED:
        {
            mavlink_mission_item_reached_t wpr;
            mavlink_msg_mission_item_reached_decode(&message, &wpr);
            waypointManager.handleWaypointReached(message.sysid, message.compid, &wpr);
            QString text = QString("%1 reached waypoint %2").arg(getUASName()).arg(wpr.seq);
            GAudioOutput::instance()->say(text);
            emit textMessageReceived(message.sysid, message.compid, 0, text);
        }
            break;

        case MAVLINK_MSG_ID_MISSION_CURRENT:
        {
            mavlink_mission_current_t wpc;
            mavlink_msg_mission_current_decode(&message, &wpc);
            waypointManager.handleWaypointCurrent(message.sysid, message.compid, &wpc);
        }
            break;
        case MAVLINK_MSG_ID_STATUSTEXT:
        {
            QByteArray b;
            b.resize(MAVLINK_MSG_STATUSTEXT_FIELD_TEXT_LEN+1);
            mavlink_msg_statustext_get_text(&message, b.data());
            // Ensure NUL-termination
            b[b.length()-1] = '\0';
            QString text = QString(b);
            int severity = mavlink_msg_statustext_get_severity(&message);

            if (text.startsWith("#audio:"))
            {
                text.remove("#audio:");
                emit textMessageReceived(uasId, message.compid, severity, QString("Audio message: ") + text);
                GAudioOutput::instance()->say(text, severity);
            }
            else
            {
                emit textMessageReceived(uasId, message.compid, severity, text);
            }
        }
            break;
        case MAVLINK_MSG_ID_SERVO_OUTPUT_RAW:
        {
            mavlink_servo_output_raw_t raw;
            mavlink_msg_servo_output_raw_decode(&message, &raw);

            if (hilEnabled && raw.port == 0)
            {
                emit hilActuatorsChanged(static_cast<uint64_t>(getUnixTimeFromMs(raw.time_usec)), static_cast<float>(raw.servo1_raw),
                                     static_cast<float>(raw.servo2_raw), static_cast<float>(raw.servo3_raw),
                                     static_cast<float>(raw.servo4_raw), static_cast<float>(raw.servo5_raw), static_cast<float>(raw.servo6_raw),
                                     static_cast<float>(raw.servo7_raw), static_cast<float>(raw.servo8_raw));
            }
            emit servoRawOutputChanged(static_cast<uint64_t>(getUnixTimeFromMs(raw.time_usec)), static_cast<float>(raw.servo1_raw),
                                 static_cast<float>(raw.servo2_raw), static_cast<float>(raw.servo3_raw),
                                 static_cast<float>(raw.servo4_raw), static_cast<float>(raw.servo5_raw), static_cast<float>(raw.servo6_raw),
                                 static_cast<float>(raw.servo7_raw), static_cast<float>(raw.servo8_raw));
        }
        break;
#ifdef MAVLINK_ENABLED_PIXHAWK
        case MAVLINK_MSG_ID_DATA_TRANSMISSION_HANDSHAKE:
        {
            mavlink_data_transmission_handshake_t p;
            mavlink_msg_data_transmission_handshake_decode(&message, &p);
            imageSize = p.size;
            imagePackets = p.packets;
            imagePayload = p.payload;
            imageQuality = p.jpg_quality;
            imageType = p.type;
            imageWidth = p.width;
            imageHeight = p.height;
            imageStart = QGC::groundTimeMilliseconds();
            imagePacketsArrived = 0;

        }
            break;

        case MAVLINK_MSG_ID_ENCAPSULATED_DATA:
        {
            mavlink_encapsulated_data_t img;
            mavlink_msg_encapsulated_data_decode(&message, &img);
            int seq = img.seqnr;
            int pos = seq * imagePayload;

            // Check if we have a valid transaction
            if (imagePackets == 0)
            {
                // NO VALID TRANSACTION - ABORT
                // Restart statemachine
                imagePacketsArrived = 0;
            }

            for (int i = 0; i < imagePayload; ++i)
            {
                if (pos <= imageSize) {
                    imageRecBuffer[pos] = img.data[i];
                }
                ++pos;
            }

            ++imagePacketsArrived;

            // emit signal if all packets arrived
            if ((imagePacketsArrived >= imagePackets))
            {
                // Restart statemachine
                emit imageReady(this);
                //QLOG_DEBUG() << "imageReady emitted. all packets arrived";
            }
        }
            break;



#endif
            //        case MAVLINK_MSG_ID_OBJECT_DETECTION_EVENT:
            //        {
            //            mavlink_object_detection_event_t event;
            //            mavlink_msg_object_detection_event_decode(&message, &event);
            //            QString str(event.name);
            //            emit objectDetected(event.time, event.object_id, event.type, str, event.quality, event.bearing, event.distance);
            //        }
            //        break;
            // WILL BE ENABLED ONCE MESSAGE IS IN COMMON MESSAGE SET
            //        case MAVLINK_MSG_ID_MEMORY_VECT:
            //        {
            //            mavlink_memory_vect_t vect;
            //            mavlink_msg_memory_vect_decode(&message, &vect);
            //            QString str("mem_%1");
            //            quint64 time = getUnixTime(0);
            //            int16_t *mem0 = (int16_t *)&vect.value[0];
            //            uint16_t *mem1 = (uint16_t *)&vect.value[0];
            //            int32_t *mem2 = (int32_t *)&vect.value[0];
            //            // uint32_t *mem3 = (uint32_t *)&vect.value[0]; causes overload problem
            //            float *mem4 = (float *)&vect.value[0];
            //            if ( vect.ver == 0) vect.type = 0, vect.ver = 1; else ;
            //            if ( vect.ver == 1)
            //            {
            //                switch (vect.type) {
            //                default:
            //                case 0:
            //                    for (int i = 0; i < 16; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*2)), "i16", mem0[i], time);
            //                    break;
            //                case 1:
            //                    for (int i = 0; i < 16; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*2)), "ui16", mem1[i], time);
            //                    break;
            //                case 2:
            //                    for (int i = 0; i < 16; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*2)), "Q15", (float)mem0[i]/32767.0, time);
            //                    break;
            //                case 3:
            //                    for (int i = 0; i < 16; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*2)), "1Q14", (float)mem0[i]/16383.0, time);
            //                    break;
            //                case 4:
            //                    for (int i = 0; i < 8; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*4)), "i32", mem2[i], time);
            //                    break;
            //                case 5:
            //                    for (int i = 0; i < 8; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*4)), "i32", mem2[i], time);
            //                    break;
            //                case 6:
            //                    for (int i = 0; i < 8; i++)
            //                        // FIXME REMOVE LATER emit valueChanged(uasId, str.arg(vect.address+(i*4)), "float", mem4[i], time);
            //                    break;
            //                }
            //            }
            //        }
            //        break;
#ifdef MAVLINK_ENABLED_UALBERTA
        case MAVLINK_MSG_ID_NAV_FILTER_BIAS:
        {
            mavlink_nav_filter_bias_t bias;
            mavlink_msg_nav_filter_bias_decode(&message, &bias);
            quint64 time = getUnixTime();
            // FIXME REMOVE LATER emit valueChanged(uasId, "b_f[0]", "raw", bias.accel_0, time);
            // FIXME REMOVE LATER emit valueChanged(uasId, "b_f[1]", "raw", bias.accel_1, time);
            // FIXME REMOVE LATER emit valueChanged(uasId, "b_f[2]", "raw", bias.accel_2, time);
            // FIXME REMOVE LATER emit valueChanged(uasId, "b_w[0]", "raw", bias.gyro_0, time);
            // FIXME REMOVE LATER emit valueChanged(uasId, "b_w[1]", "raw", bias.gyro_1, time);
            // FIXME REMOVE LATER emit valueChanged(uasId, "b_w[2]", "raw", bias.gyro_2, time);
        }
            break;
        case MAVLINK_MSG_ID_RADIO_CALIBRATION:
        {
            mavlink_radio_calibration_t radioMsg;
            mavlink_msg_radio_calibration_decode(&message, &radioMsg);
            QVector<uint16_t> aileron;
            QVector<uint16_t> elevator;
            QVector<uint16_t> rudder;
            QVector<uint16_t> gyro;
            QVector<uint16_t> pitch;
            QVector<uint16_t> throttle;

            for (int i=0; i<MAVLINK_MSG_RADIO_CALIBRATION_FIELD_AILERON_LEN; ++i)
                aileron << radioMsg.aileron[i];
            for (int i=0; i<MAVLINK_MSG_RADIO_CALIBRATION_FIELD_ELEVATOR_LEN; ++i)
                elevator << radioMsg.elevator[i];
            for (int i=0; i<MAVLINK_MSG_RADIO_CALIBRATION_FIELD_RUDDER_LEN; ++i)
                rudder << radioMsg.rudder[i];
            for (int i=0; i<MAVLINK_MSG_RADIO_CALIBRATION_FIELD_GYRO_LEN; ++i)
                gyro << radioMsg.gyro[i];
            for (int i=0; i<MAVLINK_MSG_RADIO_CALIBRATION_FIELD_PITCH_LEN; ++i)
                pitch << radioMsg.pitch[i];
            for (int i=0; i<MAVLINK_MSG_RADIO_CALIBRATION_FIELD_THROTTLE_LEN; ++i)
                throttle << radioMsg.throttle[i];

            QPointer<RadioCalibrationData> radioData = new RadioCalibrationData(aileron, elevator, rudder, gyro, pitch, throttle);
            emit radioCalibrationReceived(radioData);
            delete radioData;
        }
            break;

#endif
        case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
        {
            //mavlink_set_local_position_setpoint_t p;
            //mavlink_msg_set_local_position_setpoint_decode(&message, &p);
            //emit userPositionSetPointsChanged(uasId, p.x, p.y, p.z, p.yaw);
            mavlink_nav_controller_output_t p;
            mavlink_msg_nav_controller_output_decode(&message,&p);
            setDistToWaypoint(p.wp_dist);
            setBearingToWaypoint(p.nav_bearing);
            //setAltitudeError(p.alt_error);
            //setSpeedError(p.aspd_error);
            //setCrosstrackingError(p.xtrack_error);
            emit navigationControllerErrorsChanged(this, p.alt_error, p.aspd_error, p.xtrack_error);
        }
            break;

        case MAVLINK_MSG_ID_RAW_IMU:
        {
            mavlink_raw_imu_t rawImu;
            mavlink_msg_raw_imu_decode(&message, &rawImu);
            emit rawImuMessageUpdate(this, rawImu);
        }
            break;
        case MAVLINK_MSG_ID_SENSOR_OFFSETS:
        {
            mavlink_sensor_offsets_t sensorOffsets;
            mavlink_msg_sensor_offsets_decode(&message, &sensorOffsets);
            emit sensorOffsetsMessageUpdate(this, sensorOffsets);
        }
            break;
        case MAVLINK_MSG_ID_RADIO:
        {
            quint64 time = getUnixTime();
            mavlink_radio_t radio;
            mavlink_msg_radio_decode(&message, &radio);
            emit radioMessageUpdate(this, radio);

            emit valueChanged(uasId, statusName().arg("Radio RSSI"), "", radio.rssi, time);
            emit valueChanged(uasId, statusName().arg("Radio REM RSSI"), "", radio.remrssi, time);
            emit valueChanged(uasId, statusName().arg("Radio noise"), "", radio.noise, time);
            emit valueChanged(uasId, statusName().arg("Radio REM noise"), "", radio.remnoise, time);
        }
            break;
        // MAVLink Log donwload messages
        case MAVLINK_MSG_ID_LOG_ENTRY:
        {
            // we have revceived a log entry from the MAV
            mavlink_log_entry_t log_entry;
            mavlink_msg_log_entry_decode(&message, &log_entry);
            emit logEntry(uasId, log_entry.time_utc, log_entry.size, log_entry.id, log_entry.num_logs, log_entry.last_log_num);
        }
            break;
        case MAVLINK_MSG_ID_LOG_DATA:
        {
            //data that is part of a paticular log
            mavlink_log_data_t log_data;
            mavlink_msg_log_data_decode(&message, &log_data);
            emit logData(uasId, log_data.ofs, log_data.id, log_data.count, (const char*)log_data.data);
        }
            break;
        case MAVLINK_MSG_ID_COMPASSMOT_STATUS:
        {
            // Configuration Messages for Compass Calibration
            mavlink_compassmot_status_t compassmot_status;
            mavlink_msg_compassmot_status_decode(&message, &compassmot_status);
            emit compassMotCalibration(&compassmot_status);
        }
            break;
        case MAVLINK_MSG_ID_SCALED_IMU:
        {
            mavlink_scaled_imu_t scaledImu;
            mavlink_msg_scaled_imu_decode(&message, &scaledImu);
            emit scaledImuMessageUpdate(this, scaledImu);
        }
            break;
        case MAVLINK_MSG_ID_SCALED_IMU2:
        {
            mavlink_scaled_imu2_t scaledImu2;
            mavlink_msg_scaled_imu2_decode(&message, &scaledImu2);
            emit scaledImu2MessageUpdate(this, scaledImu2);
        }
            break;
        case MAVLINK_MSG_ID_RANGEFINDER:
        {
            mavlink_rangefinder_t rangeFinder;
            mavlink_msg_rangefinder_decode(&message, &rangeFinder);
            emit rangeFinderUpdate(this, rangeFinder.distance, rangeFinder.voltage);
        }
            break;

        case MAVLINK_MSG_ID_TERRAIN_REQUEST:
        {
            mavlink_terrain_request_t terrainRequest;
            mavlink_msg_terrain_request_decode(&message, &terrainRequest);

            QLOG_INFO() << QString("Terrain Request: lat%1,lon%2 %3").arg(terrainRequest.lat).arg(terrainRequest.lon).arg(terrainRequest.grid_spacing);
        }

        // Messages to ignore
        case MAVLINK_MSG_ID_RAW_PRESSURE:
        case MAVLINK_MSG_ID_SCALED_PRESSURE:
        case MAVLINK_MSG_ID_OPTICAL_FLOW:
        case MAVLINK_MSG_ID_DEBUG_VECT:
        case MAVLINK_MSG_ID_DEBUG:
        case MAVLINK_MSG_ID_NAMED_VALUE_FLOAT:
        case MAVLINK_MSG_ID_NAMED_VALUE_INT:
        case MAVLINK_MSG_ID_MANUAL_CONTROL:
        case MAVLINK_MSG_ID_HIGHRES_IMU:
            break;
        default:
        {
            if (!unknownPackets.contains(message.msgid))
            {
                unknownPackets.append(message.msgid);
#ifdef QT_DEBUG // Remove these messages from the release build
                QString errString = tr("UNABLE TO DECODE MESSAGE NUMBER %1").arg(message.msgid);
                emit textMessageReceived(uasId, message.compid, 255, errString);
#endif
                QLOG_INFO() << "Unable to decode message from system " << message.sysid
                            << " with message id:" << message.msgid;
            }
        }
            break;
        }
    }
    emit mavlinkMessageRecieved(link,message);
}


#if defined(QGC_PROTOBUF_ENABLED)
/**
* Receive an extended message.
* @param link
* @param message
*/
void UAS::receiveExtendedMessage(LinkInterface* link, std::tr1::shared_ptr<google::protobuf::Message> message)
{
    if (!link)
    {
        return;
    }
    if (!links->contains(link))
    {
        addLink(link);
    }

    const google::protobuf::Descriptor* descriptor = message->GetDescriptor();
    if (!descriptor)
    {
        return;
    }

    const google::protobuf::FieldDescriptor* headerField = descriptor->FindFieldByName("header");
    if (!headerField)
    {
        return;
    }

    const google::protobuf::Descriptor* headerDescriptor = headerField->message_type();
    if (!headerDescriptor)
    {
        return;
    }

    const google::protobuf::FieldDescriptor* sourceSysIdField = headerDescriptor->FindFieldByName("source_sysid");
    if (!sourceSysIdField)
    {
        return;
    }

    const google::protobuf::Reflection* reflection = message->GetReflection();
    const google::protobuf::Message& headerMsg = reflection->GetMessage(*message, headerField);
    const google::protobuf::Reflection* headerReflection = headerMsg.GetReflection();

    int source_sysid = headerReflection->GetInt32(headerMsg, sourceSysIdField);

    if (source_sysid != uasId)
    {
        return;
    }

#ifdef QGC_USE_PIXHAWK_MESSAGES
    if (message->GetTypeName() == overlay.GetTypeName())
    {
        receivedOverlayTimestamp = QGC::groundTimeSeconds();
        overlayMutex.lock();
        overlay.CopyFrom(*message);
        overlayMutex.unlock();
        emit overlayChanged(this);
    }
    else if (message->GetTypeName() == obstacleList.GetTypeName())
    {
        receivedObstacleListTimestamp = QGC::groundTimeSeconds();
        obstacleListMutex.lock();
        obstacleList.CopyFrom(*message);
        obstacleListMutex.unlock();
        emit obstacleListChanged(this);
    }
    else if (message->GetTypeName() == path.GetTypeName())
    {
        receivedPathTimestamp = QGC::groundTimeSeconds();
        pathMutex.lock();
        path.CopyFrom(*message);
        pathMutex.unlock();
        emit pathChanged(this);
    }
    else if (message->GetTypeName() == pointCloud.GetTypeName())
    {
        receivedPointCloudTimestamp = QGC::groundTimeSeconds();
        pointCloudMutex.lock();
        pointCloud.CopyFrom(*message);
        pointCloudMutex.unlock();
        emit pointCloudChanged(this);
    }
    else if (message->GetTypeName() == rgbdImage.GetTypeName())
    {
        receivedRGBDImageTimestamp = QGC::groundTimeSeconds();
        rgbdImageMutex.lock();
        rgbdImage.CopyFrom(*message);
        rgbdImageMutex.unlock();
        emit rgbdImageChanged(this);
    }
#endif
}

#endif

/**
* Set the home position of the UAS.
* @param lat The latitude fo the home position
* @param lon The longitude of the home position
* @param alt The altitude of the home position
*/
void UAS::setHomePosition(double lat, double lon, double alt)
{
    if (blockHomePositionChanges)
        return;

    QString uasName = (getUASName() == "")?
                tr("UAS") + QString::number(getUASID())
              : getUASName();

    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText(tr("Set a new home position for vehicle %1").arg(uasName));
    msgBox.setInformativeText("Do you want to set a new origin? Waypoints defined in the local frame will be shifted in their physical location");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    // Close the message box shortly after the click to prevent accidental clicks
    QTimer::singleShot(5000, &msgBox, SLOT(reject()));


    if (ret == QMessageBox::Yes)
    {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(systemId, componentId, &msg, this->getUASID(), 0, MAV_CMD_DO_SET_HOME, 1, 0, 0, 0, 0, lat, lon, alt);
        // Send message twice to increase chance that it reaches its goal
        sendMessage(msg);

        // Send new home position to UAS
        mavlink_set_gps_global_origin_t home;
        home.target_system = uasId;
        home.latitude = lat*1E7;
        home.longitude = lon*1E7;
        home.altitude = alt*1000;
        QLOG_DEBUG() << "lat:" << home.latitude << " lon:" << home.longitude;
        mavlink_msg_set_gps_global_origin_encode(systemId, componentId, &msg, &home);
        sendMessage(msg);
    } else {
        blockHomePositionChanges = true;
    }
}

/**
* Set the origin to the current GPS location.
**/
void UAS::setLocalOriginAtCurrentGPSPosition()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.setText("Set the home position at the current GPS position?");
    msgBox.setInformativeText("Do you want to set a new origin? Waypoints defined in the local frame will be shifted in their physical location");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    // Close the message box shortly after the click to prevent accidental clicks
    QTimer::singleShot(5000, &msgBox, SLOT(reject()));


    if (ret == QMessageBox::Yes)
    {
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(systemId, componentId, &msg, this->getUASID(), 0, MAV_CMD_DO_SET_HOME, 1, 1, 0, 0, 0, 0, 0, 0);
        // Send message twice to increase chance that it reaches its goal
        sendMessage(msg);
    }
}

/**
* Set a local position setpoint.
* @param x postion
* @param y position
* @param z position
*/
void UAS::setLocalPositionSetpoint(float x, float y, float z, float yaw)
{
#ifdef MAVLINK_ENABLED_PIXHAWK
    mavlink_message_t msg;
    mavlink_msg_set_local_position_setpoint_pack(systemId, componentId, &msg, uasId, 0, MAV_FRAME_LOCAL_NED, x, y, z, yaw/M_PI*180.0);
    sendMessage(msg);
#else
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
    Q_UNUSED(yaw);
#endif
}

/**
* Set a offset of the local position.
* @param x position
* @param y position
* @param z position
* @param yaw
*/
void UAS::setLocalPositionOffset(float x, float y, float z, float yaw)
{
#ifdef MAVLINK_ENABLED_PIXHAWK
    mavlink_message_t msg;
    mavlink_msg_set_position_control_offset_pack(systemId, componentId, &msg, uasId, 0, x, y, z, yaw);
    sendMessage(msg);
#else
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(z);
    Q_UNUSED(yaw);
#endif
}

void UAS::startRadioControlCalibration(int param)
{
    mavlink_message_t msg;
    // Param 1: gyro cal, param 2: mag cal, param 3: pressure cal, Param 4: radio
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_PREFLIGHT_CALIBRATION, 1, 0, 0, 0, param, 0, 0, 0);
    sendMessage(msg);
}

void UAS::endRadioControlCalibration()
{
    mavlink_message_t msg;
    // Param 1: gyro cal, param 2: mag cal, param 3: pressure cal, Param 4: radio
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_PREFLIGHT_CALIBRATION, 1, 0, 0, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

void UAS::startDataRecording()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_DO_CONTROL_VIDEO, 1, -1, -1, -1, 2, 0, 0, 0);
    sendMessage(msg);
}

void UAS::stopDataRecording()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_DO_CONTROL_VIDEO, 1, -1, -1, -1, 0, 0, 0, 0);
    sendMessage(msg);
}

void UAS::startMagnetometerCalibration()
{
    mavlink_message_t msg;
    // Param 1: gyro cal, param 2: mag cal, param 3: pressure cal, Param 4: radio
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_IMU, MAV_CMD_PREFLIGHT_CALIBRATION, 1, 0, 1, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

void UAS::startGyroscopeCalibration()
{
    mavlink_message_t msg;
    // Param 1: gyro cal, param 2: mag cal, param 3: pressure cal, Param 4: radio
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_IMU, MAV_CMD_PREFLIGHT_CALIBRATION, 1, 1, 0, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

void UAS::startPressureCalibration()
{
    mavlink_message_t msg;
    // Param 1: gyro cal, param 2: mag cal, param 3: pressure cal, Param 4: radio
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_IMU, MAV_CMD_PREFLIGHT_CALIBRATION, 1, 0, 0, 1, 0, 0, 0, 0);
    sendMessage(msg);
}

void UAS::startCompassMotCalibration()
{
    mavlink_message_t msg;
    // Param 1: gyro cal, param 2: mag cal, param 3: pressure cal, Param 4: radio, Param5: Accel Calib Param 6: Compass Mot
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_PREFLIGHT_CALIBRATION, 1, 0, 0, 0, 0, 0, 1, 0);
    sendMessage(msg);
}

/**
* Check if time is smaller than 40 years, assuming no system without Unix
* timestamp runs longer than 40 years continuously without reboot. In worst case
* this will add/subtract the communication delay between GCS and MAV, it will
* never alter the timestamp in a safety critical way.
*/
quint64 UAS::getUnixReferenceTime(quint64 time)
{
    // Same as getUnixTime, but does not react to attitudeStamped mode
    if (time == 0)
    {
        //        QLOG_DEBUG() << "XNEW time:" <<QGC::groundTimeMilliseconds();
        return QGC::groundTimeMilliseconds();
    }
    // Check if time is smaller than 40 years,
    // assuming no system without Unix timestamp
    // runs longer than 40 years continuously without
    // reboot. In worst case this will add/subtract the
    // communication delay between GCS and MAV,
    // it will never alter the timestamp in a safety
    // critical way.
    //
    // Calculation:
    // 40 years
    // 365 days
    // 24 hours
    // 60 minutes
    // 60 seconds
    // 1000 milliseconds
    // 1000 microseconds
#ifndef _MSC_VER
    else if (time < 1261440000000000LLU)
#else
    else if (time < 1261440000000000)
#endif
    {
        //        QLOG_DEBUG() << "GEN time:" << time/1000 + onboardTimeOffset;
        if (onboardTimeOffset == 0)
        {
            onboardTimeOffset = QGC::groundTimeMilliseconds() - time/1000;
        }
        return time/1000 + onboardTimeOffset;
    }
    else
    {
        // Time is not zero and larger than 40 years -> has to be
        // a Unix epoch timestamp. Do nothing.
        return time/1000;
    }
}

/**
* @warning If attitudeStamped is enabled, this function will not actually return
* the precise time stamp of this measurement augmented to UNIX time, but will
* MOVE the timestamp IN TIME to match the last measured attitude. There is no
* reason why one would want this, except for system setups where the onboard
* clock is not present or broken and datasets should be collected that are still
* roughly synchronized. PLEASE NOTE THAT ENABLING ATTITUDE STAMPED RUINS THE
* SCIENTIFIC NATURE OF THE CORRECT LOGGING FUNCTIONS OF QGROUNDCONTROL!
*/
quint64 UAS::getUnixTimeFromMs(quint64 time)
{
    return getUnixTime(time*1000);
}

/**
* @warning If attitudeStamped is enabled, this function will not actually return
* the precise time stam of this measurement augmented to UNIX time, but will
* MOVE the timestamp IN TIME to match the last measured attitude. There is no
* reason why one would want this, except for system setups where the onboard
* clock is not present or broken and datasets should be collected that are
* still roughly synchronized. PLEASE NOTE THAT ENABLING ATTITUDE STAMPED
* RUINS THE SCIENTIFIC NATURE OF THE CORRECT LOGGING FUNCTIONS OF QGROUNDCONTROL!
*/
quint64 UAS::getUnixTime(quint64 time)
{
    quint64 ret = 0;
    if (attitudeStamped)
    {
        ret = lastAttitude;
    }

    if (time == 0)
    {
        ret = QGC::groundTimeMilliseconds();
    }
    // Check if time is smaller than 40 years,
    // assuming no system without Unix timestamp
    // runs longer than 40 years continuously without
    // reboot. In worst case this will add/subtract the
    // communication delay between GCS and MAV,
    // it will never alter the timestamp in a safety
    // critical way.
    //
    // Calculation:
    // 40 years
    // 365 days
    // 24 hours
    // 60 minutes
    // 60 seconds
    // 1000 milliseconds
    // 1000 microseconds
#ifndef _MSC_VER
    else if (time < 1261440000000000LLU)
#else
    else if (time < 1261440000000000)
#endif
    {
        //        QLOG_DEBUG() << "GEN time:" << time/1000 + onboardTimeOffset;
        if (onboardTimeOffset == 0 || time < (lastNonNullTime - 100))
        {
            lastNonNullTime = time;
            onboardTimeOffset = QGC::groundTimeMilliseconds() - time/1000;
        }
        if (time > lastNonNullTime) lastNonNullTime = time;

        ret = time/1000 + onboardTimeOffset;
    }
    else
    {
        // Time is not zero and larger than 40 years -> has to be
        // a Unix epoch timestamp. Do nothing.
        ret = time/1000;
    }

    return ret;
}

/**
* @param component that will be searched for in the map of parameters.
*/
QList<QString> UAS::getParameterNames(int component)
{
    if (parameters.contains(component))
    {
        return parameters.value(component)->keys();
    }
    else
    {
        return QList<QString>();
    }
}

QList<int> UAS::getComponentIds()
{
    return parameters.keys();
}

void UAS::setMode(int mode)
{
    //this->mode = mode; //no call assignament, update receive message from UAS

    // Strip armed / disarmed call, this is not relevant for setting the mode
    uint8_t newMode = mode;
    newMode &= (~(uint8_t)MAV_MODE_FLAG_SAFETY_ARMED);
    // Now set current state (request no change)
    newMode |= (uint8_t)(this->base_mode) & (uint8_t)(MAV_MODE_FLAG_SAFETY_ARMED);

    // Strip HIL part, replace it with current system state
    newMode &= (~(uint8_t)MAV_MODE_FLAG_HIL_ENABLED);
    // Now set current state (request no change)
    newMode |= (uint8_t)(this->base_mode) & (uint8_t)(MAV_MODE_FLAG_HIL_ENABLED);

    QLOG_DEBUG() << "SENDING REQUEST TO SET MODE TO SYSTEM" << uasId << ", REQUEST TO SET MODE " << newMode;

    mavlink_message_t msg;
    mavlink_msg_set_mode_pack(systemId, componentId, &msg, (uint8_t)uasId, newMode, (uint16_t)custom_mode);
    sendMessage(msg);

}

/**
* @param newBaseMode that UAS is to be set to.
* @param newCustomMode that UAS is to be set to.
*/
void UAS::setMode(uint8_t newBaseMode, uint32_t newCustomMode)
{
    if (receivedMode)
    {
        //base_mode = mode; //no call assignament, update receive message from UAS

        // Strip armed / disarmed call for safety reasons, this is not relevant for setting the mode
        newBaseMode &= ~MAV_MODE_FLAG_SAFETY_ARMED;
        // Now set current state (request no change)
        newBaseMode |= this->base_mode & MAV_MODE_FLAG_SAFETY_ARMED;

//        // Strip HIL part, replace it with current system state
//        newBaseMode &= (~MAV_MODE_FLAG_HIL_ENABLED);
//        // Now set current state (request no change)
//        newBaseMode |= this->base_mode & MAV_MODE_FLAG_HIL_ENABLED;

        setModeArm(newBaseMode, newCustomMode);
    }
    else
    {
        qDebug() << "WARNING: setMode called before base_mode bitmask was received from UAS, new mode was not sent to system";
    }
}

/**
* @param newBaseMode that UAS is to be set to.
* @param newCustomMode that UAS is to be set to.
*/
void UAS::setModeArm(uint8_t newBaseMode, uint32_t newCustomMode)
{
    if (receivedMode)
    {
        mavlink_message_t msg;
        mavlink_msg_set_mode_pack(systemId, componentId, &msg, (uint8_t)uasId, newBaseMode, newCustomMode);
        qDebug() << "mavlink_msg_set_mode_pack 1";
        sendMessage(msg);
        qDebug() << "SENDING REQUEST TO SET MODE TO SYSTEM" << uasId << ", MODE " << newBaseMode << " " << newCustomMode;
    }
    else
    {
        qDebug() << "WARNING: setModeArm called before base_mode bitmask was received from UAS, new mode was not sent to system";
    }
}

/**
* Send a message to every link that is connected.
* @param message that is to be sent
*/
void UAS::sendMessage(mavlink_message_t message)
{
    if (!LinkManager::instance())
    {
        QLOG_WARN() << "LINKMANAGER NOT AVAILABLE!";
        return;
    }

    if (links->count() < 1) {
        QLOG_WARN() << "NO LINK AVAILABLE TO SEND!";
    }

    // Emit message on all links that are currently connected
    foreach (LinkInterface* link, *links)
    {
        //if (LinkManager::instance()->getLinks().contains(link))
        //{
            if (link->isConnected())
                sendMessage(link, message);
        //}
        //else
        //{
        //    // Remove from list
        //    links->removeAt(links->indexOf(link));
        //}
    }
}

/**
* Forward a message to all links that are currently connected.
* @param message that is to be forwarded
*/
void UAS::forwardMessage(mavlink_message_t message)
{
    Q_UNUSED(message);
    // Emit message on all links that are currently connected
    /*QList<LinkInterface*>link_list = LinkManager::instance()->getLinksForProtocol(mavlink);

    foreach(LinkInterface* link, link_list)
    {
        if (link)
        {
            SerialLink* serial = dynamic_cast<SerialLink*>(link); //SeriakLink Deprecated use SerialConnection
            if(serial != 0)
            {
                for(int i=0; i<links->size(); i++)
                {
                    if(serial != links->at(i))
                    {
                        if (link->isConnected()) {
                            QLOG_TRACE()<<"Antenna tracking: Forwarding Over link: "<<serial->getName()<<" "<<serial;
                            sendMessage(serial, message);
                        }
                    }
                }
            }
        }
    }*/
}


void UAS::sendHeartbeat()
{
    if (m_heartbeatsEnabled)
    {
        mavlink_message_t beat;
        mavlink_msg_heartbeat_pack(getSystemId(), getComponentId(),&beat, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, MAV_MODE_MANUAL_ARMED, 0, MAV_STATE_ACTIVE);
        sendMessage(beat);
    }
}



/**
* Send a message to the link that is connected.
* @param link that the message will be sent to
* @message that is to be sent
*/
void UAS::sendMessage(LinkInterface* link, mavlink_message_t message)
{
    if(!link) return;
    // Create buffer
    uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
    // Write message into buffer, prepending start sign
    int len = mavlink_msg_to_send_buffer(buffer, &message);
    static uint8_t messageKeys[256] = MAVLINK_MESSAGE_CRCS;
    mavlink_finalize_message_chan(&message, systemId, componentId, link->getId(), 0, message.len, messageKeys[message.msgid]);

    // If link is connected
    if (link->isConnected())
    {
        // Send the portion of the buffer now occupied by the message
        link->writeBytes((const char*)buffer, len);
    }
    else
    {
        QLOG_ERROR() << "LINK NOT CONNECTED, NOT SENDING!";
    }
}

/**
 * @param value battery voltage
 */
double UAS::filterVoltage(double value) const
{
    return lpVoltage * 0.6f + value * 0.4f;
}

/**
* The string representation of the custom_mode.
* @Return string
*/
QString UAS::getCustomModeText()
{
    return QString("UNKNOWN");
}

/**
* The audio string representation of the custom_mode.
* @Return string
*/
QString UAS::getCustomModeAudioText()
{
    QString customModeString = tr("changed custom mode to ");
    return customModeString + getCustomModeText();
}

/** 
* Get the status of the code and a description of the status.
* Status can be unitialized, booting up, calibrating sensors, active
* standby, cirtical, emergency, shutdown or unknown.
*/
void UAS::getStatusForCode(int statusCode, QString& uasState, QString& stateDescription)
{
    switch (statusCode)
    {
    case MAV_STATE_UNINIT:
        uasState = tr("UNINIT");
        stateDescription = tr("Unitialized, booting up.");
        break;
    case MAV_STATE_BOOT:
        uasState = tr("BOOT");
        stateDescription = tr("Booting system, please wait.");
        break;
    case MAV_STATE_CALIBRATING:
        uasState = tr("CALIBRATING");
        stateDescription = tr("Calibrating sensors, please wait.");
        break;
    case MAV_STATE_ACTIVE:
        uasState = tr("ACTIVE");
        stateDescription = tr("Active, normal operation.");
        break;
    case MAV_STATE_STANDBY:
        uasState = tr("STANDBY");
        stateDescription = tr("Standby mode, ready for launch.");
        break;
    case MAV_STATE_CRITICAL:
        uasState = tr("CRITICAL");
        stateDescription = tr("FAILURE: Continuing operation.");
        break;
    case MAV_STATE_EMERGENCY:
        uasState = tr("EMERGENCY");
        stateDescription = tr("EMERGENCY: Land Immediately!");
        break;
        //case MAV_STATE_HILSIM:
        //uasState = tr("HIL SIM");
        //stateDescription = tr("HIL Simulation, Sensors read from SIM");
        //break;

    case MAV_STATE_POWEROFF:
        uasState = tr("SHUTDOWN");
        stateDescription = tr("Powering off system.");
        break;

    default:
        uasState = tr("UNKNOWN");
        stateDescription = tr("Unknown system state");
        break;
    }
}

QImage UAS::getImage()
{
#ifdef MAVLINK_ENABLED_PIXHAWK

//    QLOG_DEBUG() << "IMAGE TYPE:" << imageType;

    // RAW greyscale
    if (imageType == MAVLINK_DATA_STREAM_IMG_RAW8U)
    {
        int imgColors = 255;

        // Construct PGM header
        QString header("P5\n%1 %2\n%3\n");
        header = header.arg(imageWidth).arg(imageHeight).arg(imgColors);

        QByteArray tmpImage(header.toStdString().c_str(), header.toStdString().size());
        tmpImage.append(imageRecBuffer);

        //QLOG_DEBUG() << "IMAGE SIZE:" << tmpImage.size() << "HEADER SIZE: (15):" << header.size() << "HEADER: " << header;

        if (imageRecBuffer.isNull())
        {
            QLOG_DEBUG()<< "could not convertToPGM()";
            return QImage();
        }

        if (!image.loadFromData(tmpImage, "PGM"))
        {
            QLOG_DEBUG()<< __FILE__ << __LINE__ << "could not create extracted image";
            return QImage();
        }

    }
    // BMP with header
    else if (imageType == MAVLINK_DATA_STREAM_IMG_BMP ||
             imageType == MAVLINK_DATA_STREAM_IMG_JPEG ||
             imageType == MAVLINK_DATA_STREAM_IMG_PGM ||
             imageType == MAVLINK_DATA_STREAM_IMG_PNG)
    {
        if (!image.loadFromData(imageRecBuffer))
        {
            QLOG_DEBUG() << __FILE__ << __LINE__ << "Loading data from image buffer failed!";
        }
    }
    // Restart statemachine
    imagePacketsArrived = 0;
    //imageRecBuffer.clear();
    return image;
#else
    return QImage();
#endif

}

void UAS::requestImage()
{
#ifdef MAVLINK_ENABLED_PIXHAWK
    QLOG_DEBUG() << "trying to get an image from the uas...";

    // check if there is already an image transmission going on
    if (imagePacketsArrived == 0)
    {
        mavlink_message_t msg;
        mavlink_msg_data_transmission_handshake_pack(systemId, componentId, &msg, MAVLINK_DATA_STREAM_IMG_JPEG, 0, 0, 0, 0, 0, 50);
        sendMessage(msg);
    }
#endif
}


/* MANAGEMENT */

/**
 *
 * @return The uptime in milliseconds
 *
 */
quint64 UAS::getUptime() const
{
    if(startTime == 0)
    {
        return 0;
    }
    else
    {
        return QGC::groundTimeMilliseconds() - startTime;
    }
}

int UAS::getCommunicationStatus() const
{
    return commStatus;
}

void UAS::requestParameters()
{
    mavlink_message_t msg;
    mavlink_msg_param_request_list_pack(systemId, componentId, &msg, this->getUASID(), MAV_COMP_ID_PRIMARY);
    sendMessage(msg);
    QLOG_DEBUG() << __FILE__ << __LINE__ << "LOADING PARAM LIST";
}

void UAS::writeParametersToStorage()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_PREFLIGHT_STORAGE, 1, 1, -1, -1, -1, 0, 0, 0);
    QLOG_DEBUG() << "SENT COMMAND" << MAV_CMD_PREFLIGHT_STORAGE;
    sendMessage(msg);
}

void UAS::readParametersFromStorage()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, 0, MAV_CMD_PREFLIGHT_STORAGE, 1, 0, -1, -1, -1, 0, 0, 0);
    sendMessage(msg);
}




/**
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableAllDataTransmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    // 0 is a magic ID and will enable/disable the standard message set except for heartbeat
    stream.req_stream_id = MAV_DATA_STREAM_ALL;
    // Select the update rate in Hz the message should be send
    // All messages will be send with their default rate
    // TODO: use 0 to turn off and get rid of enable/disable? will require
    //  a different magic flag for turning on defaults, possibly something really high like 1111 ?
    stream.req_message_rate = 0;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableRawSensorDataTransmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_RAW_SENSORS;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableExtendedSystemStatusTransmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_EXTENDED_STATUS;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableRCChannelDataTransmission(int rate)
{
#if defined(MAVLINK_ENABLED_UALBERTA_MESSAGES)
    mavlink_message_t msg;
    mavlink_msg_request_rc_channels_pack(systemId, componentId, &msg, enabled);
    sendMessage(msg);
#else
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_RC_CHANNELS;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
#endif
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableRawControllerDataTransmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_RAW_CONTROLLER;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
}

//void UAS::enableRawSensorFusionTransmission(int rate)
//{
//    // Buffers to write data to
//    mavlink_message_t msg;
//    mavlink_request_data_stream_t stream;
//    // Select the message to request from now on
//    stream.req_stream_id = MAV_DATA_STREAM_RAW_SENSOR_FUSION;
//    // Select the update rate in Hz the message should be send
//    stream.req_message_rate = rate;
//    // Start / stop the message
//    stream.start_stop = (rate) ? 1 : 0;
//    // The system which should take this command
//    stream.target_system = uasId;
//    // The component / subsystem which should take this command
//    stream.target_component = 0;
//    // Encode and send the message
//    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
//    // Send message twice to increase chance of reception
//    sendMessage(msg);
//    sendMessage(msg);
//}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enablePositionTransmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_POSITION;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableExtra1Transmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_EXTRA1;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
    sendMessage(msg);
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableExtra2Transmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_EXTRA2;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
    sendMessage(msg);
}

/** 
* @param rate The update rate in Hz the message should be sent
*/
void UAS::enableExtra3Transmission(int rate)
{
    // Buffers to write data to
    mavlink_message_t msg;
    mavlink_request_data_stream_t stream;
    // Select the message to request from now on
    stream.req_stream_id = MAV_DATA_STREAM_EXTRA3;
    // Select the update rate in Hz the message should be send
    stream.req_message_rate = rate;
    // Start / stop the message
    stream.start_stop = (rate) ? 1 : 0;
    // The system which should take this command
    stream.target_system = uasId;
    // The component / subsystem which should take this command
    stream.target_component = 0;
    // Encode and send the message
    mavlink_msg_request_data_stream_encode(systemId, componentId, &msg, &stream);
    // Send message twice to increase chance of reception
    sendMessage(msg);
    sendMessage(msg);
}

/**
 * Set a parameter value onboard
 *
 * @param component The component to set the parameter
 * @param id Name of the parameter 
 */
void UAS::setParameter(const int compId, const QString& paramId, const QVariant& value)
{
    if (!paramId.isNull())
    {
        mavlink_message_t msg;
        mavlink_param_set_t p;
        mavlink_param_union_t union_value;

        // Assign correct value based on QVariant
        // TODO: This is a hack for MAV_AUTOPILOT_ARDUPILOTMEGA until the new version of MAVLink and a fix for their param handling.
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA)
        {
            switch (static_cast<QMetaType::Type>(value.type()))
            {
            case QMetaType::QChar:
                union_value.param_float = static_cast<char>(value.toChar().toLatin1());
                p.param_type = MAV_PARAM_TYPE_INT8;
                break;
            case QMetaType::Int:
                union_value.param_float = value.toInt();
                p.param_type = MAV_PARAM_TYPE_INT32;
                break;
            case QMetaType::UInt:
                union_value.param_float = value.toUInt();
                p.param_type = MAV_PARAM_TYPE_UINT32;
                break;
            case QMetaType::Double:
            case QMetaType::Float:
                union_value.param_float = value.toFloat();
                p.param_type = MAV_PARAM_TYPE_REAL32;
                break;
            default:
                QLOG_ERROR() << "ABORTED PARAM SEND, NO VALID QVARIANT TYPE:" << paramId << "TYPE IS:" << value.type();
                return;
            }
        }
        else
        {
            switch (static_cast<QMetaType::Type>(value.type()))
            {
            case QMetaType::QChar:
                union_value.param_int8 = static_cast<unsigned char>(value.toChar().toLatin1());
                p.param_type = MAV_PARAM_TYPE_INT8;
                break;
            case QMetaType::Int:
                union_value.param_int32 = value.toInt();
                p.param_type = MAV_PARAM_TYPE_INT32;
                break;
            case QMetaType::UInt:
                union_value.param_uint32 = value.toUInt();
                p.param_type = MAV_PARAM_TYPE_UINT32;
                break;
            case QMetaType::Double:
            case QMetaType::Float:
                union_value.param_float = value.toFloat();
                p.param_type = MAV_PARAM_TYPE_REAL32;
                break;
            default:
                QLOG_ERROR() << "ABORTED PARAM SEND, NO VALID QVARIANT TYPE:" << paramId << "TYPE IS:" << value.type();
                return;
            }
        }

        p.param_value = union_value.param_float;
        p.target_system = (uint8_t)uasId;
        p.target_component = (uint8_t)compId;

        //QLOG_DEBUG() << "SENT PARAM:" << value;

        // Copy string into buffer, ensuring not to exceed the buffer size
        for (unsigned int i = 0; i < sizeof(p.param_id); i++)
        {
            // String characters
            if ((int)i < paramId.length())
            {
                p.param_id[i] = paramId.toLatin1()[i];
            }
            else
            {
                // Fill rest with zeros
                p.param_id[i] = 0;
            }
        }
        mavlink_msg_param_set_encode(systemId, compId, &msg, &p);
        sendMessage(msg);
    }
}




//TODO update this to use the parameter manager / param data model instead
void UAS::processParamValueMsg(mavlink_message_t& msg, const QString& paramName, const mavlink_param_value_t& rawValue,  mavlink_param_union_t& paramValue)
{
    int compId = msg.compid;

    // Insert component if necessary
    if (!parameters.contains(compId)) {
        parameters.insert(compId, new QMap<QString, QVariant>());
    }

    // Insert parameter into registry
    if (parameters.value(compId)->contains(paramName)) {
        parameters.value(compId)->remove(paramName);
    }

    QVariant param;

    // Insert with correct type
    // TODO: This is a hack for MAV_AUTOPILOT_ARDUPILOTMEGA until the new version of MAVLink and a fix for their param handling.
    switch (rawValue.param_type)
    {
    case MAV_PARAM_TYPE_REAL32:
    {
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA) {
            param = QVariant(static_cast<double>(paramValue.param_float));
        }
        else {
            param = QVariant(paramValue.param_float);
        }
        parameters.value(compId)->insert(paramName, param);
        // Emit change
        emit parameterChanged(uasId, compId, paramName, param);
        emit parameterChanged(uasId, compId, rawValue.param_count, rawValue.param_index, paramName, param);
        //                QLOG_DEBUG() << "RECEIVED PARAM:" << param;
    }
        break;
    case MAV_PARAM_TYPE_UINT8:
    {
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA) {
            param = QVariant(static_cast<uint>(paramValue.param_float));
        }
        else {
            param = QVariant(QChar((unsigned char)paramValue.param_uint8));
        }
        parameters.value(compId)->insert(paramName, param);
        // Emit change
        emit parameterChanged(uasId, compId, paramName, param);
        emit parameterChanged(uasId, compId, rawValue.param_count, rawValue.param_index, paramName, param);
        //QLOG_DEBUG() << "RECEIVED PARAM:" << param;
    }
        break;
    case MAV_PARAM_TYPE_INT8:
    {
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA) {
            param = QVariant(static_cast<int>(paramValue.param_float));
        }
        else  {
            param = QVariant(QChar((char)paramValue.param_int8));
        }
        parameters.value(compId)->insert(paramName, param);
        // Emit change
        emit parameterChanged(uasId, compId, paramName, param);
        emit parameterChanged(uasId, compId, rawValue.param_count, rawValue.param_index, paramName, param);
        //QLOG_DEBUG() << "RECEIVED PARAM:" << param;
    }
        break;
    case MAV_PARAM_TYPE_INT16:
    {
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA) {
            param = QVariant(static_cast<int>(paramValue.param_float));
        }
        else {
            param = QVariant(paramValue.param_int16);
        }
        parameters.value(compId)->insert(paramName, param);
        // Emit change
        emit parameterChanged(uasId, compId, paramName, param);
        emit parameterChanged(uasId, compId, rawValue.param_count, rawValue.param_index, paramName, param);
        //QLOG_DEBUG() << "RECEIVED PARAM:" << param;
    }
        break;
    case MAV_PARAM_TYPE_UINT32:
    {
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA) {
            param = QVariant(static_cast<uint>(paramValue.param_float));
        }
        else {
            param = QVariant(paramValue.param_uint32);
        }
        parameters.value(compId)->insert(paramName, param);
        // Emit change
        emit parameterChanged(uasId, compId, paramName, param);
        emit parameterChanged(uasId, compId, rawValue.param_count, rawValue.param_index, paramName, param);
    }
        break;
    case MAV_PARAM_TYPE_INT32:
    {
        if (getAutopilotType() == MAV_AUTOPILOT_ARDUPILOTMEGA) {
            param = QVariant(static_cast<int>(paramValue.param_float));
        }
        else {
            param = QVariant(paramValue.param_int32);
        }
        parameters.value(compId)->insert(paramName, param);
        // Emit change
        emit parameterChanged(uasId, compId, paramName, param);
        emit parameterChanged(uasId, compId, rawValue.param_count, rawValue.param_index, paramName, param);
        //                QLOG_DEBUG() << "RECEIVED PARAM:" << param;
    }
        break;
    default:
        QLOG_ERROR() << "INVALID DATA TYPE USED AS PARAMETER VALUE: " << rawValue.param_type;
    } //switch (value.param_type)
}

/**
* Request parameter, use parameter name to request it.
*/
void UAS::requestParameter(int component, int id)
{
    // Request parameter, use parameter name to request it
    mavlink_message_t msg;
    mavlink_param_request_read_t read;
    read.param_index = id;
    read.param_id[0] = '\0'; // Enforce null termination
    read.target_system = uasId;
    read.target_component = component;
    mavlink_msg_param_request_read_encode(systemId, componentId, &msg, &read);
    sendMessage(msg);
    //QLOG_DEBUG() << __FILE__ << __LINE__ << "REQUESTING PARAM RETRANSMISSION FROM COMPONENT" << component << "FOR PARAM ID" << id;
}

void UAS::requestNextParamFromQueue() {
    if(paramRequestQueue.isEmpty()){
        m_parameterSendTimer.stop();
        return;
    }

    QPair<int,QString> pr = paramRequestQueue.front();
    int component = pr.first;
    QString parameter = pr.second;

    // Request parameter, use parameter name to request it
    mavlink_message_t msg;
    mavlink_param_request_read_t read;
    read.param_index = -1;
    // Copy full param name or maximum max field size
    if (parameter.length() > MAVLINK_MSG_PARAM_REQUEST_READ_FIELD_PARAM_ID_LEN)
    {
        emit textMessageReceived(uasId, 0, 255, QString("QGC WARNING: Parameter name %1 is more than %2 bytes long. This might lead to errors and mishaps!").arg(parameter).arg(MAVLINK_MSG_PARAM_REQUEST_READ_FIELD_PARAM_ID_LEN-1));
        return;
    }
    memcpy(read.param_id, parameter.toStdString().c_str(), qMax(parameter.length(), MAVLINK_MSG_PARAM_REQUEST_READ_FIELD_PARAM_ID_LEN));
    if (parameter.length() < MAVLINK_MSG_PARAM_REQUEST_READ_FIELD_PARAM_ID_LEN){
        read.param_id[parameter.length()] = '\0'; // Enforce null termination
    }
    read.target_system = uasId;
    read.target_component = component;
    mavlink_msg_param_request_read_encode(systemId, componentId, &msg, &read);
    sendMessage(msg);
    QLOG_DEBUG() << __FILE__ << __LINE__ << "REQUESTING PARAM RETRANSMISSION FROM COMPONENT" << component << "FOR PARAM NAME" << parameter;

    paramRequestQueue.pop_front();
}

/**
* Request a parameter, use parameter name to request it.
*/
void UAS::requestParameter(int component, const QString& parameter)
{
    QLOG_DEBUG() << "Queuing param " << parameter;
    QPair<int,QString> p = QPair<int,QString>(component, parameter);

    paramRequestQueue.append(p);
    m_parameterSendTimer.start();
}

/**
* @param systemType Type of MAV.
*/
void UAS::setSystemType(int systemType)
{
    if((systemType >= MAV_TYPE_GENERIC) && (systemType < MAV_TYPE_ENUM_END))
    {
      type = systemType;
    
      switch (systemType)
      {
      case MAV_TYPE_FIXED_WING:
          airframe = QGC_AIRFRAME_EASYSTAR;
          break;
      case MAV_TYPE_QUADROTOR:
      case MAV_TYPE_HEXAROTOR:
      case MAV_TYPE_OCTOROTOR:
      case MAV_TYPE_TRICOPTER:
          airframe = QGC_AIRFRAME_MIKROKOPTER;
          break;
      case MAV_TYPE_HELICOPTER:
          airframe = QGC_AIRFRAME_HELICOPTER;
          break;
      default:
          airframe = QGC_AIRFRAME_GENERIC;
      }

      emit systemSpecsChanged(uasId);
   }
}

void UAS::setUASName(const QString& name)
{
    if (name != "")
    {
        this->name = name;
        writeSettings();
        emit nameChanged(name);
        emit systemSpecsChanged(uasId);
    }
}

void UAS::executeCommand(MAV_CMD command)
{
    mavlink_message_t msg;
    mavlink_command_long_t cmd;
    cmd.command = (uint16_t)command;
    cmd.confirmation = 0;
    cmd.param1 = 0.0f;
    cmd.param2 = 0.0f;
    cmd.param3 = 0.0f;
    cmd.param4 = 0.0f;
    cmd.param5 = 0.0f;
    cmd.param6 = 0.0f;
    cmd.param7 = 0.0f;
    cmd.target_system = uasId;
    cmd.target_component = 0;
    mavlink_msg_command_long_encode(systemId, componentId, &msg, &cmd);
    sendMessage(msg);
}
void UAS::executeCommandAck(int num, bool success)
{
    mavlink_message_t msg;
    mavlink_command_ack_t ack;
    ack.command = num;
    ack.result = (success ? 1 : 0);
    mavlink_msg_command_ack_encode(systemId,componentId,&msg,&ack);
    sendMessage(msg);
}

void UAS::executeCommand(MAV_CMD command, int confirmation, float param1, float param2, float param3, float param4, float param5, float param6, float param7, int component)
{
    QLOG_DEBUG() << "UAS::executeCommand" << command << "conf" << confirmation
                << "param1" << param1 << "param2" << param2 << "param3" << param3
                << "param4" << param4 << "param5" << param5 << "param6" << param6
                << "param7" << param7;

    mavlink_message_t msg;
    mavlink_command_long_t cmd;
    cmd.command = (uint16_t)command;
    cmd.confirmation = confirmation;
    cmd.param1 = param1;
    cmd.param2 = param2;
    cmd.param3 = param3;
    cmd.param4 = param4;
    cmd.param5 = param5;
    cmd.param6 = param6;
    cmd.param7 = param7;
    cmd.target_system = uasId;
    cmd.target_component = component;
    mavlink_msg_command_long_encode(systemId, componentId, &msg, &cmd);
    sendMessage(msg);
}

/**
 * Launches the system
 *
 */
void UAS::launch()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, this->getUASID(), 0, MAV_CMD_NAV_TAKEOFF, 1, 0, 0, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

/**
 * @warning Depending on the UAS, this might make the rotors of a helicopter spinning
 *
 */
void UAS::armSystem()
{
    setModeArm(base_mode | MAV_MODE_FLAG_SAFETY_ARMED, custom_mode);
}

/**
 * @warning Depending on the UAS, this might completely stop all motors.
 *
 */
void UAS::disarmSystem()
{
    setModeArm(base_mode & ~MAV_MODE_FLAG_SAFETY_ARMED, custom_mode);
}

void UAS::toggleArmedState()
{
    setModeArm(base_mode ^ MAV_MODE_FLAG_SAFETY_ARMED, custom_mode);
}

void UAS::goAutonomous()
{
    setMode((base_mode & ~MAV_MODE_FLAG_MANUAL_INPUT_ENABLED) | (MAV_MODE_FLAG_AUTO_ENABLED | MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_GUIDED_ENABLED), 0);
}

void UAS::goManual()
{
    setMode((base_mode & ~(MAV_MODE_FLAG_AUTO_ENABLED | MAV_MODE_FLAG_STABILIZE_ENABLED | MAV_MODE_FLAG_GUIDED_ENABLED))  | MAV_MODE_FLAG_MANUAL_INPUT_ENABLED, 0);
}

void UAS::toggleAutonomy()
{
    setMode(base_mode ^ MAV_MODE_FLAG_AUTO_ENABLED ^ MAV_MODE_FLAG_MANUAL_INPUT_ENABLED ^ MAV_MODE_FLAG_GUIDED_ENABLED ^ MAV_MODE_FLAG_STABILIZE_ENABLED, 0);
}

/**
* Set the manual control commands.
* This can only be done if the system has manual inputs enabled and is armed.
*/
void UAS::setManualControlCommands(double roll, double pitch, double yaw, double thrust, int xHat, int yHat, int buttons)
{
    Q_UNUSED(xHat);
    Q_UNUSED(yHat);

    if(! receivedMode
        || ! (base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY))
    {
//        QLOG_TRACE() << "JOYSTICK/MANUAL CONTROL: IGNORING COMMANDS: Not armed";
        manualControl = false;
        overrideRC = false;
        return;
    }

    if(isMultirotor()
            && (base_mode & (uint8_t)(MAV_MODE_FLAG_DECODE_POSITION_STABILIZE))
            && (! manualControl))
    {
        const QMap<QString, QVariant>* params = parameters[uasId];
        if (params == NULL
                || ! params->contains("RCMAP_ROLL")
                || ! params->contains("RCMAP_PITCH")
                || ! params->contains("RCMAP_YAW")
                || ! params->contains("RCMAP_THROTTLE")
                || ! params->contains("SYSID_MYGCS"))
        {
            QLOG_DEBUG() << "JOYSTICK/RC OVERRIDE: ignoring commands: parameters not received";
            return;
        }

        uint16_t chan_raw[8];
        memset(chan_raw, 0, sizeof(chan_raw));

        // looks like APM::Copter is not setting g.sysid_my_gcs to our systemId
        // fetch the value APM::Copter expects
        // see https://github.com/diydrones/ardupilot/issues/1515
        int gcs = params->value("SYSID_MYGCS").toInt();

        if (roll == -1.0 && pitch == -1.0 && yaw == -1.0 && thrust == 0.0)
        {
            overrideRC = false;

            QLOG_DEBUG() << "JOYSTICK/RC OVERRIDE: remove overrides";
        }
        else
        {
            overrideRC = true; // (thrust < 0.495 || 0.505 < thrust);

            uint16_t overrideRollRC = 0;
            uint16_t overridePitchRC = 0;
            uint16_t overrideYawRC = 0;
            uint16_t overrideThrottleRC = 0;

            // roll
            {
                int rcmap = params->value("RCMAP_ROLL").toInt();
                Q_ASSERT(0 < rcmap && rcmap < 8);
                Q_ASSERT(chan_raw[rcmap-1] == 0);
                chan_raw[rcmap-1] = overrideRollRC = scaleJoystickToRC(roll, rcmap);
            }
            // pitch
            {
                int rcmap = params->value("RCMAP_PITCH").toInt();
                Q_ASSERT(0 < rcmap && rcmap < 8);
                Q_ASSERT(chan_raw[rcmap-1] == 0);
                chan_raw[rcmap-1] = overridePitchRC = scaleJoystickToRC(pitch, rcmap);
            }
            // yaw
            {
                int rcmap = params->value("RCMAP_YAW").toInt();
                Q_ASSERT(0 < rcmap && rcmap < 8);
                Q_ASSERT(chan_raw[rcmap-1] == 0);
                chan_raw[rcmap-1] = overrideYawRC = scaleJoystickToRC(yaw, rcmap);
            }
            // throttle
            {
                int rcmap = params->value("RCMAP_THROTTLE").toInt();
                Q_ASSERT(0 < rcmap && rcmap < 8);
                Q_ASSERT(chan_raw[rcmap-1] == 0);
                chan_raw[rcmap-1] = overrideThrottleRC = scaleJoystickToRC(thrust*2.0 - 1.0, rcmap);
            }

            QLOG_DEBUG() << "JOYSTICK/RC OVERRIDE: Sent RC override: roll:" << overrideRollRC << " pitch:" << overridePitchRC << " yaw:" << overrideYawRC << " throttle:" << overrideThrottleRC;
        }

        mavlink_message_t message;
        mavlink_msg_rc_channels_override_pack(gcs, componentId, &message, uasId, MAV_COMP_ID_PRIMARY /* target_component */,
                                              chan_raw[0], chan_raw[1], chan_raw[2], chan_raw[3], chan_raw[4], chan_raw[5], chan_raw[6], chan_raw[7]);
        sendMessage(message);

        //emit attitudeThrustSetPointChanged(this, roll, pitch, yaw, thrust, QGC::groundTimeMilliseconds());
    }
    // If system has manual inputs enabled
    else if(((base_mode & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_MANUAL) || (base_mode & (uint8_t)MAV_MODE_FLAG_HIL_ENABLED))
            && (! overrideRC))
    {
        manualControl = true;

        // Scale values
        float manualRollAngle = -1000.0f + roll * 2000.0f;
        float manualPitchAngle = -1000.0f + pitch * 2000.0f;
        float manualYawAngle = -1000.0f + yaw * 2000.0f;
        float manualThrust = -1000.0f + thrust * 2000.0f;

        mavlink_message_t message;
        mavlink_msg_manual_control_pack(systemId, componentId, &message, this->uasId, manualPitchAngle, manualRollAngle, manualThrust, manualYawAngle, buttons);
        sendMessage(message);
        QLOG_DEBUG() << "JOYSTICK/MANUAL CONTROL: SENT MANUAL CONTROL MESSAGE: roll:" << manualRollAngle << " pitch:" << manualPitchAngle << " yaw:" << manualYawAngle << " thrust:" << manualThrust;

        emit attitudeThrustSetPointChanged(this, roll, pitch, yaw, thrust, QGC::groundTimeMilliseconds());
    }
    else
    {
        QLOG_DEBUG() << "JOYSTICK/MANUAL CONTROL: IGNORING COMMANDS: Set mode to MANUAL or STABILIZE to send joystick commands first (currently" << shortModeText << ")";
    }
}

static int intValue(const QMap<QString, QVariant>* params, const QString& key, int defaultValue)
{
    return params->contains(key) ? params->value(key).toInt() : defaultValue;
}

/** @brief scale joystick axis to RC value
 *         using min,max,trim,rev from UAS
 */
uint16_t UAS::scaleJoystickToRC(double v, int channel) const
{
    const QMap<QString, QVariant>* params = parameters[uasId];

    int min = intValue(params, QString("RC%1_MIN").arg(channel), 1000);
    int max = intValue(params, QString("RC%1_MAX").arg(channel), 2000);
    int trim = intValue(params, QString("RC%1_TRIM").arg(channel), 1500);
    int rev = intValue(params, QString("RC%1_REV").arg(channel), 1);
    if (rev == -1)
        v = -v;

    int ppm = trim;
    if (v > 0)
        ppm += (max - trim) * v;
    else if (v < 0)
        ppm -= (min - trim) * v;
    if (ppm < min) ppm = min;
    if (ppm > max) ppm = max;

    QLOG_TRACE() << v << "->" << ppm << "(min=" << min << ", trim=" << trim << ", max=" << max << ", rev=" << rev << ") for channel " << channel;
    return ppm;
}

void UAS::logRequestList(uint16_t start, uint16_t end)
{
    QLOG_DEBUG() << "send logRequestList start:" << start << " end:" << end;
    mavlink_message_t msg;
    mavlink_msg_log_request_list_pack(systemId, componentId, &msg,
                                      getUASID(), MAV_COMP_ID_PRIMARY,
                                      start, end);
    sendMessage(msg);
}

void UAS::logRequestData(uint16_t id, uint32_t ofs, uint32_t count)
{
    QLOG_DEBUG() << "send logRequestData id:" << id << " ofs:" << ofs << " count:" << count;
    mavlink_message_t msg;
    mavlink_msg_log_request_data_pack(systemId, componentId, &msg,
                                      getUASID(), MAV_COMP_ID_PRIMARY,
                                      id, ofs, count);
    sendMessage(msg);
}

void UAS::logEraseAll()
{
    QLOG_DEBUG() << "UAS::logEraseAll()";
    mavlink_message_t msg;
    mavlink_msg_log_erase_pack(systemId, componentId, &msg,
                                   getUASID(), MAV_COMP_ID_PRIMARY);
    sendMessage(msg);
}

void UAS::logRequestEnd()
{
    QLOG_DEBUG() << "UAS::LogRequestEnd()";
    mavlink_message_t msg;
    mavlink_msg_log_request_end_pack(systemId, componentId, &msg,
                                      getUASID(), MAV_COMP_ID_PRIMARY);
    sendMessage(msg);
}

/**
* @return the type of the system
*/
int UAS::getSystemType()
{
    return this->type;
}

/**
* @param buttonIndex
*/
void UAS::receiveButton(int buttonIndex)
{
    switch (buttonIndex)
    {
    case 0:

        break;
    case 1:

        break;
    default:

        break;
    }
    //    QLOG_DEBUG() << __FILE__ << __LINE__ << ": Received button clicked signal (button # is: " << buttonIndex << "), UNIMPLEMENTED IN MAVLINK!";

}

/**
* Halt the uas.
*/
void UAS::halt()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_OVERRIDE_GOTO, 1, MAV_GOTO_DO_HOLD, MAV_GOTO_HOLD_AT_CURRENT_POSITION, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

/**
* Make the UAS move.
*/
void UAS::go()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_OVERRIDE_GOTO, 1, MAV_GOTO_DO_CONTINUE, MAV_GOTO_HOLD_AT_CURRENT_POSITION, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

/** 
* Order the robot to return home 
*/
void UAS::home()
{
    mavlink_message_t msg;

    double latitude = UASManager::instance()->getHomeLatitude();
    double longitude = UASManager::instance()->getHomeLongitude();
    double altitude = UASManager::instance()->getHomeAltitude();
    int frame = UASManager::instance()->getHomeFrame();

    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_OVERRIDE_GOTO, 1, MAV_GOTO_DO_CONTINUE, MAV_GOTO_HOLD_AT_CURRENT_POSITION, frame, 0, latitude, longitude, altitude);
    sendMessage(msg);
}

/**
* Order the robot to land on the runway 
*/
void UAS::land()
{
    mavlink_message_t msg;

    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_NAV_LAND, 1, 0, 0, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

/**
* Order the robot to start receiver pairing
*/
void UAS::pairRX(int rxType, int rxSubType)
{
    mavlink_message_t msg;

    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_START_RX_PAIR, 0, rxType, rxSubType, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

/**
 * The MAV starts the emergency landing procedure. The behaviour depends on the onboard implementation
 * and might differ between systems.
 */
void UAS::emergencySTOP()
{
    // FIXME MAVLINKV10PORTINGNEEDED
    halt();
}

/**
 * Shut down this mav - All onboard systems are immediately shut down (e.g. the
 *  main power line is cut).
 * @warning This might lead to a crash.
 *
 * The command will not be executed until emergencyKILLConfirm is issues immediately afterwards
 */
bool UAS::emergencyKILL()
{
    halt();
    // FIXME MAVLINKV10PORTINGNEEDED
    //    bool result = false;
    //    QMessageBox msgBox;
    //    msgBox.setIcon(QMessageBox::Critical);
    //    msgBox.setText("EMERGENCY: KILL ALL MOTORS ON UAS");
    //    msgBox.setInformativeText("Do you want to cut power on all systems?");
    //    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    //    msgBox.setDefaultButton(QMessageBox::Cancel);
    //    int ret = msgBox.exec();

    //    // Close the message box shortly after the click to prevent accidental clicks
    //    QTimer::singleShot(5000, &msgBox, SLOT(reject()));


    //    if (ret == QMessageBox::Yes)
    //    {
    //        mavlink_message_t msg;
    //        // TODO Replace MG System ID with static function call and allow to change ID in GUI
    //        mavlink_msg_action_pack(MG::SYSTEM::ID, MG::SYSTEM::COMPID, &msg, this->getUASID(), MAV_COMP_ID_IMU, (int)MAV_ACTION_EMCY_KILL);
    //        // Send message twice to increase chance of reception
    //        sendMessage(msg);
    //        sendMessage(msg);
    //        result = true;
    //    }
    //    return result;
    return false;
}

/**
* If enabled, connect the flight gear link.
*/
void UAS::enableHilFlightGear(bool enable, QString options, bool sensorHil, QObject * configuration)
{
    QGCFlightGearLink* link = dynamic_cast<QGCFlightGearLink*>(simulation);
    if (!link || !simulation) {
        // Delete wrong sim
        if (simulation) {
            stopHil();
            delete simulation;
        }
        simulation = new QGCFlightGearLink(this, options);
    }
    // Connect Flight Gear Link
    link = dynamic_cast<QGCFlightGearLink*>(simulation);
    link->setStartupArguments(options);
    link->sensorHilEnabled(sensorHil);
    QObject::connect(configuration, SIGNAL(barometerOffsetChanged(float)), link, SLOT(setBarometerOffset(float)));
    if (enable)
    {
        startHil();
    }
    else
    {
        stopHil();
    }
}

/**
* If enabled, connect the JSBSim link.
*/
void UAS::enableHilJSBSim(bool enable, QString options)
{
    QGCJSBSimLink* link = dynamic_cast<QGCJSBSimLink*>(simulation);
    if (!link || !simulation) {
        // Delete wrong sim
        if (simulation) {
            stopHil();
            delete simulation;
        }
        simulation = new QGCJSBSimLink(this, options);
    }
    // Connect Flight Gear Link
    link = dynamic_cast<QGCJSBSimLink*>(simulation);
    link->setStartupArguments(options);
    if (enable)
    {
        startHil();
    }
    else
    {
        stopHil();
    }
}

/**
* @param time_us Timestamp (microseconds since UNIX epoch or microseconds since system boot)
* @param roll Roll angle (rad)
* @param pitch Pitch angle (rad)
* @param yaw Yaw angle (rad)
* @param rollspeed Roll angular speed (rad/s)
* @param pitchspeed Pitch angular speed (rad/s)
* @param yawspeed Yaw angular speed (rad/s)
* @param lat Latitude, expressed as * 1E7
* @param lon Longitude, expressed as * 1E7
* @param alt Altitude in meters, expressed as * 1000 (millimeters)
* @param vx Ground X Speed (Latitude), expressed as m/s * 100
* @param vy Ground Y Speed (Longitude), expressed as m/s * 100
* @param vz Ground Z Speed (Altitude), expressed as m/s * 100
* @param xacc X acceleration (mg)
* @param yacc Y acceleration (mg)
* @param zacc Z acceleration (mg)
*/
void UAS::sendHilGroundTruth(quint64 time_us, float roll, float pitch, float yaw, float rollspeed,
                       float pitchspeed, float yawspeed, double lat, double lon, double alt,
                       float vx, float vy, float vz, float ind_airspeed, float true_airspeed, float xacc, float yacc, float zacc)
{
    Q_UNUSED(time_us);
    Q_UNUSED(xacc);
    Q_UNUSED(yacc);
    Q_UNUSED(zacc);
    
        // Emit attitude for cross-check
        emit valueChanged(uasId, "roll sim", "rad", roll, getUnixTime());
        emit valueChanged(uasId, "pitch sim", "rad", pitch, getUnixTime());
        emit valueChanged(uasId, "yaw sim", "rad", yaw, getUnixTime());

        emit valueChanged(uasId, "roll rate sim", "rad/s", rollspeed, getUnixTime());
        emit valueChanged(uasId, "pitch rate sim", "rad/s", pitchspeed, getUnixTime());
        emit valueChanged(uasId, "yaw rate sim", "rad/s", yawspeed, getUnixTime());

        emit valueChanged(uasId, "lat sim", "deg", lat*1e7, getUnixTime());
        emit valueChanged(uasId, "lon sim", "deg", lon*1e7, getUnixTime());
        emit valueChanged(uasId, "alt sim", "deg", alt*1e3, getUnixTime());

        emit valueChanged(uasId, "vx sim", "m/s", vx*1e2, getUnixTime());
        emit valueChanged(uasId, "vy sim", "m/s", vy*1e2, getUnixTime());
        emit valueChanged(uasId, "vz sim", "m/s", vz*1e2, getUnixTime());

        emit valueChanged(uasId, "IAS sim", "m/s", ind_airspeed, getUnixTime());
        emit valueChanged(uasId, "TAS sim", "m/s", true_airspeed, getUnixTime());
}

/**
* @param time_us Timestamp (microseconds since UNIX epoch or microseconds since system boot)
* @param roll Roll angle (rad)
* @param pitch Pitch angle (rad)
* @param yaw Yaw angle (rad)
* @param rollspeed Roll angular speed (rad/s)
* @param pitchspeed Pitch angular speed (rad/s)
* @param yawspeed Yaw angular speed (rad/s)
* @param lat Latitude, expressed as * 1E7
* @param lon Longitude, expressed as * 1E7
* @param alt Altitude in meters, expressed as * 1000 (millimeters)
* @param vx Ground X Speed (Latitude), expressed as m/s * 100
* @param vy Ground Y Speed (Longitude), expressed as m/s * 100
* @param vz Ground Z Speed (Altitude), expressed as m/s * 100
* @param xacc X acceleration (mg)
* @param yacc Y acceleration (mg)
* @param zacc Z acceleration (mg)
*/
void UAS::sendHilState(quint64 time_us, float roll, float pitch, float yaw, float rollspeed,
                       float pitchspeed, float yawspeed, double lat, double lon, double alt,
                       float vx, float vy, float vz, float ind_airspeed, float true_airspeed, float xacc, float yacc, float zacc)
{
    if (this->base_mode & MAV_MODE_FLAG_HIL_ENABLED)
    {
        float q[4];

        double cosPhi_2 = cos(double(roll) / 2.0);
        double sinPhi_2 = sin(double(roll) / 2.0);
        double cosTheta_2 = cos(double(pitch) / 2.0);
        double sinTheta_2 = sin(double(pitch) / 2.0);
        double cosPsi_2 = cos(double(yaw) / 2.0);
        double sinPsi_2 = sin(double(yaw) / 2.0);
        q[0] = (cosPhi_2 * cosTheta_2 * cosPsi_2 +
                sinPhi_2 * sinTheta_2 * sinPsi_2);
        q[1] = (sinPhi_2 * cosTheta_2 * cosPsi_2 -
                cosPhi_2 * sinTheta_2 * sinPsi_2);
        q[2] = (cosPhi_2 * sinTheta_2 * cosPsi_2 +
                sinPhi_2 * cosTheta_2 * sinPsi_2);
        q[3] = (cosPhi_2 * cosTheta_2 * sinPsi_2 -
                sinPhi_2 * sinTheta_2 * cosPsi_2);

        mavlink_message_t msg;
        mavlink_msg_hil_state_quaternion_pack(systemId, componentId, &msg,
                                   time_us, q, rollspeed, pitchspeed, yawspeed,
                                   lat*1e7f, lon*1e7f, alt*1000, vx*100, vy*100, vz*100, ind_airspeed*100, true_airspeed*100, xacc*1000/9.81, yacc*1000/9.81, zacc*1000/9.81);
        sendMessage(msg);
    }
    else
    {
        // Attempt to set HIL mode
        setMode(base_mode | MAV_MODE_FLAG_HIL_ENABLED, custom_mode);
        qDebug() << __FILE__ << __LINE__ << "HIL is onboard not enabled, trying to enable.";
    }
}

void UAS::sendHilSensors(quint64 time_us, float xacc, float yacc, float zacc, float rollspeed, float pitchspeed, float yawspeed,
                                    float xmag, float ymag, float zmag, float abs_pressure, float diff_pressure, float pressure_alt, float temperature, quint32 fields_changed)
{
    if (this->base_mode & MAV_MODE_FLAG_HIL_ENABLED)
    {
        mavlink_message_t msg;
        mavlink_msg_hil_sensor_pack(systemId, componentId, &msg,
                                   time_us, xacc, yacc, zacc, rollspeed, pitchspeed, yawspeed,
                                     xmag, ymag, zmag, abs_pressure, diff_pressure, pressure_alt, temperature,
                                     fields_changed);
        sendMessage(msg);
        lastSendTimeSensors = QGC::groundTimeMilliseconds();
    }
    else
    {
        // Attempt to set HIL mode
        setMode(base_mode | MAV_MODE_FLAG_HIL_ENABLED, custom_mode);
        qDebug() << __FILE__ << __LINE__ << "HIL is onboard not enabled, trying to enable.";
    }
}

void UAS::sendHilGps(quint64 time_us, double lat, double lon, double alt, int fix_type, float eph, float epv, float vel, float vn, float ve, float vd, float cog, int satellites)
{
    // Only send at 10 Hz max rate
    if (QGC::groundTimeMilliseconds() - lastSendTimeGPS < 100)
        return;

    if (this->base_mode & MAV_MODE_FLAG_HIL_ENABLED)
    {
        float course = cog;
        // map to 0..2pi
        if (course < 0)
            course += 2.0f * static_cast<float>(M_PI);
        // scale from radians to degrees
        course = (course / M_PI) * 180.0f;

        mavlink_message_t msg;
        mavlink_msg_hil_gps_pack(systemId, componentId, &msg,
                                   time_us, fix_type, lat*1e7, lon*1e7, alt*1e3, eph*1e2, epv*1e2, vel*1e2, vn*1e2, ve*1e2, vd*1e2, course*1e2, satellites);
        lastSendTimeGPS = QGC::groundTimeMilliseconds();
        sendMessage(msg);
    }
    else
    {
        // Attempt to set HIL mode
        setMode(base_mode | MAV_MODE_FLAG_HIL_ENABLED, custom_mode);
        qDebug() << __FILE__ << __LINE__ << "HIL is onboard not enabled, trying to enable.";
    }
}


/**
* Connect flight gear link.
**/
void UAS::startHil()
{
    if (hilEnabled) return;
    hilEnabled = true;
    sensorHil = false;
    setMode(base_mode | MAV_MODE_FLAG_HIL_ENABLED, custom_mode);
    // Connect HIL simulation link
    simulation->connectSimulation();
}

/**
* disable flight gear link.
*/
void UAS::stopHil()
{
    if (simulation) simulation->disconnectSimulation();
    setMode(base_mode & ~MAV_MODE_FLAG_HIL_ENABLED, custom_mode);
    hilEnabled = false;
    sensorHil = false;
}
void UAS::reboot()
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, 1, 1, 1, 0, 0, 0, 0, 0);
    sendMessage(msg);
}

void UAS::shutdown()
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText("Shutting down the UAS");
    msgBox.setInformativeText("Do you want to shut down the onboard computer?");

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    // Close the message box shortly after the click to prevent accidental clicks
    QTimer::singleShot(5000, &msgBox, SLOT(reject()));

    if (ret == QMessageBox::Yes)
    {
        // If the active UAS is set, execute command
        mavlink_message_t msg;
        mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN, 1, 2, 2, 0, 0, 0, 0, 0);
        sendMessage(msg);
    }
}

/**
* @param x position
* @param y position
* @param z position
* @param yaw
*/
void UAS::setTargetPosition(float x, float y, float z, float yaw)
{
    mavlink_message_t msg;
    mavlink_msg_command_long_pack(systemId, componentId, &msg, uasId, MAV_COMP_ID_PRIMARY, MAV_CMD_NAV_PATHPLANNING, 1, 1, 1, 0, yaw, x, y, z);
    sendMessage(msg);
}

/**
 * @return The name of this system as string in human-readable form
 */
QString UAS::getUASName(void) const
{
    QString result;
    if (name == "")
    {
        result = tr("MAV ") + result.sprintf("%03d", getUASID());
    }
    else
    {
        result = name;
    }
    return result;
}

/**
* @return the state of the uas as a short text.
*/
const QString& UAS::getShortState() const
{
    return shortStateText;
}

/** 
* The mode can be autonomous, guided, manual or armed. It will also return if 
* hardware in the loop is being used.
* @return the audio mode text for the id given.
*/
QString UAS::getAudioModeTextFor(int id)
{
    QString mode;
    uint8_t modeid = id;

    // BASE MODE DECODING
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_AUTO)
    {
        mode += "autonomous";
    }
    else if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_GUIDED)
    {
        mode += "guided";
    }
    else if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_STABILIZE)
    {
        mode += "stabilized";
    }
    else if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_MANUAL)
    {
        mode += "manual";
    }
    else
    {
        // Nothing else applies, we're in preflight
        mode += "preflight";
    }

    if (modeid != 0)
    {
        mode += " mode";
    }

    // ARMED STATE DECODING
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_SAFETY)
    {
        mode.append(" and armed");
    }

    // HARDWARE IN THE LOOP DECODING
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_HIL)
    {
        mode.append(" using hardware in the loop simulation");
    }

    return mode;
}
/**

* The mode returned can be auto, stabilized, test, manual, preflight or unknown.
* @return the short text of the mode for the id given.
*/
QString UAS::getShortModeTextFor(int id)
{
    QString mode;
    uint8_t modeid = id;

    QLOG_DEBUG() << "MODE:" << modeid;

    // BASE MODE DECODING
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_AUTO)
    {
        mode += "|AUTO";
    }
    else if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_GUIDED)
    {
        mode += "|VECTOR";
    }
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_STABILIZE)
    {
        mode += "|STABILIZED";
    }
    else if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_TEST)
    {
        mode += "|TEST";
    }
    else if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_MANUAL)
    {
        mode += "|MANUAL";
    }
    else if (modeid == 0)
    {
        mode = "|PREFLIGHT";
    }
    else
    {
        mode = "|UNKNOWN";
    }

    // ARMED STATE DECODING
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_SAFETY)
    {
        mode.prepend("A");
    }
    else
    {
        mode.prepend("D");
    }

    // HARDWARE IN THE LOOP DECODING
    if (modeid & (uint8_t)MAV_MODE_FLAG_DECODE_POSITION_HIL)
    {
        mode.prepend("HIL:");
    }
    QLOG_DEBUG() << mode;
    return mode;
}

const QString& UAS::getShortMode() const
{
    return shortModeText;
}

/**
* Add the link and connect a signal to it which will be set off when it is destroyed.
*/
void UAS::addLink(LinkInterface* link)
{
    if (!links->contains(link))
    {
        links->append(link);
        connect(link, SIGNAL(destroyed(QObject*)), this, SLOT(removeLink(QObject*)));
        connect(link,SIGNAL(disconnected(QObject*)),this,SLOT(disconnected(QObject*)));
        connect(link,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
        connect(link,SIGNAL(connected()),this,SIGNAL(connected()));
        if(link->isConnected())
            emit connected(); // Fixes issue that the link is already connected and signal not sent
    }
}

void UAS::removeLink(QObject* object)
{
    for (int i=0;i<links->size();i++)
    {
        if (links->at(i) == object)
        {
            links->removeAt(i);
            i--;
        }

    }
    disconnect(object,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
    disconnect(object,SIGNAL(connected()),this,SIGNAL(connected()));
    if (links->size() == 0)
    {
        //no more links, should remove this.
        UASManager::instance()->removeUAS(this);
    }
}

/**
* @return the list of links
*/
QList<LinkInterface*>* UAS::getLinks()
{
    return links;
}

QList<int> UAS::getLinkIdList()
{
    QList<int> linklist;
    for (int i=0;i<links->size();i++)
    {
        linklist.append(links->at(i)->getId());
    }
    return linklist;
}

/**
* @rerturn the map of the components
*/
QMap<int, QString> UAS::getComponents()
{
    return components;
}

/**
* Set the battery type and the  number of cells.
* @param type of the battery
* @param cells Number of cells.
*/
void UAS::setBattery(BatteryType type, int cells)
{
    this->batteryType = type;
    this->cells = cells;
    switch (batteryType)
    {
    case NICD:
        break;
    case NIMH:
        break;
    case LIION:
        break;
    case LIPOLY:
        fullVoltage = this->cells * UAS::lipoFull;
        emptyVoltage = this->cells * UAS::lipoEmpty;
        break;
    case LIFE:
        break;
    case AGZN:
        break;
    }
}

/**
* Set the battery specificaitons: empty voltage, warning voltage, and full voltage.
* @param specifications of the battery
*/
void UAS::setBatterySpecs(const QString& specs)
{
    if (specs.length() == 0 || specs.contains("%"))
    {
        batteryRemainingEstimateEnabled = false;
        bool ok;
        QString percent = specs;
        percent = percent.remove("%");
        float temp = percent.toFloat(&ok);
        if (ok)
        {
            warnLevelPercent = temp;
        }
        else
        {
            emit textMessageReceived(0, 0, 0, "Could not set battery options, format is wrong");
        }
    }
    else
    {
        batteryRemainingEstimateEnabled = true;
        QString stringList = specs;
        stringList = stringList.remove("V");
        stringList = stringList.remove("v");
        QStringList parts = stringList.split(",");
        if (parts.length() == 3)
        {
            float temp;
            bool ok;
            // Get the empty voltage
            temp = parts.at(0).toFloat(&ok);
            if (ok) emptyVoltage = temp;
            // Get the warning voltage
            temp = parts.at(1).toFloat(&ok);
            if (ok) warnVoltage = temp;
            // Get the full voltage
            temp = parts.at(2).toFloat(&ok);
            if (ok) fullVoltage = temp;
        }
        else
        {
            emit textMessageReceived(0, 0, 0, "Could not set battery options, format is wrong");
        }
    }
}

/**
* @return the battery specifications(empty voltage, warning voltage, full voltage)
*/
QString UAS::getBatterySpecs()
{
    if (batteryRemainingEstimateEnabled)
    {
        return QString("%1V,%2V,%3V").arg(emptyVoltage).arg(warnVoltage).arg(fullVoltage);
    }
    else
    {
        return QString("%1%").arg(warnLevelPercent);
    }
}

/**
* @return the time remaining.
*/
int UAS::calculateTimeRemaining()
{
    quint64 dt = QGC::groundTimeMilliseconds() - startTime;
    double seconds = dt / 1000.0f;
    double voltDifference = startVoltage - currentVoltage;
    if (voltDifference <= 0) voltDifference = 0.00000000001f;
    double dischargePerSecond = voltDifference / seconds;
    int remaining = static_cast<int>((currentVoltage - emptyVoltage) / dischargePerSecond);
    // Can never be below 0
    if (remaining < 0) remaining = 0;
    return remaining;
}

/**
 * @return charge level in percent - 0 - 100
 */
double UAS::getChargeLevel()
{
    if (batteryRemainingEstimateEnabled)
    {
        if (lpVoltage < emptyVoltage)
        {
            chargeLevel = 0.0;
        }
        else if (lpVoltage > fullVoltage)
        {
            chargeLevel = 100.0;
        }
        else
        {
            chargeLevel = 100.0 * ((lpVoltage - emptyVoltage)/(fullVoltage - emptyVoltage));
        }
    }
    return chargeLevel;
}

void UAS::startLowBattAlarm()
{
    if (!lowBattAlarm)
    {
        GAudioOutput::instance()->alert(tr("system %1 has low battery").arg(getUASName()));
        QTimer::singleShot(3000, GAudioOutput::instance(), SLOT(startEmergency()));
        lowBattAlarm = true;
    }
}

void UAS::stopLowBattAlarm()
{
    if (lowBattAlarm)
    {
        GAudioOutput::instance()->stopEmergency();
        lowBattAlarm = false;
    }
}

int UAS::getCustomMode()
{
    return custom_mode;
}

bool UAS::isMultirotor()
{
    switch(type){
    case MAV_TYPE_TRICOPTER:
    case MAV_TYPE_QUADROTOR:
    case MAV_TYPE_HEXAROTOR:
    case MAV_TYPE_OCTOROTOR:
    case MAV_TYPE_HELICOPTER:
    case MAV_TYPE_COAXIAL:
        return true;
    default:
        return false;
    }
}

bool UAS::isRotaryWing()
{
    switch (type) {
        case MAV_TYPE_QUADROTOR:
        /* fallthrough */
        case MAV_TYPE_COAXIAL:
        case MAV_TYPE_HELICOPTER:
        case MAV_TYPE_HEXAROTOR:
        case MAV_TYPE_OCTOROTOR:
        case MAV_TYPE_TRICOPTER:
            return true;
        default:
            return false;
    }
}

bool UAS::isHelicopter()
{
    switch(type){
    case MAV_TYPE_HELICOPTER:
        return true;
    default:
        return false;
    }
}

bool UAS::isFixedWing()
{
    switch(type){
    case MAV_TYPE_FIXED_WING:
        return true;
    default:
        return false;
    }
}

bool UAS::isGroundRover()
{
    switch(type){
    case MAV_TYPE_GROUND_ROVER:
        return true;
    default:
        return false;
    }
}

void UAS::playCustomModeChangedAudioMessage()
{
    // Do nothing as its custom message only a autopilot will know the correct action
}

void UAS::playArmStateChangedAudioMessage(bool armedState)
{
    if (armedState){
        GAudioOutput::instance()->say("armed");
    } else {
        GAudioOutput::instance()->say("disarmed");
    }
}
void UAS::protocolStatusMessageRec(const QString& title, const QString& message)
{
    emit protocolStatusMessage(title,message);
}

void UAS::valueChangedRec(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec)
{
    emit valueChanged(uasId,name,unit,value,msec);
}

void UAS::textMessageReceivedRec(int uasid, int componentid, int severity, const QString& text)
{
    emit textMessageReceived(uasid,componentid,severity,text);
}

void UAS::receiveLossChangedRec(int id,float value)
{
    emit receiveLossChanged(id,value);
}
