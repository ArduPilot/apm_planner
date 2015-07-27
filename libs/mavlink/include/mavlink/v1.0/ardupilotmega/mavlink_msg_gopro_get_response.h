// MESSAGE GOPRO_GET_RESPONSE PACKING

#define MAVLINK_MSG_ID_GOPRO_GET_RESPONSE 217

typedef struct __mavlink_gopro_get_response_t
{
 uint8_t cmd_id; ///< Command ID
 uint8_t value; ///< Value
} mavlink_gopro_get_response_t;

#define MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN 2
#define MAVLINK_MSG_ID_217_LEN 2

#define MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC 163
#define MAVLINK_MSG_ID_217_CRC 163



#define MAVLINK_MESSAGE_INFO_GOPRO_GET_RESPONSE { \
	"GOPRO_GET_RESPONSE", \
	2, \
	{  { "cmd_id", NULL, MAVLINK_TYPE_UINT8_T, 0, 0, offsetof(mavlink_gopro_get_response_t, cmd_id) }, \
         { "value", NULL, MAVLINK_TYPE_UINT8_T, 0, 1, offsetof(mavlink_gopro_get_response_t, value) }, \
         } \
}


/**
 * @brief Pack a gopro_get_response message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param cmd_id Command ID
 * @param value Value
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gopro_get_response_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
						       uint8_t cmd_id, uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN];
	_mav_put_uint8_t(buf, 0, cmd_id);
	_mav_put_uint8_t(buf, 1, value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#else
	mavlink_gopro_get_response_t packet;
	packet.cmd_id = cmd_id;
	packet.value = value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GOPRO_GET_RESPONSE;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC);
#else
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
}

/**
 * @brief Pack a gopro_get_response message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param cmd_id Command ID
 * @param value Value
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_gopro_get_response_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
							   mavlink_message_t* msg,
						           uint8_t cmd_id,uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN];
	_mav_put_uint8_t(buf, 0, cmd_id);
	_mav_put_uint8_t(buf, 1, value);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#else
	mavlink_gopro_get_response_t packet;
	packet.cmd_id = cmd_id;
	packet.value = value;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif

	msg->msgid = MAVLINK_MSG_ID_GOPRO_GET_RESPONSE;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC);
#else
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
}

/**
 * @brief Encode a gopro_get_response struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param gopro_get_response C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gopro_get_response_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_gopro_get_response_t* gopro_get_response)
{
	return mavlink_msg_gopro_get_response_pack(system_id, component_id, msg, gopro_get_response->cmd_id, gopro_get_response->value);
}

/**
 * @brief Encode a gopro_get_response struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param gopro_get_response C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_gopro_get_response_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_gopro_get_response_t* gopro_get_response)
{
	return mavlink_msg_gopro_get_response_pack_chan(system_id, component_id, chan, msg, gopro_get_response->cmd_id, gopro_get_response->value);
}

/**
 * @brief Send a gopro_get_response message
 * @param chan MAVLink channel to send the message
 *
 * @param cmd_id Command ID
 * @param value Value
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_gopro_get_response_send(mavlink_channel_t chan, uint8_t cmd_id, uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char buf[MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN];
	_mav_put_uint8_t(buf, 0, cmd_id);
	_mav_put_uint8_t(buf, 1, value);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, buf, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, buf, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
#else
	mavlink_gopro_get_response_t packet;
	packet.cmd_id = cmd_id;
	packet.value = value;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, (const char *)&packet, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, (const char *)&packet, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
#endif
}

#if MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This varient of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_gopro_get_response_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t cmd_id, uint8_t value)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
	char *buf = (char *)msgbuf;
	_mav_put_uint8_t(buf, 0, cmd_id);
	_mav_put_uint8_t(buf, 1, value);

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, buf, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, buf, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
#else
	mavlink_gopro_get_response_t *packet = (mavlink_gopro_get_response_t *)msgbuf;
	packet->cmd_id = cmd_id;
	packet->value = value;

#if MAVLINK_CRC_EXTRA
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, (const char *)packet, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_CRC);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE, (const char *)packet, MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
#endif
}
#endif

#endif

// MESSAGE GOPRO_GET_RESPONSE UNPACKING


/**
 * @brief Get field cmd_id from gopro_get_response message
 *
 * @return Command ID
 */
static inline uint8_t mavlink_msg_gopro_get_response_get_cmd_id(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  0);
}

/**
 * @brief Get field value from gopro_get_response message
 *
 * @return Value
 */
static inline uint8_t mavlink_msg_gopro_get_response_get_value(const mavlink_message_t* msg)
{
	return _MAV_RETURN_uint8_t(msg,  1);
}

/**
 * @brief Decode a gopro_get_response message into a struct
 *
 * @param msg The message to decode
 * @param gopro_get_response C-struct to decode the message contents into
 */
static inline void mavlink_msg_gopro_get_response_decode(const mavlink_message_t* msg, mavlink_gopro_get_response_t* gopro_get_response)
{
#if MAVLINK_NEED_BYTE_SWAP
	gopro_get_response->cmd_id = mavlink_msg_gopro_get_response_get_cmd_id(msg);
	gopro_get_response->value = mavlink_msg_gopro_get_response_get_value(msg);
#else
	memcpy(gopro_get_response, _MAV_PAYLOAD(msg), MAVLINK_MSG_ID_GOPRO_GET_RESPONSE_LEN);
#endif
}
