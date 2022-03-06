// MESSAGE GPS2_RAW support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief GPS2_RAW message
 *
 * Second GPS data.
 */
struct GPS2_RAW : mavlink::Message {
    static constexpr msgid_t MSG_ID = 124;
    static constexpr size_t LENGTH = 57;
    static constexpr size_t MIN_LENGTH = 35;
    static constexpr uint8_t CRC_EXTRA = 87;
    static constexpr auto NAME = "GPS2_RAW";


    uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. */
    uint8_t fix_type; /*<  GPS fix type. */
    int32_t lat; /*< [degE7] Latitude (WGS84) */
    int32_t lon; /*< [degE7] Longitude (WGS84) */
    int32_t alt; /*< [mm] Altitude (MSL). Positive for up. */
    uint16_t eph; /*<  GPS HDOP horizontal dilution of position (unitless). If unknown, set to: UINT16_MAX */
    uint16_t epv; /*<  GPS VDOP vertical dilution of position (unitless). If unknown, set to: UINT16_MAX */
    uint16_t vel; /*< [cm/s] GPS ground speed. If unknown, set to: UINT16_MAX */
    uint16_t cog; /*< [cdeg] Course over ground (NOT heading, but direction of movement): 0.0..359.99 degrees. If unknown, set to: UINT16_MAX */
    uint8_t satellites_visible; /*<  Number of satellites visible. If unknown, set to 255 */
    uint8_t dgps_numch; /*<  Number of DGPS satellites */
    uint32_t dgps_age; /*< [ms] Age of DGPS info */
    uint16_t yaw; /*< [cdeg] Yaw in earth frame from north. Use 0 if this GPS does not provide yaw. Use 65535 if this GPS is configured to provide yaw and is currently unable to provide it. Use 36000 for north. */
    int32_t alt_ellipsoid; /*< [mm] Altitude (above WGS84, EGM96 ellipsoid). Positive for up. */
    uint32_t h_acc; /*< [mm] Position uncertainty. */
    uint32_t v_acc; /*< [mm] Altitude uncertainty. */
    uint32_t vel_acc; /*< [mm] Speed uncertainty. */
    uint32_t hdg_acc; /*< [degE5] Heading / track uncertainty */


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
        ss << "  dgps_numch: " << +dgps_numch << std::endl;
        ss << "  dgps_age: " << dgps_age << std::endl;
        ss << "  yaw: " << yaw << std::endl;
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
        map << dgps_age;                      // offset: 20
        map << eph;                           // offset: 24
        map << epv;                           // offset: 26
        map << vel;                           // offset: 28
        map << cog;                           // offset: 30
        map << fix_type;                      // offset: 32
        map << satellites_visible;            // offset: 33
        map << dgps_numch;                    // offset: 34
        map << yaw;                           // offset: 35
        map << alt_ellipsoid;                 // offset: 37
        map << h_acc;                         // offset: 41
        map << v_acc;                         // offset: 45
        map << vel_acc;                       // offset: 49
        map << hdg_acc;                       // offset: 53
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> lat;                           // offset: 8
        map >> lon;                           // offset: 12
        map >> alt;                           // offset: 16
        map >> dgps_age;                      // offset: 20
        map >> eph;                           // offset: 24
        map >> epv;                           // offset: 26
        map >> vel;                           // offset: 28
        map >> cog;                           // offset: 30
        map >> fix_type;                      // offset: 32
        map >> satellites_visible;            // offset: 33
        map >> dgps_numch;                    // offset: 34
        map >> yaw;                           // offset: 35
        map >> alt_ellipsoid;                 // offset: 37
        map >> h_acc;                         // offset: 41
        map >> v_acc;                         // offset: 45
        map >> vel_acc;                       // offset: 49
        map >> hdg_acc;                       // offset: 53
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
