// MESSAGE CAMERA_SETTINGS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief CAMERA_SETTINGS message
 *
 * Settings of a camera. Can be requested with a MAV_CMD_REQUEST_MESSAGE command.
 */
struct CAMERA_SETTINGS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 260;
    static constexpr size_t LENGTH = 13;
    static constexpr size_t MIN_LENGTH = 5;
    static constexpr uint8_t CRC_EXTRA = 146;
    static constexpr auto NAME = "CAMERA_SETTINGS";


    uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot). */
    uint8_t mode_id; /*<  Camera mode */
    float zoomLevel; /*<  Current zoom level (0.0 to 100.0, NaN if not known) */
    float focusLevel; /*<  Current focus level (0.0 to 100.0, NaN if not known) */


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
        ss << "  mode_id: " << +mode_id << std::endl;
        ss << "  zoomLevel: " << zoomLevel << std::endl;
        ss << "  focusLevel: " << focusLevel << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << mode_id;                       // offset: 4
        map << zoomLevel;                     // offset: 5
        map << focusLevel;                    // offset: 9
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> mode_id;                       // offset: 4
        map >> zoomLevel;                     // offset: 5
        map >> focusLevel;                    // offset: 9
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
