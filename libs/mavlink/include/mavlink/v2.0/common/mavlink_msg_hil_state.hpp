// MESSAGE HIL_STATE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief HIL_STATE message
 *
 * DEPRECATED PACKET! Suffers from missing airspeed fields and singularities due to Euler angles. Please use HIL_STATE_QUATERNION instead. Sent from simulation to autopilot. This packet is useful for high throughput applications such as hardware in the loop simulations.
 */
struct HIL_STATE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 90;
    static constexpr size_t LENGTH = 56;
    static constexpr size_t MIN_LENGTH = 56;
    static constexpr uint8_t CRC_EXTRA = 183;
    static constexpr auto NAME = "HIL_STATE";


    uint64_t time_usec; /*< Timestamp (microseconds since UNIX epoch or microseconds since system boot) */
    float roll; /*< Roll angle (rad) */
    float pitch; /*< Pitch angle (rad) */
    float yaw; /*< Yaw angle (rad) */
    float rollspeed; /*< Body frame roll / phi angular speed (rad/s) */
    float pitchspeed; /*< Body frame pitch / theta angular speed (rad/s) */
    float yawspeed; /*< Body frame yaw / psi angular speed (rad/s) */
    int32_t lat; /*< Latitude, expressed as degrees * 1E7 */
    int32_t lon; /*< Longitude, expressed as degrees * 1E7 */
    int32_t alt; /*< Altitude in meters, expressed as * 1000 (millimeters) */
    int16_t vx; /*< Ground X Speed (Latitude), expressed as m/s * 100 */
    int16_t vy; /*< Ground Y Speed (Longitude), expressed as m/s * 100 */
    int16_t vz; /*< Ground Z Speed (Altitude), expressed as m/s * 100 */
    int16_t xacc; /*< X acceleration (mg) */
    int16_t yacc; /*< Y acceleration (mg) */
    int16_t zacc; /*< Z acceleration (mg) */


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
        ss << "  roll: " << roll << std::endl;
        ss << "  pitch: " << pitch << std::endl;
        ss << "  yaw: " << yaw << std::endl;
        ss << "  rollspeed: " << rollspeed << std::endl;
        ss << "  pitchspeed: " << pitchspeed << std::endl;
        ss << "  yawspeed: " << yawspeed << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  alt: " << alt << std::endl;
        ss << "  vx: " << vx << std::endl;
        ss << "  vy: " << vy << std::endl;
        ss << "  vz: " << vz << std::endl;
        ss << "  xacc: " << xacc << std::endl;
        ss << "  yacc: " << yacc << std::endl;
        ss << "  zacc: " << zacc << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << roll;                          // offset: 8
        map << pitch;                         // offset: 12
        map << yaw;                           // offset: 16
        map << rollspeed;                     // offset: 20
        map << pitchspeed;                    // offset: 24
        map << yawspeed;                      // offset: 28
        map << lat;                           // offset: 32
        map << lon;                           // offset: 36
        map << alt;                           // offset: 40
        map << vx;                            // offset: 44
        map << vy;                            // offset: 46
        map << vz;                            // offset: 48
        map << xacc;                          // offset: 50
        map << yacc;                          // offset: 52
        map << zacc;                          // offset: 54
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> roll;                          // offset: 8
        map >> pitch;                         // offset: 12
        map >> yaw;                           // offset: 16
        map >> rollspeed;                     // offset: 20
        map >> pitchspeed;                    // offset: 24
        map >> yawspeed;                      // offset: 28
        map >> lat;                           // offset: 32
        map >> lon;                           // offset: 36
        map >> alt;                           // offset: 40
        map >> vx;                            // offset: 44
        map >> vy;                            // offset: 46
        map >> vz;                            // offset: 48
        map >> xacc;                          // offset: 50
        map >> yacc;                          // offset: 52
        map >> zacc;                          // offset: 54
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
