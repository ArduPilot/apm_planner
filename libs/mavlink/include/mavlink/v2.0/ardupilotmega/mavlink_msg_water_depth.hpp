// MESSAGE WATER_DEPTH support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief WATER_DEPTH message
 *
 * Water depth
 */
struct WATER_DEPTH : mavlink::Message {
    static constexpr msgid_t MSG_ID = 11038;
    static constexpr size_t LENGTH = 38;
    static constexpr size_t MIN_LENGTH = 38;
    static constexpr uint8_t CRC_EXTRA = 47;
    static constexpr auto NAME = "WATER_DEPTH";


    uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot) */
    uint8_t id; /*<  Onboard ID of the sensor */
    uint8_t healthy; /*<  Sensor data healthy (0=unhealthy, 1=healthy) */
    int32_t lat; /*< [degE7] Latitude */
    int32_t lng; /*< [degE7] Longitude */
    float alt; /*< [m] Altitude (MSL) of vehicle */
    float roll; /*< [rad] Roll angle */
    float pitch; /*< [rad] Pitch angle */
    float yaw; /*< [rad] Yaw angle */
    float distance; /*< [m] Distance (uncorrected) */
    float temperature; /*< [degC] Water temperature */


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
        ss << "  id: " << +id << std::endl;
        ss << "  healthy: " << +healthy << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lng: " << lng << std::endl;
        ss << "  alt: " << alt << std::endl;
        ss << "  roll: " << roll << std::endl;
        ss << "  pitch: " << pitch << std::endl;
        ss << "  yaw: " << yaw << std::endl;
        ss << "  distance: " << distance << std::endl;
        ss << "  temperature: " << temperature << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << lat;                           // offset: 4
        map << lng;                           // offset: 8
        map << alt;                           // offset: 12
        map << roll;                          // offset: 16
        map << pitch;                         // offset: 20
        map << yaw;                           // offset: 24
        map << distance;                      // offset: 28
        map << temperature;                   // offset: 32
        map << id;                            // offset: 36
        map << healthy;                       // offset: 37
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> lat;                           // offset: 4
        map >> lng;                           // offset: 8
        map >> alt;                           // offset: 12
        map >> roll;                          // offset: 16
        map >> pitch;                         // offset: 20
        map >> yaw;                           // offset: 24
        map >> distance;                      // offset: 28
        map >> temperature;                   // offset: 32
        map >> id;                            // offset: 36
        map >> healthy;                       // offset: 37
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
