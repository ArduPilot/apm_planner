/** @file
 *	@brief MAVLink comm protocol generated from common.xml
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
namespace common {

/**
 * Array of msg_entry needed for @p mavlink_parse_char() (through @p mavlink_get_msg_entry())
 */
constexpr std::array<mavlink_msg_entry_t, 182> MESSAGE_ENTRIES {{ {0, 50, 9, 9, 0, 0, 0}, {1, 124, 31, 31, 0, 0, 0}, {2, 137, 12, 12, 0, 0, 0}, {4, 237, 14, 14, 3, 12, 13}, {5, 217, 28, 28, 1, 0, 0}, {6, 104, 3, 3, 0, 0, 0}, {7, 119, 32, 32, 0, 0, 0}, {11, 89, 6, 6, 1, 4, 0}, {20, 214, 20, 20, 3, 2, 3}, {21, 159, 2, 2, 3, 0, 1}, {22, 220, 25, 25, 0, 0, 0}, {23, 168, 23, 23, 3, 4, 5}, {24, 24, 30, 52, 0, 0, 0}, {25, 23, 101, 101, 0, 0, 0}, {26, 170, 22, 24, 0, 0, 0}, {27, 144, 26, 29, 0, 0, 0}, {28, 67, 16, 16, 0, 0, 0}, {29, 115, 14, 16, 0, 0, 0}, {30, 39, 28, 28, 0, 0, 0}, {31, 246, 32, 48, 0, 0, 0}, {32, 185, 28, 28, 0, 0, 0}, {33, 104, 28, 28, 0, 0, 0}, {34, 237, 22, 22, 0, 0, 0}, {35, 244, 22, 22, 0, 0, 0}, {36, 222, 21, 37, 0, 0, 0}, {37, 212, 6, 7, 3, 4, 5}, {38, 9, 6, 7, 3, 4, 5}, {39, 254, 37, 38, 3, 32, 33}, {40, 230, 4, 5, 3, 2, 3}, {41, 28, 4, 4, 3, 2, 3}, {42, 28, 2, 2, 0, 0, 0}, {43, 132, 2, 3, 3, 0, 1}, {44, 221, 4, 5, 3, 2, 3}, {45, 232, 2, 3, 3, 0, 1}, {46, 11, 2, 2, 0, 0, 0}, {47, 153, 3, 4, 3, 0, 1}, {48, 41, 13, 21, 1, 12, 0}, {49, 39, 12, 20, 0, 0, 0}, {50, 78, 37, 37, 3, 18, 19}, {51, 196, 4, 5, 3, 2, 3}, {54, 15, 27, 27, 3, 24, 25}, {55, 3, 25, 25, 0, 0, 0}, {61, 167, 72, 72, 0, 0, 0}, {62, 183, 26, 26, 0, 0, 0}, {63, 119, 181, 181, 0, 0, 0}, {64, 191, 225, 225, 0, 0, 0}, {65, 118, 42, 42, 0, 0, 0}, {66, 148, 6, 6, 3, 2, 3}, {67, 21, 4, 4, 0, 0, 0}, {69, 243, 11, 11, 1, 10, 0}, {70, 124, 18, 38, 3, 16, 17}, {73, 38, 37, 38, 3, 32, 33}, {74, 20, 20, 20, 0, 0, 0}, {75, 158, 35, 35, 3, 30, 31}, {76, 152, 33, 33, 3, 30, 31}, {77, 143, 3, 10, 3, 8, 9}, {81, 106, 22, 22, 0, 0, 0}, {82, 49, 39, 39, 3, 36, 37}, {83, 22, 37, 37, 0, 0, 0}, {84, 143, 53, 53, 3, 50, 51}, {85, 140, 51, 51, 0, 0, 0}, {86, 5, 53, 53, 3, 50, 51}, {87, 150, 51, 51, 0, 0, 0}, {89, 231, 28, 28, 0, 0, 0}, {90, 183, 56, 56, 0, 0, 0}, {91, 63, 42, 42, 0, 0, 0}, {92, 54, 33, 33, 0, 0, 0}, {93, 47, 81, 81, 0, 0, 0}, {100, 175, 26, 34, 0, 0, 0}, {101, 102, 32, 117, 0, 0, 0}, {102, 158, 32, 117, 0, 0, 0}, {103, 208, 20, 57, 0, 0, 0}, {104, 56, 32, 116, 0, 0, 0}, {105, 93, 62, 63, 0, 0, 0}, {106, 138, 44, 44, 0, 0, 0}, {107, 108, 64, 65, 0, 0, 0}, {108, 32, 84, 84, 0, 0, 0}, {109, 185, 9, 9, 0, 0, 0}, {110, 84, 254, 254, 3, 1, 2}, {111, 34, 16, 16, 0, 0, 0}, {112, 174, 12, 12, 0, 0, 0}, {113, 124, 36, 39, 0, 0, 0}, {114, 237, 44, 44, 0, 0, 0}, {115, 4, 64, 64, 0, 0, 0}, {116, 76, 22, 24, 0, 0, 0}, {117, 128, 6, 6, 3, 4, 5}, {118, 56, 14, 14, 0, 0, 0}, {119, 116, 12, 12, 3, 10, 11}, {120, 134, 97, 97, 0, 0, 0}, {121, 237, 2, 2, 3, 0, 1}, {122, 203, 2, 2, 3, 0, 1}, {123, 250, 113, 113, 3, 0, 1}, {124, 87, 35, 57, 0, 0, 0}, {125, 203, 6, 6, 0, 0, 0}, {126, 220, 79, 79, 0, 0, 0}, {127, 25, 35, 35, 0, 0, 0}, {128, 226, 35, 35, 0, 0, 0}, {129, 46, 22, 24, 0, 0, 0}, {130, 29, 13, 13, 0, 0, 0}, {131, 223, 255, 255, 0, 0, 0}, {132, 85, 14, 39, 0, 0, 0}, {133, 6, 18, 18, 0, 0, 0}, {134, 229, 43, 43, 0, 0, 0}, {135, 203, 8, 8, 0, 0, 0}, {136, 1, 22, 22, 0, 0, 0}, {137, 195, 14, 16, 0, 0, 0}, {138, 109, 36, 120, 0, 0, 0}, {139, 168, 43, 43, 3, 41, 42}, {140, 181, 41, 41, 0, 0, 0}, {141, 47, 32, 32, 0, 0, 0}, {142, 72, 243, 243, 0, 0, 0}, {143, 131, 14, 16, 0, 0, 0}, {144, 127, 93, 93, 0, 0, 0}, {146, 103, 100, 100, 0, 0, 0}, {147, 154, 36, 54, 0, 0, 0}, {148, 178, 60, 78, 0, 0, 0}, {149, 200, 30, 60, 0, 0, 0}, {162, 189, 8, 9, 0, 0, 0}, {192, 36, 44, 54, 0, 0, 0}, {225, 208, 65, 65, 0, 0, 0}, {230, 163, 42, 42, 0, 0, 0}, {231, 105, 40, 40, 0, 0, 0}, {232, 151, 63, 65, 0, 0, 0}, {233, 35, 182, 182, 0, 0, 0}, {234, 150, 40, 40, 0, 0, 0}, {235, 179, 42, 42, 0, 0, 0}, {241, 90, 32, 32, 0, 0, 0}, {242, 104, 52, 60, 0, 0, 0}, {243, 85, 53, 61, 1, 52, 0}, {244, 95, 6, 6, 0, 0, 0}, {245, 130, 2, 2, 0, 0, 0}, {246, 184, 38, 38, 0, 0, 0}, {247, 81, 19, 19, 0, 0, 0}, {248, 8, 254, 254, 3, 3, 4}, {249, 204, 36, 36, 0, 0, 0}, {250, 49, 30, 30, 0, 0, 0}, {251, 170, 18, 18, 0, 0, 0}, {252, 44, 18, 18, 0, 0, 0}, {253, 83, 51, 54, 0, 0, 0}, {254, 46, 9, 9, 0, 0, 0}, {256, 71, 42, 42, 3, 8, 9}, {257, 131, 9, 9, 0, 0, 0}, {258, 187, 32, 232, 3, 0, 1}, {259, 92, 235, 235, 0, 0, 0}, {260, 146, 5, 13, 0, 0, 0}, {261, 179, 27, 60, 0, 0, 0}, {262, 12, 18, 22, 0, 0, 0}, {263, 133, 255, 255, 0, 0, 0}, {264, 49, 28, 28, 0, 0, 0}, {265, 26, 16, 20, 0, 0, 0}, {266, 193, 255, 255, 3, 2, 3}, {267, 35, 255, 255, 3, 2, 3}, {268, 14, 4, 4, 3, 2, 3}, {269, 109, 213, 213, 0, 0, 0}, {270, 59, 19, 19, 0, 0, 0}, {283, 74, 144, 144, 0, 0, 0}, {286, 210, 53, 53, 3, 50, 51}, {299, 19, 96, 96, 0, 0, 0}, {301, 243, 58, 58, 0, 0, 0}, {310, 28, 17, 17, 0, 0, 0}, {311, 95, 116, 116, 0, 0, 0}, {320, 243, 20, 20, 3, 2, 3}, {321, 88, 2, 2, 3, 0, 1}, {322, 243, 149, 149, 0, 0, 0}, {323, 78, 147, 147, 3, 0, 1}, {324, 132, 146, 146, 0, 0, 0}, {330, 23, 158, 167, 0, 0, 0}, {331, 91, 230, 232, 0, 0, 0}, {335, 225, 24, 24, 0, 0, 0}, {339, 199, 5, 5, 0, 0, 0}, {340, 99, 70, 70, 0, 0, 0}, {350, 232, 20, 252, 0, 0, 0}, {370, 75, 87, 109, 0, 0, 0}, {373, 117, 42, 42, 0, 0, 0}, {375, 251, 140, 140, 0, 0, 0}, {385, 147, 133, 133, 3, 2, 3}, {386, 132, 16, 16, 3, 4, 5}, {387, 4, 72, 72, 3, 4, 5}, {388, 8, 37, 37, 3, 32, 33}, {9000, 113, 137, 137, 0, 0, 0}, {9005, 117, 34, 34, 0, 0, 0}, {12920, 20, 5, 5, 0, 0, 0} }};

//! MAVLINK VERSION
constexpr auto MAVLINK_VERSION = 3;


// ENUM DEFINITIONS


/** @brief These values define the type of firmware release.  These values indicate the first version or release of this type.  For example the first alpha release would be 64, the second would be 65. */
enum class FIRMWARE_VERSION_TYPE
{
    DEV=0, /* development release | */
    ALPHA=64, /* alpha release | */
    BETA=128, /* beta release | */
    RC=192, /* release candidate | */
    OFFICIAL=255, /* official stable release | */
};

//! FIRMWARE_VERSION_TYPE ENUM_END
constexpr auto FIRMWARE_VERSION_TYPE_ENUM_END = 256;

/** @brief Flags to report failure cases over the high latency telemtry. */
enum class HL_FAILURE_FLAG : uint16_t
{
    GPS=1, /* GPS failure. | */
    DIFFERENTIAL_PRESSURE=2, /* Differential pressure sensor failure. | */
    ABSOLUTE_PRESSURE=4, /* Absolute pressure sensor failure. | */
    FLAG_3D_ACCEL=8, /* Accelerometer sensor failure. | */
    FLAG_3D_GYRO=16, /* Gyroscope sensor failure. | */
    FLAG_3D_MAG=32, /* Magnetometer sensor failure. | */
    TERRAIN=64, /* Terrain subsystem failure. | */
    BATTERY=128, /* Battery failure/critical low battery. | */
    RC_RECEIVER=256, /* RC receiver failure/no rc connection. | */
    OFFBOARD_LINK=512, /* Offboard link failure. | */
    ENGINE=1024, /* Engine failure. | */
    GEOFENCE=2048, /* Geofence violation. | */
    ESTIMATOR=4096, /* Estimator failure, for example measurement rejection or large variances. | */
    MISSION=8192, /* Mission failure. | */
};

//! HL_FAILURE_FLAG ENUM_END
constexpr auto HL_FAILURE_FLAG_ENUM_END = 8193;

/** @brief Actions that may be specified in MAV_CMD_OVERRIDE_GOTO to override mission execution. */
enum class MAV_GOTO
{
    DO_HOLD=0, /* Hold at the current position. | */
    DO_CONTINUE=1, /* Continue with the next item in mission execution. | */
    HOLD_AT_CURRENT_POSITION=2, /* Hold at the current position of the system | */
    HOLD_AT_SPECIFIED_POSITION=3, /* Hold at the position specified in the parameters of the DO_HOLD action | */
};

