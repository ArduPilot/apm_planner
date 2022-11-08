/** @file
 *  @brief MAVLink comm protocol built from ardupilotmega.xml
 *  @see http://mavlink.org
 */
#pragma once
#ifndef MAVLINK_H
#define MAVLINK_H

#define MAVLINK_PRIMARY_XML_HASH 8243252250405391882

#ifndef MAVLINK_STX
#define MAVLINK_STX 253
#endif

#ifndef MAVLINK_ENDIAN
#define MAVLINK_ENDIAN MAVLINK_LITTLE_ENDIAN
#endif

#ifndef MAVLINK_ALIGNED_FIELDS
#define MAVLINK_ALIGNED_FIELDS 1
#endif

#ifndef MAVLINK_CRC_EXTRA
#define MAVLINK_CRC_EXTRA 1
#endif

#ifndef MAVLINK_COMMAND_24BIT
#define MAVLINK_COMMAND_24BIT 1
#endif

// gcc version 9 brings a lot of new warnings. We supress them here for the mavlink
// library as it is not "our" code
#if defined(__GNUC__) && (__GNUC__ > 8)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Waddress-of-packed-member" // Suppress lots of warnings
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    #pragma GCC diagnostic ignored "-Wuninitialized"
#endif

#include "version.h"
#include "ardupilotmega.h"

#if defined(__GNUC__) && (__GNUC__ > 8)
    #pragma GCC diagnostic pop
#endif

#endif // MAVLINK_H
