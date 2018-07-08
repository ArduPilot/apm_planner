// MESSAGE SIMSTATE support class

#pragma once

namespace mavlink {
namespace ardupilotmega {
namespace msg {

/**
 * @brief SIMSTATE message
 *
 * Status of simulation environment, if used
 */
struct SIMSTATE : mavlink::Message {
    static constexpr msgid_t MSG_ID = 164;
    static constexpr size_t LENGTH = 44;
    static constexpr size_t MIN_LENGTH = 44;
    static constexpr uint8_t CRC_EXTRA = 154;
    static constexpr auto NAME = "SIMSTATE";


    float roll; /*< Roll angle (rad) */
    float pitch; /*< Pitch angle (rad) */
    float yaw; /*< Yaw angle (rad) */
    float xacc; /*< X acceleration m/s/s */
    float yacc; /*< Y acceleration m/s/s */
    float zacc; /*< Z acceleration m/s/s */
    float xgyro; /*< Angular speed around X axis rad/s */
    float ygyro; /*< Angular speed around Y axis rad/s */
    float zgyro; /*< Angular speed around Z axis rad/s */
    int32_t lat; /*< Latitude in degrees * 1E7 */
    int32_t lng; /*< Longitude in degrees * 1E7 */


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
        ss << "  roll: " << roll << std::endl;
        ss << "  pitch: " << pitch << std::endl;
        ss << "  yaw: " << yaw << std::endl;
        ss << "  xacc: " << xacc << std::endl;
        ss << "  yacc: " << yacc << std::endl;
        ss << "  zacc: " << zacc << std::endl;
        ss << "  xgyro: " << xgyro << std::endl;
        ss << "  ygyro: " << ygyro << std::endl;
        ss << "  zgyro: " << zgyro << std::endl;
        ss << "  lat: " << lat << std::endl;
        ss << "  lng: " << lng << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << roll;                          // offset: 0
        map << pitch;                         // offset: 4
        map << yaw;                           // offset: 8
        map << xacc;                          // offset: 12
        map << yacc;                          // offset: 16
        map << zacc;                          // offset: 20
        map << xgyro;                         // offset: 24
        map << ygyro;                         // offset: 28
        map << zgyro;                         // offset: 32
        map << lat;                           // offset: 36
        map << lng;                           // offset: 40
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> roll;                          // offset: 0
        map >> pitch;                         // offset: 4
        map >> yaw;                           // offset: 8
        map >> xacc;                          // offset: 12
        map >> yacc;                          // offset: 16
        map >> zacc;                          // offset: 20
        map >> xgyro;                         // offset: 24
        map >> ygyro;                         // offset: 28
        map >> zgyro;                         // offset: 32
        map >> lat;                           // offset: 36
        map >> lng;                           // offset: 40
    }
};

} // namespace msg
} // namespace ardupilotmega
} // namespace mavlink
