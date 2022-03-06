// MESSAGE ODOMETRY support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief ODOMETRY message
 *
 * Odometry message to communicate odometry information with an external interface. Fits ROS REP 147 standard for aerial vehicles (http://www.ros.org/reps/rep-0147.html).
 */
struct ODOMETRY : mavlink::Message {
    static constexpr msgid_t MSG_ID = 331;
    static constexpr size_t LENGTH = 232;
    static constexpr size_t MIN_LENGTH = 230;
    static constexpr uint8_t CRC_EXTRA = 91;
    static constexpr auto NAME = "ODOMETRY";


    uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. */
    uint8_t frame_id; /*<  Coordinate frame of reference for the pose data. */
    uint8_t child_frame_id; /*<  Coordinate frame of reference for the velocity in free space (twist) data. */
    float x; /*< [m] X Position */
    float y; /*< [m] Y Position */
    float z; /*< [m] Z Position */
    std::array<float, 4> q; /*<  Quaternion components, w, x, y, z (1 0 0 0 is the null-rotation) */
    float vx; /*< [m/s] X linear speed */
    float vy; /*< [m/s] Y linear speed */
    float vz; /*< [m/s] Z linear speed */
    float rollspeed; /*< [rad/s] Roll angular speed */
    float pitchspeed; /*< [rad/s] Pitch angular speed */
    float yawspeed; /*< [rad/s] Yaw angular speed */
    std::array<float, 21> pose_covariance; /*<  Row-major representation of a 6x6 pose cross-covariance matrix upper right triangle (states: x, y, z, roll, pitch, yaw; first six entries are the first ROW, next five entries are the second ROW, etc.). If unknown, assign NaN value to first element in the array. */
    std::array<float, 21> velocity_covariance; /*<  Row-major representation of a 6x6 velocity cross-covariance matrix upper right triangle (states: vx, vy, vz, rollspeed, pitchspeed, yawspeed; first six entries are the first ROW, next five entries are the second ROW, etc.). If unknown, assign NaN value to first element in the array. */
    uint8_t reset_counter; /*<  Estimate reset counter. This should be incremented when the estimate resets in any of the dimensions (position, velocity, attitude, angular speed). This is designed to be used when e.g an external SLAM system detects a loop-closure and the estimate jumps. */
    uint8_t estimator_type; /*<  Type of estimator that is providing the odometry. */


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
        ss << "  frame_id: " << +frame_id << std::endl;
        ss << "  child_frame_id: " << +child_frame_id << std::endl;
        ss << "  x: " << x << std::endl;
        ss << "  y: " << y << std::endl;
        ss << "  z: " << z << std::endl;
        ss << "  q: [" << to_string(q) << "]" << std::endl;
        ss << "  vx: " << vx << std::endl;
        ss << "  vy: " << vy << std::endl;
        ss << "  vz: " << vz << std::endl;
        ss << "  rollspeed: " << rollspeed << std::endl;
        ss << "  pitchspeed: " << pitchspeed << std::endl;
        ss << "  yawspeed: " << yawspeed << std::endl;
        ss << "  pose_covariance: [" << to_string(pose_covariance) << "]" << std::endl;
        ss << "  velocity_covariance: [" << to_string(velocity_covariance) << "]" << std::endl;
        ss << "  reset_counter: " << +reset_counter << std::endl;
        ss << "  estimator_type: " << +estimator_type << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << x;                             // offset: 8
        map << y;                             // offset: 12
        map << z;                             // offset: 16
        map << q;                             // offset: 20
        map << vx;                            // offset: 36
        map << vy;                            // offset: 40
        map << vz;                            // offset: 44
        map << rollspeed;                     // offset: 48
        map << pitchspeed;                    // offset: 52
        map << yawspeed;                      // offset: 56
        map << pose_covariance;               // offset: 60
        map << velocity_covariance;           // offset: 144
        map << frame_id;                      // offset: 228
        map << child_frame_id;                // offset: 229
        map << reset_counter;                 // offset: 230
        map << estimator_type;                // offset: 231
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> x;                             // offset: 8
        map >> y;                             // offset: 12
        map >> z;                             // offset: 16
        map >> q;                             // offset: 20
        map >> vx;                            // offset: 36
        map >> vy;                            // offset: 40
        map >> vz;                            // offset: 44
        map >> rollspeed;                     // offset: 48
        map >> pitchspeed;                    // offset: 52
        map >> yawspeed;                      // offset: 56
        map >> pose_covariance;               // offset: 60
        map >> velocity_covariance;           // offset: 144
        map >> frame_id;                      // offset: 228
        map >> child_frame_id;                // offset: 229
        map >> reset_counter;                 // offset: 230
        map >> estimator_type;                // offset: 231
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
