/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of QGroundControl/APM Planner.

QGROUNDCONTROL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

======================================================================*/

#include "MAVFTPManager.h"

#include "LinkInterface.h"
#include "UAS.h"
#include "logging.h"

#include <QList>
#include <QtGlobal>

#include <cstring>

MAVFTPManager::MAVFTPManager(UAS* uas) :
    QObject(uas),
    _uas(uas),
    _link(nullptr),
    _state(Idle),
    _targetComponent(MAV_COMP_ID_PRIMARY),
    _session(0),
    _sequence(0),
    _lastSequence(0),
    _offset(0),
    _lastOpcode(0),
    _lastSize(0),
    _lastOffset(0),
    _retryCount(0)
{
    _timer.setInterval(kTimeoutMs);
    _timer.setSingleShot(true);
    connect(&_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

bool MAVFTPManager::isBusy() const
{
    return _state != Idle;
}

bool MAVFTPManager::downloadParameterFile()
{
    return downloadFile(QStringLiteral("@PARAM/param.pck?withdefaults=1"), MAV_COMP_ID_PRIMARY);
}

bool MAVFTPManager::downloadFile(const QString& remotePath, uint8_t targetComponent)
{
    if (_state != Idle) {
        return false;
    }

    if (remotePath.isEmpty()) {
        return false;
    }

    _link = activeLink();
    if (!_link) {
        return false;
    }

    const QByteArray path = remotePath.toLatin1();
    if (path.size() > kMaxDataLength) {
        return false;
    }

    _targetComponent = targetComponent;
    _session = 0;
    _offset = 0;
    _remotePath = remotePath;
    _download.clear();
    _upload.clear();
    _state = Opening;

    QLOG_DEBUG() << "Starting MAVFTP download of" << remotePath;
    if (!sendRequest(OpOpenFileRO, static_cast<uint8_t>(path.size()), 0, path)) {
        reset();
        return false;
    }

    return true;
}

bool MAVFTPManager::uploadFile(const QString& remotePath, const QByteArray& data, uint8_t targetComponent)
{
    if (_state != Idle) {
        return false;
    }

    if (remotePath.isEmpty()) {
        return false;
    }

    _link = activeLink();
    if (!_link) {
        return false;
    }

    const QByteArray path = remotePath.toLatin1();
    if (path.size() > kMaxDataLength) {
        return false;
    }

    _targetComponent = targetComponent;
    _session = 0;
    _offset = 0;
    _remotePath = remotePath;
    _download.clear();
    _upload = data;
    _state = Creating;

    QLOG_DEBUG() << "Starting MAVFTP upload of" << remotePath << "with" << data.size() << "bytes";
    if (!sendRequest(OpCreateFile, static_cast<uint8_t>(path.size()), 0, path)) {
        reset();
        return false;
    }

    return true;
}

bool MAVFTPManager::handleMessage(LinkInterface* link, const mavlink_message_t& message)
{
    if (_state == Idle || link != _link || message.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
        return false;
    }
    if (message.sysid != _uas->getUASID() || message.compid != _targetComponent) {
        return false;
    }

    Response response;
    if (!decodeResponse(message, &response)) {
        return false;
    }

    const uint8_t expectedRequestOpcode =
            _state == Opening ? OpOpenFileRO :
            _state == Reading ? OpReadFile :
            _state == Creating ? OpCreateFile :
            _state == Writing ? OpWriteFile : 0;
    if (response.requestOpcode != expectedRequestOpcode) {
        return false;
    }
    if (response.sequence != _lastSequence) {
        return false;
    }
    if ((_state == Reading || _state == Writing) && response.session != _session) {
        return false;
    }
    if (_state == Reading && response.opcode == OpAck && response.offset < _offset) {
        return false;
    }
    if (_state == Writing && response.opcode == OpAck && response.offset != _offset) {
        return false;
    }

    _timer.stop();
    _retryCount = 0;

    switch (_state) {
    case Opening:
        handleOpenResponse(response);
        break;
    case Reading:
        handleReadResponse(response);
        break;
    case Creating:
        handleCreateResponse(response);
        break;
    case Writing:
        handleWriteResponse(response);
        break;
    case Idle:
        break;
    }

    return true;
}

void MAVFTPManager::cancel()
{
    if (_state == Idle) {
        return;
    }

    sendRequest(OpTerminateSession, 0, 0, QByteArray());
    finish(QStringLiteral("MAVFTP transfer cancelled"));
}

void MAVFTPManager::timeout()
{
    if (_state == Idle) {
        return;
    }

    if (_retryCount < kMaxRetries) {
        _retryCount++;
        if (!sendRequest(_lastOpcode, _lastSize, _lastOffset, _lastData, true)) {
            finish(QStringLiteral("MAVFTP request could not be sent"));
        }
        return;
    }

    finish(QStringLiteral("MAVFTP request timed out"));
}

LinkInterface* MAVFTPManager::activeLink() const
{
    if (!_uas || !_uas->getLinks()) {
        return nullptr;
    }

    foreach (LinkInterface* link, *_uas->getLinks()) {
        if (link && link->isConnected()) {
            return link;
        }
    }

    return nullptr;
}

bool MAVFTPManager::sendRequest(uint8_t opcode, uint8_t size, quint32 offset, const QByteArray& data, bool retrying)
{
    if (!_uas || !_link || !_link->isConnected()) {
        return false;
    }

    uint8_t payload[kPayloadLength];
    memset(payload, 0, sizeof(payload));

    const quint16 requestSequence = retrying ? _lastSequence : _sequence++;
    writeUInt16(&payload[0], requestSequence);
    payload[2] = _session;
    payload[3] = opcode;
    payload[4] = size;
    payload[5] = 0;
    payload[6] = 0;
    payload[7] = 0;
    writeUInt32(&payload[8], offset);

    if (!data.isEmpty()) {
        if (data.size() > kMaxDataLength) {
            return false;
        }
        memcpy(&payload[kHeaderLength], data.constData(), data.size());
    }

    mavlink_message_t message;
    UASInterface* uasInterface = static_cast<UASInterface*>(_uas);
    mavlink_msg_file_transfer_protocol_pack(static_cast<uint8_t>(uasInterface->getSystemId()),
                                            static_cast<uint8_t>(uasInterface->getComponentId()),
                                            &message,
                                            0,
                                            static_cast<uint8_t>(_uas->getUASID()),
                                            _targetComponent,
                                            payload);
    _uas->sendMessage(_link, message);

    if (!retrying) {
        _lastSequence = requestSequence;
        _lastOpcode = opcode;
        _lastSize = size;
        _lastOffset = offset;
        _lastData = data;
        _retryCount = 0;
    }

    _timer.start();
    return true;
}

bool MAVFTPManager::decodeResponse(const mavlink_message_t& message, Response* response) const
{
    mavlink_file_transfer_protocol_t packet;
    mavlink_msg_file_transfer_protocol_decode(&message, &packet);

    UASInterface* uasInterface = static_cast<UASInterface*>(_uas);
    const uint8_t gcsSystemId = static_cast<uint8_t>(uasInterface->getSystemId());
    const uint8_t gcsComponentId = static_cast<uint8_t>(uasInterface->getComponentId());
    if (packet.target_system != 0 && packet.target_system != gcsSystemId) {
        return false;
    }
    if (packet.target_component != 0 && packet.target_component != gcsComponentId) {
        return false;
    }

    response->sequence = readUInt16(&packet.payload[0]);
    response->session = packet.payload[2];
    response->opcode = packet.payload[3];
    response->size = packet.payload[4];
    response->requestOpcode = packet.payload[5];
    response->burstComplete = packet.payload[6];
    response->offset = readUInt32(&packet.payload[8]);
    if (response->size > kMaxDataLength) {
        return false;
    }

    response->data = QByteArray(reinterpret_cast<const char*>(&packet.payload[kHeaderLength]), response->size);
    return true;
}

void MAVFTPManager::handleOpenResponse(const Response& response)
{
    if (response.requestOpcode != OpOpenFileRO) {
        return;
    }

    if (response.opcode != OpAck) {
        const uint8_t errorCode = responseErrorCode(response);
        finish(QStringLiteral("MAVFTP open failed: %1").arg(errorString(errorCode)));
        return;
    }

    _session = response.session;
    _offset = 0;
    _state = Reading;
    if (!sendReadRequest()) {
        finish(QStringLiteral("MAVFTP read request could not be sent"));
    }
}

void MAVFTPManager::handleReadResponse(const Response& response)
{
    if (response.requestOpcode != OpReadFile) {
        return;
    }

    if (response.opcode == OpNack) {
        const uint8_t errorCode = responseErrorCode(response);
        if (errorCode == ErrEndOfFile || errorCode == ErrNone) {
            sendRequest(OpTerminateSession, 0, 0, QByteArray());
            finish(QString());
            return;
        }

        finish(QStringLiteral("MAVFTP read failed: %1").arg(errorString(errorCode)));
        return;
    }

    if (response.opcode != OpAck || response.offset != _offset) {
        finish(QStringLiteral("MAVFTP received an unexpected read response"));
        return;
    }

    _download.append(response.data.constData(), response.data.size());
    _offset += response.data.size();

    if (response.size == 0 || response.size < _lastSize) {
        sendRequest(OpTerminateSession, 0, 0, QByteArray());
        finish(QString());
        return;
    }

    if (!sendReadRequest()) {
        finish(QStringLiteral("MAVFTP read request could not be sent"));
    }
}

void MAVFTPManager::handleCreateResponse(const Response& response)
{
    if (response.requestOpcode != OpCreateFile) {
        return;
    }

    if (response.opcode != OpAck) {
        const uint8_t errorCode = responseErrorCode(response);
        finish(QStringLiteral("MAVFTP create failed: %1").arg(errorString(errorCode)));
        return;
    }

    _session = response.session;
    _offset = 0;
    _state = Writing;

    if (_upload.isEmpty()) {
        sendRequest(OpTerminateSession, 0, 0, QByteArray());
        finish(QString());
        return;
    }

    if (!sendWriteRequest()) {
        finish(QStringLiteral("MAVFTP write request could not be sent"));
    }
}

void MAVFTPManager::handleWriteResponse(const Response& response)
{
    if (response.requestOpcode != OpWriteFile) {
        return;
    }

    if (response.opcode == OpNack) {
        const uint8_t errorCode = responseErrorCode(response);
        finish(QStringLiteral("MAVFTP write failed: %1").arg(errorString(errorCode)));
        return;
    }

    if (response.opcode != OpAck || response.offset != _offset) {
        finish(QStringLiteral("MAVFTP received an unexpected write response"));
        return;
    }

    _offset += _lastSize;
    if (_offset >= static_cast<quint32>(_upload.size())) {
        sendRequest(OpTerminateSession, 0, 0, QByteArray());
        finish(QString());
        return;
    }

    if (!sendWriteRequest()) {
        finish(QStringLiteral("MAVFTP write request could not be sent"));
    }
}

bool MAVFTPManager::sendReadRequest()
{
    return sendRequest(OpReadFile, static_cast<uint8_t>(kMaxDataLength), _offset, QByteArray());
}

bool MAVFTPManager::sendWriteRequest()
{
    const int remaining = _upload.size() - static_cast<int>(_offset);
    const int writeSize = qMin(static_cast<int>(kMaxDataLength), remaining);
    if (writeSize <= 0) {
        return false;
    }

    const QByteArray data = _upload.mid(static_cast<int>(_offset), writeSize);
    return sendRequest(OpWriteFile, static_cast<uint8_t>(data.size()), _offset, data);
}

void MAVFTPManager::finish(const QString& errorString)
{
    const State completedState = _state;
    const QString remotePath = _remotePath;
    const QByteArray data = _download;
    reset();

    if (completedState == Opening || completedState == Reading) {
        emit fileDownloadComplete(remotePath, data, errorString);
        if (remotePath.startsWith(QStringLiteral("@PARAM/param.pck"))) {
            emit downloadComplete(data, errorString);
        }
    } else if (completedState == Creating || completedState == Writing) {
        emit fileUploadComplete(remotePath, errorString);
    }
}

void MAVFTPManager::reset()
{
    _timer.stop();
    _state = Idle;
    _link = nullptr;
    _offset = 0;
    _lastSequence = 0;
    _remotePath.clear();
    _download.clear();
    _upload.clear();
    _lastOpcode = 0;
    _lastSize = 0;
    _lastOffset = 0;
    _lastData.clear();
    _retryCount = 0;
}

uint8_t MAVFTPManager::responseErrorCode(const Response& response)
{
    if (response.data.isEmpty()) {
        return static_cast<uint8_t>(ErrFail);
    }

    return static_cast<uint8_t>(static_cast<uchar>(response.data.at(0)));
}

void MAVFTPManager::writeUInt16(uint8_t* bytes, quint16 value)
{
    bytes[0] = static_cast<uint8_t>(value & 0xff);
    bytes[1] = static_cast<uint8_t>((value >> 8) & 0xff);
}

void MAVFTPManager::writeUInt32(uint8_t* bytes, quint32 value)
{
    bytes[0] = static_cast<uint8_t>(value & 0xff);
    bytes[1] = static_cast<uint8_t>((value >> 8) & 0xff);
    bytes[2] = static_cast<uint8_t>((value >> 16) & 0xff);
    bytes[3] = static_cast<uint8_t>((value >> 24) & 0xff);
}

quint16 MAVFTPManager::readUInt16(const uint8_t* bytes)
{
    return static_cast<quint16>(bytes[0]) |
            (static_cast<quint16>(bytes[1]) << 8);
}

quint32 MAVFTPManager::readUInt32(const uint8_t* bytes)
{
    return static_cast<quint32>(bytes[0]) |
            (static_cast<quint32>(bytes[1]) << 8) |
            (static_cast<quint32>(bytes[2]) << 16) |
            (static_cast<quint32>(bytes[3]) << 24);
}

QString MAVFTPManager::errorString(uint8_t errorCode)
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
