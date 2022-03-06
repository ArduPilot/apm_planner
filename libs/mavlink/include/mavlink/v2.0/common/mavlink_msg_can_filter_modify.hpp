// MESSAGE CAN_FILTER_MODIFY support class

#pragma once

namespace mavlink {
namespace common {
namespace msg {

/**
 * @brief CAN_FILTER_MODIFY message
 *
 * Modify the filter of what CAN messages to forward over the mavlink. This can be used to make CAN forwarding work well on low bandwith links. The filtering is applied on bits 8 to 24 of the CAN id (2nd and 3rd bytes) which corresponds to the DroneCAN message ID for DroneCAN. Filters with more than 16 IDs can be constructed by sending multiple CAN_FILTER_MODIFY messages.
 */
struct CAN_FILTER_MODIFY : mavlink::Message {
    static constexpr msgid_t MSG_ID = 388;
    static constexpr size_t LENGTH = 37;
    static constexpr size_t MIN_LENGTH = 37;
    static constexpr uint8_t CRC_EXTRA = 8;
    static constexpr auto NAME = "CAN_FILTER_MODIFY";


    uint8_t target_system; /*<  System ID. */
    uint8_t target_component; /*<  Component ID. */
    uint8_t bus; /*<  bus number */
    uint8_t operation; /*<  what operation to perform on the filter list. See CAN_FILTER_OP enum. */
    uint8_t num_ids; /*<  number of IDs in filter list */
    std::array<uint16_t, 16> ids; /*<  filter IDs, length num_ids */


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
        ss << "  target_system: " << +target_system << std::endl;
        ss << "  target_component: " << +target_component << std::endl;
        ss << "  bus: " << +bus << std::endl;
        ss << "  operation: " << +operation << std::endl;
        ss << "  num_ids: " << +num_ids << std::endl;
        ss << "  ids: [" << to_string(ids) << "]" << std::endl;

        return ss.str();
    }

    inline void serialize(mavlink::MsgMap &map) const override
    {
        map.reset(MSG_ID, LENGTH);

        map << ids;                           // offset: 0
        map << target_system;                 // offset: 32
        map << target_component;              // offset: 33
        map << bus;                           // offset: 34
        map << operation;                     // offset: 35
        map << num_ids;                       // offset: 36
    }

    inline void deserialize(mavlink::MsgMap &map) override
    {
        map >> ids;                           // offset: 0
        map >> target_system;                 // offset: 32
        map >> target_component;              // offset: 33
        map >> bus;                           // offset: 34
        map >> operation;                     // offset: 35
        map >> num_ids;                       // offset: 36
    }
};

} // namespace msg
} // namespace common
} // namespace mavlink
