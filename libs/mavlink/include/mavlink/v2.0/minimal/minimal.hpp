/** @file
 *	@brief MAVLink comm protocol generated from minimal.xml
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
namespace minimal {

/**
 * Array of msg_entry needed for @p mavlink_parse_char() (through @p mavlink_get_msg_entry())
 */
constexpr std::array<mavlink_msg_entry_t, 1> MESSAGE_ENTRIES {{ {0, 50, 9, 9, 0, 0, 0} }};

//! MAVLINK VERSION
constexpr auto MAVLINK_VERSION = 3;


// ENUM DEFINITIONS


/** @brief Micro air vehicle / autopilot classes. This identifies the individual model. */
enum class MAV_AUTOPILOT : uint8_t
{
    GENERIC=0, /* Generic autopilot, full support for everything | */
    RESERVED=1, /* Reserved for future use. | */
    SLUGS=2, /* SLUGS autopilot, http://slugsuav.soe.ucsc.edu | */
    ARDUPILOTMEGA=3, /* ArduPilot - Plane/Copter/Rover/Sub/Tracker, https://ardupilot.org | */
    OPENPILOT=4, /* OpenPilot, http://openpilot.org | */
    GENERIC_WAYPOINTS_ONLY=5, /* Generic autopilot only supporting simple waypoints | */
    GENERIC_WAYPOINTS_AND_SIMPLE_NAVIGATION_ONLY=6, /* Generic autopilot supporting waypoints and other simple navigation commands | */
    GENERIC_MISSION_FULL=7, /* Generic autopilot supporting the full mission command set | */
    INVALID=8, /* No valid autopilot, e.g. a GCS or other MAVLink component | */
    PPZ=9, /* PPZ UAV - http://nongnu.org/paparazzi | */
    UDB=10, /* UAV Dev Board | */
    FP=11, /* FlexiPilot | */
    PX4=12, /* PX4 Autopilot - http://px4.io/ | */
    SMACCMPILOT=13, /* SMACCMPilot - http://smaccmpilot.org | */
    AUTOQUAD=14, /* AutoQuad -- http://autoquad.org | */
    ARMAZILA=15, /* Armazila -- http://armazila.com | */
    AEROB=16, /* Aerob -- http://aerob.ru | */
    ASLUAV=17, /* ASLUAV autopilot -- http://www.asl.ethz.ch | */
    SMARTAP=18, /* SmartAP Autopilot - http://sky-drones.com | */
    AIRRAILS=19, /* AirRails - http://uaventure.com | */
};

//! MAV_AUTOPILOT ENUM_END
constexpr auto MAV_AUTOPILOT_ENUM_END = 20;

/** @brief MAVLINK component type reported in HEARTBEAT message. Flight controllers must report the type of the vehicle on which they are mounted (e.g. MAV_TYPE_OCTOROTOR). All other components must report a value appropriate for their type (e.g. a camera must use MAV_TYPE_CAMERA). */
enum class MAV_TYPE : uint8_t
{
    GENERIC=0, /* Generic micro air vehicle | */
    FIXED_WING=1, /* Fixed wing aircraft. | */
    QUADROTOR=2, /* Quadrotor | */
    COAXIAL=3, /* Coaxial helicopter | */
    HELICOPTER=4, /* Normal helicopter with tail rotor. | */
    ANTENNA_TRACKER=5, /* Ground installation | */
    GCS=6, /* Operator control unit / ground control station | */
    AIRSHIP=7, /* Airship, controlled | */
    FREE_BALLOON=8, /* Free balloon, uncontrolled | */
    ROCKET=9, /* Rocket | */
    GROUND_ROVER=10, /* Ground rover | */
    SURFACE_BOAT=11, /* Surface vessel, boat, ship | */
    SUBMARINE=12, /* Submarine | */
    HEXAROTOR=13, /* Hexarotor | */
    OCTOROTOR=14, /* Octorotor | */
    TRICOPTER=15, /* Tricopter | */
    FLAPPING_WING=16, /* Flapping wing | */
    KITE=17, /* Kite | */
    ONBOARD_CONTROLLER=18, /* Onboard companion controller | */
    VTOL_DUOROTOR=19, /* Two-rotor VTOL using control surfaces in vertical operation in addition. Tailsitter. | */
    VTOL_QUADROTOR=20, /* Quad-rotor VTOL using a V-shaped quad config in vertical operation. Tailsitter. | */
    VTOL_TILTROTOR=21, /* Tiltrotor VTOL | */
    VTOL_RESERVED2=22, /* VTOL reserved 2 | */
    VTOL_RESERVED3=23, /* VTOL reserved 3 | */
    VTOL_RESERVED4=24, /* VTOL reserved 4 | */
    VTOL_RESERVED5=25, /* VTOL reserved 5 | */
    GIMBAL=26, /* Gimbal | */
    ADSB=27, /* ADSB system | */
    PARAFOIL=28, /* Steerable, nonrigid airfoil | */
    DODECAROTOR=29, /* Dodecarotor | */
    CAMERA=30, /* Camera | */
    CHARGING_STATION=31, /* Charging station | */
    FLARM=32, /* FLARM collision avoidance system | */
    SERVO=33, /* Servo | */
    ODID=34, /* Open Drone ID. See https://mavlink.io/en/services/opendroneid.html. | */
    DECAROTOR=35, /* Decarotor | */
};

