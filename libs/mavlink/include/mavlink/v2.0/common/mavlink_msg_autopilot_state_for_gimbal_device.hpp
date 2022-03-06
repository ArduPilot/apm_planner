// MESSAGE AUTOPILOT_STATE_FOR_GIMBAL_DEVICE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief AUTOPILOT_STATE_FOR_GIMBAL_DEVICE message
 *
 * Low level message containing autopilot state relevant for a gimbal device. This message is to be sent from the gimbal manager to the gimbal device component. The data of this message server for the gimbal's estimator corrections in particular horizon compensation, as well as the autopilot's control intention e.g. feed forward angular control in z-axis.
 */
struct AUTOPILOT_STATE_FOR_GIMBAL_DEVICE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 286;
    static constexpr size_t LENGTH = 53;
    static constexpr size_t MIN_LENGTH = 53;
    static constexpr uint8_t CRC_EXTRA = 210;
    static constexpr auto NAME = "AUTOPILOT_STATE_FOR_GIMBAL_DEVICE";


    uint8_t target_system; /*<  System ID */
    uint8_t target_component; /*<  Component ID */
    uint64_t time_boot_us; /*< [us] Timestamp (time since system boot). */
    std::array<float, 4> q; /*<  Quaternion components of autopilot attitude: w, x, y, z (1 0 0 0 is the null-rotation, Hamilton convention). */
    uint32_t q_estimated_delay_us; /*< [us] Estimated delay of the attitude data. */
    float vx; /*< [m/s] X Speed in NED (North, East, Down). */
    float vy; /*< [m/s] Y Speed in NED (North, East, Down). */
    float vz; /*< [m/s] Z Speed in NED (North, East, Down). */
    uint32_t v_estimated_delay_us; /*< [us] Estimated delay of the speed data. */
    float feed_forward_angular_velocity_z; /*< [rad/s] Feed forward Z component of angular velocity, positive is yawing to the right, NaN to be ignored. This is to indicate if the autopilot is actively yawing. */
    uint16_t estimator_status; /*<  Bitmap indicating which estimator outputs are valid. */
    uint8_t landed_state; /*<  The landed state. Is set to MAV_LANDED_STATE_UNDEFINED if landed state is unknown. */


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
        ss << "  time_boot_us: " << time_boot_us << std::endl;
        ss << "  q: [" << to_string(q) << "]" << std::endl;
        ss << "  q_estimated_delay_us: " << q_estimated_delay_us << std::endl;
        ss << "  vx: " << vx << std::endl;
        ss << "  vy: " << vy << std::endl;
        ss << "  vz: " << vz << std::endl;
        ss << "  v_estimated_delay_us: " << v_estimated_delay_us << std::endl;
        ss << "  feed_forward_angular_velocity_z: " << feed_forward_angular_velocity_z << std::endl;
        ss << "  estimator_status: " << estimator_status << std::endl;
        ss << "  landed_state: " << +landed_state << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_us;                  // offset: 0
        map << q;                             // offset: 8
        map << q_estimated_delay_us;          // offset: 24
        map << vx;                            // offset: 28
        map << vy;                            // offset: 32
        map << vz;                            // offset: 36
        map << v_estimated_delay_us;          // offset: 40
        map << feed_forward_angular_velocity_z; // offset: 44
        map << estimator_status;              // offset: 48
        map << target_system;                 // offset: 50
        map << target_component;              // offset: 51
        map << landed_state;                  // offset: 52
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_us;                  // offset: 0
        map >> q;                             // offset: 8
        map >> q_estimated_delay_us;          // offset: 24
        map >> vx;                            // offset: 28
        map >> vy;                            // offset: 32
        map >> vz;                            // offset: 36
        map >> v_estimated_delay_us;          // offset: 40
        map >> feed_forward_angular_velocity_z; // offset: 44
        map >> estimator_status;              // offset: 48
        map >> target_system;                 // offset: 50
        map >> target_component;              // offset: 51
        map >> landed_state;                  // offset: 52
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
