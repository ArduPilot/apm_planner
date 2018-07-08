// MESSAGE SERVO_OUTPUT_RAW support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief SERVO_OUTPUT_RAW message
 *
 * The RAW values of the servo outputs (for RC input from the remote, use the RC_CHANNELS messages). The standard PPM modulation is as follows: 1000 microseconds: 0%, 2000 microseconds: 100%.
 */
struct SERVO_OUTPUT_RAW : mavlink::Message {
    static constexpr msgid_t MSG_ID = 36;
    static constexpr size_t LENGTH = 37;
    static constexpr size_t MIN_LENGTH = 21;
    static constexpr uint8_t CRC_EXTRA = 222;
    static constexpr auto NAME = "SERVO_OUTPUT_RAW";


    uint32_t time_usec; /*< Timestamp (microseconds since system boot) */
    uint8_t port; /*< Servo output port (set of 8 outputs = 1 port). Most MAVs will just use one, but this allows to encode more than 8 servos. */
    uint16_t servo1_raw; /*< Servo output 1 value, in microseconds */
    uint16_t servo2_raw; /*< Servo output 2 value, in microseconds */
    uint16_t servo3_raw; /*< Servo output 3 value, in microseconds */
    uint16_t servo4_raw; /*< Servo output 4 value, in microseconds */
    uint16_t servo5_raw; /*< Servo output 5 value, in microseconds */
    uint16_t servo6_raw; /*< Servo output 6 value, in microseconds */
    uint16_t servo7_raw; /*< Servo output 7 value, in microseconds */
    uint16_t servo8_raw; /*< Servo output 8 value, in microseconds */
    uint16_t servo9_raw; /*< Servo output 9 value, in microseconds */
    uint16_t servo10_raw; /*< Servo output 10 value, in microseconds */
    uint16_t servo11_raw; /*< Servo output 11 value, in microseconds */
    uint16_t servo12_raw; /*< Servo output 12 value, in microseconds */
    uint16_t servo13_raw; /*< Servo output 13 value, in microseconds */
    uint16_t servo14_raw; /*< Servo output 14 value, in microseconds */
    uint16_t servo15_raw; /*< Servo output 15 value, in microseconds */
    uint16_t servo16_raw; /*< Servo output 16 value, in microseconds */


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
        ss << "  port: " << +port << std::endl;
        ss << "  servo1_raw: " << servo1_raw << std::endl;
        ss << "  servo2_raw: " << servo2_raw << std::endl;
        ss << "  servo3_raw: " << servo3_raw << std::endl;
        ss << "  servo4_raw: " << servo4_raw << std::endl;
        ss << "  servo5_raw: " << servo5_raw << std::endl;
        ss << "  servo6_raw: " << servo6_raw << std::endl;
        ss << "  servo7_raw: " << servo7_raw << std::endl;
        ss << "  servo8_raw: " << servo8_raw << std::endl;
        ss << "  servo9_raw: " << servo9_raw << std::endl;
        ss << "  servo10_raw: " << servo10_raw << std::endl;
        ss << "  servo11_raw: " << servo11_raw << std::endl;
        ss << "  servo12_raw: " << servo12_raw << std::endl;
        ss << "  servo13_raw: " << servo13_raw << std::endl;
        ss << "  servo14_raw: " << servo14_raw << std::endl;
        ss << "  servo15_raw: " << servo15_raw << std::endl;
        ss << "  servo16_raw: " << servo16_raw << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << servo1_raw;                    // offset: 4
        map << servo2_raw;                    // offset: 6
        map << servo3_raw;                    // offset: 8
        map << servo4_raw;                    // offset: 10
        map << servo5_raw;                    // offset: 12
        map << servo6_raw;                    // offset: 14
        map << servo7_raw;                    // offset: 16
        map << servo8_raw;                    // offset: 18
        map << port;                          // offset: 20
        map << servo9_raw;                    // offset: 21
        map << servo10_raw;                   // offset: 23
        map << servo11_raw;                   // offset: 25
        map << servo12_raw;                   // offset: 27
        map << servo13_raw;                   // offset: 29
        map << servo14_raw;                   // offset: 31
        map << servo15_raw;                   // offset: 33
        map << servo16_raw;                   // offset: 35
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> servo1_raw;                    // offset: 4
        map >> servo2_raw;                    // offset: 6
        map >> servo3_raw;                    // offset: 8
        map >> servo4_raw;                    // offset: 10
        map >> servo5_raw;                    // offset: 12
        map >> servo6_raw;                    // offset: 14
        map >> servo7_raw;                    // offset: 16
        map >> servo8_raw;                    // offset: 18
        map >> port;                          // offset: 20
        map >> servo9_raw;                    // offset: 21
        map >> servo10_raw;                   // offset: 23
        map >> servo11_raw;                   // offset: 25
        map >> servo12_raw;                   // offset: 27
        map >> servo13_raw;                   // offset: 29
        map >> servo14_raw;                   // offset: 31
        map >> servo15_raw;                   // offset: 33
        map >> servo16_raw;                   // offset: 35
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