//! MAV_GOTO ENUM_END
constexpr auto MAV_GOTO_ENUM_END = 4;

/** @brief These defines are predefined OR-combined mode flags. There is no need to use values from this enum, but it
               simplifies the use of the mode flags. Note that manual input is enabled in all modes as a safety override. */
enum class MAV_MODE : uint8_t
{
    PREFLIGHT=0, /* System is not ready to fly, booting, calibrating, etc. No flag is set. | */
    MANUAL_DISARMED=64, /* System is allowed to be active, under manual (RC) control, no stabilization | */
    TEST_DISARMED=66, /* UNDEFINED mode. This solely depends on the autopilot - use with caution, intended for developers only. | */
    STABILIZE_DISARMED=80, /* System is allowed to be active, under assisted RC control. | */
    GUIDED_DISARMED=88, /* System is allowed to be active, under autonomous control, manual setpoint | */
    AUTO_DISARMED=92, /* System is allowed to be active, under autonomous control and navigation (the trajectory is decided onboard and not pre-programmed by waypoints) | */
    MANUAL_ARMED=192, /* System is allowed to be active, under manual (RC) control, no stabilization | */
    TEST_ARMED=194, /* UNDEFINED mode. This solely depends on the autopilot - use with caution, intended for developers only. | */
    STABILIZE_ARMED=208, /* System is allowed to be active, under assisted RC control. | */
    GUIDED_ARMED=216, /* System is allowed to be active, under autonomous control, manual setpoint | */
    AUTO_ARMED=220, /* System is allowed to be active, under autonomous control and navigation (the trajectory is decided onboard and not pre-programmed by waypoints) | */
};

//! MAV_MODE ENUM_END
constexpr auto MAV_MODE_ENUM_END = 221;

/** @brief These encode the sensors whose status is sent as part of the SYS_STATUS message. */
enum class MAV_SYS_STATUS_SENSOR : uint32_t
{
    SENSOR_3D_GYRO=1, /* 0x01 3D gyro | */
    SENSOR_3D_ACCEL=2, /* 0x02 3D accelerometer | */
    SENSOR_3D_MAG=4, /* 0x04 3D magnetometer | */
    ABSOLUTE_PRESSURE=8, /* 0x08 absolute pressure | */
    DIFFERENTIAL_PRESSURE=16, /* 0x10 differential pressure | */
    GPS=32, /* 0x20 GPS | */
    OPTICAL_FLOW=64, /* 0x40 optical flow | */
    VISION_POSITION=128, /* 0x80 computer vision position | */
    LASER_POSITION=256, /* 0x100 laser based position | */
    EXTERNAL_GROUND_TRUTH=512, /* 0x200 external ground truth (Vicon or Leica) | */
    ANGULAR_RATE_CONTROL=1024, /* 0x400 3D angular rate control | */
    ATTITUDE_STABILIZATION=2048, /* 0x800 attitude stabilization | */
    YAW_POSITION=4096, /* 0x1000 yaw position | */
    Z_ALTITUDE_CONTROL=8192, /* 0x2000 z/altitude control | */
    XY_POSITION_CONTROL=16384, /* 0x4000 x/y position control | */
    MOTOR_OUTPUTS=32768, /* 0x8000 motor outputs / control | */
    RC_RECEIVER=65536, /* 0x10000 rc receiver | */
    SENSOR_3D_GYRO2=131072, /* 0x20000 2nd 3D gyro | */
    SENSOR_3D_ACCEL2=262144, /* 0x40000 2nd 3D accelerometer | */
    SENSOR_3D_MAG2=524288, /* 0x80000 2nd 3D magnetometer | */
    GEOFENCE=1048576, /* 0x100000 geofence | */
    AHRS=2097152, /* 0x200000 AHRS subsystem health | */
    TERRAIN=4194304, /* 0x400000 Terrain subsystem health | */
    REVERSE_MOTOR=8388608, /* 0x800000 Motors are reversed | */
    LOGGING=16777216, /* 0x1000000 Logging | */
    BATTERY=33554432, /* 0x2000000 Battery | */
    PROXIMITY=67108864, /* 0x4000000 Proximity | */
    SATCOM=134217728, /* 0x8000000 Satellite Communication  | */
    PREARM_CHECK=268435456, /* 0x10000000 pre-arm check status. Always healthy when armed | */
    OBSTACLE_AVOIDANCE=536870912, /* 0x20000000 Avoidance/collision prevention | */
    PROPULSION=1073741824, /* 0x40000000 propulsion (actuator, esc, motor or propellor) | */
};

//! MAV_SYS_STATUS_SENSOR ENUM_END
constexpr auto MAV_SYS_STATUS_SENSOR_ENUM_END = 1073741825;

/** @brief  */
enum class MAV_FRAME : uint8_t
{
    GLOBAL=0, /* Global (WGS84) coordinate frame + MSL altitude. First value / x: latitude, second value / y: longitude, third value / z: positive altitude over mean sea level (MSL). | */
    LOCAL_NED=1, /* Local coordinate frame, Z-down (x: North, y: East, z: Down). | */
    MISSION=2, /* NOT a coordinate frame, indicates a mission command. | */
    GLOBAL_RELATIVE_ALT=3, /* Global (WGS84) coordinate frame + altitude relative to the home position. First value / x: latitude, second value / y: longitude, third value / z: positive altitude with 0 being at the altitude of the home location. | */
    LOCAL_ENU=4, /* Local coordinate frame, Z-up (x: East, y: North, z: Up). | */
    GLOBAL_INT=5, /* Global (WGS84) coordinate frame (scaled) + MSL altitude. First value / x: latitude in degrees*1.0e-7, second value / y: longitude in degrees*1.0e-7, third value / z: positive altitude over mean sea level (MSL). | */
    GLOBAL_RELATIVE_ALT_INT=6, /* Global (WGS84) coordinate frame (scaled) + altitude relative to the home position. First value / x: latitude in degrees*10e-7, second value / y: longitude in degrees*10e-7, third value / z: positive altitude with 0 being at the altitude of the home location. | */
    LOCAL_OFFSET_NED=7, /* Offset to the current local frame. Anything expressed in this frame should be added to the current local frame position. | */
    BODY_NED=8, /* Setpoint in body NED frame. This makes sense if all position control is externalized - e.g. useful to command 2 m/s^2 acceleration to the right. | */
    BODY_OFFSET_NED=9, /* Offset in body NED frame. This makes sense if adding setpoints to the current flight path, to avoid an obstacle - e.g. useful to command 2 m/s^2 acceleration to the east. | */
    GLOBAL_TERRAIN_ALT=10, /* Global (WGS84) coordinate frame with AGL altitude (at the waypoint coordinate). First value / x: latitude in degrees, second value / y: longitude in degrees, third value / z: positive altitude in meters with 0 being at ground level in terrain model. | */
    GLOBAL_TERRAIN_ALT_INT=11, /* Global (WGS84) coordinate frame (scaled) with AGL altitude (at the waypoint coordinate). First value / x: latitude in degrees*10e-7, second value / y: longitude in degrees*10e-7, third value / z: positive altitude in meters with 0 being at ground level in terrain model. | */
    BODY_FRD=12, /* Body fixed frame of reference, Z-down (x: Forward, y: Right, z: Down). | */
    RESERVED_13=13, /* MAV_FRAME_BODY_FLU - Body fixed frame of reference, Z-up (x: Forward, y: Left, z: Up). | */
    RESERVED_14=14, /* MAV_FRAME_MOCAP_NED - Odometry local coordinate frame of data given by a motion capture system, Z-down (x: North, y: East, z: Down). | */
    RESERVED_15=15, /* MAV_FRAME_MOCAP_ENU - Odometry local coordinate frame of data given by a motion capture system, Z-up (x: East, y: North, z: Up). | */
    RESERVED_16=16, /* MAV_FRAME_VISION_NED - Odometry local coordinate frame of data given by a vision estimation system, Z-down (x: North, y: East, z: Down). | */
    RESERVED_17=17, /* MAV_FRAME_VISION_ENU - Odometry local coordinate frame of data given by a vision estimation system, Z-up (x: East, y: North, z: Up). | */
    RESERVED_18=18, /* MAV_FRAME_ESTIM_NED - Odometry local coordinate frame of data given by an estimator running onboard the vehicle, Z-down (x: North, y: East, z: Down). | */
    RESERVED_19=19, /* MAV_FRAME_ESTIM_ENU - Odometry local coordinate frame of data given by an estimator running onboard the vehicle, Z-up (x: East, y: North, z: Up). | */
    LOCAL_FRD=20, /* Forward, Right, Down coordinate frame. This is a local frame with Z-down and arbitrary F/R alignment (i.e. not aligned with NED/earth frame). | */
    LOCAL_FLU=21, /* Forward, Left, Up coordinate frame. This is a local frame with Z-up and arbitrary F/L alignment (i.e. not aligned with ENU/earth frame). | */
};

//! MAV_FRAME ENUM_END
constexpr auto MAV_FRAME_ENUM_END = 22;

/** @brief  */
enum class MAVLINK_DATA_STREAM_TYPE : uint8_t
{
    IMG_JPEG=0, /*  | */
    IMG_BMP=1, /*  | */
    IMG_RAW8U=2, /*  | */
    IMG_RAW32U=3, /*  | */
    IMG_PGM=4, /*  | */
    IMG_PNG=5, /*  | */
};

//! MAVLINK_DATA_STREAM_TYPE ENUM_END
constexpr auto MAVLINK_DATA_STREAM_TYPE_ENUM_END = 6;

/** @brief  */
enum class FENCE_ACTION
{
    NONE=0, /* Disable fenced mode | */
    GUIDED=1, /* Switched to guided mode to return point (fence point 0) | */
    REPORT=2, /* Report fence breach, but don't take action | */
    GUIDED_THR_PASS=3, /* Switched to guided mode to return point (fence point 0) with manual throttle control | */
    RTL=4, /* Switch to RTL (return to launch) mode and head for the return point. | */
};

//! FENCE_ACTION ENUM_END
constexpr auto FENCE_ACTION_ENUM_END = 5;

/** @brief  */
enum class FENCE_BREACH : uint8_t
{
    NONE=0, /* No last fence breach | */
    MINALT=1, /* Breached minimum altitude | */
    MAXALT=2, /* Breached maximum altitude | */
    BOUNDARY=3, /* Breached fence boundary | */
};

//! FENCE_BREACH ENUM_END
constexpr auto FENCE_BREACH_ENUM_END = 4;

/** @brief Actions being taken to mitigate/prevent fence breach */
enum class FENCE_MITIGATE : uint8_t
{
    UNKNOWN=0, /* Unknown | */
    NONE=1, /* No actions being taken | */
    VEL_LIMIT=2, /* Velocity limiting active to prevent breach | */
};

//! FENCE_MITIGATE ENUM_END
constexpr auto FENCE_MITIGATE_ENUM_END = 3;

/** @brief Enumeration of possible mount operation modes */
enum class MAV_MOUNT_MODE
{
    RETRACT=0, /* Load and keep safe position (Roll,Pitch,Yaw) from permant memory and stop stabilization | */
    NEUTRAL=1, /* Load and keep neutral position (Roll,Pitch,Yaw) from permanent memory. | */
    MAVLINK_TARGETING=2, /* Load neutral position and start MAVLink Roll,Pitch,Yaw control with stabilization | */
    RC_TARGETING=3, /* Load neutral position and start RC Roll,Pitch,Yaw control with stabilization | */
    GPS_POINT=4, /* Load neutral position and start to point to Lat,Lon,Alt | */
    SYSID_TARGET=5, /* Gimbal tracks system with specified system ID | */
    HOME_LOCATION=6, /* Gimbal tracks home location | */
};

//! MAV_MOUNT_MODE ENUM_END
constexpr auto MAV_MOUNT_MODE_ENUM_END = 7;

