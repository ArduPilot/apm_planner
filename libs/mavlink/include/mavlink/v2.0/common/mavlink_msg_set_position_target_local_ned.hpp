// MESSAGE SET_POSITION_TARGET_LOCAL_NED support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief SET_POSITION_TARGET_LOCAL_NED message
 *
 * Sets a desired vehicle position in a local north-east-down coordinate frame. Used by an external controller to command the vehicle (manual controller or other system).
 */
struct SET_POSITION_TARGET_LOCAL_NED : mavlink::Message {
    static constexpr msgid_t MSG_ID = 84;
    static constexpr size_t LENGTH = 53;
    static constexpr size_t MIN_LENGTH = 53;
    static constexpr uint8_t CRC_EXTRA = 143;
    static constexpr auto NAME = "SET_POSITION_TARGET_LOCAL_NED";


    uint32_t time_boot_ms; /*< Timestamp in milliseconds since system boot */
    uint8_t target_system; /*< System ID */
    uint8_t target_component; /*< Component ID */
    uint8_t coordinate_frame; /*< Valid options are: MAV_FRAME_LOCAL_NED = 1, MAV_FRAME_LOCAL_OFFSET_NED = 7, MAV_FRAME_BODY_NED = 8, MAV_FRAME_BODY_OFFSET_NED = 9 */
    uint16_t type_mask; /*< Bitmask to indicate which dimensions should be ignored by the vehicle: a value of 0b0000000000000000 or 0b0000001000000000 indicates that none of the setpoint dimensions should be ignored. If bit 10 is set the floats afx afy afz should be interpreted as force instead of acceleration. Mapping: bit 1: x, bit 2: y, bit 3: z, bit 4: vx, bit 5: vy, bit 6: vz, bit 7: ax, bit 8: ay, bit 9: az, bit 10: is force setpoint, bit 11: yaw, bit 12: yaw rate */
    float x; /*< X Position in NED frame in meters */
    float y; /*< Y Position in NED frame in meters */
    float z; /*< Z Position in NED frame in meters (note, altitude is negative in NED) */
    float vx; /*< X velocity in NED frame in meter / s */
    float vy; /*< Y velocity in NED frame in meter / s */
    float vz; /*< Z velocity in NED frame in meter / s */
    float afx; /*< X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N */
    float afy; /*< Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N */
    float afz; /*< Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N */
    float yaw; /*< yaw setpoint in rad */
    float yaw_rate; /*< yaw rate setpoint in rad/s */


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
        ss << "  time_boot_ms: " << time_boot_ms << std::endl;
        ss << "  target_system: " << +target_system << std::endl;
        ss << "  target_component: " << +target_component << std::endl;
        ss << "  coordinate_frame: " << +coordinate_frame << std::endl;
        ss << "  type_mask: " << type_mask << std::endl;
        ss << "  x: " << x << std::endl;
        ss << "  y: " << y << std::endl;
        ss << "  z: " << z << std::endl;
        ss << "  vx: " << vx << std::endl;
        ss << "  vy: " << vy << std::endl;
        ss << "  vz: " << vz << std::endl;
        ss << "  afx: " << afx << std::endl;
        ss << "  afy: " << afy << std::endl;
        ss << "  afz: " << afz << std::endl;
        ss << "  yaw: " << yaw << std::endl;
        ss << "  yaw_rate: " << yaw_rate << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << x;                             // offset: 4
        map << y;                             // offset: 8
        map << z;                             // offset: 12
        map << vx;                            // offset: 16
        map << vy;                            // offset: 20
        map << vz;                            // offset: 24
        map << afx;                           // offset: 28
        map << afy;                           // offset: 32
        map << afz;                           // offset: 36
        map << yaw;                           // offset: 40
        map << yaw_rate;                      // offset: 44
        map << type_mask;                     // offset: 48
        map << target_system;                 // offset: 50
        map << target_component;              // offset: 51
        map << coordinate_frame;              // offset: 52
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> x;                             // offset: 4
        map >> y;                             // offset: 8
        map >> z;                             // offset: 12
        map >> vx;                            // offset: 16
        map >> vy;                            // offset: 20
        map >> vz;                            // offset: 24
        map >> afx;                           // offset: 28
        map >> afy;                           // offset: 32
        map >> afz;                           // offset: 36
        map >> yaw;                           // offset: 40
        map >> yaw_rate;                      // offset: 44
        map >> type_mask;                     // offset: 48
        map >> target_system;                 // offset: 50
        map >> target_component;              // offset: 51
        map >> coordinate_frame;              // offset: 52
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
