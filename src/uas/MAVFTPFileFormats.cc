/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of QGroundControl/APM Planner.

QGROUNDCONTROL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

======================================================================*/

#include "MAVFTPFileFormats.h"

#include <QMetaType>
#include <QtGlobal>

#include <cstring>
#include <limits>

namespace {

const quint16 kParamMagicStandard = 0x671B;
const quint16 kParamMagicWithDefaults = 0x671C;
const quint16 kMissionMagic = 0x763d;
const int kMissionHeaderLength = 10;

enum ApParamType
{
    AP_PARAM_NONE = 0,
    AP_PARAM_INT8 = 1,
    AP_PARAM_INT16 = 2,
    AP_PARAM_INT32 = 3,
    AP_PARAM_FLOAT = 4
};

void setError(QString* errorString, const QString& error)
{
    if (errorString) {
        *errorString = error;
    }
}

quint16 readUInt16(const QByteArray& data, int offset)
{
    const uchar* bytes = reinterpret_cast<const uchar*>(data.constData() + offset);
    return static_cast<quint16>(bytes[0]) |
            (static_cast<quint16>(bytes[1]) << 8);
}

qint16 readInt16(const QByteArray& data, int offset)
{
    return static_cast<qint16>(readUInt16(data, offset));
}

quint32 readUInt32(const QByteArray& data, int offset)
{
    const uchar* bytes = reinterpret_cast<const uchar*>(data.constData() + offset);
    return static_cast<quint32>(bytes[0]) |
            (static_cast<quint32>(bytes[1]) << 8) |
            (static_cast<quint32>(bytes[2]) << 16) |
            (static_cast<quint32>(bytes[3]) << 24);
}

qint32 readInt32(const QByteArray& data, int offset)
{
    const quint32 raw = readUInt32(data, offset);
    qint32 value = 0;
    memcpy(&value, &raw, sizeof(value));
    return value;
}

float readFloat(const QByteArray& data, int offset)
{
    const quint32 raw = readUInt32(data, offset);
    float value = 0.0f;
    memcpy(&value, &raw, sizeof(value));
    return value;
}

void appendUInt16(QByteArray* data, quint16 value)
{
    data->append(static_cast<char>(value & 0xff));
    data->append(static_cast<char>((value >> 8) & 0xff));
}

void writeUInt16(QByteArray* data, int offset, quint16 value)
{
    (*data)[offset] = static_cast<char>(value & 0xff);
    (*data)[offset + 1] = static_cast<char>((value >> 8) & 0xff);
}

void appendUInt32(QByteArray* data, quint32 value)
{
    data->append(static_cast<char>(value & 0xff));
    data->append(static_cast<char>((value >> 8) & 0xff));
    data->append(static_cast<char>((value >> 16) & 0xff));
    data->append(static_cast<char>((value >> 24) & 0xff));
}

void appendFloat(QByteArray* data, float value)
{
    quint32 raw = 0;
    memcpy(&raw, &value, sizeof(raw));
    appendUInt32(data, raw);
}

int packedParamTypeForValue(const QVariant& value)
{
    switch (static_cast<QMetaType::Type>(value.type())) {
    case QMetaType::QChar:
        return AP_PARAM_INT8;
    case QMetaType::Int:
        return AP_PARAM_INT32;
    case QMetaType::UInt:
        if (value.toUInt() > static_cast<uint>(std::numeric_limits<qint32>::max())) {
            return AP_PARAM_NONE;
        }
        return AP_PARAM_INT32;
    case QMetaType::Double:
    case QMetaType::Float:
        return AP_PARAM_FLOAT;
    default:
        return AP_PARAM_NONE;
    }
}

bool appendPackedParamValue(QByteArray* data, int paramType, const QVariant& value)
{
    switch (paramType) {
    case AP_PARAM_INT8:
        data->append(static_cast<char>(value.type() == QVariant::Char ? value.toChar().toLatin1() : value.toInt()));
        return true;
    case AP_PARAM_INT32:
        appendUInt32(data, static_cast<quint32>(static_cast<qint32>(value.toInt())));
        return true;
    case AP_PARAM_FLOAT:
        appendFloat(data, value.toFloat());
        return true;
    default:
        return false;
    }
}

bool readPackedParamValue(const QByteArray& data, int* offset, int paramType, QVariant* value)
{
    switch (paramType) {
    case AP_PARAM_INT8:
        if (*offset + 1 > data.size()) {
            return false;
        }
        *value = QVariant(static_cast<int>(static_cast<qint8>(static_cast<uchar>(data.at(*offset)))));
        *offset += 1;
        return true;
    case AP_PARAM_INT16:
        if (*offset + 2 > data.size()) {
            return false;
        }
        *value = QVariant(static_cast<int>(readInt16(data, *offset)));
        *offset += 2;
        return true;
    case AP_PARAM_INT32:
        if (*offset + 4 > data.size()) {
            return false;
        }
        *value = QVariant(static_cast<int>(readInt32(data, *offset)));
        *offset += 4;
        return true;
    case AP_PARAM_FLOAT:
        if (*offset + 4 > data.size()) {
            return false;
        }
        *value = QVariant(static_cast<double>(readFloat(data, *offset)));
        *offset += 4;
        return true;
    default:
        return false;
    }
}

void appendMissionItem(QByteArray* data, const mavlink_mission_item_int_t& item)
{
    appendFloat(data, item.param1);
    appendFloat(data, item.param2);
    appendFloat(data, item.param3);
    appendFloat(data, item.param4);
    appendUInt32(data, static_cast<quint32>(item.x));
    appendUInt32(data, static_cast<quint32>(item.y));
    appendFloat(data, item.z);
    appendUInt16(data, item.seq);
    appendUInt16(data, item.command);
    data->append(static_cast<char>(item.target_system));
    data->append(static_cast<char>(item.target_component));
    data->append(static_cast<char>(item.frame));
    data->append(static_cast<char>(item.current));
    data->append(static_cast<char>(item.autocontinue));
    data->append(static_cast<char>(item.mission_type));
}

mavlink_mission_item_int_t readMissionItem(const QByteArray& data, int offset)
{
    mavlink_mission_item_int_t item;
    memset(&item, 0, sizeof(item));
    item.param1 = readFloat(data, offset);
    item.param2 = readFloat(data, offset + 4);
    item.param3 = readFloat(data, offset + 8);
    item.param4 = readFloat(data, offset + 12);
    item.x = readInt32(data, offset + 16);
    item.y = readInt32(data, offset + 20);
    item.z = readFloat(data, offset + 24);
    item.seq = readUInt16(data, offset + 28);
    item.command = readUInt16(data, offset + 30);
    item.target_system = static_cast<uint8_t>(data.at(offset + 32));
    item.target_component = static_cast<uint8_t>(data.at(offset + 33));
    item.frame = static_cast<uint8_t>(data.at(offset + 34));
    item.current = static_cast<uint8_t>(data.at(offset + 35));
    item.autocontinue = static_cast<uint8_t>(data.at(offset + 36));
    item.mission_type = static_cast<uint8_t>(data.at(offset + 37));
    return item;
}

} // namespace

