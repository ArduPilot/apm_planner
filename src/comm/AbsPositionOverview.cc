#include "AbsPositionOverview.h"

AbsPositionOverview::AbsPositionOverview(QObject *parent) :
    QObject(parent)
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
