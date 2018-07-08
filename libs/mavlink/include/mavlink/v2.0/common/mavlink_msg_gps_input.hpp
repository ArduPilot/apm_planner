// MESSAGE GPS_INPUT support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief GPS_INPUT message
 *
 * GPS sensor input message.  This is a raw sensor value sent by the GPS. This is NOT the global position estimate of the sytem.
 */
struct GPS_INPUT : mavlink::Message {
    static constexpr msgid_t MSG_ID = 232;
    static constexpr size_t LENGTH = 63;
    static constexpr size_t MIN_LENGTH = 63;
    static constexpr uint8_t CRC_EXTRA = 151;
    static constexpr auto NAME = "GPS_INPUT";


    uint64_t time_usec; /*< Timestamp (micros since boot or Unix epoch) */
    uint8_t gps_id; /*< ID of the GPS for multiple GPS inputs */
    uint16_t ignore_flags; /*< Flags indicating which fields to ignore (see GPS_INPUT_IGNORE_FLAGS enum).  All other fields must be provided. */
    uint32_t time_week_ms; /*< GPS time (milliseconds from start of GPS week) */
    uint16_t time_week; /*< GPS week number */
    uint8_t fix_type; /*< 0-1: no fix, 2: 2D fix, 3: 3D fix. 4: 3D with DGPS. 5: 3D with RTK */
    int32_t lat; /*< Latitude (WGS84), in degrees * 1E7 */
    int32_t lon; /*< Longitude (WGS84), in degrees * 1E7 */
    float alt; /*< Altitude (AMSL, not WGS84), in m (positive for up) */
    float hdop; /*< GPS HDOP horizontal dilution of position in m */
    float vdop; /*< GPS VDOP vertical dilution of position in m */
    float vn; /*< GPS velocity in m/s in NORTH direction in earth-fixed NED frame */
    float ve; /*< GPS velocity in m/s in EAST direction in earth-fixed NED frame */
    float vd; /*< GPS velocity in m/s in DOWN direction in earth-fixed NED frame */
    float speed_accuracy; /*< GPS speed accuracy in m/s */
    float horiz_accuracy; /*< GPS horizontal accuracy in m */
    float vert_accuracy; /*< GPS vertical accuracy in m */
    uint8_t satellites_visible; /*< Number of satellites visible. */


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
        ss << "  gps_id: " << +gps_id << std::endl;
        ss << "  ignore_flags: " << ignore_flags << std::endl;
        ss << "  time_week_ms: " << time_week_ms << std::endl;
        ss << "  time_week: " << time_week << std::endl;
        ss << "  fix_type: " << +fix_type << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  alt: " << alt << std::endl;
        ss << "  hdop: " << hdop << std::endl;
        ss << "  vdop: " << vdop << std::endl;
        ss << "  vn: " << vn << std::endl;
        ss << "  ve: " << ve << std::endl;
        ss << "  vd: " << vd << std::endl;
        ss << "  speed_accuracy: " << speed_accuracy << std::endl;
        ss << "  horiz_accuracy: " << horiz_accuracy << std::endl;
        ss << "  vert_accuracy: " << vert_accuracy << std::endl;
        ss << "  satellites_visible: " << +satellites_visible << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << time_week_ms;                  // offset: 8
        map << lat;                           // offset: 12
        map << lon;                           // offset: 16
        map << alt;                           // offset: 20
        map << hdop;                          // offset: 24
        map << vdop;                          // offset: 28
        map << vn;                            // offset: 32
        map << ve;                            // offset: 36
        map << vd;                            // offset: 40
        map << speed_accuracy;                // offset: 44
        map << horiz_accuracy;                // offset: 48
        map << vert_accuracy;                 // offset: 52
        map << ignore_flags;                  // offset: 56
        map << time_week;                     // offset: 58
        map << gps_id;                        // offset: 60
        map << fix_type;                      // offset: 61
        map << satellites_visible;            // offset: 62
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> time_week_ms;                  // offset: 8
        map >> lat;                           // offset: 12
        map >> lon;                           // offset: 16
        map >> alt;                           // offset: 20
        map >> hdop;                          // offset: 24
        map >> vdop;                          // offset: 28
        map >> vn;                            // offset: 32
        map >> ve;                            // offset: 36
        map >> vd;                            // offset: 40
        map >> speed_accuracy;                // offset: 44
        map >> horiz_accuracy;                // offset: 48
        map >> vert_accuracy;                 // offset: 52
        map >> ignore_flags;                  // offset: 56
        map >> time_week;                     // offset: 58
        map >> gps_id;                        // offset: 60
        map >> fix_type;                      // offset: 61
        map >> satellites_visible;            // offset: 62
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
