// MESSAGE POSITION_TARGET_LOCAL_NED support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief POSITION_TARGET_LOCAL_NED message
 *
 * Reports the current commanded vehicle position, velocity, and acceleration as specified by the autopilot. This should match the commands sent in SET_POSITION_TARGET_LOCAL_NED if the vehicle is being controlled this way.
 */
struct POSITION_TARGET_LOCAL_NED : mavlink::Message {
    static constexpr msgid_t MSG_ID = 85;
    static constexpr size_t LENGTH = 51;
    static constexpr size_t MIN_LENGTH = 51;
    static constexpr uint8_t CRC_EXTRA = 140;
    static constexpr auto NAME = "POSITION_TARGET_LOCAL_NED";


    uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot). */
    uint8_t coordinate_frame; /*<  Valid options are: MAV_FRAME_LOCAL_NED = 1, MAV_FRAME_LOCAL_OFFSET_NED = 7, MAV_FRAME_BODY_NED = 8, MAV_FRAME_BODY_OFFSET_NED = 9 */
    uint16_t type_mask; /*<  Bitmap to indicate which dimensions should be ignored by the vehicle. */
    float x; /*< [m] X Position in NED frame */
    float y; /*< [m] Y Position in NED frame */
    float z; /*< [m] Z Position in NED frame (note, altitude is negative in NED) */
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
        map << coordinate_frame;              // offset: 50
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
        map >> coordinate_frame;              // offset: 50
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
