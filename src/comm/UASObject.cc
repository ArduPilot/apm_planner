#include "UASObject.h"
//#include "libs/mavlink/include/mavlink/v1.0/common/mavlink_msg_heartbeat.h"
#include <QMetaType>
UASObject::UASObject(QObject *parent) : QObject(parent)
{
    m_vehicleOverview = new VehicleOverview(this);
    m_relPositionOverview = new RelPositionOverview(this);
    m_absPositionOverview = new AbsPositionOverview(this);
}
void UASObject::messageReceived(LinkInterface* link,mavlink_message_t message)
{
    m_vehicleOverview->messageReceived(link,message);
    m_relPositionOverview->messageReceived(link,message);
    m_absPositionOverview->messageReceived(link,message);
}
