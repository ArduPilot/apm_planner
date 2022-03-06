/** @file
 *	@brief MAVLink comm protocol generated from uAvionix.xml
 *	@see http://mavlink.org
 */

#pragma once

#include <array>
#include <cstdint>
#include <sstream>

#ifndef MAVLINK_STX
#define MAVLINK_STX 253
#endif

#include "../message.hpp"

namespace mavlink {
namespace uAvionix {

/**
 * Array of msg_entry needed for @p mavlink_parse_char() (through @p mavlink_get_msg_entry())
 */
constexpr std::array<mavlink_msg_entry_t, 190> MESSAGE_ENTRIES {{ {0, 50, 9, 9, 0, 0, 0}, {1, 124, 31, 31, 0, 0, 0}, {2, 137, 12, 12, 0, 0, 0}, {4, 237, 14, 14, 3, 12, 13}, {5, 217, 28, 28, 1, 0, 0}, {6, 104, 3, 3, 0, 0, 0}, {7, 119, 32, 32, 0, 0, 0}, {11, 89, 6, 6, 1, 4, 0}, {20, 214, 20, 20, 3, 2, 3}, {21, 159, 2, 2, 3, 0, 1}, {22, 220, 25, 25, 0, 0, 0}, {23, 168, 23, 23, 3, 4, 5}, {24, 24, 30, 52, 0, 0, 0}, {25, 23, 101, 101, 0, 0, 0}, {26, 170, 22, 24, 0, 0, 0}, {27, 144, 26, 29, 0, 0, 0}, {28, 67, 16, 16, 0, 0, 0}, {29, 115, 14, 16, 0, 0, 0}, {30, 39, 28, 28, 0, 0, 0}, {31, 246, 32, 48, 0, 0, 0}, {32, 185, 28, 28, 0, 0, 0}, {33, 104, 28, 28, 0, 0, 0}, {34, 237, 22, 22, 0, 0, 0}, {35, 244, 22, 22, 0, 0, 0}, {36, 222, 21, 37, 0, 0, 0}, {37, 212, 6, 7, 3, 4, 5}, {38, 9, 6, 7, 3, 4, 5}, {39, 254, 37, 38, 3, 32, 33}, {40, 230, 4, 5, 3, 2, 3}, {41, 28, 4, 4, 3, 2, 3}, {42, 28, 2, 2, 0, 0, 0}, {43, 132, 2, 3, 3, 0, 1}, {44, 221, 4, 5, 3, 2, 3}, {45, 232, 2, 3, 3, 0, 1}, {46, 11, 2, 2, 0, 0, 0}, {47, 153, 3, 4, 3, 0, 1}, {48, 41, 13, 21, 1, 12, 0}, {49, 39, 12, 20, 0, 0, 0}, {50, 78, 37, 37, 3, 18, 19}, {51, 196, 4, 5, 3, 2, 3}, {54, 15, 27, 27, 3, 24, 25}, {55, 3, 25, 25, 0, 0, 0}, {61, 167, 72, 72, 0, 0, 0}, {62, 183, 26, 26, 0, 0, 0}, {63, 119, 181, 181, 0, 0, 0}, {64, 191, 225, 225, 0, 0, 0}, {65, 118, 42, 42, 0, 0, 0}, {66, 148, 6, 6, 3, 2, 3}, {67, 21, 4, 4, 0, 0, 0}, {69, 243, 11, 11, 1, 10, 0}, {70, 124, 18, 38, 3, 16, 17}, {73, 38, 37, 38, 3, 32, 33}, {74, 20, 20, 20, 0, 0, 0}, {75, 158, 35, 35, 3, 30, 31}, {76, 152, 33, 33, 3, 30, 31}, {77, 143, 3, 10, 3, 8, 9}, {81, 106, 22, 22, 0, 0, 0}, {82, 49, 39, 39, 3, 36, 37}, {83, 22, 37, 37, 0, 0, 0}, {84, 143, 53, 53, 3, 50, 51}, {85, 140, 51, 51, 0, 0, 0}, {86, 5, 53, 53, 3, 50, 51}, {87, 150, 51, 51, 0, 0, 0}, {89, 231, 28, 28, 0, 0, 0}, {90, 183, 56, 56, 0, 0, 0}, {91, 63, 42, 42, 0, 0, 0}, {92, 54, 33, 33, 0, 0, 0}, {93, 47, 81, 81, 0, 0, 0}, {100, 175, 26, 34, 0, 0, 0}, {101, 102, 32, 117, 0, 0, 0}, {102, 158, 32, 117, 0, 0, 0}, {103, 208, 20, 57, 0, 0, 0}, {104, 56, 32, 116, 0, 0, 0}, {105, 93, 62, 63, 0, 0, 0}, {106, 138, 44, 44, 0, 0, 0}, {107, 108, 64, 65, 0, 0, 0}, {108, 32, 84, 84, 0, 0, 0}, {109, 185, 9, 9, 0, 0, 0}, {110, 84, 254, 254, 3, 1, 2}, {111, 34, 16, 16, 0, 0, 0}, {112, 174, 12, 12, 0, 0, 0}, {113, 124, 36, 39, 0, 0, 0}, {114, 237, 44, 44, 0, 0, 0}, {115, 4, 64, 64, 0, 0, 0}, {116, 76, 22, 24, 0, 0, 0}, {117, 128, 6, 6, 3, 4, 5}, {118, 56, 14, 14, 0, 0, 0}, {119, 116, 12, 12, 3, 10, 11}, {120, 134, 97, 97, 0, 0, 0}, {121, 237, 2, 2, 3, 0, 1}, {122, 203, 2, 2, 3, 0, 1}, {123, 250, 113, 113, 3, 0, 1}, {124, 87, 35, 57, 0, 0, 0}, {125, 203, 6, 6, 0, 0, 0}, {126, 220, 79, 79, 0, 0, 0}, {127, 25, 35, 35, 0, 0, 0}, {128, 226, 35, 35, 0, 0, 0}, {129, 46, 22, 24, 0, 0, 0}, {130, 29, 13, 13, 0, 0, 0}, {131, 223, 255, 255, 0, 0, 0}, {132, 85, 14, 39, 0, 0, 0}, {133, 6, 18, 18, 0, 0, 0}, {134, 229, 43, 43, 0, 0, 0}, {135, 203, 8, 8, 0, 0, 0}, {136, 1, 22, 22, 0, 0, 0}, {137, 195, 14, 16, 0, 0, 0}, {138, 109, 36, 120, 0, 0, 0}, {139, 168, 43, 43, 3, 41, 42}, {140, 181, 41, 41, 0, 0, 0}, {141, 47, 32, 32, 0, 0, 0}, {142, 72, 243, 243, 0, 0, 0}, {143, 131, 14, 16, 0, 0, 0}, {144, 127, 93, 93, 0, 0, 0}, {146, 103, 100, 100, 0, 0, 0}, {147, 154, 36, 54, 0, 0, 0}, {148, 178, 60, 78, 0, 0, 0}, {149, 200, 30, 60, 0, 0, 0}, {162, 189, 8, 9, 0, 0, 0}, {192, 36, 44, 54, 0, 0, 0}, {225, 208, 65, 65, 0, 0, 0}, {230, 163, 42, 42, 0, 0, 0}, {231, 105, 40, 40, 0, 0, 0}, {232, 151, 63, 65, 0, 0, 0}, {233, 35, 182, 182, 0, 0, 0}, {234, 150, 40, 40, 0, 0, 0}, {235, 179, 42, 42, 0, 0, 0}, {241, 90, 32, 32, 0, 0, 0}, {242, 104, 52, 60, 0, 0, 0}, {243, 85, 53, 61, 1, 52, 0}, {244, 95, 6, 6, 0, 0, 0}, {245, 130, 2, 2, 0, 0, 0}, {246, 184, 38, 38, 0, 0, 0}, {247, 81, 19, 19, 0, 0, 0}, {248, 8, 254, 254, 3, 3, 4}, {249, 204, 36, 36, 0, 0, 0}, {250, 49, 30, 30, 0, 0, 0}, {251, 170, 18, 18, 0, 0, 0}, {252, 44, 18, 18, 0, 0, 0}, {253, 83, 51, 54, 0, 0, 0}, {254, 46, 9, 9, 0, 0, 0}, {256, 71, 42, 42, 3, 8, 9}, {257, 131, 9, 9, 0, 0, 0}, {258, 187, 32, 232, 3, 0, 1}, {259, 92, 235, 235, 0, 0, 0}, {260, 146, 5, 13, 0, 0, 0}, {261, 179, 27, 60, 0, 0, 0}, {262, 12, 18, 22, 0, 0, 0}, {263, 133, 255, 255, 0, 0, 0}, {264, 49, 28, 28, 0, 0, 0}, {265, 26, 16, 20, 0, 0, 0}, {266, 193, 255, 255, 3, 2, 3}, {267, 35, 255, 255, 3, 2, 3}, {268, 14, 4, 4, 3, 2, 3}, {269, 109, 213, 213, 0, 0, 0}, {270, 59, 19, 19, 0, 0, 0}, {283, 74, 144, 144, 0, 0, 0}, {286, 210, 53, 53, 3, 50, 51}, {299, 19, 96, 96, 0, 0, 0}, {301, 243, 58, 58, 0, 0, 0}, {310, 28, 17, 17, 0, 0, 0}, {311, 95, 116, 116, 0, 0, 0}, {320, 243, 20, 20, 3, 2, 3}, {321, 88, 2, 2, 3, 0, 1}, {322, 243, 149, 149, 0, 0, 0}, {323, 78, 147, 147, 3, 0, 1}, {324, 132, 146, 146, 0, 0, 0}, {330, 23, 158, 167, 0, 0, 0}, {331, 91, 230, 232, 0, 0, 0}, {335, 225, 24, 24, 0, 0, 0}, {339, 199, 5, 5, 0, 0, 0}, {340, 99, 70, 70, 0, 0, 0}, {350, 232, 20, 252, 0, 0, 0}, {370, 75, 87, 109, 0, 0, 0}, {373, 117, 42, 42, 0, 0, 0}, {375, 251, 140, 140, 0, 0, 0}, {385, 147, 133, 133, 3, 2, 3}, {386, 132, 16, 16, 3, 4, 5}, {387, 4, 72, 72, 3, 4, 5}, {388, 8, 37, 37, 3, 32, 33}, {9000, 113, 137, 137, 0, 0, 0}, {9005, 117, 34, 34, 0, 0, 0}, {10001, 209, 20, 20, 0, 0, 0}, {10002, 186, 41, 41, 0, 0, 0}, {10003, 4, 1, 1, 0, 0, 0}, {10004, 133, 9, 9, 0, 0, 0}, {10005, 103, 9, 9, 0, 0, 0}, {10006, 193, 4, 4, 0, 0, 0}, {10007, 71, 17, 17, 0, 0, 0}, {10008, 240, 14, 14, 0, 0, 0}, {12920, 20, 5, 5, 0, 0, 0} }};

//! MAVLINK VERSION
constexpr auto MAVLINK_VERSION = 2;


// ENUM DEFINITIONS


/** @brief State flags for ADS-B transponder dynamic report */
enum class UAVIONIX_ADSB_OUT_DYNAMIC_STATE : uint16_t
{
    INTENT_CHANGE=1, /*  | */
    AUTOPILOT_ENABLED=2, /*  | */
    NICBARO_CROSSCHECKED=4, /*  | */
    ON_GROUND=8, /*  | */
    IDENT=16, /*  | */
};

//! UAVIONIX_ADSB_OUT_DYNAMIC_STATE ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_DYNAMIC_STATE_ENUM_END = 17;

/** @brief Transceiver RF control flags for ADS-B transponder dynamic reports */
enum class UAVIONIX_ADSB_OUT_RF_SELECT : uint8_t
{
    STANDBY=0, /*  | */
    RX_ENABLED=1, /*  | */
    TX_ENABLED=2, /*  | */
};

//! UAVIONIX_ADSB_OUT_RF_SELECT ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_RF_SELECT_ENUM_END = 3;

/** @brief Status for ADS-B transponder dynamic input */
enum class UAVIONIX_ADSB_OUT_DYNAMIC_GPS_FIX : uint8_t
{
    NONE_0=0, /*  | */
    NONE_1=1, /*  | */
    FIX_2D=2, /*  | */
    FIX_3D=3, /*  | */
    DGPS=4, /*  | */
    RTK=5, /*  | */
};

//! UAVIONIX_ADSB_OUT_DYNAMIC_GPS_FIX ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_DYNAMIC_GPS_FIX_ENUM_END = 6;

/** @brief Status flags for ADS-B transponder dynamic output */
enum class UAVIONIX_ADSB_RF_HEALTH : uint8_t
{
    INITIALIZING=0, /*  | */
    OK=1, /*  | */
    FAIL_TX=2, /*  | */
    FAIL_RX=16, /*  | */
};

//! UAVIONIX_ADSB_RF_HEALTH ENUM_END
constexpr auto UAVIONIX_ADSB_RF_HEALTH_ENUM_END = 17;

/** @brief Definitions for aircraft size */
enum class UAVIONIX_ADSB_OUT_CFG_AIRCRAFT_SIZE : uint8_t
{
    NO_DATA_=0, /*  | */
    L15M_W23M=1, /*  | */
    L25M_W28P5M=2, /*  | */
    L25_34M=3, /*  | */
    L35_33M=4, /*  | */
    L35_38M=5, /*  | */
    L45_39P5M=6, /*  | */
    L45_45M=7, /*  | */
    L55_45M=8, /*  | */
    L55_52M=9, /*  | */
    L65_59P5M=10, /*  | */
    L65_67M=11, /*  | */
    L75_W72P5M=12, /*  | */
    L75_W80M=13, /*  | */
    L85_W80M=14, /*  | */
    L85_W90M=15, /*  | */
};

//! UAVIONIX_ADSB_OUT_CFG_AIRCRAFT_SIZE ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_CFG_AIRCRAFT_SIZE_ENUM_END = 16;

/** @brief GPS lataral offset encoding */
enum class UAVIONIX_ADSB_OUT_CFG_GPS_OFFSET_LAT : uint8_t
{
    NO_DATA_=0, /*  | */
    LEFT_2M=1, /*  | */
    LEFT_4M=2, /*  | */
    LEFT_6M=3, /*  | */
    RIGHT_0M=4, /*  | */
    RIGHT_2M=5, /*  | */
    RIGHT_4M=6, /*  | */
    RIGHT_6M=7, /*  | */
};

//! UAVIONIX_ADSB_OUT_CFG_GPS_OFFSET_LAT ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_CFG_GPS_OFFSET_LAT_ENUM_END = 8;

/** @brief GPS longitudinal offset encoding */
enum class UAVIONIX_ADSB_OUT_CFG_GPS_OFFSET_LON : uint8_t
{
    NO_DATA_=0, /*  | */
    APPLIED_BY_SENSOR=1, /*  | */
};

//! UAVIONIX_ADSB_OUT_CFG_GPS_OFFSET_LON ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_CFG_GPS_OFFSET_LON_ENUM_END = 2;

/** @brief Emergency status encoding */
enum class UAVIONIX_ADSB_EMERGENCY_STATUS : uint8_t
{
    OUT_NO_EMERGENCY=0, /*  | */
    OUT_GENERAL_EMERGENCY=1, /*  | */
    OUT_LIFEGUARD_EMERGENCY=2, /*  | */
    OUT_MINIMUM_FUEL_EMERGENCY=3, /*  | */
    OUT_NO_COMM_EMERGENCY=4, /*  | */
    OUT_UNLAWFUL_INTERFERANCE_EMERGENCY=5, /*  | */
    OUT_DOWNED_AIRCRAFT_EMERGENCY=6, /*  | */
    OUT_RESERVED=7, /*  | */
};

//! UAVIONIX_ADSB_EMERGENCY_STATUS ENUM_END
constexpr auto UAVIONIX_ADSB_EMERGENCY_STATUS_ENUM_END = 8;

/** @brief State flags for ADS-B transponder dynamic report */
enum class UAVIONIX_ADSB_OUT_CONTROL_STATE : uint8_t
{
    EXTERNAL_BARO_CROSSCHECKED=1, /*  | */
    ON_GROUND=4, /*  | */
    IDENT_BUTTON_ACTIVE=8, /*  | */
    MODE_A_ENABLED=16, /*  | */
    MODE_C_ENABLED=32, /*  | */
    MODE_S_ENABLED=64, /*  | */
    STATE_1090ES_TX_ENABLED=128, /*  | */
};

//! UAVIONIX_ADSB_OUT_CONTROL_STATE ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_CONTROL_STATE_ENUM_END = 129;

/** @brief State flags for X-Bit and reserved fields. */
enum class UAVIONIX_ADSB_XBIT : uint8_t
{
    ENABLED=128, /*  | */
};

//! UAVIONIX_ADSB_XBIT ENUM_END
constexpr auto UAVIONIX_ADSB_XBIT_ENUM_END = 129;

/** @brief State flags for ADS-B transponder status report */
enum class UAVIONIX_ADSB_OUT_STATUS_STATE : uint8_t
{
    ON_GROUND=1, /*  | */
    INTERROGATED_SINCE_LAST=2, /*  | */
    XBIT_ENABLED=4, /*  | */
    IDENT_ACTIVE=8, /*  | */
    MODE_A_ENABLED=16, /*  | */
    MODE_C_ENABLED=32, /*  | */
    MODE_S_ENABLED=64, /*  | */
    STATE_1090ES_TX_ENABLED=128, /*  | */
};

//! UAVIONIX_ADSB_OUT_STATUS_STATE ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_STATUS_STATE_ENUM_END = 129;

/** @brief State flags for ADS-B transponder status report */
enum class UAVIONIX_ADSB_OUT_STATUS_NIC_NACP : uint8_t
{
    NIC_CR_20_NM=1, /*  | */
    NIC_CR_8_NM=2, /*  | */
    NIC_CR_4_NM=3, /*  | */
    NIC_CR_2_NM=4, /*  | */
    NIC_CR_1_NM=5, /*  | */
    NIC_CR_0_3_NM=6, /*  | */
    NIC_CR_0_2_NM=7, /*  | */
    NIC_CR_0_1_NM=8, /*  | */
    NIC_CR_75_M=9, /*  | */
    NIC_CR_25_M=10, /*  | */
    NIC_CR_7_5_M=11, /*  | */
    NACP_EPU_10_NM=16, /*  | */
    NACP_EPU_4_NM=32, /*  | */
    NACP_EPU_2_NM=48, /*  | */
    NACP_EPU_1_NM=64, /*  | */
    NACP_EPU_0_5_NM=80, /*  | */
    NACP_EPU_0_3_NM=96, /*  | */
    NACP_EPU_0_1_NM=112, /*  | */
    NACP_EPU_0_05_NM=128, /*  | */
    NACP_EPU_30_M=144, /*  | */
    NACP_EPU_10_M=160, /*  | */
    NACP_EPU_3_M=176, /*  | */
};

//! UAVIONIX_ADSB_OUT_STATUS_NIC_NACP ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_STATUS_NIC_NACP_ENUM_END = 177;

/** @brief State flags for ADS-B transponder fault report */
enum class UAVIONIX_ADSB_OUT_STATUS_FAULT : uint8_t
{
    STATUS_MESSAGE_UNAVAIL=8, /*  | */
    GPS_NO_POS=16, /*  | */
    GPS_UNAVAIL=32, /*  | */
    TX_SYSTEM_FAIL=64, /*  | */
    MAINT_REQ=128, /*  | */
};

//! UAVIONIX_ADSB_OUT_STATUS_FAULT ENUM_END
constexpr auto UAVIONIX_ADSB_OUT_STATUS_FAULT_ENUM_END = 129;


} // namespace uAvionix
} // namespace mavlink

// MESSAGE DEFINITIONS
#include "./mavlink_msg_uavionix_adsb_out_cfg.hpp"
#include "./mavlink_msg_uavionix_adsb_out_dynamic.hpp"
#include "./mavlink_msg_uavionix_adsb_transceiver_health_report.hpp"
#include "./mavlink_msg_uavionix_adsb_out_cfg_registration.hpp"
#include "./mavlink_msg_uavionix_adsb_out_cfg_flightid.hpp"
#include "./mavlink_msg_uavionix_adsb_get.hpp"
#include "./mavlink_msg_uavionix_adsb_out_control.hpp"
#include "./mavlink_msg_uavionix_adsb_out_status.hpp"

// base include
#include "../common/common.hpp"
