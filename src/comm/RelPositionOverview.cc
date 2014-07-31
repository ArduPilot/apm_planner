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

void RelPositionOverview::messageReceived(LinkInterface* link,mavlink_message_t message,QByteArray payload)
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
    }
}
