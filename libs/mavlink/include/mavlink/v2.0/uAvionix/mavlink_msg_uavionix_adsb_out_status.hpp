// MESSAGE UAVIONIX_ADSB_OUT_STATUS support class

#pragma once

namespace mavlink {
namespace uAvionix {
namespace msg {

/**
 * @brief UAVIONIX_ADSB_OUT_STATUS message
 *
 * Status message with information from UCP Heartbeat and Status messages.
 */
struct UAVIONIX_ADSB_OUT_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 10008;
    static constexpr size_t LENGTH = 14;
    static constexpr size_t MIN_LENGTH = 14;
    static constexpr uint8_t CRC_EXTRA = 240;
    static constexpr auto NAME = "UAVIONIX_ADSB_OUT_STATUS";


    uint8_t state; /*<  ADS-B transponder status state flags */
    uint16_t squawk; /*<  Mode A code (typically 1200 [0x04B0] for VFR) */
    uint8_t NIC_NACp; /*<  Integrity and Accuracy of traffic reported as a 4-bit value for each field (NACp 7:4, NIC 3:0) and encoded by Containment Radius (HPL) and Estimated Position Uncertainty (HFOM), respectively */
    uint8_t boardTemp; /*<  Board temperature in C */
    uint8_t fault; /*<  ADS-B transponder fault flags */
    std::array<char, 8> flight_id; /*<  Flight Identification: 8 ASCII characters, '0' through '9', 'A' through 'Z' or space. Spaces (0x20) used as a trailing pad character, or when call sign is unavailable. */


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
        ss << "  squawk: " << squawk << std::endl;
        ss << "  NIC_NACp: " << +NIC_NACp << std::endl;
        ss << "  boardTemp: " << +boardTemp << std::endl;
        ss << "  fault: " << +fault << std::endl;
        ss << "  flight_id: \"" << to_string(flight_id) << "\"" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << squawk;                        // offset: 0
        map << state;                         // offset: 2
        map << NIC_NACp;                      // offset: 3
        map << boardTemp;                     // offset: 4
        map << fault;                         // offset: 5
        map << flight_id;                     // offset: 6
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> squawk;                        // offset: 0
        map >> state;                         // offset: 2
        map >> NIC_NACp;                      // offset: 3
        map >> boardTemp;                     // offset: 4
        map >> fault;                         // offset: 5
        map >> flight_id;                     // offset: 6
    }
};

} // namespace msg
} // namespace uAvionix
} // namespace mavlink