/** @brief Gimbal device (low level) capability flags (bitmap) */
enum class GIMBAL_DEVICE_CAP_FLAGS : uint16_t
{
    HAS_RETRACT=1, /* Gimbal device supports a retracted position | */
    HAS_NEUTRAL=2, /* Gimbal device supports a horizontal, forward looking position, stabilized | */
    HAS_ROLL_AXIS=4, /* Gimbal device supports rotating around roll axis. | */
    HAS_ROLL_FOLLOW=8, /* Gimbal device supports to follow a roll angle relative to the vehicle | */
    HAS_ROLL_LOCK=16, /* Gimbal device supports locking to an roll angle (generally that's the default with roll stabilized) | */
    HAS_PITCH_AXIS=32, /* Gimbal device supports rotating around pitch axis. | */
    HAS_PITCH_FOLLOW=64, /* Gimbal device supports to follow a pitch angle relative to the vehicle | */
    HAS_PITCH_LOCK=128, /* Gimbal device supports locking to an pitch angle (generally that's the default with pitch stabilized) | */
    HAS_YAW_AXIS=256, /* Gimbal device supports rotating around yaw axis. | */
    HAS_YAW_FOLLOW=512, /* Gimbal device supports to follow a yaw angle relative to the vehicle (generally that's the default) | */
    HAS_YAW_LOCK=1024, /* Gimbal device supports locking to an absolute heading (often this is an option available) | */
    SUPPORTS_INFINITE_YAW=2048, /* Gimbal device supports yawing/panning infinetely (e.g. using slip disk). | */
};

//! GIMBAL_DEVICE_CAP_FLAGS ENUM_END
constexpr auto GIMBAL_DEVICE_CAP_FLAGS_ENUM_END = 2049;

/** @brief Gripper actions. */
enum class GRIPPER_ACTIONS
{
    ACTION_RELEASE=0, /* Gripper release cargo. | */
    ACTION_GRAB=1, /* Gripper grab onto cargo. | */
};

//! GRIPPER_ACTIONS ENUM_END
constexpr auto GRIPPER_ACTIONS_ENUM_END = 2;

/** @brief Winch actions. */
enum class WINCH_ACTIONS
{
    RELAXED=0, /* Relax winch. | */
    RELATIVE_LENGTH_CONTROL=1, /* Wind or unwind specified length of cable, optionally using specified rate. | */
    RATE_CONTROL=2, /* Wind or unwind cable at specified rate. | */
};

//! WINCH_ACTIONS ENUM_END
constexpr auto WINCH_ACTIONS_ENUM_END = 3;

/** @brief Generalized UAVCAN node health */
enum class UAVCAN_NODE_HEALTH : uint8_t
{
    OK=0, /* The node is functioning properly. | */
    WARNING=1, /* A critical parameter went out of range or the node has encountered a minor failure. | */
    ERROR=2, /* The node has encountered a major failure. | */
    CRITICAL=3, /* The node has suffered a fatal malfunction. | */
};

//! UAVCAN_NODE_HEALTH ENUM_END
constexpr auto UAVCAN_NODE_HEALTH_ENUM_END = 4;

/** @brief Generalized UAVCAN node mode */
enum class UAVCAN_NODE_MODE : uint8_t
{
    OPERATIONAL=0, /* The node is performing its primary functions. | */
    INITIALIZATION=1, /* The node is initializing; this mode is entered immediately after startup. | */
    MAINTENANCE=2, /* The node is under maintenance. | */
    SOFTWARE_UPDATE=3, /* The node is in the process of updating its software. | */
    OFFLINE=7, /* The node is no longer available online. | */
};

//! UAVCAN_NODE_MODE ENUM_END
constexpr auto UAVCAN_NODE_MODE_ENUM_END = 8;

/** @brief Flags to indicate the status of camera storage. */
enum class STORAGE_STATUS : uint8_t
{
    EMPTY=0, /* Storage is missing (no microSD card loaded for example.) | */
    UNFORMATTED=1, /* Storage present but unformatted. | */
    READY=2, /* Storage present and ready. | */
    NOT_SUPPORTED=3, /* Camera does not supply storage status information. Capacity information in STORAGE_INFORMATION fields will be ignored. | */
};

//! STORAGE_STATUS ENUM_END
constexpr auto STORAGE_STATUS_ENUM_END = 4;

/** @brief Enable axes that will be tuned via autotuning. Used in MAV_CMD_DO_AUTOTUNE_ENABLE. */
enum class AUTOTUNE_AXIS
{
    DEFAULT=0, /* Flight stack tunes axis according to its default settings. | */
    ROLL=1, /* Autotune roll axis. | */
    PITCH=2, /* Autotune pitch axis. | */
    YAW=4, /* Autotune yaw axis. | */
};

//! AUTOTUNE_AXIS ENUM_END
constexpr auto AUTOTUNE_AXIS_ENUM_END = 5;

/** @brief A data stream is not a fixed set of messages, but rather a
     recommendation to the autopilot software. Individual autopilots may or may not obey
     the recommended messages. */
enum class MAV_DATA_STREAM
{
    ALL=0, /* Enable all data streams | */
    RAW_SENSORS=1, /* Enable IMU_RAW, GPS_RAW, GPS_STATUS packets. | */
    EXTENDED_STATUS=2, /* Enable GPS_STATUS, CONTROL_STATUS, AUX_STATUS | */
    RC_CHANNELS=3, /* Enable RC_CHANNELS_SCALED, RC_CHANNELS_RAW, SERVO_OUTPUT_RAW | */
    RAW_CONTROLLER=4, /* Enable ATTITUDE_CONTROLLER_OUTPUT, POSITION_CONTROLLER_OUTPUT, NAV_CONTROLLER_OUTPUT. | */
    POSITION=6, /* Enable LOCAL_POSITION, GLOBAL_POSITION/GLOBAL_POSITION_INT messages. | */
    EXTRA1=10, /* Dependent on the autopilot | */
    EXTRA2=11, /* Dependent on the autopilot | */
    EXTRA3=12, /* Dependent on the autopilot | */
};

//! MAV_DATA_STREAM ENUM_END
constexpr auto MAV_DATA_STREAM_ENUM_END = 13;

/** @brief The ROI (region of interest) for the vehicle. This can be
                be used by the vehicle for camera/vehicle attitude alignment (see
                MAV_CMD_NAV_ROI). */
enum class MAV_ROI
{
    NONE=0, /* No region of interest. | */
    WPNEXT=1, /* Point toward next waypoint, with optional pitch/roll/yaw offset. | */
    WPINDEX=2, /* Point toward given waypoint. | */
    LOCATION=3, /* Point toward fixed location. | */
    TARGET=4, /* Point toward of given id. | */
};

//! MAV_ROI ENUM_END
constexpr auto MAV_ROI_ENUM_END = 5;

/** @brief ACK / NACK / ERROR values as a result of MAV_CMDs and for mission item transmission. */
enum class MAV_CMD_ACK
{
    OK=0, /* Command / mission item is ok. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_FAIL=1, /* Generic error message if none of the other reasons fails or if no detailed error reporting is implemented. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_ACCESS_DENIED=2, /* The system is refusing to accept this command from this source / communication partner. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_NOT_SUPPORTED=3, /* Command or mission item is not supported, other commands would be accepted. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_COORDINATE_FRAME_NOT_SUPPORTED=4, /* The coordinate frame of this command / mission item is not supported. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_COORDINATES_OUT_OF_RANGE=5, /* The coordinate frame of this command is ok, but he coordinate values exceed the safety limits of this system. This is a generic error, please use the more specific error messages below if possible. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_X_LAT_OUT_OF_RANGE=6, /* The X or latitude value is out of range. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_Y_LON_OUT_OF_RANGE=7, /* The Y or longitude value is out of range. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
    ERR_Z_ALT_OUT_OF_RANGE=8, /* The Z or altitude value is out of range. |Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)| Reserved (default:0)|  */
};

//! MAV_CMD_ACK ENUM_END
constexpr auto MAV_CMD_ACK_ENUM_END = 9;

/** @brief Specifies the datatype of a MAVLink parameter. */
enum class MAV_PARAM_TYPE : uint8_t
{
    UINT8=1, /* 8-bit unsigned integer | */
    INT8=2, /* 8-bit signed integer | */
    UINT16=3, /* 16-bit unsigned integer | */
    INT16=4, /* 16-bit signed integer | */
    UINT32=5, /* 32-bit unsigned integer | */
    INT32=6, /* 32-bit signed integer | */
    UINT64=7, /* 64-bit unsigned integer | */
    INT64=8, /* 64-bit signed integer | */
    REAL32=9, /* 32-bit floating-point | */
    REAL64=10, /* 64-bit floating-point | */
};

//! MAV_PARAM_TYPE ENUM_END
constexpr auto MAV_PARAM_TYPE_ENUM_END = 11;

/** @brief Specifies the datatype of a MAVLink extended parameter. */
enum class MAV_PARAM_EXT_TYPE : uint8_t
{
    UINT8=1, /* 8-bit unsigned integer | */
    INT8=2, /* 8-bit signed integer | */
    UINT16=3, /* 16-bit unsigned integer | */
    INT16=4, /* 16-bit signed integer | */
    UINT32=5, /* 32-bit unsigned integer | */
    INT32=6, /* 32-bit signed integer | */
    UINT64=7, /* 64-bit unsigned integer | */
    INT64=8, /* 64-bit signed integer | */
    REAL32=9, /* 32-bit floating-point | */
    REAL64=10, /* 64-bit floating-point | */
    CUSTOM=11, /* Custom Type | */
};

//! MAV_PARAM_EXT_TYPE ENUM_END
constexpr auto MAV_PARAM_EXT_TYPE_ENUM_END = 12;

/** @brief Result from a MAVLink command (MAV_CMD) */
enum class MAV_RESULT : uint8_t
{
    ACCEPTED=0, /* Command is valid (is supported and has valid parameters), and was executed. | */
    TEMPORARILY_REJECTED=1, /* Command is valid, but cannot be executed at this time. This is used to indicate a problem that should be fixed just by waiting (e.g. a state machine is busy, can't arm because have not got GPS lock, etc.). Retrying later should work. | */
    DENIED=2, /* Command is invalid (is supported but has invalid parameters). Retrying same command and parameters will not work. | */
    UNSUPPORTED=3, /* Command is not supported (unknown). | */
    FAILED=4, /* Command is valid, but execution has failed. This is used to indicate any non-temporary or unexpected problem, i.e. any problem that must be fixed before the command can succeed/be retried. For example, attempting to write a file when out of memory, attempting to arm when sensors are not calibrated, etc. | */
    IN_PROGRESS=5, /* Command is valid and is being executed. This will be followed by further progress updates, i.e. the component may send further COMMAND_ACK messages with result MAV_RESULT_IN_PROGRESS (at a rate decided by the implementation), and must terminate by sending a COMMAND_ACK message with final result of the operation. The COMMAND_ACK.progress field can be used to indicate the progress of the operation. There is no need for the sender to retry the command, but if done during execution, the component will return MAV_RESULT_IN_PROGRESS with an updated progress. | */
};

//! MAV_RESULT ENUM_END
constexpr auto MAV_RESULT_ENUM_END = 6;

/** @brief Result of mission operation (in a MISSION_ACK message). */
enum class MAV_MISSION_RESULT : uint8_t
{
    ACCEPTED=0, /* mission accepted OK | */
    ERROR=1, /* Generic error / not accepting mission commands at all right now. | */
    UNSUPPORTED_FRAME=2, /* Coordinate frame is not supported. | */
    UNSUPPORTED=3, /* Command is not supported. | */
    NO_SPACE=4, /* Mission items exceed storage space. | */
    INVALID=5, /* One of the parameters has an invalid value. | */
    INVALID_PARAM1=6, /* param1 has an invalid value. | */
    INVALID_PARAM2=7, /* param2 has an invalid value. | */
    INVALID_PARAM3=8, /* param3 has an invalid value. | */
    INVALID_PARAM4=9, /* param4 has an invalid value. | */
    INVALID_PARAM5_X=10, /* x / param5 has an invalid value. | */
    INVALID_PARAM6_Y=11, /* y / param6 has an invalid value. | */
    INVALID_PARAM7=12, /* z / param7 has an invalid value. | */
    INVALID_SEQUENCE=13, /* Mission item received out of sequence | */
    DENIED=14, /* Not accepting any mission commands from this communication partner. | */
    OPERATION_CANCELLED=15, /* Current mission operation cancelled (e.g. mission upload, mission download). | */
};

//! MAV_MISSION_RESULT ENUM_END
constexpr auto MAV_MISSION_RESULT_ENUM_END = 16;

