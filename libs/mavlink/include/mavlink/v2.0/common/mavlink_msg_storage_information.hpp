// MESSAGE STORAGE_INFORMATION support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief STORAGE_INFORMATION message
 *
 * WIP: Information about a storage medium
 */
struct STORAGE_INFORMATION : mavlink::Message {
    static constexpr msgid_t MSG_ID = 261;
    static constexpr size_t LENGTH = 26;
    static constexpr size_t MIN_LENGTH = 26;
    static constexpr uint8_t CRC_EXTRA = 244;
    static constexpr auto NAME = "STORAGE_INFORMATION";


    uint32_t time_boot_ms; /*< Timestamp (milliseconds since system boot) */
    uint8_t storage_id; /*< Storage ID if there are multiple */
    uint8_t status; /*< Status of storage (0 not available, 1 unformatted, 2 formatted) */
    float total_capacity; /*< Total capacity in MiB */
    float used_capacity; /*< Used capacity in MiB */
    float available_capacity; /*< Available capacity in MiB */
    float read_speed; /*< Read speed in MiB/s */
    float write_speed; /*< Write speed in MiB/s */


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
        ss << "  time_boot_ms: " << time_boot_ms << std::endl;
        ss << "  storage_id: " << +storage_id << std::endl;
        ss << "  status: " << +status << std::endl;
        ss << "  total_capacity: " << total_capacity << std::endl;
        ss << "  used_capacity: " << used_capacity << std::endl;
        ss << "  available_capacity: " << available_capacity << std::endl;
        ss << "  read_speed: " << read_speed << std::endl;
        ss << "  write_speed: " << write_speed << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << total_capacity;                // offset: 4
        map << used_capacity;                 // offset: 8
        map << available_capacity;            // offset: 12
        map << read_speed;                    // offset: 16
        map << write_speed;                   // offset: 20
        map << storage_id;                    // offset: 24
        map << status;                        // offset: 25
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> total_capacity;                // offset: 4
        map >> used_capacity;                 // offset: 8
        map >> available_capacity;            // offset: 12
        map >> read_speed;                    // offset: 16
        map >> write_speed;                   // offset: 20
        map >> storage_id;                    // offset: 24
        map >> status;                        // offset: 25
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
