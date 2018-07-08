// MESSAGE GPS_RAW_INT support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief GPS_RAW_INT message
 *
 * The global position, as returned by the Global Positioning System (GPS). This is
                NOT the global position estimate of the system, but rather a RAW sensor value. See message GLOBAL_POSITION for the global position estimate.
 */
struct GPS_RAW_INT : mavlink::Message {
    static constexpr msgid_t MSG_ID = 24;
    static constexpr size_t LENGTH = 50;
    static constexpr size_t MIN_LENGTH = 30;
    static constexpr uint8_t CRC_EXTRA = 24;
    static constexpr auto NAME = "GPS_RAW_INT";


    uint64_t time_usec; /*< Timestamp (microseconds since UNIX epoch or microseconds since system boot) */
    uint8_t fix_type; /*< See the GPS_FIX_TYPE enum. */
    int32_t lat; /*< Latitude (WGS84, EGM96 ellipsoid), in degrees * 1E7 */
    int32_t lon; /*< Longitude (WGS84, EGM96 ellipsoid), in degrees * 1E7 */
    int32_t alt; /*< Altitude (AMSL, NOT WGS84), in meters * 1000 (positive for up). Note that virtually all GPS modules provide the AMSL altitude in addition to the WGS84 altitude. */
    uint16_t eph; /*< GPS HDOP horizontal dilution of position (unitless). If unknown, set to: UINT16_MAX */
    uint16_t epv; /*< GPS VDOP vertical dilution of position (unitless). If unknown, set to: UINT16_MAX */
    uint16_t vel; /*< GPS ground speed (m/s * 100). If unknown, set to: UINT16_MAX */
    uint16_t cog; /*< Course over ground (NOT heading, but direction of movement) in degrees * 100, 0.0..359.99 degrees. If unknown, set to: UINT16_MAX */
    uint8_t satellites_visible; /*< Number of satellites visible. If unknown, set to 255 */
    int32_t alt_ellipsoid; /*< Altitude (above WGS84, EGM96 ellipsoid), in meters * 1000 (positive for up). */
    uint32_t h_acc; /*< Position uncertainty in meters * 1000 (positive for up). */
    uint32_t v_acc; /*< Altitude uncertainty in meters * 1000 (positive for up). */
    uint32_t vel_acc; /*< Speed uncertainty in meters * 1000 (positive for up). */
    uint32_t hdg_acc; /*< Heading / track uncertainty in degrees * 1e5. */


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
        ss << "  cog: " << cog << std::endl;
        ss << "  satellites_visible: " << +satellites_visible << std::endl;
        ss << "  alt_ellipsoid: " << alt_ellipsoid << std::endl;
        ss << "  h_acc: " << h_acc << std::endl;
        ss << "  v_acc: " << v_acc << std::endl;
        ss << "  vel_acc: " << vel_acc << std::endl;
        ss << "  hdg_acc: " << hdg_acc << std::endl;

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
        map << cog;                           // offset: 26
        map << fix_type;                      // offset: 28
        map << satellites_visible;            // offset: 29
        map << alt_ellipsoid;                 // offset: 30
        map << h_acc;                         // offset: 34
        map << v_acc;                         // offset: 38
        map << vel_acc;                       // offset: 42
        map << hdg_acc;                       // offset: 46
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
        map >> cog;                           // offset: 26
        map >> fix_type;                      // offset: 28
        map >> satellites_visible;            // offset: 29
        map >> alt_ellipsoid;                 // offset: 30
        map >> h_acc;                         // offset: 34
        map >> v_acc;                         // offset: 38
        map >> vel_acc;                       // offset: 42
        map >> hdg_acc;                       // offset: 46
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