/** @brief Indicates the severity level, generally used for status messages to indicate their relative urgency. Based on RFC-5424 using expanded definitions at: http://www.kiwisyslog.com/kb/info:-syslog-message-levels/. */
enum class MAV_SEVERITY : uint8_t
{
    EMERGENCY=0, /* System is unusable. This is a "panic" condition. | */
    ALERT=1, /* Action should be taken immediately. Indicates error in non-critical systems. | */
    CRITICAL=2, /* Action must be taken immediately. Indicates failure in a primary system. | */
    ERROR=3, /* Indicates an error in secondary/redundant systems. | */
    WARNING=4, /* Indicates about a possible future error if this is not resolved within a given timeframe. Example would be a low battery warning. | */
    NOTICE=5, /* An unusual event has occurred, though not an error condition. This should be investigated for the root cause. | */
    INFO=6, /* Normal operational messages. Useful for logging. No action is required for these messages. | */
    DEBUG=7, /* Useful non-operational messages that can assist in debugging. These should not occur during normal operation. | */
};

//! MAV_SEVERITY ENUM_END
constexpr auto MAV_SEVERITY_ENUM_END = 8;

/** @brief Power supply status flags (bitmask) */
enum class MAV_POWER_STATUS : uint16_t
{
    BRICK_VALID=1, /* main brick power supply valid | */
    SERVO_VALID=2, /* main servo power supply valid for FMU | */
    USB_CONNECTED=4, /* USB power is connected | */
    PERIPH_OVERCURRENT=8, /* peripheral supply is in over-current state | */
    PERIPH_HIPOWER_OVERCURRENT=16, /* hi-power peripheral supply is in over-current state | */
    CHANGED=32, /* Power status has changed since boot | */
};

//! MAV_POWER_STATUS ENUM_END
constexpr auto MAV_POWER_STATUS_ENUM_END = 33;

/** @brief SERIAL_CONTROL device types */
enum class SERIAL_CONTROL_DEV : uint8_t
{
    TELEM1=0, /* First telemetry port | */
    TELEM2=1, /* Second telemetry port | */
    GPS1=2, /* First GPS port | */
    GPS2=3, /* Second GPS port | */
    SHELL=10, /* system shell | */
    SERIAL0=100, /* SERIAL0 | */
    SERIAL1=101, /* SERIAL1 | */
    SERIAL2=102, /* SERIAL2 | */
    SERIAL3=103, /* SERIAL3 | */
    SERIAL4=104, /* SERIAL4 | */
    SERIAL5=105, /* SERIAL5 | */
    SERIAL6=106, /* SERIAL6 | */
    SERIAL7=107, /* SERIAL7 | */
    SERIAL8=108, /* SERIAL8 | */
    SERIAL9=109, /* SERIAL9 | */
};

//! SERIAL_CONTROL_DEV ENUM_END
constexpr auto SERIAL_CONTROL_DEV_ENUM_END = 110;

/** @brief SERIAL_CONTROL flags (bitmask) */
enum class SERIAL_CONTROL_FLAG : uint8_t
{
    REPLY=1, /* Set if this is a reply | */
    RESPOND=2, /* Set if the sender wants the receiver to send a response as another SERIAL_CONTROL message | */
    EXCLUSIVE=4, /* Set if access to the serial port should be removed from whatever driver is currently using it, giving exclusive access to the SERIAL_CONTROL protocol. The port can be handed back by sending a request without this flag set | */
    BLOCKING=8, /* Block on writes to the serial port | */
    MULTI=16, /* Send multiple replies until port is drained | */
};

//! SERIAL_CONTROL_FLAG ENUM_END
constexpr auto SERIAL_CONTROL_FLAG_ENUM_END = 17;

/** @brief Enumeration of distance sensor types */
enum class MAV_DISTANCE_SENSOR : uint8_t
{
    LASER=0, /* Laser rangefinder, e.g. LightWare SF02/F or PulsedLight units | */
    ULTRASOUND=1, /* Ultrasound rangefinder, e.g. MaxBotix units | */
    INFRARED=2, /* Infrared rangefinder, e.g. Sharp units | */
    RADAR=3, /* Radar type, e.g. uLanding units | */
    UNKNOWN=4, /* Broken or unknown type, e.g. analog units | */
};

//! MAV_DISTANCE_SENSOR ENUM_END
constexpr auto MAV_DISTANCE_SENSOR_ENUM_END = 5;

/** @brief Enumeration of sensor orientation, according to its rotations */
enum class MAV_SENSOR_ORIENTATION : uint8_t
{
    ROTATION_NONE=0, /* Roll: 0, Pitch: 0, Yaw: 0 | */
    ROTATION_YAW_45=1, /* Roll: 0, Pitch: 0, Yaw: 45 | */
    ROTATION_YAW_90=2, /* Roll: 0, Pitch: 0, Yaw: 90 | */
    ROTATION_YAW_135=3, /* Roll: 0, Pitch: 0, Yaw: 135 | */
    ROTATION_YAW_180=4, /* Roll: 0, Pitch: 0, Yaw: 180 | */
    ROTATION_YAW_225=5, /* Roll: 0, Pitch: 0, Yaw: 225 | */
    ROTATION_YAW_270=6, /* Roll: 0, Pitch: 0, Yaw: 270 | */
    ROTATION_YAW_315=7, /* Roll: 0, Pitch: 0, Yaw: 315 | */
    ROTATION_ROLL_180=8, /* Roll: 180, Pitch: 0, Yaw: 0 | */
    ROTATION_ROLL_180_YAW_45=9, /* Roll: 180, Pitch: 0, Yaw: 45 | */
    ROTATION_ROLL_180_YAW_90=10, /* Roll: 180, Pitch: 0, Yaw: 90 | */
    ROTATION_ROLL_180_YAW_135=11, /* Roll: 180, Pitch: 0, Yaw: 135 | */
    ROTATION_PITCH_180=12, /* Roll: 0, Pitch: 180, Yaw: 0 | */
    ROTATION_ROLL_180_YAW_225=13, /* Roll: 180, Pitch: 0, Yaw: 225 | */
    ROTATION_ROLL_180_YAW_270=14, /* Roll: 180, Pitch: 0, Yaw: 270 | */
    ROTATION_ROLL_180_YAW_315=15, /* Roll: 180, Pitch: 0, Yaw: 315 | */
    ROTATION_ROLL_90=16, /* Roll: 90, Pitch: 0, Yaw: 0 | */
    ROTATION_ROLL_90_YAW_45=17, /* Roll: 90, Pitch: 0, Yaw: 45 | */
    ROTATION_ROLL_90_YAW_90=18, /* Roll: 90, Pitch: 0, Yaw: 90 | */
    ROTATION_ROLL_90_YAW_135=19, /* Roll: 90, Pitch: 0, Yaw: 135 | */
    ROTATION_ROLL_270=20, /* Roll: 270, Pitch: 0, Yaw: 0 | */
    ROTATION_ROLL_270_YAW_45=21, /* Roll: 270, Pitch: 0, Yaw: 45 | */
    ROTATION_ROLL_270_YAW_90=22, /* Roll: 270, Pitch: 0, Yaw: 90 | */
    ROTATION_ROLL_270_YAW_135=23, /* Roll: 270, Pitch: 0, Yaw: 135 | */
    ROTATION_PITCH_90=24, /* Roll: 0, Pitch: 90, Yaw: 0 | */
    ROTATION_PITCH_270=25, /* Roll: 0, Pitch: 270, Yaw: 0 | */
    ROTATION_PITCH_180_YAW_90=26, /* Roll: 0, Pitch: 180, Yaw: 90 | */
    ROTATION_PITCH_180_YAW_270=27, /* Roll: 0, Pitch: 180, Yaw: 270 | */
    ROTATION_ROLL_90_PITCH_90=28, /* Roll: 90, Pitch: 90, Yaw: 0 | */
    ROTATION_ROLL_180_PITCH_90=29, /* Roll: 180, Pitch: 90, Yaw: 0 | */
    ROTATION_ROLL_270_PITCH_90=30, /* Roll: 270, Pitch: 90, Yaw: 0 | */
    ROTATION_ROLL_90_PITCH_180=31, /* Roll: 90, Pitch: 180, Yaw: 0 | */
    ROTATION_ROLL_270_PITCH_180=32, /* Roll: 270, Pitch: 180, Yaw: 0 | */
    ROTATION_ROLL_90_PITCH_270=33, /* Roll: 90, Pitch: 270, Yaw: 0 | */
    ROTATION_ROLL_180_PITCH_270=34, /* Roll: 180, Pitch: 270, Yaw: 0 | */
    ROTATION_ROLL_270_PITCH_270=35, /* Roll: 270, Pitch: 270, Yaw: 0 | */
    ROTATION_ROLL_90_PITCH_180_YAW_90=36, /* Roll: 90, Pitch: 180, Yaw: 90 | */
    ROTATION_ROLL_90_YAW_270=37, /* Roll: 90, Pitch: 0, Yaw: 270 | */
    ROTATION_ROLL_90_PITCH_68_YAW_293=38, /* Roll: 90, Pitch: 68, Yaw: 293 | */
    ROTATION_PITCH_315=39, /* Pitch: 315 | */
    ROTATION_ROLL_90_PITCH_315=40, /* Roll: 90, Pitch: 315 | */
    ROTATION_CUSTOM=100, /* Custom orientation | */
};

//! MAV_SENSOR_ORIENTATION ENUM_END
constexpr auto MAV_SENSOR_ORIENTATION_ENUM_END = 101;

/** @brief Bitmask of (optional) autopilot capabilities (64 bit). If a bit is set, the autopilot supports this capability. */
enum class MAV_PROTOCOL_CAPABILITY : uint64_t
{
    MISSION_FLOAT=1, /* Autopilot supports MISSION float message type. | */
    PARAM_FLOAT=2, /* Autopilot supports the new param float message type. | */
    MISSION_INT=4, /* Autopilot supports MISSION_ITEM_INT scaled integer message type. | */
    COMMAND_INT=8, /* Autopilot supports COMMAND_INT scaled integer message type. | */
    PARAM_UNION=16, /* Autopilot supports the new param union message type. | */
    FTP=32, /* Autopilot supports the new FILE_TRANSFER_PROTOCOL message type. | */
    SET_ATTITUDE_TARGET=64, /* Autopilot supports commanding attitude offboard. | */
    SET_POSITION_TARGET_LOCAL_NED=128, /* Autopilot supports commanding position and velocity targets in local NED frame. | */
    SET_POSITION_TARGET_GLOBAL_INT=256, /* Autopilot supports commanding position and velocity targets in global scaled integers. | */
    TERRAIN=512, /* Autopilot supports terrain protocol / data handling. | */
    SET_ACTUATOR_TARGET=1024, /* Autopilot supports direct actuator control. | */
    FLIGHT_TERMINATION=2048, /* Autopilot supports the flight termination command. | */
    COMPASS_CALIBRATION=4096, /* Autopilot supports onboard compass calibration. | */
    MAVLINK2=8192, /* Autopilot supports MAVLink version 2. | */
    MISSION_FENCE=16384, /* Autopilot supports mission fence protocol. | */
    MISSION_RALLY=32768, /* Autopilot supports mission rally point protocol. | */
    FLIGHT_INFORMATION=65536, /* Autopilot supports the flight information protocol. | */
};

//! MAV_PROTOCOL_CAPABILITY ENUM_END
constexpr auto MAV_PROTOCOL_CAPABILITY_ENUM_END = 65537;

/** @brief Type of mission items being requested/sent in mission protocol. */
enum class MAV_MISSION_TYPE : uint8_t
{
    MISSION=0, /* Items are mission commands for main mission. | */
    FENCE=1, /* Specifies GeoFence area(s). Items are MAV_CMD_NAV_FENCE_ GeoFence items. | */
    RALLY=2, /* Specifies the rally points for the vehicle. Rally points are alternative RTL points. Items are MAV_CMD_NAV_RALLY_POINT rally point items. | */
    ALL=255, /* Only used in MISSION_CLEAR_ALL to clear all mission types. | */
};

//! MAV_MISSION_TYPE ENUM_END
constexpr auto MAV_MISSION_TYPE_ENUM_END = 256;

/** @brief Enumeration of estimator types */
enum class MAV_ESTIMATOR_TYPE : uint8_t
{
    UNKNOWN=0, /* Unknown type of the estimator. | */
    NAIVE=1, /* This is a naive estimator without any real covariance feedback. | */
    VISION=2, /* Computer vision based estimate. Might be up to scale. | */
    VIO=3, /* Visual-inertial estimate. | */
    GPS=4, /* Plain GPS estimate. | */
    GPS_INS=5, /* Estimator integrating GPS and inertial sensing. | */
    MOCAP=6, /* Estimate from external motion capturing system. | */
    LIDAR=7, /* Estimator based on lidar sensor input. | */
    AUTOPILOT=8, /* Estimator on autopilot. | */
};

