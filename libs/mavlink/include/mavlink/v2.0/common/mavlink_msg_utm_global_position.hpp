// MESSAGE UTM_GLOBAL_POSITION support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief UTM_GLOBAL_POSITION message
 *
 * The global position resulting from GPS and sensor fusion.
 */
struct UTM_GLOBAL_POSITION : mavlink::Message {
    static constexpr msgid_t MSG_ID = 340;
    static constexpr size_t LENGTH = 70;
    static constexpr size_t MIN_LENGTH = 70;
    static constexpr uint8_t CRC_EXTRA = 99;
    static constexpr auto NAME = "UTM_GLOBAL_POSITION";


    uint64_t time; /*< [us] Time of applicability of position (microseconds since UNIX epoch). */
    std::array<uint8_t, 18> uas_id; /*<  Unique UAS ID. */
    int32_t lat; /*< [degE7] Latitude (WGS84) */
    int32_t lon; /*< [degE7] Longitude (WGS84) */
    int32_t alt; /*< [mm] Altitude (WGS84) */
    int32_t relative_alt; /*< [mm] Altitude above ground */
    int16_t vx; /*< [cm/s] Ground X speed (latitude, positive north) */
    int16_t vy; /*< [cm/s] Ground Y speed (longitude, positive east) */
    int16_t vz; /*< [cm/s] Ground Z speed (altitude, positive down) */
    uint16_t h_acc; /*< [mm] Horizontal position uncertainty (standard deviation) */
    uint16_t v_acc; /*< [mm] Altitude uncertainty (standard deviation) */
    uint16_t vel_acc; /*< [cm/s] Speed uncertainty (standard deviation) */
    int32_t next_lat; /*< [degE7] Next waypoint, latitude (WGS84) */
    int32_t next_lon; /*< [degE7] Next waypoint, longitude (WGS84) */
    int32_t next_alt; /*< [mm] Next waypoint, altitude (WGS84) */
    uint16_t update_rate; /*< [cs] Time until next update. Set to 0 if unknown or in data driven mode. */
    uint8_t flight_state; /*<  Flight state */
    uint8_t flags; /*<  Bitwise OR combination of the data available flags. */


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
        ss << "  time: " << time << std::endl;
        ss << "  uas_id: [" << to_string(uas_id) << "]" << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  alt: " << alt << std::endl;
        ss << "  relative_alt: " << relative_alt << std::endl;
        ss << "  vx: " << vx << std::endl;
        ss << "  vy: " << vy << std::endl;
        ss << "  vz: " << vz << std::endl;
        ss << "  h_acc: " << h_acc << std::endl;
        ss << "  v_acc: " << v_acc << std::endl;
        ss << "  vel_acc: " << vel_acc << std::endl;
        ss << "  next_lat: " << next_lat << std::endl;
        ss << "  next_lon: " << next_lon << std::endl;
        ss << "  next_alt: " << next_alt << std::endl;
        ss << "  update_rate: " << update_rate << std::endl;
        ss << "  flight_state: " << +flight_state << std::endl;
        ss << "  flags: " << +flags << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time;                          // offset: 0
        map << lat;                           // offset: 8
        map << lon;                           // offset: 12
        map << alt;                           // offset: 16
        map << relative_alt;                  // offset: 20
        map << next_lat;                      // offset: 24
        map << next_lon;                      // offset: 28
        map << next_alt;                      // offset: 32
        map << vx;                            // offset: 36
        map << vy;                            // offset: 38
        map << vz;                            // offset: 40
        map << h_acc;                         // offset: 42
        map << v_acc;                         // offset: 44
        map << vel_acc;                       // offset: 46
        map << update_rate;                   // offset: 48
        map << uas_id;                        // offset: 50
        map << flight_state;                  // offset: 68
        map << flags;                         // offset: 69
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time;                          // offset: 0
        map >> lat;                           // offset: 8
        map >> lon;                           // offset: 12
        map >> alt;                           // offset: 16
        map >> relative_alt;                  // offset: 20
        map >> next_lat;                      // offset: 24
        map >> next_lon;                      // offset: 28
        map >> next_alt;                      // offset: 32
        map >> vx;                            // offset: 36
        map >> vy;                            // offset: 38
        map >> vz;                            // offset: 40
        map >> h_acc;                         // offset: 42
        map >> v_acc;                         // offset: 44
        map >> vel_acc;                       // offset: 46
        map >> update_rate;                   // offset: 48
        map >> uas_id;                        // offset: 50
        map >> flight_state;                  // offset: 68
        map >> flags;                         // offset: 69
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
