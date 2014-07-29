#include "UASObject.h"
//#include "libs/mavlink/include/mavlink/v1.0/common/mavlink_msg_heartbeat.h"
#include <QMetaType>
UASObject::UASObject(QObject *parent) : QObject(parent)
{
    /*qRegisterMetaType<QSharedPointer<mavlink_message_ahrs_t> >("QSharedPointer<mavlink_message_ahrs_t>");
    qRegisterMetaType<QSharedPointer<mavlink_message_global_position_int_t> >("QSharedPointer<mavlink_message_global_position_int_t>");
    qRegisterMetaType<QSharedPointer<mavlink_message_local_position_ned_t> >("QSharedPointer<mavlink_message_local_position_ned_t>");
    qRegisterMetaType<QSharedPointer<mavlink_message_vfr_hud_t> >("QSharedPointer<mavlink_message_vfr_hud_t>");
    qRegisterMetaType<QSharedPointer<mavlink_message_hil_controls_t> >("QSharedPointer<mavlink_message_hil_controls_t>");
    qRegisterMetaType<QSharedPointer<mavlink_message_attitude_t> >("QSharedPointer<mavlink_message_attitude_t>");*/
    m_vehicleOverview = new VehicleOverview(this);
    m_relPositionOverview = new RelPositionOverview(this);
}
void UASObject::messageReceived(LinkInterface* link,mavlink_message_t message)
{
	QByteArray payload;
	for (int i=0;i<33;i++)
	{
        payload.append(message.payload64[i]);
        payload.append(message.payload64[i] >> 8);
        payload.append(message.payload64[i] >> 16);
        payload.append(message.payload64[i] >> 24);
        payload.append(message.payload64[i] >> 32);
        payload.append(message.payload64[i] >> 40);
        payload.append(message.payload64[i] >> 48);
        payload.append(message.payload64[i] >> 56);

	}
    m_vehicleOverview->messageReceived(link,message,payload);
    m_relPositionOverview->messageReceived(link,message,payload);
    /*switch (message.msgid)
	{
		case MAVLINK_MSG_ID_HEARTBEAT:
		{
            mavlink_message_heartbeat_t t;
            t->parseFromArray(payload);
            heartbeatReceived(t);
            //emit heartbeatReceived(t);
			break;
		}
        case MAVLINK_MSG_ID_AHRS:
        {
            QSharedPointer<mavlink_message_ahrs_t> t(new mavlink_message_ahrs_t);
            t->parseFromArray(payload);
            emit ahrsReceived(t);
            break;
        }
        case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
        {
            QSharedPointer<mavlink_message_global_position_int_t> t(new mavlink_message_global_position_int_t);
            t->parseFromArray(payload);
            emit globalPositionReceived(t);
            break;
        }
        case MAVLINK_MSG_ID_LOCAL_POSITION_NED:
        {
            QSharedPointer<mavlink_message_local_position_ned_t> t(new mavlink_message_local_position_ned_t);
            t->parseFromArray(payload);
            emit localPositionNedReceived(t);
            break;
        }
        case MAVLINK_MSG_ID_VFR_HUD:
        {
            QSharedPointer<mavlink_message_vfr_hud_t> t(new mavlink_message_vfr_hud_t);
            t->parseFromArray(payload);
            emit vfrHudReceived(t);
            break;
        }
        case MAVLINK_MSG_ID_HIL_CONTROLS:
        {
            QSharedPointer<mavlink_message_hil_controls_t> t(new mavlink_message_hil_controls_t);
            t->parseFromArray(payload);
            emit hilControlsReceived(t);
            break;
        }
        case MAVLINK_MSG_ID_ATTITUDE:
        {
            QSharedPointer<mavlink_message_attitude_t> t(new mavlink_message_attitude_t);
            t->parseFromArray(payload);
            emit attitudeReceived(t);
            break;
        }
    }*/
}
/*void UASObject::heartbeatReceived(const mavlink_message_heartbeat_t &heartbeat)
{
    if (lastHeartbeat.getCustomMode() != heartbeat.getCustomMode())
    {
        lastHeartbeat.setCustomMode(heartbeat.getCustomMode());
        emit customModeChanged(heartbeat.getCustomMode());
    }
    if (lastHeartbeat.getBaseMode() != heartbeat.getBaseMode())
    {
        lastHeartbeat.setBaseMode(heartbeat.getBaseMode());
        emit baseModeChanged(heartbeat.getBaseMode());
    }
}
*/
