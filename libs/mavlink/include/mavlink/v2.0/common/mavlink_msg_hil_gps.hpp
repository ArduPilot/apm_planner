// MESSAGE HIL_GPS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief HIL_GPS message
 *
 * The global position, as returned by the Global Positioning System (GPS). This is
                 NOT the global position estimate of the sytem, but rather a RAW sensor value. See message GLOBAL_POSITION for the global position estimate.
 */
struct HIL_GPS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 113;
    static constexpr size_t LENGTH = 36;
    static constexpr size_t MIN_LENGTH = 36;
    static constexpr uint8_t CRC_EXTRA = 124;
    static constexpr auto NAME = "HIL_GPS";


    uint64_t time_usec; /*< Timestamp (microseconds since UNIX epoch or microseconds since system boot) */
    uint8_t fix_type; /*< 0-1: no fix, 2: 2D fix, 3: 3D fix. Some applications will not use the value of this field unless it is at least two, so always correctly fill in the fix. */
    int32_t lat; /*< Latitude (WGS84), in degrees * 1E7 */
    int32_t lon; /*< Longitude (WGS84), in degrees * 1E7 */
    int32_t alt; /*< Altitude (AMSL, not WGS84), in meters * 1000 (positive for up) */
    uint16_t eph; /*< GPS HDOP horizontal dilution of position in cm (m*100). If unknown, set to: 65535 */
    uint16_t epv; /*< GPS VDOP vertical dilution of position in cm (m*100). If unknown, set to: 65535 */
    uint16_t vel; /*< GPS ground speed in cm/s. If unknown, set to: 65535 */
    int16_t vn; /*< GPS velocity in cm/s in NORTH direction in earth-fixed NED frame */
    int16_t ve; /*< GPS velocity in cm/s in EAST direction in earth-fixed NED frame */
    int16_t vd; /*< GPS velocity in cm/s in DOWN direction in earth-fixed NED frame */
    uint16_t cog; /*< Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: 65535 */
    uint8_t satellites_visible; /*< Number of satellites visible. If unknown, set to 255 */


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
        ss << "  fix_type: " << +fix_type << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  alt: " << alt << std::endl;
        ss << "  eph: " << eph << std::endl;
        ss << "  epv: " << epv << std::endl;
        ss << "  vel: " << vel << std::endl;
        ss << "  vn: " << vn << std::endl;
        ss << "  ve: " << ve << std::endl;
        ss << "  vd: " << vd << std::endl;
        ss << "  cog: " << cog << std::endl;
        ss << "  satellites_visible: " << +satellites_visible << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << lat;                           // offset: 8
        map << lon;                           // offset: 12
        map << alt;                           // offset: 16
        map << eph;                           // offset: 20
        map << epv;                           // offset: 22
        map << vel;                           // offset: 24
        map << vn;                            // offset: 26
        map << ve;                            // offset: 28
        map << vd;                            // offset: 30
        map << cog;                           // offset: 32
        map << fix_type;                      // offset: 34
        map << satellites_visible;            // offset: 35
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> lat;                           // offset: 8
        map >> lon;                           // offset: 12
        map >> alt;                           // offset: 16
        map >> eph;                           // offset: 20
        map >> epv;                           // offset: 22
        map >> vel;                           // offset: 24
        map >> vn;                            // offset: 26
        map >> ve;                            // offset: 28
        map >> vd;                            // offset: 30
        map >> cog;                           // offset: 32
        map >> fix_type;                      // offset: 34
        map >> satellites_visible;            // offset: 35
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
