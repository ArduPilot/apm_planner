#include "AbsPositionOverview.h"

AbsPositionOverview::AbsPositionOverview(QObject *parent) :
    QObject(parent)
{
}
void AbsPositionOverview::parseGpsRawInt(LinkInterface *link, const mavlink_message_t &message, const mavlink_gps_raw_int_t &state)
{
    if (state.fix_type > 2)
    {
        this->setLat(state.lat);
        this->setLon(state.lon);
        this->setAlt(state.alt);
        this->setVel(state.vel);
        altitude_gps = pos.alt/1000.0;
    }
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
    }
}