//! MAV_ESTIMATOR_TYPE ENUM_END
constexpr auto MAV_ESTIMATOR_TYPE_ENUM_END = 9;

/** @brief Enumeration of battery types */
enum class MAV_BATTERY_TYPE : uint8_t
{
    UNKNOWN=0, /* Not specified. | */
    LIPO=1, /* Lithium polymer battery | */
    LIFE=2, /* Lithium-iron-phosphate battery | */
    LION=3, /* Lithium-ION battery | */
    NIMH=4, /* Nickel metal hydride battery | */
};

//! MAV_BATTERY_TYPE ENUM_END
constexpr auto MAV_BATTERY_TYPE_ENUM_END = 5;

/** @brief Enumeration of battery functions */
enum class MAV_BATTERY_FUNCTION : uint8_t
{
    UNKNOWN=0, /* Battery function is unknown | */
    ALL=1, /* Battery supports all flight systems | */
    PROPULSION=2, /* Battery for the propulsion system | */
    AVIONICS=3, /* Avionics battery | */
    TYPE_PAYLOAD=4, /* Payload battery | */
};

//! MAV_BATTERY_FUNCTION ENUM_END
constexpr auto MAV_BATTERY_FUNCTION_ENUM_END = 5;

/** @brief Enumeration for battery charge states. */
enum class MAV_BATTERY_CHARGE_STATE : uint8_t
{
    UNDEFINED=0, /* Low battery state is not provided | */
    OK=1, /* Battery is not in low state. Normal operation. | */
    LOW=2, /* Battery state is low, warn and monitor close. | */
    CRITICAL=3, /* Battery state is critical, return or abort immediately. | */
    EMERGENCY=4, /* Battery state is too low for ordinary abort sequence. Perform fastest possible emergency stop to prevent damage. | */
    FAILED=5, /* Battery failed, damage unavoidable. | */
    UNHEALTHY=6, /* Battery is diagnosed to be defective or an error occurred, usage is discouraged / prohibited. | */
    CHARGING=7, /* Battery is charging. | */
};

//! MAV_BATTERY_CHARGE_STATE ENUM_END
constexpr auto MAV_BATTERY_CHARGE_STATE_ENUM_END = 8;

/** @brief Battery mode. Note, the normal operation mode (i.e. when flying) should be reported as MAV_BATTERY_MODE_UNKNOWN to allow message trimming in normal flight. */
enum class MAV_BATTERY_MODE : uint8_t
{
    UNKNOWN=0, /* Battery mode not supported/unknown battery mode/normal operation. | */
    AUTO_DISCHARGING=1, /* Battery is auto discharging (towards storage level). | */
    HOT_SWAP=2, /* Battery in hot-swap mode (current limited to prevent spikes that might damage sensitive electrical circuits). | */
};

//! MAV_BATTERY_MODE ENUM_END
constexpr auto MAV_BATTERY_MODE_ENUM_END = 3;

/** @brief Smart battery supply status/fault flags (bitmask) for health indication. The battery must also report either MAV_BATTERY_CHARGE_STATE_FAILED or MAV_BATTERY_CHARGE_STATE_UNHEALTHY if any of these are set. */
enum class MAV_BATTERY_FAULT : uint32_t
{
    DEEP_DISCHARGE=1, /* Battery has deep discharged. | */
    SPIKES=2, /* Voltage spikes. | */
    CELL_FAIL=4, /* One or more cells have failed. Battery should also report MAV_BATTERY_CHARGE_STATE_FAILE (and should not be used). | */
    OVER_CURRENT=8, /* Over-current fault. | */
    OVER_TEMPERATURE=16, /* Over-temperature fault. | */
    UNDER_TEMPERATURE=32, /* Under-temperature fault. | */
    INCOMPATIBLE_VOLTAGE=64, /* Vehicle voltage is not compatible with this battery (batteries on same power rail should have similar voltage). | */
    INCOMPATIBLE_FIRMWARE=128, /* Battery firmware is not compatible with current autopilot firmware. | */
    BATTERY_FAULT_INCOMPATIBLE_CELLS_CONFIGURATION=256, /* Battery is not compatible due to cell configuration (e.g. 5s1p when vehicle requires 6s). | */
};

//! MAV_BATTERY_FAULT ENUM_END
constexpr auto MAV_BATTERY_FAULT_ENUM_END = 257;

/** @brief Flags to report status/failure cases for a power generator (used in GENERATOR_STATUS). Note that FAULTS are conditions that cause the generator to fail. Warnings are conditions that require attention before the next use (they indicate the system is not operating properly). */
enum class MAV_GENERATOR_STATUS_FLAG : uint64_t
{
    OFF=1, /* Generator is off. | */
    READY=2, /* Generator is ready to start generating power. | */
    GENERATING=4, /* Generator is generating power. | */
    CHARGING=8, /* Generator is charging the batteries (generating enough power to charge and provide the load). | */
    REDUCED_POWER=16, /* Generator is operating at a reduced maximum power. | */
    MAXPOWER=32, /* Generator is providing the maximum output. | */
    OVERTEMP_WARNING=64, /* Generator is near the maximum operating temperature, cooling is insufficient. | */
    OVERTEMP_FAULT=128, /* Generator hit the maximum operating temperature and shutdown. | */
    ELECTRONICS_OVERTEMP_WARNING=256, /* Power electronics are near the maximum operating temperature, cooling is insufficient. | */
    ELECTRONICS_OVERTEMP_FAULT=512, /* Power electronics hit the maximum operating temperature and shutdown. | */
    ELECTRONICS_FAULT=1024, /* Power electronics experienced a fault and shutdown. | */
    POWERSOURCE_FAULT=2048, /* The power source supplying the generator failed e.g. mechanical generator stopped, tether is no longer providing power, solar cell is in shade, hydrogen reaction no longer happening. | */
    COMMUNICATION_WARNING=4096, /* Generator controller having communication problems. | */
    COOLING_WARNING=8192, /* Power electronic or generator cooling system error. | */
    POWER_RAIL_FAULT=16384, /* Generator controller power rail experienced a fault. | */
    OVERCURRENT_FAULT=32768, /* Generator controller exceeded the overcurrent threshold and shutdown to prevent damage. | */
    BATTERY_OVERCHARGE_CURRENT_FAULT=65536, /* Generator controller detected a high current going into the batteries and shutdown to prevent battery damage. | */
    OVERVOLTAGE_FAULT=131072, /* Generator controller exceeded it's overvoltage threshold and shutdown to prevent it exceeding the voltage rating. | */
    BATTERY_UNDERVOLT_FAULT=262144, /* Batteries are under voltage (generator will not start). | */
    START_INHIBITED=524288, /* Generator start is inhibited by e.g. a safety switch. | */
    MAINTENANCE_REQUIRED=1048576, /* Generator requires maintenance. | */
    WARMING_UP=2097152, /* Generator is not ready to generate yet. | */
    IDLE=4194304, /* Generator is idle. | */
};

//! MAV_GENERATOR_STATUS_FLAG ENUM_END
constexpr auto MAV_GENERATOR_STATUS_FLAG_ENUM_END = 4194305;

/** @brief Enumeration of VTOL states */
enum class MAV_VTOL_STATE : uint8_t
{
    UNDEFINED=0, /* MAV is not configured as VTOL | */
    TRANSITION_TO_FW=1, /* VTOL is in transition from multicopter to fixed-wing | */
    TRANSITION_TO_MC=2, /* VTOL is in transition from fixed-wing to multicopter | */
    MC=3, /* VTOL is in multicopter state | */
    FW=4, /* VTOL is in fixed-wing state | */
};

//! MAV_VTOL_STATE ENUM_END
constexpr auto MAV_VTOL_STATE_ENUM_END = 5;

/** @brief Enumeration of landed detector states */
enum class MAV_LANDED_STATE : uint8_t
{
    UNDEFINED=0, /* MAV landed state is unknown | */
    ON_GROUND=1, /* MAV is landed (on ground) | */
    IN_AIR=2, /* MAV is in air | */
    TAKEOFF=3, /* MAV currently taking off | */
    LANDING=4, /* MAV currently landing | */
};

//! MAV_LANDED_STATE ENUM_END
constexpr auto MAV_LANDED_STATE_ENUM_END = 5;

/** @brief Enumeration of the ADSB altimeter types */
enum class ADSB_ALTITUDE_TYPE : uint8_t
{
    PRESSURE_QNH=0, /* Altitude reported from a Baro source using QNH reference | */
    GEOMETRIC=1, /* Altitude reported from a GNSS source | */
};

//! ADSB_ALTITUDE_TYPE ENUM_END
constexpr auto ADSB_ALTITUDE_TYPE_ENUM_END = 2;

/** @brief ADSB classification for the type of vehicle emitting the transponder signal */
enum class ADSB_EMITTER_TYPE : uint8_t
{
    NO_INFO=0, /*  | */
    LIGHT=1, /*  | */
    SMALL=2, /*  | */
    LARGE=3, /*  | */
    HIGH_VORTEX_LARGE=4, /*  | */
    HEAVY=5, /*  | */
    HIGHLY_MANUV=6, /*  | */
    ROTOCRAFT=7, /*  | */
    UNASSIGNED=8, /*  | */
    GLIDER=9, /*  | */
    LIGHTER_AIR=10, /*  | */
    PARACHUTE=11, /*  | */
    ULTRA_LIGHT=12, /*  | */
    UNASSIGNED2=13, /*  | */
    UAV=14, /*  | */
    SPACE=15, /*  | */
    UNASSGINED3=16, /*  | */
    EMERGENCY_SURFACE=17, /*  | */
    SERVICE_SURFACE=18, /*  | */
    POINT_OBSTACLE=19, /*  | */
};

//! ADSB_EMITTER_TYPE ENUM_END
constexpr auto ADSB_EMITTER_TYPE_ENUM_END = 20;

/** @brief These flags indicate status such as data validity of each data source. Set = data valid */
enum class ADSB_FLAGS : uint16_t
{
    VALID_COORDS=1, /*  | */
    VALID_ALTITUDE=2, /*  | */
    VALID_HEADING=4, /*  | */
    VALID_VELOCITY=8, /*  | */
    VALID_CALLSIGN=16, /*  | */
    VALID_SQUAWK=32, /*  | */
    SIMULATED=64, /*  | */
    VERTICAL_VELOCITY_VALID=128, /*  | */
    BARO_VALID=256, /*  | */
    SOURCE_UAT=32768, /*  | */
};

//! ADSB_FLAGS ENUM_END
constexpr auto ADSB_FLAGS_ENUM_END = 32769;

/** @brief Bitmap of options for the MAV_CMD_DO_REPOSITION */
enum class MAV_DO_REPOSITION_FLAGS
{
    CHANGE_MODE=1, /* The aircraft should immediately transition into guided. This should not be set for follow me applications | */
};

//! MAV_DO_REPOSITION_FLAGS ENUM_END
constexpr auto MAV_DO_REPOSITION_FLAGS_ENUM_END = 2;

/** @brief Flags in ESTIMATOR_STATUS message */
enum class ESTIMATOR_STATUS_FLAGS : uint16_t
{
    ATTITUDE=1, /* True if the attitude estimate is good | */
    VELOCITY_HORIZ=2, /* True if the horizontal velocity estimate is good | */
    VELOCITY_VERT=4, /* True if the  vertical velocity estimate is good | */
    POS_HORIZ_REL=8, /* True if the horizontal position (relative) estimate is good | */
    POS_HORIZ_ABS=16, /* True if the horizontal position (absolute) estimate is good | */
    POS_VERT_ABS=32, /* True if the vertical position (absolute) estimate is good | */
    POS_VERT_AGL=64, /* True if the vertical position (above ground) estimate is good | */
    CONST_POS_MODE=128, /* True if the EKF is in a constant position mode and is not using external measurements (eg GPS or optical flow) | */
    PRED_POS_HORIZ_REL=256, /* True if the EKF has sufficient data to enter a mode that will provide a (relative) position estimate | */
    PRED_POS_HORIZ_ABS=512, /* True if the EKF has sufficient data to enter a mode that will provide a (absolute) position estimate | */
    GPS_GLITCH=1024, /* True if the EKF has detected a GPS glitch | */
    ACCEL_ERROR=2048, /* True if the EKF has detected bad accelerometer data | */
};

