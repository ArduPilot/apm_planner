// MESSAGE UAVIONIX_ADSB_OUT_CFG_FLIGHTID support class

#pragma once

namespace mavlink {
namespace uAvionix {
namespace msg {

/**
 * @brief UAVIONIX_ADSB_OUT_CFG_FLIGHTID message
 *
 * Flight Identification for ADSB-Out vehicles.
 */
struct UAVIONIX_ADSB_OUT_CFG_FLIGHTID : mavlink::Message {
    static constexpr msgid_t MSG_ID = 10005;
    static constexpr size_t LENGTH = 9;
    static constexpr size_t MIN_LENGTH = 9;
    static constexpr uint8_t CRC_EXTRA = 103;
    static constexpr auto NAME = "UAVIONIX_ADSB_OUT_CFG_FLIGHTID";


    std::array<char, 9> flight_id; /*<  Flight Identification: 8 ASCII characters, '0' through '9', 'A' through 'Z' or space. Spaces (0x20) used as a trailing pad character, or when call sign is unavailable. Reflects Control message setting. This is null-terminated. */


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
        ss << "  flight_id: \"" << to_string(flight_id) << "\"" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << flight_id;                     // offset: 0
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> flight_id;                     // offset: 0
    }
};

} // namespace msg
} // namespace uAvionix
} // namespace mavlink
