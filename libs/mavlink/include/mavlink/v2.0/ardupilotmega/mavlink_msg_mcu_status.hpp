// MESSAGE MCU_STATUS support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief MCU_STATUS message
 *
 * The MCU status, giving MCU temperature and voltage. The min and max voltages are to allow for detecting power supply instability.
 */
struct MCU_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 11039;
    static constexpr size_t LENGTH = 9;
    static constexpr size_t MIN_LENGTH = 9;
    static constexpr uint8_t CRC_EXTRA = 142;
    static constexpr auto NAME = "MCU_STATUS";


    uint8_t id; /*<  MCU instance */
    int16_t MCU_temperature; /*< [cdegC] MCU Internal temperature */
    uint16_t MCU_voltage; /*< [mV] MCU voltage */
    uint16_t MCU_voltage_min; /*< [mV] MCU voltage minimum */
    uint16_t MCU_voltage_max; /*< [mV] MCU voltage maximum */


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
        ss << "  MCU_temperature: " << MCU_temperature << std::endl;
        ss << "  MCU_voltage: " << MCU_voltage << std::endl;
        ss << "  MCU_voltage_min: " << MCU_voltage_min << std::endl;
        ss << "  MCU_voltage_max: " << MCU_voltage_max << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << MCU_temperature;               // offset: 0
        map << MCU_voltage;                   // offset: 2
        map << MCU_voltage_min;               // offset: 4
        map << MCU_voltage_max;               // offset: 6
        map << id;                            // offset: 8
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> MCU_temperature;               // offset: 0
        map >> MCU_voltage;                   // offset: 2
        map >> MCU_voltage_min;               // offset: 4
        map >> MCU_voltage_max;               // offset: 6
        map >> id;                            // offset: 8
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