namespace MAVFTPFileFormats
{

QString parameterDownloadPath()
{
    return QStringLiteral("@PARAM/param.pck?withdefaults=1");
}

QString parameterUploadPath()
{
    return QStringLiteral("@PARAM/param.pck");
}

QString missionPath()
{
    return QStringLiteral("@MISSION/mission.dat");
}

bool parseParameterFile(const QByteArray& data, QList<ParameterValue>* parameters, QString* errorString)
{
    if (parameters) {
        parameters->clear();
    }
    if (data.size() < 6) {
        setError(errorString, QStringLiteral("parameter file is too small"));
        return false;
    }

    const quint16 magic = readUInt16(data, 0);
    const int paramCount = readUInt16(data, 2);
    const int totalParamCount = readUInt16(data, 4);
    if (magic != kParamMagicStandard && magic != kParamMagicWithDefaults) {
        setError(errorString, QStringLiteral("parameter file has invalid magic 0x%1").arg(magic, 4, 16, QLatin1Char('0')));
        return false;
    }
    if (paramCount != totalParamCount) {
        setError(errorString, QStringLiteral("parameter file is partial (%1 of %2 parameters)").arg(paramCount).arg(totalParamCount));
        return false;
    }

    QList<ParameterValue> parsed;
    QByteArray previousName;
    int offset = 6;
    int paramIndex = 0;
    while (paramIndex < paramCount) {
        while (offset < data.size() && data.at(offset) == '\0') {
            offset++;
        }

        if (offset + 2 > data.size()) {
            setError(errorString, QStringLiteral("unexpected end of file after %1 parameters").arg(paramIndex));
            return false;
        }

        const uchar typeAndFlags = static_cast<uchar>(data.at(offset++));
        const int paramType = typeAndFlags & 0x0f;
        const int flags = (typeAndFlags >> 4) & 0x0f;
        const bool hasDefault = (flags & 0x01) == 0x01;

        const uchar nameByte = static_cast<uchar>(data.at(offset++));
        const int commonLength = nameByte & 0x0f;
        const int nameLength = ((nameByte >> 4) & 0x0f) + 1;
        if (commonLength > previousName.size() || commonLength + nameLength > MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN) {
            setError(errorString, QStringLiteral("invalid parameter name prefix at index %1").arg(paramIndex));
            return false;
        }
        if (offset + nameLength > data.size()) {
            setError(errorString, QStringLiteral("unexpected end of file while reading parameter name at index %1").arg(paramIndex));
            return false;
        }

        QByteArray paramNameBytes = previousName.left(commonLength);
        paramNameBytes.append(data.constData() + offset, nameLength);
        offset += nameLength;
        previousName = paramNameBytes;

        QVariant paramValue;
        if (!readPackedParamValue(data, &offset, paramType, &paramValue)) {
            setError(errorString, QStringLiteral("invalid or truncated parameter value at index %1").arg(paramIndex));
            return false;
        }

        QVariant defaultValue;
        if (hasDefault && !readPackedParamValue(data, &offset, paramType, &defaultValue)) {
            setError(errorString, QStringLiteral("invalid or truncated default value at index %1").arg(paramIndex));
            return false;
        }

        ParameterValue parameter;
        parameter.name = QString::fromLatin1(paramNameBytes.constData(), paramNameBytes.size());
        parameter.value = paramValue;
        parameter.packedType = paramType;
        parameter.hasDefault = hasDefault;
        parameter.defaultValue = defaultValue;
        parsed.append(parameter);
        paramIndex++;
    }

    if (parameters) {
        *parameters = parsed;
    }
    return true;
}

bool encodeParameterUploadFile(const QMap<QString, QVariant>& parameters, QByteArray* data, QString* errorString)
{
    if (!data) {
        setError(errorString, QStringLiteral("missing output buffer"));
        return false;
    }
    data->clear();
    if (parameters.size() > std::numeric_limits<quint16>::max()) {
        setError(errorString, QStringLiteral("too many parameters for MAVFTP upload"));
        return false;
    }

    appendUInt16(data, kParamMagicStandard);
    appendUInt16(data, static_cast<quint16>(parameters.count()));
    appendUInt16(data, 0);

    QByteArray previousName;
    for (QMap<QString, QVariant>::const_iterator it = parameters.constBegin(); it != parameters.constEnd(); ++it) {
        const QByteArray name = it.key().toLatin1();
        if (name.isEmpty() || name.size() > MAVLINK_MSG_PARAM_VALUE_FIELD_PARAM_ID_LEN) {
            setError(errorString, QStringLiteral("invalid MAVFTP parameter name '%1'").arg(it.key()));
            return false;
        }

        int commonLength = 0;
        const int commonLimit = qMin(qMin(previousName.size(), name.size()), 15);
        while (commonLength < commonLimit && previousName.at(commonLength) == name.at(commonLength)) {
            commonLength++;
        }

        const int nameLength = name.size() - commonLength;
        if (nameLength <= 0 || nameLength > 16) {
            setError(errorString, QStringLiteral("invalid MAVFTP parameter name compression for '%1'").arg(it.key()));
            return false;
        }

        const int paramType = packedParamTypeForValue(it.value());
        if (paramType == AP_PARAM_NONE) {
            setError(errorString, QStringLiteral("unsupported MAVFTP parameter type for '%1'").arg(it.key()));
            return false;
        }

        data->append(static_cast<char>(paramType));
        data->append(static_cast<char>(commonLength | ((nameLength - 1) << 4)));
        data->append(name.constData() + commonLength, nameLength);
        if (!appendPackedParamValue(data, paramType, it.value())) {
            setError(errorString, QStringLiteral("failed to encode MAVFTP parameter '%1'").arg(it.key()));
            return false;
        }

        previousName = name;
        if (data->size() > std::numeric_limits<quint16>::max()) {
            setError(errorString, QStringLiteral("MAVFTP parameter file exceeds 65535 bytes"));
            return false;
        }
    }

    writeUInt16(data, 4, static_cast<quint16>(data->size()));
    return true;
}

bool parseMissionFile(const QByteArray& data, QList<mavlink_mission_item_int_t>* items, QString* errorString)
{
    if (items) {
        items->clear();
    }
    if (data.size() < kMissionHeaderLength) {
        setError(errorString, QStringLiteral("mission file is too short"));
        return false;
    }

    const quint16 magic = readUInt16(data, 0);
    const quint16 dataType = readUInt16(data, 2);
    const quint16 start = readUInt16(data, 6);
    const quint16 itemCount = readUInt16(data, 8);
    const int expectedSize = kMissionHeaderLength + itemCount * MAVLINK_MSG_ID_MISSION_ITEM_INT_LEN;

    if (magic != kMissionMagic) {
        setError(errorString, QStringLiteral("invalid mission file magic 0x%1").arg(magic, 4, 16, QLatin1Char('0')));
        return false;
    }
    if (dataType != MAV_MISSION_TYPE_MISSION) {
        setError(errorString, QStringLiteral("unsupported mission type %1").arg(dataType));
        return false;
    }
    if (start != 0) {
        setError(errorString, QStringLiteral("partial mission file starts at %1").arg(start));
        return false;
    }
    if (data.size() < expectedSize) {
        setError(errorString, QStringLiteral("mission file has %1 bytes, expected at least %2").arg(data.size()).arg(expectedSize));
        return false;
    }

    QList<mavlink_mission_item_int_t> parsed;
    for (int i = 0; i < itemCount; i++) {
        parsed.append(readMissionItem(data, kMissionHeaderLength + i * MAVLINK_MSG_ID_MISSION_ITEM_INT_LEN));
    }

    if (items) {
        *items = parsed;
    }
    return true;
}

QByteArray encodeMissionFile(const QList<mavlink_mission_item_int_t>& items)
{
    QByteArray data;
    const quint16 itemCount = static_cast<quint16>(items.count());
    data.reserve(kMissionHeaderLength + itemCount * MAVLINK_MSG_ID_MISSION_ITEM_INT_LEN);

    appendUInt16(&data, kMissionMagic);
    appendUInt16(&data, MAV_MISSION_TYPE_MISSION);
    appendUInt16(&data, 0);
    appendUInt16(&data, 0);
    appendUInt16(&data, itemCount);

    foreach (const mavlink_mission_item_int_t& item, items) {
        appendMissionItem(&data, item);
    }

    return data;
}

} // namespace MAVFTPFileFormats
