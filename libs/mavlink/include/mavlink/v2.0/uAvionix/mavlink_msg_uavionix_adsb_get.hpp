// MESSAGE UAVIONIX_ADSB_GET support class

#pragma once

namespace mavlink {
namespace uAvionix {
namespace msg {

/**
 * @brief UAVIONIX_ADSB_GET message
 *
 * Request messages.
 */
struct UAVIONIX_ADSB_GET : mavlink::Message {
    static constexpr msgid_t MSG_ID = 10006;
    static constexpr size_t LENGTH = 4;
    static constexpr size_t MIN_LENGTH = 4;
    static constexpr uint8_t CRC_EXTRA = 193;
    static constexpr auto NAME = "UAVIONIX_ADSB_GET";


    uint32_t ReqMessageId; /*<  Message ID to request. Supports any message in this 10000-10099 range */


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
        ss << "  ReqMessageId: " << ReqMessageId << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << ReqMessageId;                  // offset: 0
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> ReqMessageId;                  // offset: 0
    }
};

} // namespace msg
} // namespace uAvionix
} // namespace mavlink
