// MESSAGE GIMBAL_REPORT support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief GIMBAL_REPORT message
 *
 * 3 axis gimbal mesuraments
 */
struct GIMBAL_REPORT : mavlink::Message {
    static constexpr msgid_t MSG_ID = 200;
    static constexpr size_t LENGTH = 42;
    static constexpr size_t MIN_LENGTH = 42;
    static constexpr uint8_t CRC_EXTRA = 134;
    static constexpr auto NAME = "GIMBAL_REPORT";


    uint8_t target_system; /*< System ID */
    uint8_t target_component; /*< Component ID */
    float delta_time; /*< Time since last update (seconds) */
    float delta_angle_x; /*< Delta angle X (radians) */
    float delta_angle_y; /*< Delta angle Y (radians) */
    float delta_angle_z; /*< Delta angle X (radians) */
    float delta_velocity_x; /*< Delta velocity X (m/s) */
    float delta_velocity_y; /*< Delta velocity Y (m/s) */
    float delta_velocity_z; /*< Delta velocity Z (m/s) */
    float joint_roll; /*< Joint ROLL (radians) */
    float joint_el; /*< Joint EL (radians) */
    float joint_az; /*< Joint AZ (radians) */


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
        ss << "  delta_time: " << delta_time << std::endl;
        ss << "  delta_angle_x: " << delta_angle_x << std::endl;
        ss << "  delta_angle_y: " << delta_angle_y << std::endl;
        ss << "  delta_angle_z: " << delta_angle_z << std::endl;
        ss << "  delta_velocity_x: " << delta_velocity_x << std::endl;
        ss << "  delta_velocity_y: " << delta_velocity_y << std::endl;
        ss << "  delta_velocity_z: " << delta_velocity_z << std::endl;
        ss << "  joint_roll: " << joint_roll << std::endl;
        ss << "  joint_el: " << joint_el << std::endl;
        ss << "  joint_az: " << joint_az << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << delta_time;                    // offset: 0
        map << delta_angle_x;                 // offset: 4
        map << delta_angle_y;                 // offset: 8
        map << delta_angle_z;                 // offset: 12
        map << delta_velocity_x;              // offset: 16
        map << delta_velocity_y;              // offset: 20
        map << delta_velocity_z;              // offset: 24
        map << joint_roll;                    // offset: 28
        map << joint_el;                      // offset: 32
        map << joint_az;                      // offset: 36
        map << target_system;                 // offset: 40
        map << target_component;              // offset: 41
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> delta_time;                    // offset: 0
        map >> delta_angle_x;                 // offset: 4
        map >> delta_angle_y;                 // offset: 8
        map >> delta_angle_z;                 // offset: 12
        map >> delta_velocity_x;              // offset: 16
        map >> delta_velocity_y;              // offset: 20
        map >> delta_velocity_z;              // offset: 24
        map >> joint_roll;                    // offset: 28
        map >> joint_el;                      // offset: 32
        map >> joint_az;                      // offset: 36
        map >> target_system;                 // offset: 40
        map >> target_component;              // offset: 41
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
