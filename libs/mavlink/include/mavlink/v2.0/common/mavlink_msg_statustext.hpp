// MESSAGE STATUSTEXT support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief STATUSTEXT message
 *
 * Status text message. These messages are printed in yellow in the COMM console of QGroundControl. WARNING: They consume quite some bandwidth, so use only for important status and error messages. If implemented wisely, these messages are buffered on the MCU and sent only at a limited rate (e.g. 10 Hz).
 */
struct STATUSTEXT : mavlink::Message {
    static constexpr msgid_t MSG_ID = 253;
    static constexpr size_t LENGTH = 54;
    static constexpr size_t MIN_LENGTH = 51;
    static constexpr uint8_t CRC_EXTRA = 83;
    static constexpr auto NAME = "STATUSTEXT";


    uint8_t severity; /*<  Severity of status. Relies on the definitions within RFC-5424. */
    std::array<char, 50> text; /*<  Status text message, without null termination character */
    uint16_t id; /*<  Unique (opaque) identifier for this statustext message.  May be used to reassemble a logical long-statustext message from a sequence of chunks.  A value of zero indicates this is the only chunk in the sequence and the message can be emitted immediately. */
    uint8_t chunk_seq; /*<  This chunk's sequence number; indexing is from zero.  Any null character in the text field is taken to mean this was the last chunk. */


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
        ss << "  id: " << id << std::endl;
        ss << "  chunk_seq: " << +chunk_seq << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << severity;                      // offset: 0
        map << text;                          // offset: 1
        map << id;                            // offset: 51
        map << chunk_seq;                     // offset: 53
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> severity;                      // offset: 0
        map >> text;                          // offset: 1
        map >> id;                            // offset: 51
        map >> chunk_seq;                     // offset: 53
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
