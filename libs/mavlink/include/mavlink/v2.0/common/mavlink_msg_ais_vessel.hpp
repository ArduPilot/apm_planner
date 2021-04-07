// MESSAGE AIS_VESSEL support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief AIS_VESSEL message
 *
 * The location and information of an AIS vessel
 */
struct AIS_VESSEL : mavlink::Message {
    static constexpr msgid_t MSG_ID = 301;
    static constexpr size_t LENGTH = 58;
    static constexpr size_t MIN_LENGTH = 58;
    static constexpr uint8_t CRC_EXTRA = 243;
    static constexpr auto NAME = "AIS_VESSEL";


    uint32_t MMSI; /*<  Mobile Marine Service Identifier, 9 decimal digits */
    int32_t lat; /*< [degE7] Latitude */
    int32_t lon; /*< [degE7] Longitude */
    uint16_t COG; /*< [cdeg] Course over ground */
    uint16_t heading; /*< [cdeg] True heading */
    uint16_t velocity; /*< [cm/s] Speed over ground */
    int8_t turn_rate; /*< [cdeg/s] Turn rate */
    uint8_t navigational_status; /*<  Navigational status */
    uint8_t type; /*<  Type of vessels */
    uint16_t dimension_bow; /*< [m] Distance from lat/lon location to bow */
    uint16_t dimension_stern; /*< [m] Distance from lat/lon location to stern */
    uint8_t dimension_port; /*< [m] Distance from lat/lon location to port side */
    uint8_t dimension_starboard; /*< [m] Distance from lat/lon location to starboard side */
    std::array<char, 7> callsign; /*<  The vessel callsign */
    std::array<char, 20> name; /*<  The vessel name */
    uint16_t tslc; /*< [s] Time since last communication in seconds */
    uint16_t flags; /*<  Bitmask to indicate various statuses including valid data fields */


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
        ss << "  MMSI: " << MMSI << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  COG: " << COG << std::endl;
        ss << "  heading: " << heading << std::endl;
        ss << "  velocity: " << velocity << std::endl;
        ss << "  turn_rate: " << +turn_rate << std::endl;
        ss << "  navigational_status: " << +navigational_status << std::endl;
        ss << "  type: " << +type << std::endl;
        ss << "  dimension_bow: " << dimension_bow << std::endl;
        ss << "  dimension_stern: " << dimension_stern << std::endl;
        ss << "  dimension_port: " << +dimension_port << std::endl;
        ss << "  dimension_starboard: " << +dimension_starboard << std::endl;
        ss << "  callsign: \"" << to_string(callsign) << "\"" << std::endl;
        ss << "  name: \"" << to_string(name) << "\"" << std::endl;
        ss << "  tslc: " << tslc << std::endl;
        ss << "  flags: " << flags << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << MMSI;                          // offset: 0
        map << lat;                           // offset: 4
        map << lon;                           // offset: 8
        map << COG;                           // offset: 12
        map << heading;                       // offset: 14
        map << velocity;                      // offset: 16
        map << dimension_bow;                 // offset: 18
        map << dimension_stern;               // offset: 20
        map << tslc;                          // offset: 22
        map << flags;                         // offset: 24
        map << turn_rate;                     // offset: 26
        map << navigational_status;           // offset: 27
        map << type;                          // offset: 28
        map << dimension_port;                // offset: 29
        map << dimension_starboard;           // offset: 30
        map << callsign;                      // offset: 31
        map << name;                          // offset: 38
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> MMSI;                          // offset: 0
        map >> lat;                           // offset: 4
        map >> lon;                           // offset: 8
        map >> COG;                           // offset: 12
        map >> heading;                       // offset: 14
        map >> velocity;                      // offset: 16
        map >> dimension_bow;                 // offset: 18
        map >> dimension_stern;               // offset: 20
        map >> tslc;                          // offset: 22
        map >> flags;                         // offset: 24
        map >> turn_rate;                     // offset: 26
        map >> navigational_status;           // offset: 27
        map >> type;                          // offset: 28
        map >> dimension_port;                // offset: 29
        map >> dimension_starboard;           // offset: 30
        map >> callsign;                      // offset: 31
        map >> name;                          // offset: 38
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
