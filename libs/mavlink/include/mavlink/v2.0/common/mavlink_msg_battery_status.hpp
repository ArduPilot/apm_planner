// MESSAGE BATTERY_STATUS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief BATTERY_STATUS message
 *
 * Battery information
 */
struct BATTERY_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 147;
    static constexpr size_t LENGTH = 41;
    static constexpr size_t MIN_LENGTH = 36;
    static constexpr uint8_t CRC_EXTRA = 154;
    static constexpr auto NAME = "BATTERY_STATUS";


    uint8_t id; /*< Battery ID */
    uint8_t battery_function; /*< Function of the battery */
    uint8_t type; /*< Type (chemistry) of the battery */
    int16_t temperature; /*< Temperature of the battery in centi-degrees celsius. INT16_MAX for unknown temperature. */
    std::array<uint16_t, 10> voltages; /*< Battery voltage of cells, in millivolts (1 = 1 millivolt). Cells above the valid cell count for this battery should have the UINT16_MAX value. */
    int16_t current_battery; /*< Battery current, in 10*milliamperes (1 = 10 milliampere), -1: autopilot does not measure the current */
    int32_t current_consumed; /*< Consumed charge, in milliampere hours (1 = 1 mAh), -1: autopilot does not provide mAh consumption estimate */
    int32_t energy_consumed; /*< Consumed energy, in HectoJoules (intergrated U*I*dt)  (1 = 100 Joule), -1: autopilot does not provide energy consumption estimate */
    int8_t battery_remaining; /*< Remaining battery energy: (0%: 0, 100%: 100), -1: autopilot does not estimate the remaining battery */
    int32_t time_remaining; /*< Remaining battery time, in seconds (1 = 1s = 0% energy left), 0: autopilot does not provide remaining battery time estimate */
    uint8_t charge_state; /*< State for extent of discharge, provided by autopilot for warning or external reactions */


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
        ss << "  battery_function: " << +battery_function << std::endl;
        ss << "  type: " << +type << std::endl;
        ss << "  temperature: " << temperature << std::endl;
        ss << "  voltages: [" << to_string(voltages) << "]" << std::endl;
        ss << "  current_battery: " << current_battery << std::endl;
        ss << "  current_consumed: " << current_consumed << std::endl;
        ss << "  energy_consumed: " << energy_consumed << std::endl;
        ss << "  battery_remaining: " << +battery_remaining << std::endl;
        ss << "  time_remaining: " << time_remaining << std::endl;
        ss << "  charge_state: " << +charge_state << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << current_consumed;              // offset: 0
        map << energy_consumed;               // offset: 4
        map << temperature;                   // offset: 8
        map << voltages;                      // offset: 10
        map << current_battery;               // offset: 30
        map << id;                            // offset: 32
        map << battery_function;              // offset: 33
        map << type;                          // offset: 34
        map << battery_remaining;             // offset: 35
        map << time_remaining;                // offset: 36
        map << charge_state;                  // offset: 40
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> current_consumed;              // offset: 0
        map >> energy_consumed;               // offset: 4
        map >> temperature;                   // offset: 8
        map >> voltages;                      // offset: 10
        map >> current_battery;               // offset: 30
        map >> id;                            // offset: 32
        map >> battery_function;              // offset: 33
        map >> type;                          // offset: 34
        map >> battery_remaining;             // offset: 35
        map >> time_remaining;                // offset: 36
        map >> charge_state;                  // offset: 40
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
