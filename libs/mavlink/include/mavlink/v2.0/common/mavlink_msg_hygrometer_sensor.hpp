// MESSAGE HYGROMETER_SENSOR support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief HYGROMETER_SENSOR message
 *
 * Temperature and humidity from hygrometer.
 */
struct HYGROMETER_SENSOR : mavlink::Message {
    static constexpr msgid_t MSG_ID = 12920;
    static constexpr size_t LENGTH = 5;
    static constexpr size_t MIN_LENGTH = 5;
    static constexpr uint8_t CRC_EXTRA = 20;
    static constexpr auto NAME = "HYGROMETER_SENSOR";


    uint8_t id; /*<  Hygrometer ID */
    int16_t temperature; /*< [cdegC] Temperature */
    uint16_t humidity; /*< [c%] Humidity */


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
        ss << "  id: " << +id << std::endl;
        ss << "  temperature: " << temperature << std::endl;
        ss << "  humidity: " << humidity << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << temperature;                   // offset: 0
        map << humidity;                      // offset: 2
        map << id;                            // offset: 4
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> temperature;                   // offset: 0
        map >> humidity;                      // offset: 2
        map >> id;                            // offset: 4
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
