// MESSAGE FOLLOW_TARGET support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief FOLLOW_TARGET message
 *
 * current motion information from a designated system
 */
struct FOLLOW_TARGET : mavlink::Message {
    static constexpr msgid_t MSG_ID = 144;
    static constexpr size_t LENGTH = 93;
    static constexpr size_t MIN_LENGTH = 93;
    static constexpr uint8_t CRC_EXTRA = 127;
    static constexpr auto NAME = "FOLLOW_TARGET";


    uint64_t timestamp; /*< Timestamp in milliseconds since system boot */
    uint8_t est_capabilities; /*< bit positions for tracker reporting capabilities (POS = 0, VEL = 1, ACCEL = 2, ATT + RATES = 3) */
    int32_t lat; /*< Latitude (WGS84), in degrees * 1E7 */
    int32_t lon; /*< Longitude (WGS84), in degrees * 1E7 */
    float alt; /*< AMSL, in meters */
    std::array<float, 3> vel; /*< target velocity (0,0,0) for unknown */
    std::array<float, 3> acc; /*< linear target acceleration (0,0,0) for unknown */
    std::array<float, 4> attitude_q; /*< (1 0 0 0 for unknown) */
    std::array<float, 3> rates; /*< (0 0 0 for unknown) */
    std::array<float, 3> position_cov; /*< eph epv */
    uint64_t custom_state; /*< button states or switches of a tracker device */


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
        ss << "  timestamp: " << timestamp << std::endl;
        ss << "  est_capabilities: " << +est_capabilities << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lon: " << lon << std::endl;
        ss << "  alt: " << alt << std::endl;
        ss << "  vel: [" << to_string(vel) << "]" << std::endl;
        ss << "  acc: [" << to_string(acc) << "]" << std::endl;
        ss << "  attitude_q: [" << to_string(attitude_q) << "]" << std::endl;
        ss << "  rates: [" << to_string(rates) << "]" << std::endl;
        ss << "  position_cov: [" << to_string(position_cov) << "]" << std::endl;
        ss << "  custom_state: " << custom_state << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << timestamp;                     // offset: 0
        map << custom_state;                  // offset: 8
        map << lat;                           // offset: 16
        map << lon;                           // offset: 20
        map << alt;                           // offset: 24
        map << vel;                           // offset: 28
        map << acc;                           // offset: 40
        map << attitude_q;                    // offset: 52
        map << rates;                         // offset: 68
        map << position_cov;                  // offset: 80
        map << est_capabilities;              // offset: 92
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> timestamp;                     // offset: 0
        map >> custom_state;                  // offset: 8
        map >> lat;                           // offset: 16
        map >> lon;                           // offset: 20
        map >> alt;                           // offset: 24
        map >> vel;                           // offset: 28
        map >> acc;                           // offset: 40
        map >> attitude_q;                    // offset: 52
        map >> rates;                         // offset: 68
        map >> position_cov;                  // offset: 80
        map >> est_capabilities;              // offset: 92
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
