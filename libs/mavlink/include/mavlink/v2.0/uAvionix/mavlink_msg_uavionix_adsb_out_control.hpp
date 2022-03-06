// MESSAGE UAVIONIX_ADSB_OUT_CONTROL support class

#pragma once

namespace mavlink {
namespace uAvionix {
namespace msg {

/**
 * @brief UAVIONIX_ADSB_OUT_CONTROL message
 *
 * Control message with all data sent in UCP control message.
 */
struct UAVIONIX_ADSB_OUT_CONTROL : mavlink::Message {
    static constexpr msgid_t MSG_ID = 10007;
    static constexpr size_t LENGTH = 17;
    static constexpr size_t MIN_LENGTH = 17;
    static constexpr uint8_t CRC_EXTRA = 71;
    static constexpr auto NAME = "UAVIONIX_ADSB_OUT_CONTROL";


    uint8_t state; /*<  ADS-B transponder control state flags */
    int32_t baroAltMSL; /*< [mbar] Barometric pressure altitude (MSL) relative to a standard atmosphere of 1013.2 mBar and NOT bar corrected altitude (m * 1E-3). (up +ve). If unknown set to INT32_MAX */
    uint16_t squawk; /*<  Mode A code (typically 1200 [0x04B0] for VFR) */
    uint8_t emergencyStatus; /*<  Emergency status */
    std::array<char, 8> flight_id; /*<  Flight Identification: 8 ASCII characters, '0' through '9', 'A' through 'Z' or space. Spaces (0x20) used as a trailing pad character, or when call sign is unavailable. */
    uint8_t x_bit; /*<  X-Bit enable (military transponders only) */


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
        ss << "  state: " << +state << std::endl;
        ss << "  baroAltMSL: " << baroAltMSL << std::endl;
        ss << "  squawk: " << squawk << std::endl;
        ss << "  emergencyStatus: " << +emergencyStatus << std::endl;
        ss << "  flight_id: \"" << to_string(flight_id) << "\"" << std::endl;
        ss << "  x_bit: " << +x_bit << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << baroAltMSL;                    // offset: 0
        map << squawk;                        // offset: 4
        map << state;                         // offset: 6
        map << emergencyStatus;               // offset: 7
        map << flight_id;                     // offset: 8
        map << x_bit;                         // offset: 16
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> baroAltMSL;                    // offset: 0
        map >> squawk;                        // offset: 4
        map >> state;                         // offset: 6
        map >> emergencyStatus;               // offset: 7
        map >> flight_id;                     // offset: 8
        map >> x_bit;                         // offset: 16
    }
};

} // namespace msg
} // namespace uAvionix
} // namespace mavlink
