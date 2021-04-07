// MESSAGE ACTUATOR_OUTPUT_STATUS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief ACTUATOR_OUTPUT_STATUS message
 *
 * The raw values of the actuator outputs (e.g. on Pixhawk, from MAIN, AUX ports). This message supersedes SERVO_OUTPUT_RAW.
 */
struct ACTUATOR_OUTPUT_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 375;
    static constexpr size_t LENGTH = 140;
    static constexpr size_t MIN_LENGTH = 140;
    static constexpr uint8_t CRC_EXTRA = 251;
    static constexpr auto NAME = "ACTUATOR_OUTPUT_STATUS";


    uint64_t time_usec; /*< [us] Timestamp (since system boot). */
    uint32_t active; /*<  Active outputs */
    std::array<float, 32> actuator; /*<  Servo / motor output array values. Zero values indicate unused channels. */


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
        ss << "  active: " << active << std::endl;
        ss << "  actuator: [" << to_string(actuator) << "]" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << active;                        // offset: 8
        map << actuator;                      // offset: 12
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> active;                        // offset: 8
        map >> actuator;                      // offset: 12
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
