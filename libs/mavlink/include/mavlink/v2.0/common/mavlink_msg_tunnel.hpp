// MESSAGE TUNNEL support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief TUNNEL message
 *
 * Message for transporting "arbitrary" variable-length data from one component to another (broadcast is not forbidden, but discouraged). The encoding of the data is usually extension specific, i.e. determined by the source, and is usually not documented as part of the MAVLink specification.
 */
struct TUNNEL : mavlink::Message {
    static constexpr msgid_t MSG_ID = 385;
    static constexpr size_t LENGTH = 133;
    static constexpr size_t MIN_LENGTH = 133;
    static constexpr uint8_t CRC_EXTRA = 147;
    static constexpr auto NAME = "TUNNEL";


    uint8_t target_system; /*<  System ID (can be 0 for broadcast, but this is discouraged) */
    uint8_t target_component; /*<  Component ID (can be 0 for broadcast, but this is discouraged) */
    uint16_t payload_type; /*<  A code that identifies the content of the payload (0 for unknown, which is the default). If this code is less than 32768, it is a 'registered' payload type and the corresponding code should be added to the MAV_TUNNEL_PAYLOAD_TYPE enum. Software creators can register blocks of types as needed. Codes greater than 32767 are considered local experiments and should not be checked in to any widely distributed codebase. */
    uint8_t payload_length; /*<  Length of the data transported in payload */
    std::array<uint8_t, 128> payload; /*<  Variable length payload. The payload length is defined by payload_length. The entire content of this block is opaque unless you understand the encoding specified by payload_type. */


    inline std::string get_name(void) const override
    {
            return NAME;
    }

    inline Info get_message_info(void) const override
    {
            return { MSG_ID, LENGTH, MIN_LENGTH, CRC_EXTRA };
    }

    inline std::string to_yaml(void) const override
    {
        std::stringstream ss;

        ss << NAME << ":" << std::endl;
        ss << "  target_system: " << +target_system << std::endl;
        ss << "  target_component: " << +target_component << std::endl;
        ss << "  payload_type: " << payload_type << std::endl;
        ss << "  payload_length: " << +payload_length << std::endl;
        ss << "  payload: [" << to_string(payload) << "]" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << payload_type;                  // offset: 0
        map << target_system;                 // offset: 2
        map << target_component;              // offset: 3
        map << payload_length;                // offset: 4
        map << payload;                       // offset: 5
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> payload_type;                  // offset: 0
        map >> target_system;                 // offset: 2
        map >> target_component;              // offset: 3
        map >> payload_length;                // offset: 4
        map >> payload;                       // offset: 5
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
