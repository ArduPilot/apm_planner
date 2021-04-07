// MESSAGE DEBUG_FLOAT_ARRAY support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief DEBUG_FLOAT_ARRAY message
 *
 * Large debug/prototyping array. The message uses the maximum available payload for data. The array_id and name fields are used to discriminate between messages in code and in user interfaces (respectively). Do not use in production code.
 */
struct DEBUG_FLOAT_ARRAY : mavlink::Message {
    static constexpr msgid_t MSG_ID = 350;
    static constexpr size_t LENGTH = 252;
    static constexpr size_t MIN_LENGTH = 20;
    static constexpr uint8_t CRC_EXTRA = 232;
    static constexpr auto NAME = "DEBUG_FLOAT_ARRAY";


    uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude the number. */
    std::array<char, 10> name; /*<  Name, for human-friendly display in a Ground Control Station */
    uint16_t array_id; /*<  Unique ID used to discriminate between arrays */
    std::array<float, 58> data; /*<  data */


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
        ss << "  time_usec: " << time_usec << std::endl;
        ss << "  name: \"" << to_string(name) << "\"" << std::endl;
        ss << "  array_id: " << array_id << std::endl;
        ss << "  data: [" << to_string(data) << "]" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << array_id;                      // offset: 8
        map << name;                          // offset: 10
        map << data;                          // offset: 20
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> array_id;                      // offset: 8
        map >> name;                          // offset: 10
        map >> data;                          // offset: 20
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
