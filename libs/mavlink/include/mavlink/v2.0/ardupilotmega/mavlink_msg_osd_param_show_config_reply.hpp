// MESSAGE OSD_PARAM_SHOW_CONFIG_REPLY support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief OSD_PARAM_SHOW_CONFIG_REPLY message
 *
 * Read configured OSD parameter reply.
 */
struct OSD_PARAM_SHOW_CONFIG_REPLY : mavlink::Message {
    static constexpr msgid_t MSG_ID = 11036;
    static constexpr size_t LENGTH = 34;
    static constexpr size_t MIN_LENGTH = 34;
    static constexpr uint8_t CRC_EXTRA = 177;
    static constexpr auto NAME = "OSD_PARAM_SHOW_CONFIG_REPLY";


    uint32_t request_id; /*<  Request ID - copied from request. */
    uint8_t result; /*<  Config error type. */
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
        ss << "  request_id: " << request_id << std::endl;
        ss << "  result: " << +result << std::endl;
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
        map << result;                        // offset: 16
        map << param_id;                      // offset: 17
        map << config_type;                   // offset: 33
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> request_id;                    // offset: 0
        map >> min_value;                     // offset: 4
        map >> max_value;                     // offset: 8
        map >> increment;                     // offset: 12
        map >> result;                        // offset: 16
        map >> param_id;                      // offset: 17
        map >> config_type;                   // offset: 33
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
