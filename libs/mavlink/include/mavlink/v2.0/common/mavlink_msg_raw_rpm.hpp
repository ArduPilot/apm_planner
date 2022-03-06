// MESSAGE RAW_RPM support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief RAW_RPM message
 *
 * RPM sensor data message.
 */
struct RAW_RPM : mavlink::Message {
    static constexpr msgid_t MSG_ID = 339;
    static constexpr size_t LENGTH = 5;
    static constexpr size_t MIN_LENGTH = 5;
    static constexpr uint8_t CRC_EXTRA = 199;
    static constexpr auto NAME = "RAW_RPM";


    uint8_t index; /*<  Index of this RPM sensor (0-indexed) */
    float frequency; /*< [rpm] Indicated rate */


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
        ss << "  index: " << +index << std::endl;
        ss << "  frequency: " << frequency << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << frequency;                     // offset: 0
        map << index;                         // offset: 4
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> frequency;                     // offset: 0
        map >> index;                         // offset: 4
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
