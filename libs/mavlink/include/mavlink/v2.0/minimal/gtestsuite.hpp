/** @file
 *	@brief MAVLink comm testsuite protocol generated from minimal.xml
 *	@see http://mavlink.org
 */

#pragma once

#include <gtest/gtest.h>
#include "minimal.hpp"

#ifdef TEST_INTEROP
using namespace mavlink;
#undef MAVLINK_HELPER
#include "mavlink.h"
#endif


TEST(minimal, HEARTBEAT)
{
    mavlink::mavlink_message_t msg;
    mavlink::MsgMap map1(msg);
    mavlink::MsgMap map2(msg);

    mavlink::minimal::msg::HEARTBEAT packet_in{};
    packet_in.type = 17;
    packet_in.autopilot = 84;
    packet_in.base_mode = 151;
    packet_in.custom_mode = 963497464;
    packet_in.system_status = 218;
    packet_in.mavlink_version = 3;

    mavlink::minimal::msg::HEARTBEAT packet1{};
    mavlink::minimal::msg::HEARTBEAT packet2{};

    packet1 = packet_in;

    //std::cout << packet1.to_yaml() << std::endl;

    packet1.serialize(map1);

    mavlink::mavlink_finalize_message(&msg, 1, 1, packet1.MIN_LENGTH, packet1.LENGTH, packet1.CRC_EXTRA);

    packet2.deserialize(map2);

    EXPECT_EQ(packet1.type, packet2.type);
    EXPECT_EQ(packet1.autopilot, packet2.autopilot);
    EXPECT_EQ(packet1.base_mode, packet2.base_mode);
    EXPECT_EQ(packet1.custom_mode, packet2.custom_mode);
    EXPECT_EQ(packet1.system_status, packet2.system_status);
    EXPECT_EQ(packet1.mavlink_version, packet2.mavlink_version);
}

#ifdef TEST_INTEROP
TEST(minimal_interop, HEARTBEAT)
{
    mavlink_message_t msg;

    // to get nice print
    memset(&msg, 0, sizeof(msg));

    mavlink_heartbeat_t packet_c {
         963497464, 17, 84, 151, 218, 3
    };

    mavlink::minimal::msg::HEARTBEAT packet_in{};
    packet_in.type = 17;
    packet_in.autopilot = 84;
    packet_in.base_mode = 151;
    packet_in.custom_mode = 963497464;
    packet_in.system_status = 218;
    packet_in.mavlink_version = 3;

    mavlink::minimal::msg::HEARTBEAT packet2{};

    mavlink_msg_heartbeat_encode(1, 1, &msg, &packet_c);

    // simulate message-handling callback
    [&packet2](const mavlink_message_t *cmsg) {
        MsgMap map2(cmsg);

        packet2.deserialize(map2);
    } (&msg);

    EXPECT_EQ(packet_in.type, packet2.type);
    EXPECT_EQ(packet_in.autopilot, packet2.autopilot);
    EXPECT_EQ(packet_in.base_mode, packet2.base_mode);
    EXPECT_EQ(packet_in.custom_mode, packet2.custom_mode);
    EXPECT_EQ(packet_in.system_status, packet2.system_status);
    EXPECT_EQ(packet_in.mavlink_version, packet2.mavlink_version);

#ifdef PRINT_MSG
    PRINT_MSG(msg);
#endif
}
#endif
