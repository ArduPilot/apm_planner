#include "UASObject.h"
#include "libs/mavlink/include/mavlink/v1.0-qt/common/mavlink_message_heartbeat.h"
UASObject::UASObject(QObject *parent) : QObject(parent)
{
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

	}
	switch (message.msgid)
	{
		case MAVLINK_MSG_ID_HEARTBEAT:
		{
			mavlink_message_heartbeat_t t;

			t.parseFromArray(payload);

			break;
		}
	}
}
