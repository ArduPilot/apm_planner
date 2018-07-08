// MESSAGE LANDING_TARGET support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief LANDING_TARGET message
 *
 * The location of a landing area captured from a downward facing camera
 */
struct LANDING_TARGET : mavlink::Message {
    static constexpr msgid_t MSG_ID = 149;
    static constexpr size_t LENGTH = 60;
    static constexpr size_t MIN_LENGTH = 30;
    static constexpr uint8_t CRC_EXTRA = 200;
    static constexpr auto NAME = "LANDING_TARGET";


    uint64_t time_usec; /*< Timestamp (micros since boot or Unix epoch) */
    uint8_t target_num; /*< The ID of the target if multiple targets are present */
    uint8_t frame; /*< MAV_FRAME enum specifying the whether the following feilds are earth-frame, body-frame, etc. */
    float angle_x; /*< X-axis angular offset (in radians) of the target from the center of the image */
    float angle_y; /*< Y-axis angular offset (in radians) of the target from the center of the image */
    float distance; /*< Distance to the target from the vehicle in meters */
    float size_x; /*< Size in radians of target along x-axis */
    float size_y; /*< Size in radians of target along y-axis */
    float x; /*< X Position of the landing target on MAV_FRAME */
    float y; /*< Y Position of the landing target on MAV_FRAME */
    float z; /*< Z Position of the landing target on MAV_FRAME */
    std::array<float, 4> q; /*< Quaternion of landing target orientation (w, x, y, z order, zero-rotation is 1, 0, 0, 0) */
    uint8_t type; /*< LANDING_TARGET_TYPE enum specifying the type of landing target */
    uint8_t position_valid; /*< Boolean indicating known position (1) or default unkown position (0), for validation of positioning of the landing target */


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
        ss << "  target_num: " << +target_num << std::endl;
        ss << "  frame: " << +frame << std::endl;
        ss << "  angle_x: " << angle_x << std::endl;
        ss << "  angle_y: " << angle_y << std::endl;
        ss << "  distance: " << distance << std::endl;
        ss << "  size_x: " << size_x << std::endl;
        ss << "  size_y: " << size_y << std::endl;
        ss << "  x: " << x << std::endl;
        ss << "  y: " << y << std::endl;
        ss << "  z: " << z << std::endl;
        ss << "  q: [" << to_string(q) << "]" << std::endl;
        ss << "  type: " << +type << std::endl;
        ss << "  position_valid: " << +position_valid << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << angle_x;                       // offset: 8
        map << angle_y;                       // offset: 12
        map << distance;                      // offset: 16
        map << size_x;                        // offset: 20
        map << size_y;                        // offset: 24
        map << target_num;                    // offset: 28
        map << frame;                         // offset: 29
        map << x;                             // offset: 30
        map << y;                             // offset: 34
        map << z;                             // offset: 38
        map << q;                             // offset: 42
        map << type;                          // offset: 58
        map << position_valid;                // offset: 59
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> angle_x;                       // offset: 8
        map >> angle_y;                       // offset: 12
        map >> distance;                      // offset: 16
        map >> size_x;                        // offset: 20
        map >> size_y;                        // offset: 24
        map >> target_num;                    // offset: 28
        map >> frame;                         // offset: 29
        map >> x;                             // offset: 30
        map >> y;                             // offset: 34
        map >> z;                             // offset: 38
        map >> q;                             // offset: 42
        map >> type;                          // offset: 58
        map >> position_valid;                // offset: 59
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
