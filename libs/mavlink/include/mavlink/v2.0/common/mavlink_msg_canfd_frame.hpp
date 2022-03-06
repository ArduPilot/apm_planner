// MESSAGE CANFD_FRAME support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief CANFD_FRAME message
 *
 * A forwarded CANFD frame as requested by MAV_CMD_CAN_FORWARD. These are separated from CAN_FRAME as they need different handling (eg. TAO handling)
 */
struct CANFD_FRAME : mavlink::Message {
    static constexpr msgid_t MSG_ID = 387;
    static constexpr size_t LENGTH = 72;
    static constexpr size_t MIN_LENGTH = 72;
    static constexpr uint8_t CRC_EXTRA = 4;
    static constexpr auto NAME = "CANFD_FRAME";


    uint8_t target_system; /*<  System ID. */
    uint8_t target_component; /*<  Component ID. */
    uint8_t bus; /*<  bus number */
    uint8_t len; /*<  Frame length */
    uint32_t id; /*<  Frame ID */
    std::array<uint8_t, 64> data; /*<  Frame data */


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
        ss << "  bus: " << +bus << std::endl;
        ss << "  len: " << +len << std::endl;
        ss << "  id: " << id << std::endl;
        ss << "  data: [" << to_string(data) << "]" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << id;                            // offset: 0
        map << target_system;                 // offset: 4
        map << target_component;              // offset: 5
        map << bus;                           // offset: 6
        map << len;                           // offset: 7
        map << data;                          // offset: 8
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> id;                            // offset: 0
        map >> target_system;                 // offset: 4
        map >> target_component;              // offset: 5
        map >> bus;                           // offset: 6
        map >> len;                           // offset: 7
        map >> data;                          // offset: 8
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
