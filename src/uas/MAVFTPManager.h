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

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QTimer>

#include "MAVFTPProtocol.h"
#include "QGCMAVLink.h"

#include <stdint.h>

class LinkInterface;
class UAS;

/**
 * @brief Minimal MAVLink FTP client used for ArduPilot virtual file access.
 */
class MAVFTPManager : public QObject
{
    Q_OBJECT
public:
    explicit MAVFTPManager(UAS* uas);

    bool isBusy() const;
    bool downloadParameterFile();
    bool downloadFile(const QString& remotePath, uint8_t targetComponent = MAV_COMP_ID_PRIMARY);
    bool uploadFile(const QString& remotePath, const QByteArray& data, uint8_t targetComponent = MAV_COMP_ID_PRIMARY);
    bool handleMessage(LinkInterface* link, const mavlink_message_t& message);
    void cancel();

signals:
    void downloadComplete(const QByteArray& data, const QString& errorString);
    void fileDownloadComplete(const QString& remotePath, const QByteArray& data, const QString& errorString);
    void fileUploadComplete(const QString& remotePath, const QString& errorString);

private slots:
    void timeout();

private:
    enum State
    {
        Idle,
        Opening,
        Reading,
        Creating,
        Writing,
        Closing
    };

    enum TransferType
    {
        NoTransfer,
        DownloadTransfer,
        UploadTransfer
    };

    typedef MAVFTPProtocol::Packet Response;

    LinkInterface* activeLink() const;
    bool sendRequest(uint8_t opcode, uint8_t size, quint32 offset, const QByteArray& data, bool retrying = false);
    bool decodeResponse(const mavlink_message_t& message, Response* response) const;
    void handleOpenResponse(const Response& response);
    void handleReadResponse(const Response& response);
    void handleCreateResponse(const Response& response);
    void handleWriteResponse(const Response& response);
    void handleTerminateResponse(const Response& response);
    bool sendReadRequest();
    bool sendWriteRequest();
    bool sendTerminateRequest();
    void finish(const QString& errorString);
    void reset();

    quint16 expectedResponseSequence() const;

    enum
    {
        kMaxDataLength = MAVFTPProtocol::MaxDataLength,
        kTimeoutMs = 1000,
        kMaxRetries = 3
    };

    UAS* _uas;
    LinkInterface* _link;
    QTimer _timer;
    State _state;
    TransferType _transferType;
    uint8_t _targetComponent;
    uint8_t _session;
    quint16 _sequence;
    quint16 _lastSequence;
    quint32 _offset;
    QString _remotePath;
    QByteArray _download;
    QByteArray _upload;

    uint8_t _lastOpcode;
    uint8_t _lastSize;
    quint32 _lastOffset;
    QByteArray _lastData;
    int _retryCount;
};
