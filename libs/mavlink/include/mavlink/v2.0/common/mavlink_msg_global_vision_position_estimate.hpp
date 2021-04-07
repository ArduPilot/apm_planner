// MESSAGE GLOBAL_VISION_POSITION_ESTIMATE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief GLOBAL_VISION_POSITION_ESTIMATE message
 *
 * Global position/attitude estimate from a vision source.
 */
struct GLOBAL_VISION_POSITION_ESTIMATE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 101;
    static constexpr size_t LENGTH = 117;
    static constexpr size_t MIN_LENGTH = 32;
    static constexpr uint8_t CRC_EXTRA = 102;
    static constexpr auto NAME = "GLOBAL_VISION_POSITION_ESTIMATE";


    uint64_t usec; /*< [us] Timestamp (UNIX time or since system boot) */
    float x; /*< [m] Global X position */
    float y; /*< [m] Global Y position */
    float z; /*< [m] Global Z position */
    float roll; /*< [rad] Roll angle */
    float pitch; /*< [rad] Pitch angle */
    float yaw; /*< [rad] Yaw angle */
    std::array<float, 21> covariance; /*<  Row-major representation of pose 6x6 cross-covariance matrix upper right triangle (states: x_global, y_global, z_global, roll, pitch, yaw; first six entries are the first ROW, next five entries are the second ROW, etc.). If unknown, assign NaN value to first element in the array. */
    uint8_t reset_counter; /*<  Estimate reset counter. This should be incremented when the estimate resets in any of the dimensions (position, velocity, attitude, angular speed). This is designed to be used when e.g an external SLAM system detects a loop-closure and the estimate jumps. */


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
        ss << "  usec: " << usec << std::endl;
        ss << "  x: " << x << std::endl;
        ss << "  y: " << y << std::endl;
        ss << "  z: " << z << std::endl;
        ss << "  roll: " << roll << std::endl;
        ss << "  pitch: " << pitch << std::endl;
        ss << "  yaw: " << yaw << std::endl;
        ss << "  covariance: [" << to_string(covariance) << "]" << std::endl;
        ss << "  reset_counter: " << +reset_counter << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << usec;                          // offset: 0
        map << x;                             // offset: 8
        map << y;                             // offset: 12
        map << z;                             // offset: 16
        map << roll;                          // offset: 20
        map << pitch;                         // offset: 24
        map << yaw;                           // offset: 28
        map << covariance;                    // offset: 32
        map << reset_counter;                 // offset: 116
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> usec;                          // offset: 0
        map >> x;                             // offset: 8
        map >> y;                             // offset: 12
        map >> z;                             // offset: 16
        map >> roll;                          // offset: 20
        map >> pitch;                         // offset: 24
        map >> yaw;                           // offset: 28
        map >> covariance;                    // offset: 32
        map >> reset_counter;                 // offset: 116
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