//! ESTIMATOR_STATUS_FLAGS ENUM_END
constexpr auto ESTIMATOR_STATUS_FLAGS_ENUM_END = 2049;

/** @brief  */
enum class MOTOR_TEST_ORDER
{
    DEFAULT=0, /* default autopilot motor test method | */
    SEQUENCE=1, /* motor numbers are specified as their index in a predefined vehicle-specific sequence | */
    BOARD=2, /* motor numbers are specified as the output as labeled on the board | */
};

//! MOTOR_TEST_ORDER ENUM_END
constexpr auto MOTOR_TEST_ORDER_ENUM_END = 3;

/** @brief  */
enum class MOTOR_TEST_THROTTLE_TYPE
{
    PERCENT=0, /* throttle as a percentage from 0 ~ 100 | */
    PWM=1, /* throttle as an absolute PWM value (normally in range of 1000~2000) | */
    PILOT=2, /* throttle pass-through from pilot's transmitter | */
    COMPASS_CAL=3, /* per-motor compass calibration test | */
};

//! MOTOR_TEST_THROTTLE_TYPE ENUM_END
constexpr auto MOTOR_TEST_THROTTLE_TYPE_ENUM_END = 4;

/** @brief  */
enum class GPS_INPUT_IGNORE_FLAGS : uint16_t
{
    FLAG_ALT=1, /* ignore altitude field | */
    FLAG_HDOP=2, /* ignore hdop field | */
    FLAG_VDOP=4, /* ignore vdop field | */
    FLAG_VEL_HORIZ=8, /* ignore horizontal velocity field (vn and ve) | */
    FLAG_VEL_VERT=16, /* ignore vertical velocity field (vd) | */
    FLAG_SPEED_ACCURACY=32, /* ignore speed accuracy field | */
    FLAG_HORIZONTAL_ACCURACY=64, /* ignore horizontal accuracy field | */
    FLAG_VERTICAL_ACCURACY=128, /* ignore vertical accuracy field | */
};

//! GPS_INPUT_IGNORE_FLAGS ENUM_END
constexpr auto GPS_INPUT_IGNORE_FLAGS_ENUM_END = 129;

/** @brief Possible actions an aircraft can take to avoid a collision. */
enum class MAV_COLLISION_ACTION : uint8_t
{
    NONE=0, /* Ignore any potential collisions | */
    REPORT=1, /* Report potential collision | */
    ASCEND_OR_DESCEND=2, /* Ascend or Descend to avoid threat | */
    MOVE_HORIZONTALLY=3, /* Move horizontally to avoid threat | */
    MOVE_PERPENDICULAR=4, /* Aircraft to move perpendicular to the collision's velocity vector | */
    RTL=5, /* Aircraft to fly directly back to its launch point | */
    HOVER=6, /* Aircraft to stop in place | */
};

//! MAV_COLLISION_ACTION ENUM_END
constexpr auto MAV_COLLISION_ACTION_ENUM_END = 7;

/** @brief Aircraft-rated danger from this threat. */
enum class MAV_COLLISION_THREAT_LEVEL : uint8_t
{
    NONE=0, /* Not a threat | */
    LOW=1, /* Craft is mildly concerned about this threat | */
    HIGH=2, /* Craft is panicking, and may take actions to avoid threat | */
};

//! MAV_COLLISION_THREAT_LEVEL ENUM_END
constexpr auto MAV_COLLISION_THREAT_LEVEL_ENUM_END = 3;

/** @brief Source of information about this collision. */
enum class MAV_COLLISION_SRC : uint8_t
{
    ADSB=0, /* ID field references ADSB_VEHICLE packets | */
    MAVLINK_GPS_GLOBAL_INT=1, /* ID field references MAVLink SRC ID | */
};

//! MAV_COLLISION_SRC ENUM_END
constexpr auto MAV_COLLISION_SRC_ENUM_END = 2;

/** @brief Type of GPS fix */
enum class GPS_FIX_TYPE : uint8_t
{
    NO_GPS=0, /* No GPS connected | */
    NO_FIX=1, /* No position information, GPS is connected | */
    TYPE_2D_FIX=2, /* 2D position | */
    TYPE_3D_FIX=3, /* 3D position | */
    DGPS=4, /* DGPS/SBAS aided 3D position | */
    RTK_FLOAT=5, /* RTK float, 3D position | */
    RTK_FIXED=6, /* RTK Fixed, 3D position | */
    STATIC=7, /* Static fixed, typically used for base stations | */
    PPP=8, /* PPP, 3D position. | */
};

//! GPS_FIX_TYPE ENUM_END
constexpr auto GPS_FIX_TYPE_ENUM_END = 9;

/** @brief RTK GPS baseline coordinate system, used for RTK corrections */
enum class RTK_BASELINE_COORDINATE_SYSTEM : uint8_t
{
    ECEF=0, /* Earth-centered, Earth-fixed | */
    NED=1, /* RTK basestation centered, north, east, down | */
};

//! RTK_BASELINE_COORDINATE_SYSTEM ENUM_END
constexpr auto RTK_BASELINE_COORDINATE_SYSTEM_ENUM_END = 2;

/** @brief Type of landing target */
enum class LANDING_TARGET_TYPE : uint8_t
{
    LIGHT_BEACON=0, /* Landing target signaled by light beacon (ex: IR-LOCK) | */
    RADIO_BEACON=1, /* Landing target signaled by radio beacon (ex: ILS, NDB) | */
    VISION_FIDUCIAL=2, /* Landing target represented by a fiducial marker (ex: ARTag) | */
    VISION_OTHER=3, /* Landing target represented by a pre-defined visual shape/feature (ex: X-marker, H-marker, square) | */
};

//! LANDING_TARGET_TYPE ENUM_END
constexpr auto LANDING_TARGET_TYPE_ENUM_END = 4;

/** @brief Direction of VTOL transition */
enum class VTOL_TRANSITION_HEADING
{
    VEHICLE_DEFAULT=0, /* Respect the heading configuration of the vehicle. | */
    NEXT_WAYPOINT=1, /* Use the heading pointing towards the next waypoint. | */
    TAKEOFF=2, /* Use the heading on takeoff (while sitting on the ground). | */
    SPECIFIED=3, /* Use the specified heading in parameter 4. | */
    ANY=4, /* Use the current heading when reaching takeoff altitude (potentially facing the wind when weather-vaning is active). | */
};

//! VTOL_TRANSITION_HEADING ENUM_END
constexpr auto VTOL_TRANSITION_HEADING_ENUM_END = 5;

/** @brief Camera capability flags (Bitmap) */
enum class CAMERA_CAP_FLAGS : uint32_t
{
    CAPTURE_VIDEO=1, /* Camera is able to record video | */
    CAPTURE_IMAGE=2, /* Camera is able to capture images | */
    HAS_MODES=4, /* Camera has separate Video and Image/Photo modes (MAV_CMD_SET_CAMERA_MODE) | */
    CAN_CAPTURE_IMAGE_IN_VIDEO_MODE=8, /* Camera can capture images while in video mode | */
    CAN_CAPTURE_VIDEO_IN_IMAGE_MODE=16, /* Camera can capture videos while in Photo/Image mode | */
    HAS_IMAGE_SURVEY_MODE=32, /* Camera has image survey mode (MAV_CMD_SET_CAMERA_MODE) | */
    HAS_BASIC_ZOOM=64, /* Camera has basic zoom control (MAV_CMD_SET_CAMERA_ZOOM) | */
    HAS_BASIC_FOCUS=128, /* Camera has basic focus control (MAV_CMD_SET_CAMERA_FOCUS) | */
    HAS_VIDEO_STREAM=256, /* Camera has video streaming capabilities (request VIDEO_STREAM_INFORMATION with MAV_CMD_REQUEST_MESSAGE for video streaming info) | */
    HAS_TRACKING_POINT=512, /* Camera supports tracking of a point on the camera view. | */
    HAS_TRACKING_RECTANGLE=1024, /* Camera supports tracking of a selection rectangle on the camera view. | */
    HAS_TRACKING_GEO_STATUS=2048, /* Camera supports tracking geo status (CAMERA_TRACKING_GEO_STATUS). | */
};

//! CAMERA_CAP_FLAGS ENUM_END
constexpr auto CAMERA_CAP_FLAGS_ENUM_END = 2049;

/** @brief Stream status flags (Bitmap) */
enum class VIDEO_STREAM_STATUS_FLAGS : uint16_t
{
    RUNNING=1, /* Stream is active (running) | */
    THERMAL=2, /* Stream is thermal imaging | */
};

//! VIDEO_STREAM_STATUS_FLAGS ENUM_END
constexpr auto VIDEO_STREAM_STATUS_FLAGS_ENUM_END = 3;

/** @brief Video stream types */
enum class VIDEO_STREAM_TYPE : uint8_t
{
    RTSP=0, /* Stream is RTSP | */
    RTPUDP=1, /* Stream is RTP UDP (URI gives the port number) | */
    TCP_MPEG=2, /* Stream is MPEG on TCP | */
    MPEG_TS_H264=3, /* Stream is h.264 on MPEG TS (URI gives the port number) | */
};

//! VIDEO_STREAM_TYPE ENUM_END
constexpr auto VIDEO_STREAM_TYPE_ENUM_END = 4;

/** @brief Result from PARAM_EXT_SET message (or a PARAM_SET within a transaction). */
enum class PARAM_ACK : uint8_t
{
    ACCEPTED=0, /* Parameter value ACCEPTED and SET | */
    VALUE_UNSUPPORTED=1, /* Parameter value UNKNOWN/UNSUPPORTED | */
    FAILED=2, /* Parameter failed to set | */
    IN_PROGRESS=3, /* Parameter value received but not yet set/accepted. A subsequent PARAM_ACK_TRANSACTION or PARAM_EXT_ACK with the final result will follow once operation is completed. This is returned immediately for parameters that take longer to set, indicating taht the the parameter was recieved and does not need to be resent. | */
};

//! PARAM_ACK ENUM_END
constexpr auto PARAM_ACK_ENUM_END = 4;

/** @brief Camera Modes. */
enum class CAMERA_MODE : uint8_t
{
    IMAGE=0, /* Camera is in image/photo capture mode. | */
    VIDEO=1, /* Camera is in video capture mode. | */
    IMAGE_SURVEY=2, /* Camera is in image survey capture mode. It allows for camera controller to do specific settings for surveys. | */
};

//! CAMERA_MODE ENUM_END
constexpr auto CAMERA_MODE_ENUM_END = 3;

/** @brief  */
enum class MAV_ARM_AUTH_DENIED_REASON
{
    GENERIC=0, /* Not a specific reason | */
    NONE=1, /* Authorizer will send the error as string to GCS | */
    INVALID_WAYPOINT=2, /* At least one waypoint have a invalid value | */
    TIMEOUT=3, /* Timeout in the authorizer process(in case it depends on network) | */
    AIRSPACE_IN_USE=4, /* Airspace of the mission in use by another vehicle, second result parameter can have the waypoint id that caused it to be denied. | */
    BAD_WEATHER=5, /* Weather is not good to fly | */
};

//! MAV_ARM_AUTH_DENIED_REASON ENUM_END
constexpr auto MAV_ARM_AUTH_DENIED_REASON_ENUM_END = 6;

/** @brief RC type */
enum class RC_TYPE
{
    SPEKTRUM_DSM2=0, /* Spektrum DSM2 | */
    SPEKTRUM_DSMX=1, /* Spektrum DSMX | */
};

//! RC_TYPE ENUM_END
constexpr auto RC_TYPE_ENUM_END = 2;

/** @brief Bitmap to indicate which dimensions should be ignored by the vehicle: a value of 0b0000000000000000 or 0b0000001000000000 indicates that none of the setpoint dimensions should be ignored. If bit 9 is set the floats afx afy afz should be interpreted as force instead of acceleration. */
enum class POSITION_TARGET_TYPEMASK : uint16_t
{
    X_IGNORE=1, /* Ignore position x | */
    Y_IGNORE=2, /* Ignore position y | */
    Z_IGNORE=4, /* Ignore position z | */
    VX_IGNORE=8, /* Ignore velocity x | */
    VY_IGNORE=16, /* Ignore velocity y | */
    VZ_IGNORE=32, /* Ignore velocity z | */
    AX_IGNORE=64, /* Ignore acceleration x | */
    AY_IGNORE=128, /* Ignore acceleration y | */
    AZ_IGNORE=256, /* Ignore acceleration z | */
    FORCE_SET=512, /* Use force instead of acceleration | */
    YAW_IGNORE=1024, /* Ignore yaw | */
    YAW_RATE_IGNORE=2048, /* Ignore yaw rate | */
};

