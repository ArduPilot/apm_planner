#include "RelPositionOverview.h"

RelPositionOverview::RelPositionOverview(QObject *parent) :
    QObject(parent), m_airspeed (0.0), m_groundspeed (0.0), m_alt (0.0),
    m_climb (0.0),  m_heading (0.0), m_throttle (0.0), m_timeBootMs (0.0),
    m_roll (0.0), m_pitch (0.0), m_yaw (0.0), m_rollspeed (0.0), m_pitchspeed (0.0),
    m_yawspeed (0.0)
{
    /* The defaults above are used to ensure the variables are
     * initalised to known values and dont cause random errors.
     */

}

RelPositionOverview::~RelPositionOverview()
{

}

void RelPositionOverview::parseAttitude(LinkInterface *link, const mavlink_message_t &message, const mavlink_attitude_t &state)
{
    Q_UNUSED(link);
    Q_UNUSED(message);

    this->setRoll(ToDeg(state.roll));
    this->setPitch(ToDeg(state.pitch));
    this->setYaw(ToDeg(state.yaw));
}
void RelPositionOverview::parseVfrHud(LinkInterface *link, const mavlink_message_t &message, const mavlink_vfr_hud_t &state)
{
    Q_UNUSED(link);
    Q_UNUSED(message);

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
