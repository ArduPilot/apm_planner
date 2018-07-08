// MESSAGE HIL_SENSOR support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief HIL_SENSOR message
 *
 * The IMU readings in SI units in NED body frame
 */
struct HIL_SENSOR : mavlink::Message {
    static constexpr msgid_t MSG_ID = 107;
    static constexpr size_t LENGTH = 64;
    static constexpr size_t MIN_LENGTH = 64;
    static constexpr uint8_t CRC_EXTRA = 108;
    static constexpr auto NAME = "HIL_SENSOR";


    uint64_t time_usec; /*< Timestamp (microseconds, synced to UNIX time or since system boot) */
    float xacc; /*< X acceleration (m/s^2) */
    float yacc; /*< Y acceleration (m/s^2) */
    float zacc; /*< Z acceleration (m/s^2) */
    float xgyro; /*< Angular speed around X axis in body frame (rad / sec) */
    float ygyro; /*< Angular speed around Y axis in body frame (rad / sec) */
    float zgyro; /*< Angular speed around Z axis in body frame (rad / sec) */
    float xmag; /*< X Magnetic field (Gauss) */
    float ymag; /*< Y Magnetic field (Gauss) */
    float zmag; /*< Z Magnetic field (Gauss) */
    float abs_pressure; /*< Absolute pressure in millibar */
    float diff_pressure; /*< Differential pressure (airspeed) in millibar */
    float pressure_alt; /*< Altitude calculated from pressure */
    float temperature; /*< Temperature in degrees celsius */
    uint32_t fields_updated; /*< Bitmask for fields that have updated since last message, bit 0 = xacc, bit 12: temperature, bit 31: full reset of attitude/position/velocities/etc was performed in sim. */


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
        ss << "  xacc: " << xacc << std::endl;
        ss << "  yacc: " << yacc << std::endl;
        ss << "  zacc: " << zacc << std::endl;
        ss << "  xgyro: " << xgyro << std::endl;
        ss << "  ygyro: " << ygyro << std::endl;
        ss << "  zgyro: " << zgyro << std::endl;
        ss << "  xmag: " << xmag << std::endl;
        ss << "  ymag: " << ymag << std::endl;
        ss << "  zmag: " << zmag << std::endl;
        ss << "  abs_pressure: " << abs_pressure << std::endl;
        ss << "  diff_pressure: " << diff_pressure << std::endl;
        ss << "  pressure_alt: " << pressure_alt << std::endl;
        ss << "  temperature: " << temperature << std::endl;
        ss << "  fields_updated: " << fields_updated << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << xacc;                          // offset: 8
        map << yacc;                          // offset: 12
        map << zacc;                          // offset: 16
        map << xgyro;                         // offset: 20
        map << ygyro;                         // offset: 24
        map << zgyro;                         // offset: 28
        map << xmag;                          // offset: 32
        map << ymag;                          // offset: 36
        map << zmag;                          // offset: 40
        map << abs_pressure;                  // offset: 44
        map << diff_pressure;                 // offset: 48
        map << pressure_alt;                  // offset: 52
        map << temperature;                   // offset: 56
        map << fields_updated;                // offset: 60
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> xacc;                          // offset: 8
        map >> yacc;                          // offset: 12
        map >> zacc;                          // offset: 16
        map >> xgyro;                         // offset: 20
        map >> ygyro;                         // offset: 24
        map >> zgyro;                         // offset: 28
        map >> xmag;                          // offset: 32
        map >> ymag;                          // offset: 36
        map >> zmag;                          // offset: 40
        map >> abs_pressure;                  // offset: 44
        map >> diff_pressure;                 // offset: 48
        map >> pressure_alt;                  // offset: 52
        map >> temperature;                   // offset: 56
        map >> fields_updated;                // offset: 60
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
