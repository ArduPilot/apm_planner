// MESSAGE SET_POSITION_TARGET_GLOBAL_INT support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief SET_POSITION_TARGET_GLOBAL_INT message
 *
 * Sets a desired vehicle position, velocity, and/or acceleration in a global coordinate system (WGS84). Used by an external controller to command the vehicle (manual controller or other system).
 */
struct SET_POSITION_TARGET_GLOBAL_INT : mavlink::Message {
    static constexpr msgid_t MSG_ID = 86;
    static constexpr size_t LENGTH = 53;
    static constexpr size_t MIN_LENGTH = 53;
    static constexpr uint8_t CRC_EXTRA = 5;
    static constexpr auto NAME = "SET_POSITION_TARGET_GLOBAL_INT";


    uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot). The rationale for the timestamp in the setpoint is to allow the system to compensate for the transport delay of the setpoint. This allows the system to compensate processing latency. */
    uint8_t target_system; /*<  System ID */
    uint8_t target_component; /*<  Component ID */
    uint8_t coordinate_frame; /*<  Valid options are: MAV_FRAME_GLOBAL_INT = 5, MAV_FRAME_GLOBAL_RELATIVE_ALT_INT = 6, MAV_FRAME_GLOBAL_TERRAIN_ALT_INT = 11 */
    uint16_t type_mask; /*<  Bitmap to indicate which dimensions should be ignored by the vehicle. */
    int32_t lat_int; /*< [degE7] X Position in WGS84 frame */
    int32_t lon_int; /*< [degE7] Y Position in WGS84 frame */
    float alt; /*< [m] Altitude (MSL, Relative to home, or AGL - depending on frame) */
    float vx; /*< [m/s] X velocity in NED frame */
    float vy; /*< [m/s] Y velocity in NED frame */
    float vz; /*< [m/s] Z velocity in NED frame */
    float afx; /*< [m/s/s] X acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N */
    float afy; /*< [m/s/s] Y acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N */
    float afz; /*< [m/s/s] Z acceleration or force (if bit 10 of type_mask is set) in NED frame in meter / s^2 or N */
    float yaw; /*< [rad] yaw setpoint */
    float yaw_rate; /*< [rad/s] yaw rate setpoint */


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
        ss << "  lat_int: " << lat_int << std::endl;
        ss << "  lon_int: " << lon_int << std::endl;
        ss << "  alt: " << alt << std::endl;
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
        map << lat_int;                       // offset: 4
        map << lon_int;                       // offset: 8
        map << alt;                           // offset: 12
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
        map >> lat_int;                       // offset: 4
        map >> lon_int;                       // offset: 8
        map >> alt;                           // offset: 12
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
