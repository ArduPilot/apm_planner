// MESSAGE WINCH_STATUS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief WINCH_STATUS message
 *
 * Winch status.
 */
struct WINCH_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 9005;
    static constexpr size_t LENGTH = 34;
    static constexpr size_t MIN_LENGTH = 34;
    static constexpr uint8_t CRC_EXTRA = 117;
    static constexpr auto NAME = "WINCH_STATUS";


    uint64_t time_usec; /*< [us] Timestamp (synced to UNIX time or since system boot). */
    float line_length; /*< [m] Length of line released. NaN if unknown */
    float speed; /*< [m/s] Speed line is being released or retracted. Positive values if being released, negative values if being retracted, NaN if unknown */
    float tension; /*< [kg] Tension on the line. NaN if unknown */
    float voltage; /*< [V] Voltage of the battery supplying the winch. NaN if unknown */
    float current; /*< [A] Current draw from the winch. NaN if unknown */
    int16_t temperature; /*< [degC] Temperature of the motor. INT16_MAX if unknown */
    uint32_t status; /*<  Status flags */


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
        ss << "  time_usec: " << time_usec << std::endl;
        ss << "  line_length: " << line_length << std::endl;
        ss << "  speed: " << speed << std::endl;
        ss << "  tension: " << tension << std::endl;
        ss << "  voltage: " << voltage << std::endl;
        ss << "  current: " << current << std::endl;
        ss << "  temperature: " << temperature << std::endl;
        ss << "  status: " << status << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << line_length;                   // offset: 8
        map << speed;                         // offset: 12
        map << tension;                       // offset: 16
        map << voltage;                       // offset: 20
        map << current;                       // offset: 24
        map << status;                        // offset: 28
        map << temperature;                   // offset: 32
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> line_length;                   // offset: 8
        map >> speed;                         // offset: 12
        map >> tension;                       // offset: 16
        map >> voltage;                       // offset: 20
        map >> current;                       // offset: 24
        map >> status;                        // offset: 28
        map >> temperature;                   // offset: 32
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
