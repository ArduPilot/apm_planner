// MESSAGE OSD_PARAM_SHOW_CONFIG support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief OSD_PARAM_SHOW_CONFIG message
 *
 * Read a configured an OSD parameter slot.
 */
struct OSD_PARAM_SHOW_CONFIG : mavlink::Message {
    static constexpr msgid_t MSG_ID = 11035;
    static constexpr size_t LENGTH = 8;
    static constexpr size_t MIN_LENGTH = 8;
    static constexpr uint8_t CRC_EXTRA = 128;
    static constexpr auto NAME = "OSD_PARAM_SHOW_CONFIG";


    uint8_t target_system; /*<  System ID. */
    uint8_t target_component; /*<  Component ID. */
    uint32_t request_id; /*<  Request ID - copied to reply. */
    uint8_t osd_screen; /*<  OSD parameter screen index. */
    uint8_t osd_index; /*<  OSD parameter display index. */


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
        ss << "  request_id: " << request_id << std::endl;
        ss << "  osd_screen: " << +osd_screen << std::endl;
        ss << "  osd_index: " << +osd_index << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << request_id;                    // offset: 0
        map << target_system;                 // offset: 4
        map << target_component;              // offset: 5
        map << osd_screen;                    // offset: 6
        map << osd_index;                     // offset: 7
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> request_id;                    // offset: 0
        map >> target_system;                 // offset: 4
        map >> target_component;              // offset: 5
        map >> osd_screen;                    // offset: 6
        map >> osd_index;                     // offset: 7
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
