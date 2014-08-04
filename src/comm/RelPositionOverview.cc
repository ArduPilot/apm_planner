#include "RelPositionOverview.h"

RelPositionOverview::RelPositionOverview(QObject *parent) :
    QObject(parent)
{
}

void RelPositionOverview::parseAttitude(LinkInterface *link, const mavlink_message_t &message, const mavlink_attitude_t &state)
{
    this->setRoll(state.roll);
    this->setPitch(state.pitch);
    this->setYaw(state.yaw);
}
void RelPositionOverview::parseVfrHud(LinkInterface *link, const mavlink_message_t &message, const mavlink_vfr_hud_t &state)
{
    this->setAirspeed(state.airspeed);
    this->setAlt(state.alt);
    this->setClimb(state.climb);
    this->setGroundspeed(state.groundspeed);
    this->setHeading(state.heading);
    this->setThrottle(state.throttle);

}

//scaled_imu
//SCALED_IMU2
//raw_imu
//attitude
//attitude_quaternion
//STATE_CORRECTION
//VFR_HUD
//HIGHRES_IMU
void RelPositionOverview::messageReceived(LinkInterface* link,mavlink_message_t message)
{
    switch (message.msgid)
    {
        case MAVLINK_MSG_ID_ATTITUDE:
        {
            mavlink_attitude_t state;
            mavlink_msg_attitude_decode(&message, &state);
            parseAttitude(link,message,state);
            break;
        }
        case MAVLINK_MSG_ID_VFR_HUD:
        {
            mavlink_vfr_hud_t state;
            mavlink_msg_vfr_hud_decode(&message, &state);
            parseVfrHud(link,message,state);
            break;
        }
    }
}