//! MAV_TYPE ENUM_END
constexpr auto MAV_TYPE_ENUM_END = 36;

/** @brief These flags encode the MAV mode. */
enum class MAV_MODE_FLAG : uint8_t
{
    CUSTOM_MODE_ENABLED=1, /* 0b00000001 Reserved for future use. | */
    TEST_ENABLED=2, /* 0b00000010 system has a test mode enabled. This flag is intended for temporary system tests and should not be used for stable implementations. | */
    AUTO_ENABLED=4, /* 0b00000100 autonomous mode enabled, system finds its own goal positions. Guided flag can be set or not, depends on the actual implementation. | */
    GUIDED_ENABLED=8, /* 0b00001000 guided mode enabled, system flies waypoints / mission items. | */
    STABILIZE_ENABLED=16, /* 0b00010000 system stabilizes electronically its attitude (and optionally position). It needs however further control inputs to move around. | */
    HIL_ENABLED=32, /* 0b00100000 hardware in the loop simulation. All motors / actuators are blocked, but internal software is full operational. | */
    MANUAL_INPUT_ENABLED=64, /* 0b01000000 remote control input is enabled. | */
    SAFETY_ARMED=128, /* 0b10000000 MAV safety set to armed. Motors are enabled / running / can start. Ready to fly. Additional note: this flag is to be ignore when sent in the command MAV_CMD_DO_SET_MODE and MAV_CMD_COMPONENT_ARM_DISARM shall be used instead. The flag can still be used to report the armed state. | */
};

//! MAV_MODE_FLAG ENUM_END
constexpr auto MAV_MODE_FLAG_ENUM_END = 129;

/** @brief These values encode the bit positions of the decode position. These values can be used to read the value of a flag bit by combining the base_mode variable with AND with the flag position value. The result will be either 0 or 1, depending on if the flag is set or not. */
enum class MAV_MODE_FLAG_DECODE_POSITION
{
    CUSTOM_MODE=1, /* Eighth bit: 00000001 | */
    TEST=2, /* Seventh bit: 00000010 | */
    AUTO=4, /* Sixth bit:   00000100 | */
    GUIDED=8, /* Fifth bit:  00001000 | */
    STABILIZE=16, /* Fourth bit: 00010000 | */
    HIL=32, /* Third bit:  00100000 | */
    MANUAL=64, /* Second bit: 01000000 | */
    SAFETY=128, /* First bit:  10000000 | */
};

//! MAV_MODE_FLAG_DECODE_POSITION ENUM_END
constexpr auto MAV_MODE_FLAG_DECODE_POSITION_ENUM_END = 129;

