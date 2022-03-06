// MESSAGE OBSTACLE_DISTANCE support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief OBSTACLE_DISTANCE message
 *
 * Obstacle distances in front of the sensor, starting from the left in increment degrees to the right
 */
struct OBSTACLE_DISTANCE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 330;
    static constexpr size_t LENGTH = 167;
    static constexpr size_t MIN_LENGTH = 158;
    static constexpr uint8_t CRC_EXTRA = 23;
    static constexpr auto NAME = "OBSTACLE_DISTANCE";


    uint64_t time_usec; /*< [us] Timestamp (UNIX Epoch time or time since system boot). The receiving end can infer timestamp format (since 1.1.1970 or since system boot) by checking for the magnitude of the number. */
    uint8_t sensor_type; /*<  Class id of the distance sensor type. */
    std::array<uint16_t, 72> distances; /*< [cm] Distance of obstacles around the vehicle with index 0 corresponding to north + angle_offset, unless otherwise specified in the frame. A value of 0 is valid and means that the obstacle is practically touching the sensor. A value of max_distance +1 means no obstacle is present. A value of UINT16_MAX for unknown/not used. In a array element, one unit corresponds to 1cm. */
    uint8_t increment; /*< [deg] Angular width in degrees of each array element. Increment direction is clockwise. This field is ignored if increment_f is non-zero. */
    uint16_t min_distance; /*< [cm] Minimum distance the sensor can measure. */
    uint16_t max_distance; /*< [cm] Maximum distance the sensor can measure. */
    float increment_f; /*< [deg] Angular width in degrees of each array element as a float. If non-zero then this value is used instead of the uint8_t increment field. Positive is clockwise direction, negative is counter-clockwise. */
    float angle_offset; /*< [deg] Relative angle offset of the 0-index element in the distances array. Value of 0 corresponds to forward. Positive is clockwise direction, negative is counter-clockwise. */
    uint8_t frame; /*<  Coordinate frame of reference for the yaw rotation and offset of the sensor data. Defaults to MAV_FRAME_GLOBAL, which is north aligned. For body-mounted sensors use MAV_FRAME_BODY_FRD, which is vehicle front aligned. */


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
        ss << "  sensor_type: " << +sensor_type << std::endl;
        ss << "  distances: [" << to_string(distances) << "]" << std::endl;
        ss << "  increment: " << +increment << std::endl;
        ss << "  min_distance: " << min_distance << std::endl;
        ss << "  max_distance: " << max_distance << std::endl;
        ss << "  increment_f: " << increment_f << std::endl;
        ss << "  angle_offset: " << angle_offset << std::endl;
        ss << "  frame: " << +frame << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << time_usec;                     // offset: 0
        map << distances;                     // offset: 8
        map << min_distance;                  // offset: 152
        map << max_distance;                  // offset: 154
        map << sensor_type;                   // offset: 156
        map << increment;                     // offset: 157
        map << increment_f;                   // offset: 158
        map << angle_offset;                  // offset: 162
        map << frame;                         // offset: 166
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> time_usec;                     // offset: 0
        map >> distances;                     // offset: 8
        map >> min_distance;                  // offset: 152
        map >> max_distance;                  // offset: 154
        map >> sensor_type;                   // offset: 156
        map >> increment;                     // offset: 157
        map >> increment_f;                   // offset: 158
        map >> angle_offset;                  // offset: 162
        map >> frame;                         // offset: 166
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