//! POSITION_TARGET_TYPEMASK ENUM_END
constexpr auto POSITION_TARGET_TYPEMASK_ENUM_END = 2049;

/** @brief Bitmap to indicate which dimensions should be ignored by the vehicle: a value of 0b00000000 indicates that none of the setpoint dimensions should be ignored. */
enum class ATTITUDE_TARGET_TYPEMASK : uint8_t
{
    BODY_ROLL_RATE_IGNORE=1, /* Ignore body roll rate | */
    BODY_PITCH_RATE_IGNORE=2, /* Ignore body pitch rate | */
    BODY_YAW_RATE_IGNORE=4, /* Ignore body yaw rate | */
    THROTTLE_IGNORE=64, /* Ignore throttle | */
    ATTITUDE_IGNORE=128, /* Ignore attitude | */
};

//! ATTITUDE_TARGET_TYPEMASK ENUM_END
constexpr auto ATTITUDE_TARGET_TYPEMASK_ENUM_END = 129;

/** @brief Airborne status of UAS. */
enum class UTM_FLIGHT_STATE : uint8_t
{
    UNKNOWN=1, /* The flight state can't be determined. | */
    GROUND=2, /* UAS on ground. | */
    AIRBORNE=3, /* UAS airborne. | */
    EMERGENCY=16, /* UAS is in an emergency flight state. | */
    NOCTRL=32, /* UAS has no active controls. | */
};

//! UTM_FLIGHT_STATE ENUM_END
constexpr auto UTM_FLIGHT_STATE_ENUM_END = 33;

/** @brief Flags for the global position report. */
enum class UTM_DATA_AVAIL_FLAGS : uint8_t
{
    TIME_VALID=1, /* The field time contains valid data. | */
    UAS_ID_AVAILABLE=2, /* The field uas_id contains valid data. | */
    POSITION_AVAILABLE=4, /* The fields lat, lon and h_acc contain valid data. | */
    ALTITUDE_AVAILABLE=8, /* The fields alt and v_acc contain valid data. | */
    RELATIVE_ALTITUDE_AVAILABLE=16, /* The field relative_alt contains valid data. | */
    HORIZONTAL_VELO_AVAILABLE=32, /* The fields vx and vy contain valid data. | */
    VERTICAL_VELO_AVAILABLE=64, /* The field vz contains valid data. | */
    NEXT_WAYPOINT_AVAILABLE=128, /* The fields next_lat, next_lon and next_alt contain valid data. | */
};

//! UTM_DATA_AVAIL_FLAGS ENUM_END
constexpr auto UTM_DATA_AVAIL_FLAGS_ENUM_END = 129;

/** @brief Precision land modes (used in MAV_CMD_NAV_LAND). */
enum class PRECISION_LAND_MODE
{
    DISABLED=0, /* Normal (non-precision) landing. | */
    OPPORTUNISTIC=1, /* Use precision landing if beacon detected when land command accepted, otherwise land normally. | */
    REQUIRED=2, /* Use precision landing, searching for beacon if not found when land command accepted (land normally if beacon cannot be found). | */
};

//! PRECISION_LAND_MODE ENUM_END
constexpr auto PRECISION_LAND_MODE_ENUM_END = 3;

/** @brief Parachute actions. Trigger release and enable/disable auto-release. */
enum class PARACHUTE_ACTION
{
    DISABLE=0, /* Disable auto-release of parachute (i.e. release triggered by crash detectors). | */
    ENABLE=1, /* Enable auto-release of parachute. | */
    RELEASE=2, /* Release parachute and kill motors. | */
};

//! PARACHUTE_ACTION ENUM_END
constexpr auto PARACHUTE_ACTION_ENUM_END = 3;

/** @brief  */
enum class MAV_TUNNEL_PAYLOAD_TYPE : uint16_t
{
    UNKNOWN=0, /* Encoding of payload unknown. | */
    STORM32_RESERVED0=200, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED1=201, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED2=202, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED3=203, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED4=204, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED5=205, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED6=206, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED7=207, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED8=208, /* Registered for STorM32 gimbal controller. | */
    STORM32_RESERVED9=209, /* Registered for STorM32 gimbal controller. | */
};

//! MAV_TUNNEL_PAYLOAD_TYPE ENUM_END
constexpr auto MAV_TUNNEL_PAYLOAD_TYPE_ENUM_END = 210;

/** @brief Type of AIS vessel, enum duplicated from AIS standard, https://gpsd.gitlab.io/gpsd/AIVDM.html */
enum class AIS_TYPE : uint8_t
{
    UNKNOWN=0, /* Not available (default). | */
    RESERVED_1=1, /*  | */
    RESERVED_2=2, /*  | */
    RESERVED_3=3, /*  | */
    RESERVED_4=4, /*  | */
    RESERVED_5=5, /*  | */
    RESERVED_6=6, /*  | */
    RESERVED_7=7, /*  | */
    RESERVED_8=8, /*  | */
    RESERVED_9=9, /*  | */
    RESERVED_10=10, /*  | */
    RESERVED_11=11, /*  | */
    RESERVED_12=12, /*  | */
    RESERVED_13=13, /*  | */
    RESERVED_14=14, /*  | */
    RESERVED_15=15, /*  | */
    RESERVED_16=16, /*  | */
    RESERVED_17=17, /*  | */
    RESERVED_18=18, /*  | */
    RESERVED_19=19, /*  | */
    WIG=20, /* Wing In Ground effect. | */
    WIG_HAZARDOUS_A=21, /*  | */
    WIG_HAZARDOUS_B=22, /*  | */
    WIG_HAZARDOUS_C=23, /*  | */
    WIG_HAZARDOUS_D=24, /*  | */
    WIG_RESERVED_1=25, /*  | */
    WIG_RESERVED_2=26, /*  | */
    WIG_RESERVED_3=27, /*  | */
    WIG_RESERVED_4=28, /*  | */
    WIG_RESERVED_5=29, /*  | */
    FISHING=30, /*  | */
    TOWING=31, /*  | */
    TOWING_LARGE=32, /* Towing: length exceeds 200m or breadth exceeds 25m. | */
    DREDGING=33, /* Dredging or other underwater ops. | */
    DIVING=34, /*  | */
    MILITARY=35, /*  | */
    SAILING=36, /*  | */
    PLEASURE=37, /*  | */
    RESERVED_20=38, /*  | */
    RESERVED_21=39, /*  | */
    HSC=40, /* High Speed Craft. | */
    HSC_HAZARDOUS_A=41, /*  | */
    HSC_HAZARDOUS_B=42, /*  | */
    HSC_HAZARDOUS_C=43, /*  | */
    HSC_HAZARDOUS_D=44, /*  | */
    HSC_RESERVED_1=45, /*  | */
    HSC_RESERVED_2=46, /*  | */
    HSC_RESERVED_3=47, /*  | */
    HSC_RESERVED_4=48, /*  | */
    HSC_UNKNOWN=49, /*  | */
    PILOT=50, /*  | */
    SAR=51, /* Search And Rescue vessel. | */
    TUG=52, /*  | */
    PORT_TENDER=53, /*  | */
    ANTI_POLLUTION=54, /* Anti-pollution equipment. | */
    LAW_ENFORCEMENT=55, /*  | */
    SPARE_LOCAL_1=56, /*  | */
    SPARE_LOCAL_2=57, /*  | */
    MEDICAL_TRANSPORT=58, /*  | */
    NONECOMBATANT=59, /* Noncombatant ship according to RR Resolution No. 18. | */
    PASSENGER=60, /*  | */
    PASSENGER_HAZARDOUS_A=61, /*  | */
    PASSENGER_HAZARDOUS_B=62, /*  | */
    AIS_TYPE_PASSENGER_HAZARDOUS_C=63, /*  | */
    PASSENGER_HAZARDOUS_D=64, /*  | */
    PASSENGER_RESERVED_1=65, /*  | */
    PASSENGER_RESERVED_2=66, /*  | */
    PASSENGER_RESERVED_3=67, /*  | */
    AIS_TYPE_PASSENGER_RESERVED_4=68, /*  | */
    PASSENGER_UNKNOWN=69, /*  | */
    CARGO=70, /*  | */
    CARGO_HAZARDOUS_A=71, /*  | */
    CARGO_HAZARDOUS_B=72, /*  | */
    CARGO_HAZARDOUS_C=73, /*  | */
    CARGO_HAZARDOUS_D=74, /*  | */
    CARGO_RESERVED_1=75, /*  | */
    CARGO_RESERVED_2=76, /*  | */
    CARGO_RESERVED_3=77, /*  | */
    CARGO_RESERVED_4=78, /*  | */
    CARGO_UNKNOWN=79, /*  | */
    TANKER=80, /*  | */
    TANKER_HAZARDOUS_A=81, /*  | */
    TANKER_HAZARDOUS_B=82, /*  | */
    TANKER_HAZARDOUS_C=83, /*  | */
    TANKER_HAZARDOUS_D=84, /*  | */
    TANKER_RESERVED_1=85, /*  | */
    TANKER_RESERVED_2=86, /*  | */
    TANKER_RESERVED_3=87, /*  | */
    TANKER_RESERVED_4=88, /*  | */
    TANKER_UNKNOWN=89, /*  | */
    OTHER=90, /*  | */
    OTHER_HAZARDOUS_A=91, /*  | */
    OTHER_HAZARDOUS_B=92, /*  | */
    OTHER_HAZARDOUS_C=93, /*  | */
    OTHER_HAZARDOUS_D=94, /*  | */
    OTHER_RESERVED_1=95, /*  | */
    OTHER_RESERVED_2=96, /*  | */
    OTHER_RESERVED_3=97, /*  | */
    OTHER_RESERVED_4=98, /*  | */
    OTHER_UNKNOWN=99, /*  | */
};

//! AIS_TYPE ENUM_END
constexpr auto AIS_TYPE_ENUM_END = 100;

/** @brief Navigational status of AIS vessel, enum duplicated from AIS standard, https://gpsd.gitlab.io/gpsd/AIVDM.html */
enum class AIS_NAV_STATUS : uint8_t
{
    UNDER_WAY=0, /* Under way using engine. | */
    ANCHORED=1, /*  | */
    UN_COMMANDED=2, /*  | */
    RESTRICTED_MANOEUVERABILITY=3, /*  | */
    DRAUGHT_CONSTRAINED=4, /*  | */
    MOORED=5, /*  | */
    AGROUND=6, /*  | */
    FISHING=7, /*  | */
    SAILING=8, /*  | */
    RESERVED_HSC=9, /*  | */
    RESERVED_WIG=10, /*  | */
    RESERVED_1=11, /*  | */
    RESERVED_2=12, /*  | */
    RESERVED_3=13, /*  | */
    AIS_SART=14, /* Search And Rescue Transponder. | */
    UNKNOWN=15, /* Not available (default). | */
};

//! AIS_NAV_STATUS ENUM_END
constexpr auto AIS_NAV_STATUS_ENUM_END = 16;

/** @brief These flags are used in the AIS_VESSEL.fields bitmask to indicate validity of data in the other message fields. When set, the data is valid. */
enum class AIS_FLAGS : uint16_t
{
    POSITION_ACCURACY=1, /* 1 = Position accuracy less than 10m, 0 = position accuracy greater than 10m. | */
    VALID_COG=2, /*  | */
    VALID_VELOCITY=4, /*  | */
    HIGH_VELOCITY=8, /* 1 = Velocity over 52.5765m/s (102.2 knots) | */
    VALID_TURN_RATE=16, /*  | */
    TURN_RATE_SIGN_ONLY=32, /* Only the sign of the returned turn rate value is valid, either greater than 5deg/30s or less than -5deg/30s | */
    VALID_DIMENSIONS=64, /*  | */
    LARGE_BOW_DIMENSION=128, /* Distance to bow is larger than 511m | */
    LARGE_STERN_DIMENSION=256, /* Distance to stern is larger than 511m | */
    LARGE_PORT_DIMENSION=512, /* Distance to port side is larger than 63m | */
    LARGE_STARBOARD_DIMENSION=1024, /* Distance to starboard side is larger than 63m | */
    VALID_CALLSIGN=2048, /*  | */
    VALID_NAME=4096, /*  | */
};

