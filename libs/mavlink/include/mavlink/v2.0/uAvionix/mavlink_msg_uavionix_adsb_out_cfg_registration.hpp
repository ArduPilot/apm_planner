// MESSAGE UAVIONIX_ADSB_OUT_CFG_REGISTRATION support class

#pragma once

namespace mavlink {
namespace uAvionix {
namespace msg {

/**
 * @brief UAVIONIX_ADSB_OUT_CFG_REGISTRATION message
 *
 * Aircraft Registration.
 */
struct UAVIONIX_ADSB_OUT_CFG_REGISTRATION : mavlink::Message {
    static constexpr msgid_t MSG_ID = 10004;
    static constexpr size_t LENGTH = 9;
    static constexpr size_t MIN_LENGTH = 9;
    static constexpr uint8_t CRC_EXTRA = 133;
    static constexpr auto NAME = "UAVIONIX_ADSB_OUT_CFG_REGISTRATION";


    std::array<char, 9> registration; /*<  Aircraft Registration (ASCII string A-Z, 0-9 only), e.g. "N8644B ". Trailing spaces (0x20) only. This is null-terminated. */


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
        ss << "  registration: \"" << to_string(registration) << "\"" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << registration;                  // offset: 0
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> registration;                  // offset: 0
    }
};

} // namespace msg
} // namespace uAvionix
} // namespace mavlink
