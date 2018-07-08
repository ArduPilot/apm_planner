// MESSAGE ADSB_VEHICLE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief ADSB_VEHICLE message
 *
 * The location and information of an ADSB vehicle
 */
struct ADSB_VEHICLE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 246;
    static constexpr size_t LENGTH = 38;
    static constexpr size_t MIN_LENGTH = 38;
    static constexpr uint8_t CRC_EXTRA = 184;
    static constexpr auto NAME = "ADSB_VEHICLE";


    uint32_t ICAO_address; /*< ICAO address */
    int32_t lat; /*< Latitude, expressed as degrees * 1E7 */
    int32_t lon; /*< Longitude, expressed as degrees * 1E7 */
    uint8_t altitude_type; /*< Type from ADSB_ALTITUDE_TYPE enum */
    int32_t altitude; /*< Altitude(ASL) in millimeters */
    uint16_t heading; /*< Course over ground in centidegrees */
    uint16_t hor_velocity; /*< The horizontal velocity in centimeters/second */
    int16_t ver_velocity; /*< The vertical velocity in centimeters/second, positive is up */
    std::array<char, 9> callsign; /*< The callsign, 8+null */
    uint8_t emitter_type; /*< Type from ADSB_EMITTER_TYPE enum */
    uint8_t tslc; /*< Time since last communication in seconds */
    uint16_t flags; /*< Flags to indicate various statuses including valid data fields */
    uint16_t squawk; /*< Squawk code */


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
        ss << "  ICAO_address: " << ICAO_address << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  altitude_type: " << +altitude_type << std::endl;
        ss << "  altitude: " << altitude << std::endl;
        ss << "  heading: " << heading << std::endl;
        ss << "  hor_velocity: " << hor_velocity << std::endl;
        ss << "  ver_velocity: " << ver_velocity << std::endl;
        ss << "  callsign: \"" << to_string(callsign) << "\"" << std::endl;
        ss << "  emitter_type: " << +emitter_type << std::endl;
        ss << "  tslc: " << +tslc << std::endl;
        ss << "  flags: " << flags << std::endl;
        ss << "  squawk: " << squawk << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << ICAO_address;                  // offset: 0
        map << lat;                           // offset: 4
        map << lon;                           // offset: 8
        map << altitude;                      // offset: 12
        map << heading;                       // offset: 16
        map << hor_velocity;                  // offset: 18
        map << ver_velocity;                  // offset: 20
        map << flags;                         // offset: 22
        map << squawk;                        // offset: 24
        map << altitude_type;                 // offset: 26
        map << callsign;                      // offset: 27
        map << emitter_type;                  // offset: 36
        map << tslc;                          // offset: 37
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> ICAO_address;                  // offset: 0
        map >> lat;                           // offset: 4
        map >> lon;                           // offset: 8
        map >> altitude;                      // offset: 12
        map >> heading;                       // offset: 16
        map >> hor_velocity;                  // offset: 18
        map >> ver_velocity;                  // offset: 20
        map >> flags;                         // offset: 22
        map >> squawk;                        // offset: 24
        map >> altitude_type;                 // offset: 26
        map >> callsign;                      // offset: 27
        map >> emitter_type;                  // offset: 36
        map >> tslc;                          // offset: 37
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
