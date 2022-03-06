// MESSAGE OSD_PARAM_CONFIG support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief OSD_PARAM_CONFIG message
 *
 * Configure an OSD parameter slot.
 */
struct OSD_PARAM_CONFIG : mavlink::Message {
    static constexpr msgid_t MSG_ID = 11033;
    static constexpr size_t LENGTH = 37;
    static constexpr size_t MIN_LENGTH = 37;
    static constexpr uint8_t CRC_EXTRA = 195;
    static constexpr auto NAME = "OSD_PARAM_CONFIG";


    uint8_t target_system; /*<  System ID. */
    uint8_t target_component; /*<  Component ID. */
    uint32_t request_id; /*<  Request ID - copied to reply. */
    uint8_t osd_screen; /*<  OSD parameter screen index. */
    uint8_t osd_index; /*<  OSD parameter display index. */
    std::array<char, 16> param_id; /*<  Onboard parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string */
    uint8_t config_type; /*<  Config type. */
    float min_value; /*<  OSD parameter minimum value. */
    float max_value; /*<  OSD parameter maximum value. */
    float increment; /*<  OSD parameter increment. */


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
        ss << "  param_id: \"" << to_string(param_id) << "\"" << std::endl;
        ss << "  config_type: " << +config_type << std::endl;
        ss << "  min_value: " << min_value << std::endl;
        ss << "  max_value: " << max_value << std::endl;
        ss << "  increment: " << increment << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << request_id;                    // offset: 0
        map << min_value;                     // offset: 4
        map << max_value;                     // offset: 8
        map << increment;                     // offset: 12
        map << target_system;                 // offset: 16
        map << target_component;              // offset: 17
        map << osd_screen;                    // offset: 18
        map << osd_index;                     // offset: 19
        map << param_id;                      // offset: 20
        map << config_type;                   // offset: 36
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> request_id;                    // offset: 0
        map >> min_value;                     // offset: 4
        map >> max_value;                     // offset: 8
        map >> increment;                     // offset: 12
        map >> target_system;                 // offset: 16
        map >> target_component;              // offset: 17
        map >> osd_screen;                    // offset: 18
        map >> osd_index;                     // offset: 19
        map >> param_id;                      // offset: 20
        map >> config_type;                   // offset: 36
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
