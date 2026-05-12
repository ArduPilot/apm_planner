/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of QGroundControl/APM Planner.

QGROUNDCONTROL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

======================================================================*/

#pragma once

#include <QByteArray>
#include <QString>

#include "QGCMAVLink.h"

#include <stdint.h>

namespace MAVFTPProtocol
{

enum Opcode
{
    OpTerminateSession = 1,
    OpOpenFileRO = 4,
    OpReadFile = 5,
    OpCreateFile = 6,
    OpWriteFile = 7,
    OpAck = 128,
    OpNack = 129
};

enum ErrorCode
{
    ErrNone = 0,
    ErrFail = 1,
    ErrFailErrno = 2,
    ErrInvalidDataSize = 3,
    ErrInvalidSession = 4,
    ErrNoSessionsAvailable = 5,
    ErrEndOfFile = 6,
    ErrUnknownCommand = 7,
    ErrFileExists = 8,
    ErrFileProtected = 9,
    ErrFileNotFound = 10
};

enum
{
    HeaderLength = 12,
    PayloadLength = MAVLINK_MSG_FILE_TRANSFER_PROTOCOL_FIELD_PAYLOAD_LEN,
    MaxDataLength = MAVLINK_MSG_FILE_TRANSFER_PROTOCOL_FIELD_PAYLOAD_LEN - HeaderLength
};

struct Packet
{
    quint16 sequence;
    uint8_t session;
    uint8_t opcode;
    uint8_t size;
    uint8_t requestOpcode;
    uint8_t burstComplete;
    quint32 offset;
    QByteArray data;
};

bool encodePayload(quint16 sequence, uint8_t session, uint8_t opcode, uint8_t size, quint32 offset,
                   const QByteArray& data, uint8_t* payload, int payloadLength, QString* errorString);
bool decodePayload(const uint8_t* payload, int payloadLength, Packet* packet, QString* errorString);

quint16 expectedResponseSequence(quint16 requestSequence);
uint8_t responseErrorCode(const Packet& packet);
QString errorString(uint8_t errorCode);

} // namespace MAVFTPProtocol
