/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of QGroundControl/APM Planner.

QGROUNDCONTROL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

======================================================================*/

#include "MAVFTPProtocol.h"

#include <cstring>

namespace {

void setError(QString* errorString, const QString& error)
{
    if (errorString) {
        *errorString = error;
    }
}

void writeUInt16(uint8_t* bytes, quint16 value)
{
    bytes[0] = static_cast<uint8_t>(value & 0xff);
    bytes[1] = static_cast<uint8_t>((value >> 8) & 0xff);
}

void writeUInt32(uint8_t* bytes, quint32 value)
{
    bytes[0] = static_cast<uint8_t>(value & 0xff);
    bytes[1] = static_cast<uint8_t>((value >> 8) & 0xff);
    bytes[2] = static_cast<uint8_t>((value >> 16) & 0xff);
    bytes[3] = static_cast<uint8_t>((value >> 24) & 0xff);
}

quint16 readUInt16(const uint8_t* bytes)
{
    return static_cast<quint16>(bytes[0]) |
            (static_cast<quint16>(bytes[1]) << 8);
}

quint32 readUInt32(const uint8_t* bytes)
{
    return static_cast<quint32>(bytes[0]) |
            (static_cast<quint32>(bytes[1]) << 8) |
            (static_cast<quint32>(bytes[2]) << 16) |
            (static_cast<quint32>(bytes[3]) << 24);
}

} // namespace

namespace MAVFTPProtocol
{

bool encodePayload(quint16 sequence, uint8_t session, uint8_t opcode, uint8_t size, quint32 offset,
                   const QByteArray& data, uint8_t* payload, int payloadLength, QString* errorString)
{
    if (!payload || payloadLength != PayloadLength) {
        setError(errorString, QStringLiteral("invalid MAVFTP payload buffer"));
        return false;
    }
    if (data.size() > MaxDataLength || data.size() > size) {
        setError(errorString, QStringLiteral("MAVFTP payload data is too large"));
        return false;
    }

    memset(payload, 0, payloadLength);
    writeUInt16(&payload[0], sequence);
    payload[2] = session;
    payload[3] = opcode;
    payload[4] = size;
    payload[5] = 0;
    payload[6] = 0;
    payload[7] = 0;
    writeUInt32(&payload[8], offset);

    if (!data.isEmpty()) {
        memcpy(&payload[HeaderLength], data.constData(), data.size());
    }
    return true;
}

bool decodePayload(const uint8_t* payload, int payloadLength, Packet* packet, QString* errorString)
{
    if (!payload || payloadLength != PayloadLength || !packet) {
        setError(errorString, QStringLiteral("invalid MAVFTP payload"));
        return false;
    }

    Packet decoded;
    decoded.sequence = readUInt16(&payload[0]);
    decoded.session = payload[2];
    decoded.opcode = payload[3];
    decoded.size = payload[4];
    decoded.requestOpcode = payload[5];
    decoded.burstComplete = payload[6];
    decoded.offset = readUInt32(&payload[8]);
    if (decoded.size > MaxDataLength) {
        setError(errorString, QStringLiteral("MAVFTP response data is too large"));
        return false;
    }

    decoded.data = QByteArray(reinterpret_cast<const char*>(&payload[HeaderLength]), decoded.size);
    *packet = decoded;
    return true;
}

quint16 expectedResponseSequence(quint16 requestSequence)
{
    return static_cast<quint16>(requestSequence + 1);
}

uint8_t responseErrorCode(const Packet& packet)
{
    if (packet.data.isEmpty()) {
        return static_cast<uint8_t>(ErrFail);
    }
    return static_cast<uint8_t>(static_cast<uchar>(packet.data.at(0)));
}

QString errorString(uint8_t errorCode)
{
    switch (errorCode) {
    case ErrNone:
        return QStringLiteral("no error");
    case ErrFail:
        return QStringLiteral("generic failure");
    case ErrFailErrno:
        return QStringLiteral("system error");
    case ErrInvalidDataSize:
        return QStringLiteral("invalid data size");
    case ErrInvalidSession:
        return QStringLiteral("invalid session");
    case ErrNoSessionsAvailable:
        return QStringLiteral("no sessions available");
    case ErrEndOfFile:
        return QStringLiteral("end of file");
    case ErrUnknownCommand:
        return QStringLiteral("unknown command");
    case ErrFileExists:
        return QStringLiteral("file exists");
    case ErrFileProtected:
        return QStringLiteral("file protected");
    case ErrFileNotFound:
        return QStringLiteral("file not found");
    default:
        return QStringLiteral("unknown error %1").arg(errorCode);
    }
}

} // namespace MAVFTPProtocol
