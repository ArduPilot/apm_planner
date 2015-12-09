#include "AbsPositionOverview.h"

AbsPositionOverview::AbsPositionOverview(QObject *parent) :
    QObject(parent),
        m_timeUsec (0.0), m_lat (0.0), m_lon (0), m_alt(0.0),
        m_eph (0.0),m_epv (0), m_vel (0.0), m_cog (0.0),
        m_fixType (0.0), m_satellitesVisible (0.0), m_timeBootMs (0.0),
        m_relativeAlt (0.0),
        m_vx (0.0), m_vy (0.0),m_vz (0.0), m_hdg (0.0)
{
    /* The defaults above are used to ensure the variables are
     * initalised to known values and dont cause random errors.
     */

}

AbsPositionOverview::~AbsPositionOverview()
{
}

void AbsPositionOverview::parseGpsRawInt(LinkInterface *link, const mavlink_message_t &message, const mavlink_gps_raw_int_t &state)
{
    Q_UNUSED(link);
    Q_UNUSED(message);

    if (state.fix_type > 2)
    {
        this->setLat(state.lat);
        this->setLon(state.lon);
        this->setAlt(state.alt);
        this->setVel(state.vel);
    }
}

void AbsPositionOverview::parseGlobalPositionInt(LinkInterface *link, const mavlink_message_t &message, const mavlink_global_position_int_t &state)
{
    Q_UNUSED(link);
    Q_UNUSED(message);
    this->setRelativeAlt(state.relative_alt/1000.0);
}

void AbsPositionOverview::messageReceived(LinkInterface* link,mavlink_message_t message)
{
    switch (message.msgid)
    {
        case MAVLINK_MSG_ID_GPS_RAW_INT:
        {
            mavlink_gps_raw_int_t state;
            mavlink_msg_gps_raw_int_decode(&message, &state);
            parseGpsRawInt(link,message,state);
            break;
        }
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
            mavlink_global_position_int_t state;
            mavlink_msg_global_position_int_decode(&message, &state);
            parseGlobalPositionInt(link,message,state);
            break;
        }
    }
}