/** @brief  */
enum class MAV_STATE : uint8_t
{
    UNINIT=0, /* Uninitialized system, state is unknown. | */
    BOOT=1, /* System is booting up. | */
    CALIBRATING=2, /* System is calibrating and not flight-ready. | */
    STANDBY=3, /* System is grounded and on standby. It can be launched any time. | */
    ACTIVE=4, /* System is active and might be already airborne. Motors are engaged. | */
    CRITICAL=5, /* System is in a non-normal flight mode. It can however still navigate. | */
    EMERGENCY=6, /* System is in a non-normal flight mode. It lost control over parts or over the whole airframe. It is in mayday and going down. | */
    POWEROFF=7, /* System just initialized its power-down sequence, will shut down now. | */
    FLIGHT_TERMINATION=8, /* System is terminating itself. | */
};

//! MAV_STATE ENUM_END
constexpr auto MAV_STATE_ENUM_END = 9;

/** @brief Component ids (values) for the different types and instances of onboard hardware/software that might make up a MAVLink system (autopilot, cameras, servos, GPS systems, avoidance systems etc.).
      Components must use the appropriate ID in their source address when sending messages. Components can also use IDs to determine if they are the intended recipient of an incoming message. The MAV_COMP_ID_ALL value is used to indicate messages that must be processed by all components.
      When creating new entries, components that can have multiple instances (e.g. cameras, servos etc.) should be allocated sequential values. An appropriate number of values should be left free after these components to allow the number of instances to be expanded. */
