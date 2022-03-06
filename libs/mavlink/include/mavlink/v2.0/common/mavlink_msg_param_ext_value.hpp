// MESSAGE PARAM_EXT_VALUE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief PARAM_EXT_VALUE message
 *
 * Emit the value of a parameter. The inclusion of param_count and param_index in the message allows the recipient to keep track of received parameters and allows them to re-request missing parameters after a loss or timeout.
 */
struct PARAM_EXT_VALUE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 322;
    static constexpr size_t LENGTH = 149;
    static constexpr size_t MIN_LENGTH = 149;
    static constexpr uint8_t CRC_EXTRA = 243;
    static constexpr auto NAME = "PARAM_EXT_VALUE";


    std::array<char, 16> param_id; /*<  Parameter id, terminated by NULL if the length is less than 16 human-readable chars and WITHOUT null termination (NULL) byte if the length is exactly 16 chars - applications have to provide 16+1 bytes storage if the ID is stored as string */
    std::array<char, 128> param_value; /*<  Parameter value */
    uint8_t param_type; /*<  Parameter type. */
    uint16_t param_count; /*<  Total number of parameters */
    uint16_t param_index; /*<  Index of this parameter */


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
        ss << "  param_id: \"" << to_string(param_id) << "\"" << std::endl;
        ss << "  param_value: \"" << to_string(param_value) << "\"" << std::endl;
        ss << "  param_type: " << +param_type << std::endl;
        ss << "  param_count: " << param_count << std::endl;
        ss << "  param_index: " << param_index << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << param_count;                   // offset: 0
        map << param_index;                   // offset: 2
        map << param_id;                      // offset: 4
        map << param_value;                   // offset: 20
        map << param_type;                    // offset: 148
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> param_count;                   // offset: 0
        map >> param_index;                   // offset: 2
        map >> param_id;                      // offset: 4
        map >> param_value;                   // offset: 20
        map >> param_type;                    // offset: 148
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
