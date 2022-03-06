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
    static constexpr size_t LENGTH = 54;
    static constexpr size_t MIN_LENGTH = 36;
    static constexpr uint8_t CRC_EXTRA = 154;
    static constexpr auto NAME = "BATTERY_STATUS";


    uint8_t id; /*<  Battery ID */
    uint8_t battery_function; /*<  Function of the battery */
    uint8_t type; /*<  Type (chemistry) of the battery */
    int16_t temperature; /*< [cdegC] Temperature of the battery. INT16_MAX for unknown temperature. */
    std::array<uint16_t, 10> voltages; /*< [mV] Battery voltage of cells 1 to 10 (see voltages_ext for cells 11-14). Cells in this field above the valid cell count for this battery should have the UINT16_MAX value. If individual cell voltages are unknown or not measured for this battery, then the overall battery voltage should be filled in cell 0, with all others set to UINT16_MAX. If the voltage of the battery is greater than (UINT16_MAX - 1), then cell 0 should be set to (UINT16_MAX - 1), and cell 1 to the remaining voltage. This can be extended to multiple cells if the total voltage is greater than 2 * (UINT16_MAX - 1). */
    int16_t current_battery; /*< [cA] Battery current, -1: autopilot does not measure the current */
    int32_t current_consumed; /*< [mAh] Consumed charge, -1: autopilot does not provide consumption estimate */
    int32_t energy_consumed; /*< [hJ] Consumed energy, -1: autopilot does not provide energy consumption estimate */
    int8_t battery_remaining; /*< [%] Remaining battery energy. Values: [0-100], -1: autopilot does not estimate the remaining battery. */
    int32_t time_remaining; /*< [s] Remaining battery time, 0: autopilot does not provide remaining battery time estimate */
    uint8_t charge_state; /*<  State for extent of discharge, provided by autopilot for warning or external reactions */
    std::array<uint16_t, 4> voltages_ext; /*< [mV] Battery voltages for cells 11 to 14. Cells above the valid cell count for this battery should have a value of 0, where zero indicates not supported (note, this is different than for the voltages field and allows empty byte truncation). If the measured value is 0 then 1 should be sent instead. */
    uint8_t mode; /*<  Battery mode. Default (0) is that battery mode reporting is not supported or battery is in normal-use mode. */
    uint32_t fault_bitmask; /*<  Fault/health indications. These should be set when charge_state is MAV_BATTERY_CHARGE_STATE_FAILED or MAV_BATTERY_CHARGE_STATE_UNHEALTHY (if not, fault reporting is not supported). */


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
        ss << "  voltages_ext: [" << to_string(voltages_ext) << "]" << std::endl;
        ss << "  mode: " << +mode << std::endl;
        ss << "  fault_bitmask: " << fault_bitmask << std::endl;

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
        map << voltages_ext;                  // offset: 41
        map << mode;                          // offset: 49
        map << fault_bitmask;                 // offset: 50
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
        map >> voltages_ext;                  // offset: 41
        map >> mode;                          // offset: 49
        map >> fault_bitmask;                 // offset: 50
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
