// MESSAGE WHEEL_DISTANCE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief WHEEL_DISTANCE message
 *
 * Cumulative distance traveled for each reported wheel.
 */
struct WHEEL_DISTANCE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 9000;
    static constexpr size_t LENGTH = 137;
    static constexpr size_t MIN_LENGTH = 137;
    static constexpr uint8_t CRC_EXTRA = 113;
    static constexpr auto NAME = "WHEEL_DISTANCE";


    uint64_t time_usec; /*< [us] Timestamp (synced to UNIX time or since system boot). */
    uint8_t count; /*<  Number of wheels reported. */
    std::array<double, 16> distance; /*< [m] Distance reported by individual wheel encoders. Forward rotations increase values, reverse rotations decrease them. Not all wheels will necessarily have wheel encoders; the mapping of encoders to wheel positions must be agreed/understood by the endpoints. */


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
        ss << "  count: " << +count << std::endl;
        ss << "  distance: [" << to_string(distance) << "]" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << distance;                      // offset: 8
        map << count;                         // offset: 136
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> distance;                      // offset: 8
        map >> count;                         // offset: 136
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
