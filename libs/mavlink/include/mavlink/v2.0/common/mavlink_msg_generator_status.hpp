// MESSAGE GENERATOR_STATUS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief GENERATOR_STATUS message
 *
 * Telemetry of power generation system. Alternator or mechanical generator.
 */
struct GENERATOR_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 373;
    static constexpr size_t LENGTH = 42;
    static constexpr size_t MIN_LENGTH = 42;
    static constexpr uint8_t CRC_EXTRA = 117;
    static constexpr auto NAME = "GENERATOR_STATUS";


    uint64_t status; /*<  Status flags. */
    uint16_t generator_speed; /*< [rpm] Speed of electrical generator or alternator. UINT16_MAX: field not provided. */
    float battery_current; /*< [A] Current into/out of battery. Positive for out. Negative for in. NaN: field not provided. */
    float load_current; /*< [A] Current going to the UAV. If battery current not available this is the DC current from the generator. Positive for out. Negative for in. NaN: field not provided */
    float power_generated; /*< [W] The power being generated. NaN: field not provided */
    float bus_voltage; /*< [V] Voltage of the bus seen at the generator, or battery bus if battery bus is controlled by generator and at a different voltage to main bus. */
    int16_t rectifier_temperature; /*< [degC] The temperature of the rectifier or power converter. INT16_MAX: field not provided. */
    float bat_current_setpoint; /*< [A] The target battery current. Positive for out. Negative for in. NaN: field not provided */
    int16_t generator_temperature; /*< [degC] The temperature of the mechanical motor, fuel cell core or generator. INT16_MAX: field not provided. */
    uint32_t runtime; /*< [s] Seconds this generator has run since it was rebooted. UINT32_MAX: field not provided. */
    int32_t time_until_maintenance; /*< [s] Seconds until this generator requires maintenance.  A negative value indicates maintenance is past-due. INT32_MAX: field not provided. */


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
        ss << "  status: " << status << std::endl;
        ss << "  generator_speed: " << generator_speed << std::endl;
        ss << "  battery_current: " << battery_current << std::endl;
        ss << "  load_current: " << load_current << std::endl;
        ss << "  power_generated: " << power_generated << std::endl;
        ss << "  bus_voltage: " << bus_voltage << std::endl;
        ss << "  rectifier_temperature: " << rectifier_temperature << std::endl;
        ss << "  bat_current_setpoint: " << bat_current_setpoint << std::endl;
        ss << "  generator_temperature: " << generator_temperature << std::endl;
        ss << "  runtime: " << runtime << std::endl;
        ss << "  time_until_maintenance: " << time_until_maintenance << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << status;                        // offset: 0
        map << battery_current;               // offset: 8
        map << load_current;                  // offset: 12
        map << power_generated;               // offset: 16
        map << bus_voltage;                   // offset: 20
        map << bat_current_setpoint;          // offset: 24
        map << runtime;                       // offset: 28
        map << time_until_maintenance;        // offset: 32
        map << generator_speed;               // offset: 36
        map << rectifier_temperature;         // offset: 38
        map << generator_temperature;         // offset: 40
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> status;                        // offset: 0
        map >> battery_current;               // offset: 8
        map >> load_current;                  // offset: 12
        map >> power_generated;               // offset: 16
        map >> bus_voltage;                   // offset: 20
        map >> bat_current_setpoint;          // offset: 24
        map >> runtime;                       // offset: 28
        map >> time_until_maintenance;        // offset: 32
        map >> generator_speed;               // offset: 36
        map >> rectifier_temperature;         // offset: 38
        map >> generator_temperature;         // offset: 40
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
