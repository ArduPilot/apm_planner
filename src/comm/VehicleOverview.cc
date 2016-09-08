#include "VehicleOverview.h"
#include <QVariant>
#include "QGC.h"
VehicleOverview::VehicleOverview(QObject *parent) : QObject(parent),
    m_vibrationX(0),
    m_vibrationY(0),
    m_vibrationZ(0),
    m_clipping0(0),
    m_clipping1(0),
    m_clipping2(0),
    m_ekfFlags(0),
    m_velocity_variance(0),
    m_pos_horiz_variance(0),
    m_pos_vert_variance(0),
    m_compass_variance(0),
    m_terrain_alt_variance(0)
{
}

VehicleOverview::~VehicleOverview()
{

}

void VehicleOverview::parseHeartbeat(LinkInterface* link,const mavlink_message_t &message, const mavlink_heartbeat_t &state)
{
    Q_UNUSED(link)
    Q_UNUSED(message)
    // Set new type if it has changed
    if (m_type != state.type)
    {
        /*if (isFixedWing()) {
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
        }*/
        //this->autopilot = state.autopilot;
        //this->setAutopilot(state.autopilot);
        //this->setProperty("autopilot",state.autopilot);

       // emit systemTypeSet(this, type);
    }
    //Set properties to trigger UI updates
    this->setAutopilot(state.autopilot);
    this->setCustomMode(state.custom_mode);
    this->setType(state.type);
    this->setBaseMode(state.base_mode);
    this->setSystemStatus(state.system_status);
    this->setMavlinkVersion(state.mavlink_version);

    bool currentlyArmed = state.base_mode & MAV_MODE_FLAG_DECODE_POSITION_SAFETY;
    if (currentlyArmed != m_armedState)
    {
        if (currentlyArmed)
        {
            //Gone from not armed, to armed
        }
        else
        {
            //Gone from armed, to not armed
        }
        this->setArmedState(currentlyArmed);
    }

}
void VehicleOverview::parseBattery(LinkInterface *link, const mavlink_battery_status_t &state)
{
    Q_UNUSED(state)
    Q_UNUSED(link)
}

void VehicleOverview::parseSysStatus(LinkInterface *link, const mavlink_sys_status_t &state)
{
    Q_UNUSED(link)
    this->setOnboardControlSensorsEnabled(state.onboard_control_sensors_enabled);
    this->setOnboardControlSensorsHealth(state.onboard_control_sensors_health);
    this->setOnboardControlSensorsPresent(state.onboard_control_sensors_present);
    this->setLoad(state.load);
    this->setErrorsComm(state.errors_comm);
    this->setErrorsCount1(state.errors_count1);
    this->setErrorsCount2(state.errors_count2);
    this->setErrorsCount3(state.errors_count3);
    this->setErrorsCount4(state.errors_count4);
    this->setVoltageBattery(state.voltage_battery);
    this->setBatteryRemaining(state.battery_remaining);
    this->setCurrentBattery(state.current_battery);
    this->setDropRateComm(state.drop_rate_comm);
}

void VehicleOverview::messageReceived(LinkInterface* link,mavlink_message_t message)
{
    switch (message.msgid)
    {
        case MAVLINK_MSG_ID_HEARTBEAT:
        {
            mavlink_heartbeat_t state;
            mavlink_msg_heartbeat_decode(&message, &state);
            parseHeartbeat(link,message,state);
            break;
        }
        case MAVLINK_MSG_ID_BATTERY_STATUS:
        {
            mavlink_battery_status_t state;
            mavlink_msg_battery_status_decode(&message,&state);
            parseBattery(link,state);
            break;
        }
        case MAVLINK_MSG_ID_SYS_STATUS:
        {
            mavlink_sys_status_t state;
            mavlink_msg_sys_status_decode(&message,&state);
            parseSysStatus(link,state);
            break;
        }
        case MAVLINK_MSG_ID_VIBRATION:
        {
            mavlink_vibration_t vibration;
            mavlink_msg_vibration_decode(&message, &vibration);
            parseVibration(link,vibration);
            break;
        }
        case MAVLINK_MSG_ID_EKF_STATUS_REPORT:
        {
            mavlink_ekf_status_report_t report;
            mavlink_msg_ekf_status_report_decode(&message, &report);
            parseEkfStatusReport(link,report);
            break;
        }

    }
}

void VehicleOverview::parseVibration(LinkInterface *link, const mavlink_vibration_t &vibration)
{
    Q_UNUSED(link);
    setVibrationX(vibration.vibration_x);
    setVibrationY(vibration.vibration_y);
    setVibrationZ(vibration.vibration_z);

    setClipping0(vibration.clipping_0);
    setClipping1(vibration.clipping_1);
    setClipping2(vibration.clipping_2);
}

void VehicleOverview::parseEkfStatusReport(LinkInterface *link, const mavlink_ekf_status_report_t &report)
{
    Q_UNUSED(link);
    setEkfFlags(report.flags);
    setEkfPosHorizVariance(report.pos_horiz_variance);
    setEkfPosVertVariance(report.pos_vert_variance);
    setEkfComapssVariance(report.compass_variance);
    setEkfVelocityVariance(report.velocity_variance);
    setEkfTerrainAltVariance(report.terrain_alt_variance);
}
