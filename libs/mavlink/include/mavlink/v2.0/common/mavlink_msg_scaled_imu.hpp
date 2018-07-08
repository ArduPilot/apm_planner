// MESSAGE SCALED_IMU support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief SCALED_IMU message
 *
 * The RAW IMU readings for the usual 9DOF sensor setup. This message should contain the scaled values to the described units
 */
struct SCALED_IMU : mavlink::Message {
    static constexpr msgid_t MSG_ID = 26;
    static constexpr size_t LENGTH = 22;
    static constexpr size_t MIN_LENGTH = 22;
    static constexpr uint8_t CRC_EXTRA = 170;
    static constexpr auto NAME = "SCALED_IMU";


    uint32_t time_boot_ms; /*< Timestamp (milliseconds since system boot) */
    int16_t xacc; /*< X acceleration (mg) */
    int16_t yacc; /*< Y acceleration (mg) */
    int16_t zacc; /*< Z acceleration (mg) */
    int16_t xgyro; /*< Angular speed around X axis (millirad /sec) */
    int16_t ygyro; /*< Angular speed around Y axis (millirad /sec) */
    int16_t zgyro; /*< Angular speed around Z axis (millirad /sec) */
    int16_t xmag; /*< X Magnetic field (milli tesla) */
    int16_t ymag; /*< Y Magnetic field (milli tesla) */
    int16_t zmag; /*< Z Magnetic field (milli tesla) */


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
        ss << "  xacc: " << xacc << std::endl;
        ss << "  yacc: " << yacc << std::endl;
        ss << "  zacc: " << zacc << std::endl;
        ss << "  xgyro: " << xgyro << std::endl;
        ss << "  ygyro: " << ygyro << std::endl;
        ss << "  zgyro: " << zgyro << std::endl;
        ss << "  xmag: " << xmag << std::endl;
        ss << "  ymag: " << ymag << std::endl;
        ss << "  zmag: " << zmag << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << xacc;                          // offset: 4
        map << yacc;                          // offset: 6
        map << zacc;                          // offset: 8
        map << xgyro;                         // offset: 10
        map << ygyro;                         // offset: 12
        map << zgyro;                         // offset: 14
        map << xmag;                          // offset: 16
        map << ymag;                          // offset: 18
        map << zmag;                          // offset: 20
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> xacc;                          // offset: 4
        map >> yacc;                          // offset: 6
        map >> zacc;                          // offset: 8
        map >> xgyro;                         // offset: 10
        map >> ygyro;                         // offset: 12
        map >> zgyro;                         // offset: 14
        map >> xmag;                          // offset: 16
        map >> ymag;                          // offset: 18
        map >> zmag;                          // offset: 20
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
