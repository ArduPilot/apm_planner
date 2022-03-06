// MESSAGE SET_GPS_GLOBAL_ORIGIN support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief SET_GPS_GLOBAL_ORIGIN message
 *
 * Sets the GPS co-ordinates of the vehicle local origin (0,0,0) position. Vehicle should emit GPS_GLOBAL_ORIGIN irrespective of whether the origin is changed. This enables transform between the local coordinate frame and the global (GPS) coordinate frame, which may be necessary when (for example) indoor and outdoor settings are connected and the MAV should move from in- to outdoor.
 */
struct SET_GPS_GLOBAL_ORIGIN : mavlink::Message {
    static constexpr msgid_t MSG_ID = 48;
    static constexpr size_t LENGTH = 21;
    static constexpr size_t MIN_LENGTH = 13;
    static constexpr uint8_t CRC_EXTRA = 41;
    static constexpr auto NAME = "SET_GPS_GLOBAL_ORIGIN";


    uint8_t target_system; /*<  System ID */
    int32_t latitude; /*< [degE7] Latitude (WGS84) */
    int32_t longitude; /*< [degE7] Longitude (WGS84) */
    int32_t altitude; /*< [mm] Altitude (MSL). Positive for up. */
    uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. */


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
        ss << "  target_system: " << +target_system << std::endl;
        ss << "  latitude: " << latitude << std::endl;
        ss << "  longitude: " << longitude << std::endl;
        ss << "  altitude: " << altitude << std::endl;
        ss << "  time_usec: " << time_usec << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << latitude;                      // offset: 0
        map << longitude;                     // offset: 4
        map << altitude;                      // offset: 8
        map << target_system;                 // offset: 12
        map << time_usec;                     // offset: 13
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> latitude;                      // offset: 0
        map >> longitude;                     // offset: 4
        map >> altitude;                      // offset: 8
        map >> target_system;                 // offset: 12
        map >> time_usec;                     // offset: 13
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