//! AIS_FLAGS ENUM_END
constexpr auto AIS_FLAGS_ENUM_END = 4097;

/** @brief Winch status flags used in WINCH_STATUS */
enum class MAV_WINCH_STATUS_FLAG : uint32_t
{
    HEALTHY=1, /* Winch is healthy | */
    FULLY_RETRACTED=2, /* Winch thread is fully retracted | */
    MOVING=4, /* Winch motor is moving | */
    CLUTCH_ENGAGED=8, /* Winch clutch is engaged allowing motor to move freely | */
};

//! MAV_WINCH_STATUS_FLAG ENUM_END
constexpr auto MAV_WINCH_STATUS_FLAG_ENUM_END = 9;

/** @brief  */
enum class MAG_CAL_STATUS : uint8_t
{
    NOT_STARTED=0, /*  | */
    WAITING_TO_START=1, /*  | */
    RUNNING_STEP_ONE=2, /*  | */
    RUNNING_STEP_TWO=3, /*  | */
    SUCCESS=4, /*  | */
    FAILED=5, /*  | */
    BAD_ORIENTATION=6, /*  | */
    BAD_RADIUS=7, /*  | */
};

//! MAG_CAL_STATUS ENUM_END
constexpr auto MAG_CAL_STATUS_ENUM_END = 8;

/** @brief  */
enum class CAN_FILTER_OP : uint8_t
{
    REPLACE=0, /*  | */
    ADD=1, /*  | */
    REMOVE=2, /*  | */
};

//! CAN_FILTER_OP ENUM_END
constexpr auto CAN_FILTER_OP_ENUM_END = 3;


} // namespace common
} // namespace mavlink

// MESSAGE DEFINITIONS
#include "./mavlink_msg_sys_status.hpp"
#include "./mavlink_msg_system_time.hpp"
#include "./mavlink_msg_ping.hpp"
#include "./mavlink_msg_change_operator_control.hpp"
#include "./mavlink_msg_change_operator_control_ack.hpp"
#include "./mavlink_msg_auth_key.hpp"
#include "./mavlink_msg_set_mode.hpp"
#include "./mavlink_msg_param_request_read.hpp"
#include "./mavlink_msg_param_request_list.hpp"
#include "./mavlink_msg_param_value.hpp"
#include "./mavlink_msg_param_set.hpp"
#include "./mavlink_msg_gps_raw_int.hpp"
#include "./mavlink_msg_gps_status.hpp"
#include "./mavlink_msg_scaled_imu.hpp"
#include "./mavlink_msg_raw_imu.hpp"
#include "./mavlink_msg_raw_pressure.hpp"
#include "./mavlink_msg_scaled_pressure.hpp"
#include "./mavlink_msg_attitude.hpp"
#include "./mavlink_msg_attitude_quaternion.hpp"
#include "./mavlink_msg_local_position_ned.hpp"
#include "./mavlink_msg_global_position_int.hpp"
#include "./mavlink_msg_rc_channels_scaled.hpp"
#include "./mavlink_msg_rc_channels_raw.hpp"
#include "./mavlink_msg_servo_output_raw.hpp"
#include "./mavlink_msg_mission_request_partial_list.hpp"
#include "./mavlink_msg_mission_write_partial_list.hpp"
#include "./mavlink_msg_mission_item.hpp"
#include "./mavlink_msg_mission_request.hpp"
#include "./mavlink_msg_mission_set_current.hpp"
#include "./mavlink_msg_mission_current.hpp"
#include "./mavlink_msg_mission_request_list.hpp"
#include "./mavlink_msg_mission_count.hpp"
#include "./mavlink_msg_mission_clear_all.hpp"
#include "./mavlink_msg_mission_item_reached.hpp"
#include "./mavlink_msg_mission_ack.hpp"
#include "./mavlink_msg_set_gps_global_origin.hpp"
#include "./mavlink_msg_gps_global_origin.hpp"
#include "./mavlink_msg_param_map_rc.hpp"
#include "./mavlink_msg_mission_request_int.hpp"
#include "./mavlink_msg_safety_set_allowed_area.hpp"
#include "./mavlink_msg_safety_allowed_area.hpp"
#include "./mavlink_msg_attitude_quaternion_cov.hpp"
#include "./mavlink_msg_nav_controller_output.hpp"
#include "./mavlink_msg_global_position_int_cov.hpp"
#include "./mavlink_msg_local_position_ned_cov.hpp"
#include "./mavlink_msg_rc_channels.hpp"
#include "./mavlink_msg_request_data_stream.hpp"
#include "./mavlink_msg_data_stream.hpp"
#include "./mavlink_msg_manual_control.hpp"
#include "./mavlink_msg_rc_channels_override.hpp"
#include "./mavlink_msg_mission_item_int.hpp"
#include "./mavlink_msg_vfr_hud.hpp"
#include "./mavlink_msg_command_int.hpp"
#include "./mavlink_msg_command_long.hpp"
#include "./mavlink_msg_command_ack.hpp"
#include "./mavlink_msg_manual_setpoint.hpp"
#include "./mavlink_msg_set_attitude_target.hpp"
#include "./mavlink_msg_attitude_target.hpp"
#include "./mavlink_msg_set_position_target_local_ned.hpp"
#include "./mavlink_msg_position_target_local_ned.hpp"
#include "./mavlink_msg_set_position_target_global_int.hpp"
#include "./mavlink_msg_position_target_global_int.hpp"
#include "./mavlink_msg_local_position_ned_system_global_offset.hpp"
#include "./mavlink_msg_hil_state.hpp"
#include "./mavlink_msg_hil_controls.hpp"
#include "./mavlink_msg_hil_rc_inputs_raw.hpp"
#include "./mavlink_msg_hil_actuator_controls.hpp"
#include "./mavlink_msg_optical_flow.hpp"
#include "./mavlink_msg_global_vision_position_estimate.hpp"
#include "./mavlink_msg_vision_position_estimate.hpp"
#include "./mavlink_msg_vision_speed_estimate.hpp"
#include "./mavlink_msg_vicon_position_estimate.hpp"
#include "./mavlink_msg_highres_imu.hpp"
#include "./mavlink_msg_optical_flow_rad.hpp"
#include "./mavlink_msg_hil_sensor.hpp"
#include "./mavlink_msg_sim_state.hpp"
#include "./mavlink_msg_radio_status.hpp"
#include "./mavlink_msg_file_transfer_protocol.hpp"
#include "./mavlink_msg_timesync.hpp"
#include "./mavlink_msg_camera_trigger.hpp"
#include "./mavlink_msg_hil_gps.hpp"
#include "./mavlink_msg_hil_optical_flow.hpp"
#include "./mavlink_msg_hil_state_quaternion.hpp"
#include "./mavlink_msg_scaled_imu2.hpp"
#include "./mavlink_msg_log_request_list.hpp"
#include "./mavlink_msg_log_entry.hpp"
#include "./mavlink_msg_log_request_data.hpp"
#include "./mavlink_msg_log_data.hpp"
#include "./mavlink_msg_log_erase.hpp"
#include "./mavlink_msg_log_request_end.hpp"
#include "./mavlink_msg_gps_inject_data.hpp"
#include "./mavlink_msg_gps2_raw.hpp"
#include "./mavlink_msg_power_status.hpp"
#include "./mavlink_msg_serial_control.hpp"
#include "./mavlink_msg_gps_rtk.hpp"
#include "./mavlink_msg_gps2_rtk.hpp"
#include "./mavlink_msg_scaled_imu3.hpp"
#include "./mavlink_msg_data_transmission_handshake.hpp"
#include "./mavlink_msg_encapsulated_data.hpp"
#include "./mavlink_msg_distance_sensor.hpp"
#include "./mavlink_msg_terrain_request.hpp"
#include "./mavlink_msg_terrain_data.hpp"
#include "./mavlink_msg_terrain_check.hpp"
#include "./mavlink_msg_terrain_report.hpp"
#include "./mavlink_msg_scaled_pressure2.hpp"
#include "./mavlink_msg_att_pos_mocap.hpp"
#include "./mavlink_msg_set_actuator_control_target.hpp"
#include "./mavlink_msg_actuator_control_target.hpp"
#include "./mavlink_msg_altitude.hpp"
#include "./mavlink_msg_resource_request.hpp"
#include "./mavlink_msg_scaled_pressure3.hpp"
#include "./mavlink_msg_follow_target.hpp"
#include "./mavlink_msg_control_system_state.hpp"
#include "./mavlink_msg_battery_status.hpp"
#include "./mavlink_msg_autopilot_version.hpp"
#include "./mavlink_msg_landing_target.hpp"
#include "./mavlink_msg_fence_status.hpp"
#include "./mavlink_msg_mag_cal_report.hpp"
#include "./mavlink_msg_efi_status.hpp"
#include "./mavlink_msg_estimator_status.hpp"
#include "./mavlink_msg_wind_cov.hpp"
#include "./mavlink_msg_gps_input.hpp"
#include "./mavlink_msg_gps_rtcm_data.hpp"
#include "./mavlink_msg_high_latency.hpp"
#include "./mavlink_msg_high_latency2.hpp"
#include "./mavlink_msg_vibration.hpp"
#include "./mavlink_msg_home_position.hpp"
#include "./mavlink_msg_set_home_position.hpp"
#include "./mavlink_msg_message_interval.hpp"
#include "./mavlink_msg_extended_sys_state.hpp"
#include "./mavlink_msg_adsb_vehicle.hpp"
#include "./mavlink_msg_collision.hpp"
#include "./mavlink_msg_v2_extension.hpp"
#include "./mavlink_msg_memory_vect.hpp"
#include "./mavlink_msg_debug_vect.hpp"
#include "./mavlink_msg_named_value_float.hpp"
#include "./mavlink_msg_named_value_int.hpp"
#include "./mavlink_msg_statustext.hpp"
#include "./mavlink_msg_debug.hpp"
#include "./mavlink_msg_setup_signing.hpp"
#include "./mavlink_msg_button_change.hpp"
#include "./mavlink_msg_play_tune.hpp"
#include "./mavlink_msg_camera_information.hpp"
#include "./mavlink_msg_camera_settings.hpp"
#include "./mavlink_msg_storage_information.hpp"
#include "./mavlink_msg_camera_capture_status.hpp"
#include "./mavlink_msg_camera_image_captured.hpp"
#include "./mavlink_msg_flight_information.hpp"
#include "./mavlink_msg_mount_orientation.hpp"
#include "./mavlink_msg_logging_data.hpp"
#include "./mavlink_msg_logging_data_acked.hpp"
#include "./mavlink_msg_logging_ack.hpp"
#include "./mavlink_msg_video_stream_information.hpp"
#include "./mavlink_msg_video_stream_status.hpp"
#include "./mavlink_msg_gimbal_device_information.hpp"
#include "./mavlink_msg_autopilot_state_for_gimbal_device.hpp"
#include "./mavlink_msg_wifi_config_ap.hpp"
#include "./mavlink_msg_ais_vessel.hpp"
#include "./mavlink_msg_uavcan_node_status.hpp"
#include "./mavlink_msg_uavcan_node_info.hpp"
#include "./mavlink_msg_param_ext_request_read.hpp"
#include "./mavlink_msg_param_ext_request_list.hpp"
#include "./mavlink_msg_param_ext_value.hpp"
#include "./mavlink_msg_param_ext_set.hpp"
#include "./mavlink_msg_param_ext_ack.hpp"
#include "./mavlink_msg_obstacle_distance.hpp"
#include "./mavlink_msg_odometry.hpp"
#include "./mavlink_msg_isbd_link_status.hpp"
#include "./mavlink_msg_raw_rpm.hpp"
#include "./mavlink_msg_utm_global_position.hpp"
#include "./mavlink_msg_debug_float_array.hpp"
#include "./mavlink_msg_smart_battery_info.hpp"
#include "./mavlink_msg_generator_status.hpp"
#include "./mavlink_msg_actuator_output_status.hpp"
#include "./mavlink_msg_tunnel.hpp"
#include "./mavlink_msg_can_frame.hpp"
#include "./mavlink_msg_canfd_frame.hpp"
#include "./mavlink_msg_can_filter_modify.hpp"
#include "./mavlink_msg_wheel_distance.hpp"
#include "./mavlink_msg_winch_status.hpp"
#include "./mavlink_msg_hygrometer_sensor.hpp"

// base include
#include "../minimal/minimal.hpp"