enum class MAV_COMPONENT
{
    COMP_ID_ALL=0, /* Target id (target_component) used to broadcast messages to all components of the receiving system. Components should attempt to process messages with this component ID and forward to components on any other interfaces. Note: This is not a valid *source* component id for a message. | */
    COMP_ID_AUTOPILOT1=1, /* System flight controller component ("autopilot"). Only one autopilot is expected in a particular system. | */
    COMP_ID_USER1=25, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER2=26, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER3=27, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER4=28, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER5=29, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER6=30, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER7=31, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER8=32, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER9=33, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER10=34, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER11=35, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER12=36, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER13=37, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER14=38, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER15=39, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER16=40, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER17=41, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER18=42, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER19=43, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER20=44, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER21=45, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER22=46, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER23=47, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER24=48, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER25=49, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER26=50, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER27=51, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER28=52, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER29=53, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER30=54, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER31=55, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER32=56, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER33=57, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER34=58, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER35=59, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER36=60, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER37=61, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER38=62, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER39=63, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER40=64, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER41=65, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER42=66, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER43=67, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_TELEMETRY_RADIO=68, /* Telemetry radio (e.g. SiK radio, or other component that emits RADIO_STATUS messages). | */
    COMP_ID_USER45=69, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER46=70, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER47=71, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER48=72, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER49=73, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER50=74, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER51=75, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER52=76, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER53=77, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER54=78, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER55=79, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER56=80, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER57=81, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER58=82, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER59=83, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER60=84, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER61=85, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER62=86, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER63=87, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER64=88, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER65=89, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER66=90, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER67=91, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER68=92, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER69=93, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER70=94, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER71=95, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER72=96, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER73=97, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER74=98, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_USER75=99, /* Id for a component on privately managed MAVLink network. Can be used for any purpose but may not be published by components outside of the private network. | */
    COMP_ID_CAMERA=100, /* Camera #1. | */
    COMP_ID_CAMERA2=101, /* Camera #2. | */
    COMP_ID_CAMERA3=102, /* Camera #3. | */
    COMP_ID_CAMERA4=103, /* Camera #4. | */
    COMP_ID_CAMERA5=104, /* Camera #5. | */
    COMP_ID_CAMERA6=105, /* Camera #6. | */
    COMP_ID_SERVO1=140, /* Servo #1. | */
    COMP_ID_SERVO2=141, /* Servo #2. | */
    COMP_ID_SERVO3=142, /* Servo #3. | */
    COMP_ID_SERVO4=143, /* Servo #4. | */
    COMP_ID_SERVO5=144, /* Servo #5. | */
    COMP_ID_SERVO6=145, /* Servo #6. | */
    COMP_ID_SERVO7=146, /* Servo #7. | */
    COMP_ID_SERVO8=147, /* Servo #8. | */
    COMP_ID_SERVO9=148, /* Servo #9. | */
    COMP_ID_SERVO10=149, /* Servo #10. | */
    COMP_ID_SERVO11=150, /* Servo #11. | */
    COMP_ID_SERVO12=151, /* Servo #12. | */
    COMP_ID_SERVO13=152, /* Servo #13. | */
    COMP_ID_SERVO14=153, /* Servo #14. | */
    COMP_ID_GIMBAL=154, /* Gimbal #1. | */
    COMP_ID_LOG=155, /* Logging component. | */
    COMP_ID_ADSB=156, /* Automatic Dependent Surveillance-Broadcast (ADS-B) component. | */
    COMP_ID_OSD=157, /* On Screen Display (OSD) devices for video links. | */
    COMP_ID_PERIPHERAL=158, /* Generic autopilot peripheral component ID. Meant for devices that do not implement the parameter microservice. | */
    COMP_ID_QX1_GIMBAL=159, /* Gimbal ID for QX1. | */
    COMP_ID_FLARM=160, /* FLARM collision alert component. | */
    COMP_ID_GIMBAL2=171, /* Gimbal #2. | */
    COMP_ID_GIMBAL3=172, /* Gimbal #3. | */
    COMP_ID_GIMBAL4=173, /* Gimbal #4 | */
    COMP_ID_GIMBAL5=174, /* Gimbal #5. | */
    COMP_ID_GIMBAL6=175, /* Gimbal #6. | */
    COMP_ID_MAVCAN=189, /* CAN over MAVLink client. | */
    COMP_ID_MISSIONPLANNER=190, /* Component that can generate/supply a mission flight plan (e.g. GCS or developer API). | */
    COMP_ID_ONBOARD_COMPUTER=191, /* Component that lives on the onboard computer (companion computer) and has some generic functionalities, such as settings system parameters and monitoring the status of some processes that don't directly speak mavlink and so on. | */
    COMP_ID_PATHPLANNER=195, /* Component that finds an optimal path between points based on a certain constraint (e.g. minimum snap, shortest path, cost, etc.). | */
    COMP_ID_OBSTACLE_AVOIDANCE=196, /* Component that plans a collision free path between two points. | */
    COMP_ID_VISUAL_INERTIAL_ODOMETRY=197, /* Component that provides position estimates using VIO techniques. | */
    COMP_ID_PAIRING_MANAGER=198, /* Component that manages pairing of vehicle and GCS. | */
    COMP_ID_IMU=200, /* Inertial Measurement Unit (IMU) #1. | */
    COMP_ID_IMU_2=201, /* Inertial Measurement Unit (IMU) #2. | */
    COMP_ID_IMU_3=202, /* Inertial Measurement Unit (IMU) #3. | */
    COMP_ID_GPS=220, /* GPS #1. | */
    COMP_ID_GPS2=221, /* GPS #2. | */
    COMP_ID_ODID_TXRX_1=236, /* Open Drone ID transmitter/receiver (Bluetooth/WiFi/Internet). | */
    COMP_ID_ODID_TXRX_2=237, /* Open Drone ID transmitter/receiver (Bluetooth/WiFi/Internet). | */
    COMP_ID_ODID_TXRX_3=238, /* Open Drone ID transmitter/receiver (Bluetooth/WiFi/Internet). | */
    COMP_ID_UDP_BRIDGE=240, /* Component to bridge MAVLink to UDP (i.e. from a UART). | */
    COMP_ID_UART_BRIDGE=241, /* Component to bridge to UART (i.e. from UDP). | */
    COMP_ID_TUNNEL_NODE=242, /* Component handling TUNNEL messages (e.g. vendor specific GUI of a component). | */
    COMP_ID_SYSTEM_CONTROL=250, /* Component for handling system messages (e.g. to ARM, takeoff, etc.). | */
};

//! MAV_COMPONENT ENUM_END
constexpr auto MAV_COMPONENT_ENUM_END = 251;


} // namespace minimal
} // namespace mavlink

// MESSAGE DEFINITIONS
#include "./mavlink_msg_heartbeat.hpp"

// base include

