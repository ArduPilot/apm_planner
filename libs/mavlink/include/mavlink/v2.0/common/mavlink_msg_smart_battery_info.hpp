// MESSAGE SMART_BATTERY_INFO support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief SMART_BATTERY_INFO message
 *
 * Smart Battery information (static/infrequent update). Use for updates from: smart battery to flight stack, flight stack to GCS. Use BATTERY_STATUS for smart battery frequent updates.
 */
struct SMART_BATTERY_INFO : mavlink::Message {
    static constexpr msgid_t MSG_ID = 370;
    static constexpr size_t LENGTH = 109;
    static constexpr size_t MIN_LENGTH = 87;
    static constexpr uint8_t CRC_EXTRA = 75;
    static constexpr auto NAME = "SMART_BATTERY_INFO";


    uint8_t id; /*<  Battery ID */
    uint8_t battery_function; /*<  Function of the battery */
    uint8_t type; /*<  Type (chemistry) of the battery */
    int32_t capacity_full_specification; /*< [mAh] Capacity when full according to manufacturer, -1: field not provided. */
    int32_t capacity_full; /*< [mAh] Capacity when full (accounting for battery degradation), -1: field not provided. */
    uint16_t cycle_count; /*<  Charge/discharge cycle count. UINT16_MAX: field not provided. */
    std::array<char, 16> serial_number; /*<  Serial number in ASCII characters, 0 terminated. All 0: field not provided. */
    std::array<char, 50> device_name; /*<  Static device name in ASCII characters, 0 terminated. All 0: field not provided. Encode as manufacturer name then product name separated using an underscore. */
    uint16_t weight; /*< [g] Battery weight. 0: field not provided. */
    uint16_t discharge_minimum_voltage; /*< [mV] Minimum per-cell voltage when discharging. If not supplied set to UINT16_MAX value. */
    uint16_t charging_minimum_voltage; /*< [mV] Minimum per-cell voltage when charging. If not supplied set to UINT16_MAX value. */
    uint16_t resting_minimum_voltage; /*< [mV] Minimum per-cell voltage when resting. If not supplied set to UINT16_MAX value. */
    uint16_t charging_maximum_voltage; /*< [mV] Maximum per-cell voltage when charged. 0: field not provided. */
    uint8_t cells_in_series; /*<  Number of battery cells in series. 0: field not provided. */
    uint32_t discharge_maximum_current; /*< [mA] Maximum pack discharge current. 0: field not provided. */
    uint32_t discharge_maximum_burst_current; /*< [mA] Maximum pack discharge burst current. 0: field not provided. */
    std::array<char, 11> manufacture_date; /*<  Manufacture date (DD/MM/YYYY) in ASCII characters, 0 terminated. All 0: field not provided. */


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
        ss << "  capacity_full_specification: " << capacity_full_specification << std::endl;
        ss << "  capacity_full: " << capacity_full << std::endl;
        ss << "  cycle_count: " << cycle_count << std::endl;
        ss << "  serial_number: \"" << to_string(serial_number) << "\"" << std::endl;
        ss << "  device_name: \"" << to_string(device_name) << "\"" << std::endl;
        ss << "  weight: " << weight << std::endl;
        ss << "  discharge_minimum_voltage: " << discharge_minimum_voltage << std::endl;
        ss << "  charging_minimum_voltage: " << charging_minimum_voltage << std::endl;
        ss << "  resting_minimum_voltage: " << resting_minimum_voltage << std::endl;
        ss << "  charging_maximum_voltage: " << charging_maximum_voltage << std::endl;
        ss << "  cells_in_series: " << +cells_in_series << std::endl;
        ss << "  discharge_maximum_current: " << discharge_maximum_current << std::endl;
        ss << "  discharge_maximum_burst_current: " << discharge_maximum_burst_current << std::endl;
        ss << "  manufacture_date: \"" << to_string(manufacture_date) << "\"" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << capacity_full_specification;   // offset: 0
        map << capacity_full;                 // offset: 4
        map << cycle_count;                   // offset: 8
        map << weight;                        // offset: 10
        map << discharge_minimum_voltage;     // offset: 12
        map << charging_minimum_voltage;      // offset: 14
        map << resting_minimum_voltage;       // offset: 16
        map << id;                            // offset: 18
        map << battery_function;              // offset: 19
        map << type;                          // offset: 20
        map << serial_number;                 // offset: 21
        map << device_name;                   // offset: 37
        map << charging_maximum_voltage;      // offset: 87
        map << cells_in_series;               // offset: 89
        map << discharge_maximum_current;     // offset: 90
        map << discharge_maximum_burst_current; // offset: 94
        map << manufacture_date;              // offset: 98
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> capacity_full_specification;   // offset: 0
        map >> capacity_full;                 // offset: 4
        map >> cycle_count;                   // offset: 8
        map >> weight;                        // offset: 10
        map >> discharge_minimum_voltage;     // offset: 12
        map >> charging_minimum_voltage;      // offset: 14
        map >> resting_minimum_voltage;       // offset: 16
        map >> id;                            // offset: 18
        map >> battery_function;              // offset: 19
        map >> type;                          // offset: 20
        map >> serial_number;                 // offset: 21
        map >> device_name;                   // offset: 37
        map >> charging_maximum_voltage;      // offset: 87
        map >> cells_in_series;               // offset: 89
        map >> discharge_maximum_current;     // offset: 90
        map >> discharge_maximum_burst_current; // offset: 94
        map >> manufacture_date;              // offset: 98
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
