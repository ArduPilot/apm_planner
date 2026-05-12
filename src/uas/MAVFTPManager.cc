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
#include "MAVFTPFileFormats.h"
#include "MAVFTPProtocol.h"
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
    _transferType(NoTransfer),
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
    return downloadFile(MAVFTPFileFormats::parameterDownloadPath(), MAV_COMP_ID_PRIMARY);
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
    _transferType = DownloadTransfer;
    _state = Opening;

    QLOG_DEBUG() << "Starting MAVFTP download of" << remotePath;
    if (!sendRequest(MAVFTPProtocol::OpOpenFileRO, static_cast<uint8_t>(path.size()), 0, path)) {
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
    _transferType = UploadTransfer;
    _state = Creating;

    QLOG_DEBUG() << "Starting MAVFTP upload of" << remotePath << "with" << data.size() << "bytes";
    if (!sendRequest(MAVFTPProtocol::OpCreateFile, static_cast<uint8_t>(path.size()), 0, path)) {
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
            _state == Opening ? MAVFTPProtocol::OpOpenFileRO :
            _state == Reading ? MAVFTPProtocol::OpReadFile :
            _state == Creating ? MAVFTPProtocol::OpCreateFile :
            _state == Writing ? MAVFTPProtocol::OpWriteFile :
            _state == Closing ? MAVFTPProtocol::OpTerminateSession : 0;
    if (response.requestOpcode != expectedRequestOpcode) {
        return false;
    }
    if (response.sequence != expectedResponseSequence()) {
        return false;
    }
    if ((_state == Reading || _state == Writing || _state == Closing) && response.session != _session) {
        return false;
    }
    if (_state == Reading && response.opcode == MAVFTPProtocol::OpAck && response.offset < _offset) {
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
    case Closing:
        handleTerminateResponse(response);
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

    sendRequest(MAVFTPProtocol::OpTerminateSession, 0, 0, QByteArray());
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

    const quint16 requestSequence = retrying ? _lastSequence : _sequence;
    if (!retrying) {
        _sequence = static_cast<quint16>(_sequence + 2);
    }

    uint8_t payload[MAVFTPProtocol::PayloadLength];
    QString encodeError;
    if (!MAVFTPProtocol::encodePayload(requestSequence, _session, opcode, size, offset, data,
                                       payload, MAVFTPProtocol::PayloadLength, &encodeError)) {
        QLOG_WARN() << encodeError;
        return false;
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

    QString decodeError;
    if (!MAVFTPProtocol::decodePayload(packet.payload, MAVFTPProtocol::PayloadLength, response, &decodeError)) {
        QLOG_WARN() << decodeError;
        return false;
    }
    return true;
}

void MAVFTPManager::handleOpenResponse(const Response& response)
{
    if (response.requestOpcode != MAVFTPProtocol::OpOpenFileRO) {
        return;
    }

    if (response.opcode != MAVFTPProtocol::OpAck) {
        const uint8_t errorCode = MAVFTPProtocol::responseErrorCode(response);
        finish(QStringLiteral("MAVFTP open failed: %1").arg(MAVFTPProtocol::errorString(errorCode)));
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
    if (response.requestOpcode != MAVFTPProtocol::OpReadFile) {
        return;
    }

    if (response.opcode == MAVFTPProtocol::OpNack) {
        const uint8_t errorCode = MAVFTPProtocol::responseErrorCode(response);
        if (errorCode == MAVFTPProtocol::ErrEndOfFile || errorCode == MAVFTPProtocol::ErrNone) {
            if (!sendTerminateRequest()) {
                finish(QStringLiteral("MAVFTP close request could not be sent"));
            }
            return;
        }

        finish(QStringLiteral("MAVFTP read failed: %1").arg(MAVFTPProtocol::errorString(errorCode)));
        return;
    }

    if (response.opcode != MAVFTPProtocol::OpAck || response.offset != _offset) {
        finish(QStringLiteral("MAVFTP received an unexpected read response"));
        return;
    }

    _download.append(response.data.constData(), response.data.size());
    _offset += response.data.size();

    if (response.size == 0 || response.size < _lastSize) {
        if (!sendTerminateRequest()) {
            finish(QStringLiteral("MAVFTP close request could not be sent"));
        }
        return;
    }

    if (!sendReadRequest()) {
        finish(QStringLiteral("MAVFTP read request could not be sent"));
    }
}

void MAVFTPManager::handleCreateResponse(const Response& response)
{
    if (response.requestOpcode != MAVFTPProtocol::OpCreateFile) {
        return;
    }

    if (response.opcode != MAVFTPProtocol::OpAck) {
        const uint8_t errorCode = MAVFTPProtocol::responseErrorCode(response);
        finish(QStringLiteral("MAVFTP create failed: %1").arg(MAVFTPProtocol::errorString(errorCode)));
        return;
    }

    _session = response.session;
    _offset = 0;
    _state = Writing;

    if (_upload.isEmpty()) {
        if (!sendTerminateRequest()) {
            finish(QStringLiteral("MAVFTP close request could not be sent"));
        }
        return;
    }

    if (!sendWriteRequest()) {
        finish(QStringLiteral("MAVFTP write request could not be sent"));
    }
}

void MAVFTPManager::handleWriteResponse(const Response& response)
{
    if (response.requestOpcode != MAVFTPProtocol::OpWriteFile) {
        return;
    }

    if (response.opcode == MAVFTPProtocol::OpNack) {
        const uint8_t errorCode = MAVFTPProtocol::responseErrorCode(response);
        finish(QStringLiteral("MAVFTP write failed: %1").arg(MAVFTPProtocol::errorString(errorCode)));
        return;
    }

    if (response.opcode != MAVFTPProtocol::OpAck || response.offset != _offset) {
        finish(QStringLiteral("MAVFTP received an unexpected write response"));
        return;
    }

    _offset += _lastSize;
    if (_offset >= static_cast<quint32>(_upload.size())) {
        if (!sendTerminateRequest()) {
            finish(QStringLiteral("MAVFTP close request could not be sent"));
        }
        return;
    }

    if (!sendWriteRequest()) {
        finish(QStringLiteral("MAVFTP write request could not be sent"));
    }
}

void MAVFTPManager::handleTerminateResponse(const Response& response)
{
    if (response.requestOpcode != MAVFTPProtocol::OpTerminateSession) {
        return;
    }

    if (response.opcode != MAVFTPProtocol::OpAck) {
        const uint8_t errorCode = MAVFTPProtocol::responseErrorCode(response);
        finish(QStringLiteral("MAVFTP close failed: %1").arg(MAVFTPProtocol::errorString(errorCode)));
        return;
    }

    finish(QString());
}

bool MAVFTPManager::sendReadRequest()
{
    return sendRequest(MAVFTPProtocol::OpReadFile, static_cast<uint8_t>(kMaxDataLength), _offset, QByteArray());
}

bool MAVFTPManager::sendWriteRequest()
{
    const int remaining = _upload.size() - static_cast<int>(_offset);
    const int writeSize = qMin(static_cast<int>(kMaxDataLength), remaining);
    if (writeSize <= 0) {
        return false;
    }

    const QByteArray data = _upload.mid(static_cast<int>(_offset), writeSize);
    return sendRequest(MAVFTPProtocol::OpWriteFile, static_cast<uint8_t>(data.size()), _offset, data);
}

bool MAVFTPManager::sendTerminateRequest()
{
    _state = Closing;
    return sendRequest(MAVFTPProtocol::OpTerminateSession, 0, 0, QByteArray());
}

void MAVFTPManager::finish(const QString& errorString)
{
    const State completedState = _state;
    const TransferType completedTransferType = _transferType;
    const QString remotePath = _remotePath;
    const QByteArray data = _download;
    reset();

    if (completedTransferType == DownloadTransfer && (completedState == Opening || completedState == Reading || completedState == Closing)) {
        emit fileDownloadComplete(remotePath, data, errorString);
        if (remotePath.startsWith(QStringLiteral("@PARAM/param.pck"))) {
            emit downloadComplete(data, errorString);
        }
    } else if (completedTransferType == UploadTransfer && (completedState == Creating || completedState == Writing || completedState == Closing)) {
        emit fileUploadComplete(remotePath, errorString);
    }
}

void MAVFTPManager::reset()
{
    _timer.stop();
    _state = Idle;
    _transferType = NoTransfer;
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

quint16 MAVFTPManager::expectedResponseSequence() const
{
    return MAVFTPProtocol::expectedResponseSequence(_lastSequence);
}
