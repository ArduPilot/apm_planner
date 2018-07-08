// MESSAGE CAMERA_SETTINGS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief CAMERA_SETTINGS message
 *
 * WIP: Settings of a camera, can be requested using MAV_CMD_REQUEST_CAMERA_SETTINGS and written using MAV_CMD_SET_CAMERA_SETTINGS
 */
struct CAMERA_SETTINGS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 260;
    static constexpr size_t LENGTH = 28;
    static constexpr size_t MIN_LENGTH = 28;
    static constexpr uint8_t CRC_EXTRA = 8;
    static constexpr auto NAME = "CAMERA_SETTINGS";


    uint32_t time_boot_ms; /*< Timestamp (milliseconds since system boot) */
    uint8_t camera_id; /*< Camera ID if there are multiple */
    float aperture; /*< Aperture is 1/value */
    uint8_t aperture_locked; /*< Aperture locked (0: auto, 1: locked) */
    float shutter_speed; /*< Shutter speed in s */
    uint8_t shutter_speed_locked; /*< Shutter speed locked (0: auto, 1: locked) */
    float iso_sensitivity; /*< ISO sensitivity */
    uint8_t iso_sensitivity_locked; /*< ISO sensitivity locked (0: auto, 1: locked) */
    float white_balance; /*< Color temperature in degrees Kelvin */
    uint8_t white_balance_locked; /*< Color temperature locked (0: auto, 1: locked) */
    uint8_t mode_id; /*< Reserved for a camera mode ID */
    uint8_t color_mode_id; /*< Reserved for a color mode ID */
    uint8_t image_format_id; /*< Reserved for image format ID */


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
        ss << "  camera_id: " << +camera_id << std::endl;
        ss << "  aperture: " << aperture << std::endl;
        ss << "  aperture_locked: " << +aperture_locked << std::endl;
        ss << "  shutter_speed: " << shutter_speed << std::endl;
        ss << "  shutter_speed_locked: " << +shutter_speed_locked << std::endl;
        ss << "  iso_sensitivity: " << iso_sensitivity << std::endl;
        ss << "  iso_sensitivity_locked: " << +iso_sensitivity_locked << std::endl;
        ss << "  white_balance: " << white_balance << std::endl;
        ss << "  white_balance_locked: " << +white_balance_locked << std::endl;
        ss << "  mode_id: " << +mode_id << std::endl;
        ss << "  color_mode_id: " << +color_mode_id << std::endl;
        ss << "  image_format_id: " << +image_format_id << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << aperture;                      // offset: 4
        map << shutter_speed;                 // offset: 8
        map << iso_sensitivity;               // offset: 12
        map << white_balance;                 // offset: 16
        map << camera_id;                     // offset: 20
        map << aperture_locked;               // offset: 21
        map << shutter_speed_locked;          // offset: 22
        map << iso_sensitivity_locked;        // offset: 23
        map << white_balance_locked;          // offset: 24
        map << mode_id;                       // offset: 25
        map << color_mode_id;                 // offset: 26
        map << image_format_id;               // offset: 27
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> aperture;                      // offset: 4
        map >> shutter_speed;                 // offset: 8
        map >> iso_sensitivity;               // offset: 12
        map >> white_balance;                 // offset: 16
        map >> camera_id;                     // offset: 20
        map >> aperture_locked;               // offset: 21
        map >> shutter_speed_locked;          // offset: 22
        map >> iso_sensitivity_locked;        // offset: 23
        map >> white_balance_locked;          // offset: 24
        map >> mode_id;                       // offset: 25
        map >> color_mode_id;                 // offset: 26
        map >> image_format_id;               // offset: 27
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
