// MESSAGE STATUSTEXT_LONG support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief STATUSTEXT_LONG message
 *
 * Status text message (use only for important status and error messages). The full message payload can be used for status text, but we recommend that updates be kept concise. Note: The message is intended as a less restrictive replacement for STATUSTEXT.
 */
struct STATUSTEXT_LONG : mavlink::Message {
    static constexpr msgid_t MSG_ID = 365;
    static constexpr size_t LENGTH = 255;
    static constexpr size_t MIN_LENGTH = 255;
    static constexpr uint8_t CRC_EXTRA = 36;
    static constexpr auto NAME = "STATUSTEXT_LONG";


    uint8_t severity; /*<  Severity of status. Relies on the definitions within RFC-5424. */
    std::array<char, 254> text; /*<  Status text message, without null termination character. */


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
        ss << "  severity: " << +severity << std::endl;
        ss << "  text: \"" << to_string(text) << "\"" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << severity;                      // offset: 0
        map << text;                          // offset: 1
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> severity;                      // offset: 0
        map >> text;                          // offset: 1
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
