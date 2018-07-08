// MESSAGE CAMERA_CAPTURE_STATUS support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief CAMERA_CAPTURE_STATUS message
 *
 * WIP: Information about the status of a capture
 */
struct CAMERA_CAPTURE_STATUS : mavlink::Message {
    static constexpr msgid_t MSG_ID = 262;
    static constexpr size_t LENGTH = 31;
    static constexpr size_t MIN_LENGTH = 31;
    static constexpr uint8_t CRC_EXTRA = 69;
    static constexpr auto NAME = "CAMERA_CAPTURE_STATUS";


    uint32_t time_boot_ms; /*< Timestamp (milliseconds since system boot) */
    uint8_t camera_id; /*< Camera ID if there are multiple */
    uint8_t image_status; /*< Current status of image capturing (0: not running, 1: interval capture in progress) */
    uint8_t video_status; /*< Current status of video capturing (0: not running, 1: capture in progress) */
    float image_interval; /*< Image capture interval in seconds */
    float video_framerate; /*< Video frame rate in Hz */
    uint16_t image_resolution_h; /*< Image resolution in pixels horizontal */
    uint16_t image_resolution_v; /*< Image resolution in pixels vertical */
    uint16_t video_resolution_h; /*< Video resolution in pixels horizontal */
    uint16_t video_resolution_v; /*< Video resolution in pixels vertical */
    uint32_t recording_time_ms; /*< Time in milliseconds since recording started */
    float available_capacity; /*< Available storage capacity in MiB */


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
        ss << "  image_status: " << +image_status << std::endl;
        ss << "  video_status: " << +video_status << std::endl;
        ss << "  image_interval: " << image_interval << std::endl;
        ss << "  video_framerate: " << video_framerate << std::endl;
        ss << "  image_resolution_h: " << image_resolution_h << std::endl;
        ss << "  image_resolution_v: " << image_resolution_v << std::endl;
        ss << "  video_resolution_h: " << video_resolution_h << std::endl;
        ss << "  video_resolution_v: " << video_resolution_v << std::endl;
        ss << "  recording_time_ms: " << recording_time_ms << std::endl;
        ss << "  available_capacity: " << available_capacity << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << image_interval;                // offset: 4
        map << video_framerate;               // offset: 8
        map << recording_time_ms;             // offset: 12
        map << available_capacity;            // offset: 16
        map << image_resolution_h;            // offset: 20
        map << image_resolution_v;            // offset: 22
        map << video_resolution_h;            // offset: 24
        map << video_resolution_v;            // offset: 26
        map << camera_id;                     // offset: 28
        map << image_status;                  // offset: 29
        map << video_status;                  // offset: 30
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> image_interval;                // offset: 4
        map >> video_framerate;               // offset: 8
        map >> recording_time_ms;             // offset: 12
        map >> available_capacity;            // offset: 16
        map >> image_resolution_h;            // offset: 20
        map >> image_resolution_v;            // offset: 22
        map >> video_resolution_h;            // offset: 24
        map >> video_resolution_v;            // offset: 26
        map >> camera_id;                     // offset: 28
        map >> image_status;                  // offset: 29
        map >> video_status;                  // offset: 30
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
