// MESSAGE OBSTACLE_DISTANCE_3D support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief OBSTACLE_DISTANCE_3D message
 *
 * Obstacle located as a 3D vector.
 */
struct OBSTACLE_DISTANCE_3D : mavlink::Message {
    static constexpr msgid_t MSG_ID = 11037;
    static constexpr size_t LENGTH = 28;
    static constexpr size_t MIN_LENGTH = 28;
    static constexpr uint8_t CRC_EXTRA = 130;
    static constexpr auto NAME = "OBSTACLE_DISTANCE_3D";


    uint32_t time_boot_ms; /*< [ms] Timestamp (time since system boot). */
    uint8_t sensor_type; /*<  Class id of the distance sensor type. */
    uint8_t frame; /*<  Coordinate frame of reference. */
    uint16_t obstacle_id; /*<   Unique ID given to each obstacle so that its movement can be tracked. Use UINT16_MAX if object ID is unknown or cannot be determined. */
    float x; /*< [m]  X position of the obstacle. */
    float y; /*< [m]  Y position of the obstacle. */
    float z; /*< [m]  Z position of the obstacle. */
    float min_distance; /*< [m] Minimum distance the sensor can measure. */
    float max_distance; /*< [m] Maximum distance the sensor can measure. */


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
        ss << "  sensor_type: " << +sensor_type << std::endl;
        ss << "  frame: " << +frame << std::endl;
        ss << "  obstacle_id: " << obstacle_id << std::endl;
        ss << "  x: " << x << std::endl;
        ss << "  y: " << y << std::endl;
        ss << "  z: " << z << std::endl;
        ss << "  min_distance: " << min_distance << std::endl;
        ss << "  max_distance: " << max_distance << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_boot_ms;                  // offset: 0
        map << x;                             // offset: 4
        map << y;                             // offset: 8
        map << z;                             // offset: 12
        map << min_distance;                  // offset: 16
        map << max_distance;                  // offset: 20
        map << obstacle_id;                   // offset: 24
        map << sensor_type;                   // offset: 26
        map << frame;                         // offset: 27
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_boot_ms;                  // offset: 0
        map >> x;                             // offset: 4
        map >> y;                             // offset: 8
        map >> z;                             // offset: 12
        map >> min_distance;                  // offset: 16
        map >> max_distance;                  // offset: 20
        map >> obstacle_id;                   // offset: 24
        map >> sensor_type;                   // offset: 26
        map >> frame;                         // offset: 27
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
