#include "MAVFTPFileFormats.h"
#include "MAVFTPProtocol.h"

#include <QtTest/QtTest>

#include <cmath>
#include <cstring>

class MAVFTPUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void parsePackedParameterFileWithDefaults();
    void rejectMalformedParameterFiles();
    void encodeParameterUploadFile();
    void rejectInvalidParameterUploads();
    void encodeAndParseMissionFile();
    void rejectMalformedMissionFiles();
    void encodeAndDecodeFtpPayloads();
    void rejectInvalidFtpPayloads();

private:
    static void appendUInt16(QByteArray* data, quint16 value);
    static void writeUInt16(QByteArray* data, int offset, quint16 value);
    static void appendUInt32(QByteArray* data, quint32 value);
    static void appendFloat(QByteArray* data, float value);
    static void appendParamRecord(QByteArray* data, int type, bool hasDefault, int commonLength,
                                  const QByteArray& suffix, const QByteArray& value,
                                  const QByteArray& defaultValue = QByteArray());
    static QByteArray int8Value(qint8 value);
    static QByteArray int16Value(qint16 value);
    static QByteArray int32Value(qint32 value);
    static QByteArray floatValue(float value);
    static void compareFloat(float actual, float expected);
    static void compareItem(const mavlink_mission_item_int_t& actual, const mavlink_mission_item_int_t& expected);
};

void MAVFTPUnitTest::appendUInt16(QByteArray* data, quint16 value)
{
    data->append(static_cast<char>(value & 0xff));
    data->append(static_cast<char>((value >> 8) & 0xff));
}

void MAVFTPUnitTest::writeUInt16(QByteArray* data, int offset, quint16 value)
{
    (*data)[offset] = static_cast<char>(value & 0xff);
    (*data)[offset + 1] = static_cast<char>((value >> 8) & 0xff);
}

void MAVFTPUnitTest::appendUInt32(QByteArray* data, quint32 value)
{
    data->append(static_cast<char>(value & 0xff));
    data->append(static_cast<char>((value >> 8) & 0xff));
    data->append(static_cast<char>((value >> 16) & 0xff));
    data->append(static_cast<char>((value >> 24) & 0xff));
}

void MAVFTPUnitTest::appendFloat(QByteArray* data, float value)
{
    quint32 raw = 0;
    memcpy(&raw, &value, sizeof(raw));
    appendUInt32(data, raw);
}

void MAVFTPUnitTest::appendParamRecord(QByteArray* data, int type, bool hasDefault, int commonLength,
                                       const QByteArray& suffix, const QByteArray& value,
                                       const QByteArray& defaultValue)
{
    data->append(static_cast<char>(type | (hasDefault ? 0x10 : 0)));
    data->append(static_cast<char>(commonLength | ((suffix.size() - 1) << 4)));
    data->append(suffix);
    data->append(value);
    if (hasDefault) {
        data->append(defaultValue);
    }
}

QByteArray MAVFTPUnitTest::int8Value(qint8 value)
{
    QByteArray data;
    data.append(static_cast<char>(value));
    return data;
}

QByteArray MAVFTPUnitTest::int16Value(qint16 value)
{
    QByteArray data;
    appendUInt16(&data, static_cast<quint16>(value));
    return data;
}

QByteArray MAVFTPUnitTest::int32Value(qint32 value)
{
    QByteArray data;
    appendUInt32(&data, static_cast<quint32>(value));
    return data;
}

QByteArray MAVFTPUnitTest::floatValue(float value)
{
    QByteArray data;
    appendFloat(&data, value);
    return data;
}

void MAVFTPUnitTest::compareFloat(float actual, float expected)
{
    QVERIFY2(std::fabs(actual - expected) < 0.0001f, qPrintable(QStringLiteral("actual=%1 expected=%2").arg(actual).arg(expected)));
}

void MAVFTPUnitTest::compareItem(const mavlink_mission_item_int_t& actual, const mavlink_mission_item_int_t& expected)
{
    compareFloat(actual.param1, expected.param1);
    compareFloat(actual.param2, expected.param2);
    compareFloat(actual.param3, expected.param3);
    compareFloat(actual.param4, expected.param4);
    QCOMPARE(actual.x, expected.x);
    QCOMPARE(actual.y, expected.y);
    compareFloat(actual.z, expected.z);
    QCOMPARE(actual.seq, expected.seq);
    QCOMPARE(actual.command, expected.command);
    QCOMPARE(actual.target_system, expected.target_system);
    QCOMPARE(actual.target_component, expected.target_component);
    QCOMPARE(actual.frame, expected.frame);
    QCOMPARE(actual.current, expected.current);
    QCOMPARE(actual.autocontinue, expected.autocontinue);
    QCOMPARE(actual.mission_type, expected.mission_type);
}

void MAVFTPUnitTest::parsePackedParameterFileWithDefaults()
{
    QByteArray data;
    appendUInt16(&data, 0x671c);
    appendUInt16(&data, 4);
    appendUInt16(&data, 4);
    appendParamRecord(&data, 1, true, 0, "ARMED", int8Value(-3), int8Value(0));
    data.append('\0');
    data.append('\0');
    appendParamRecord(&data, 2, false, 3, "ING_CHECK", int16Value(1234));
    appendParamRecord(&data, 3, false, 0, "BATT_CAPACITY", int32Value(-4500));
    appendParamRecord(&data, 4, true, 5, "FS_VOLTS", floatValue(10.5f), floatValue(10.0f));

    QList<MAVFTPFileFormats::ParameterValue> parameters;
    QString error;
    QVERIFY2(MAVFTPFileFormats::parseParameterFile(data, &parameters, &error), qPrintable(error));
    QCOMPARE(parameters.count(), 4);

    QCOMPARE(parameters.at(0).name, QStringLiteral("ARMED"));
    QCOMPARE(parameters.at(0).value.toInt(), -3);
    QVERIFY(parameters.at(0).hasDefault);
    QCOMPARE(parameters.at(0).defaultValue.toInt(), 0);

    QCOMPARE(parameters.at(1).name, QStringLiteral("ARMING_CHECK"));
    QCOMPARE(parameters.at(1).value.toInt(), 1234);
    QVERIFY(!parameters.at(1).hasDefault);

    QCOMPARE(parameters.at(2).name, QStringLiteral("BATT_CAPACITY"));
    QCOMPARE(parameters.at(2).value.toInt(), -4500);

    QCOMPARE(parameters.at(3).name, QStringLiteral("BATT_FS_VOLTS"));
    compareFloat(parameters.at(3).value.toFloat(), 10.5f);
    QVERIFY(parameters.at(3).hasDefault);
    compareFloat(parameters.at(3).defaultValue.toFloat(), 10.0f);
}

void MAVFTPUnitTest::rejectMalformedParameterFiles()
{
    QList<MAVFTPFileFormats::ParameterValue> parameters;
    QString error;
    QVERIFY(!MAVFTPFileFormats::parseParameterFile(QByteArray("\x1b\x67", 2), &parameters, &error));
    QVERIFY(error.contains(QStringLiteral("too small")));

    QByteArray partial;
    appendUInt16(&partial, 0x671b);
    appendUInt16(&partial, 1);
    appendUInt16(&partial, 2);
    QVERIFY(!MAVFTPFileFormats::parseParameterFile(partial, &parameters, &error));
    QVERIFY(error.contains(QStringLiteral("partial")));

    QByteArray badPrefix;
    appendUInt16(&badPrefix, 0x671b);
    appendUInt16(&badPrefix, 1);
    appendUInt16(&badPrefix, 1);
    appendParamRecord(&badPrefix, 3, false, 7, "BAD", int32Value(1));
    QVERIFY(!MAVFTPFileFormats::parseParameterFile(badPrefix, &parameters, &error));
    QVERIFY(error.contains(QStringLiteral("prefix")));

    QByteArray truncatedDefault;
    appendUInt16(&truncatedDefault, 0x671c);
    appendUInt16(&truncatedDefault, 1);
    appendUInt16(&truncatedDefault, 1);
    appendParamRecord(&truncatedDefault, 4, true, 0, "FLOAT_PARAM", floatValue(1.0f), QByteArray("\x00\x00", 2));
    QVERIFY(!MAVFTPFileFormats::parseParameterFile(truncatedDefault, &parameters, &error));
    QVERIFY(error.contains(QStringLiteral("default")));
}

void MAVFTPUnitTest::encodeParameterUploadFile()
{
    QMap<QString, QVariant> values;
    values.insert(QStringLiteral("ATC_RAT_RLL_I"), QVariant(0.12));
    values.insert(QStringLiteral("ATC_RAT_RLL_P"), QVariant(0.135));
    values.insert(QStringLiteral("LOG_BITMASK"), QVariant(131071));
    values.insert(QStringLiteral("SYSID_THISMAV"), QVariant(QChar(7)));

    QByteArray encoded;
    QString error;
    QVERIFY2(MAVFTPFileFormats::encodeParameterUploadFile(values, &encoded, &error), qPrintable(error));

    QByteArray expected;
    appendUInt16(&expected, 0x671b);
    appendUInt16(&expected, 4);
    appendUInt16(&expected, 0);
    appendParamRecord(&expected, 4, false, 0, "ATC_RAT_RLL_I", floatValue(0.12f));
    appendParamRecord(&expected, 4, false, 12, "P", floatValue(0.135f));
    appendParamRecord(&expected, 3, false, 0, "LOG_BITMASK", int32Value(131071));
    appendParamRecord(&expected, 1, false, 0, "SYSID_THISMAV", int8Value(7));
    writeUInt16(&expected, 4, static_cast<quint16>(expected.size()));

    QCOMPARE(encoded, expected);
}

void MAVFTPUnitTest::rejectInvalidParameterUploads()
{
    QByteArray encoded;
    QString error;

    QMap<QString, QVariant> badName;
    badName.insert(QStringLiteral("PARAM_NAME_TOO_LONG"), QVariant(1));
    QVERIFY(!MAVFTPFileFormats::encodeParameterUploadFile(badName, &encoded, &error));
    QVERIFY(error.contains(QStringLiteral("invalid")));

    QMap<QString, QVariant> badType;
    badType.insert(QStringLiteral("BIG_UNSIGNED"), QVariant(static_cast<uint>(0xffffffffU)));
    QVERIFY(!MAVFTPFileFormats::encodeParameterUploadFile(badType, &encoded, &error));
    QVERIFY(error.contains(QStringLiteral("unsupported")));
}

void MAVFTPUnitTest::encodeAndParseMissionFile()
{
    QList<mavlink_mission_item_int_t> items;

    mavlink_mission_item_int_t takeoff;
    memset(&takeoff, 0, sizeof(takeoff));
    takeoff.param1 = 15.0f;
    takeoff.param2 = 0.5f;
    takeoff.param3 = 1.5f;
    takeoff.param4 = 90.0f;
    takeoff.x = 473977420;
    takeoff.y = -1220840000;
    takeoff.z = 45.5f;
    takeoff.seq = 0;
    takeoff.command = MAV_CMD_NAV_TAKEOFF;
    takeoff.target_system = 1;
    takeoff.target_component = MAV_COMP_ID_MISSIONPLANNER;
    takeoff.frame = MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;
    takeoff.current = 1;
    takeoff.autocontinue = 1;
    takeoff.mission_type = MAV_MISSION_TYPE_MISSION;
    items.append(takeoff);

    mavlink_mission_item_int_t waypoint;
    memset(&waypoint, 0, sizeof(waypoint));
    waypoint.param1 = 1.0f;
    waypoint.param2 = 2.0f;
    waypoint.param3 = 3.0f;
    waypoint.param4 = 4.0f;
    waypoint.x = -353632619;
    waypoint.y = 1491652370;
    waypoint.z = 120.0f;
    waypoint.seq = 1;
    waypoint.command = MAV_CMD_NAV_WAYPOINT;
    waypoint.target_system = 1;
    waypoint.target_component = MAV_COMP_ID_MISSIONPLANNER;
    waypoint.frame = MAV_FRAME_GLOBAL_RELATIVE_ALT_INT;
    waypoint.current = 0;
    waypoint.autocontinue = 1;
    waypoint.mission_type = MAV_MISSION_TYPE_MISSION;
    items.append(waypoint);

    const QByteArray data = MAVFTPFileFormats::encodeMissionFile(items);
    QCOMPARE(data.size(), 10 + 2 * MAVLINK_MSG_ID_MISSION_ITEM_INT_LEN);
    QCOMPARE(static_cast<uchar>(data.at(0)), static_cast<uchar>(0x3d));
    QCOMPARE(static_cast<uchar>(data.at(1)), static_cast<uchar>(0x76));
    QCOMPARE(static_cast<uchar>(data.at(8)), static_cast<uchar>(2));
    QCOMPARE(static_cast<uchar>(data.at(9)), static_cast<uchar>(0));

    QList<mavlink_mission_item_int_t> parsed;
    QString error;
    QVERIFY2(MAVFTPFileFormats::parseMissionFile(data, &parsed, &error), qPrintable(error));
    QCOMPARE(parsed.count(), 2);
    compareItem(parsed.at(0), takeoff);
    compareItem(parsed.at(1), waypoint);
}

void MAVFTPUnitTest::rejectMalformedMissionFiles()
{
    QList<mavlink_mission_item_int_t> parsed;
    QString error;
    QVERIFY(!MAVFTPFileFormats::parseMissionFile(QByteArray("\x3d", 1), &parsed, &error));
    QVERIFY(error.contains(QStringLiteral("too short")));

    QByteArray badMagic;
    appendUInt16(&badMagic, 0x1234);
    appendUInt16(&badMagic, MAV_MISSION_TYPE_MISSION);
    appendUInt16(&badMagic, 0);
    appendUInt16(&badMagic, 0);
    appendUInt16(&badMagic, 0);
    QVERIFY(!MAVFTPFileFormats::parseMissionFile(badMagic, &parsed, &error));
    QVERIFY(error.contains(QStringLiteral("magic")));

    QByteArray partialStart;
    appendUInt16(&partialStart, 0x763d);
    appendUInt16(&partialStart, MAV_MISSION_TYPE_MISSION);
    appendUInt16(&partialStart, 0);
    appendUInt16(&partialStart, 1);
    appendUInt16(&partialStart, 0);
    QVERIFY(!MAVFTPFileFormats::parseMissionFile(partialStart, &parsed, &error));
    QVERIFY(error.contains(QStringLiteral("partial")));

    QByteArray truncated;
    appendUInt16(&truncated, 0x763d);
    appendUInt16(&truncated, MAV_MISSION_TYPE_MISSION);
    appendUInt16(&truncated, 0);
    appendUInt16(&truncated, 0);
    appendUInt16(&truncated, 1);
    truncated.append(QByteArray(8, '\0'));
    QVERIFY(!MAVFTPFileFormats::parseMissionFile(truncated, &parsed, &error));
    QVERIFY(error.contains(QStringLiteral("expected")));
}

void MAVFTPUnitTest::encodeAndDecodeFtpPayloads()
{
    uint8_t payload[MAVFTPProtocol::PayloadLength];
    QString error;
    QVERIFY2(MAVFTPProtocol::encodePayload(42, 7, MAVFTPProtocol::OpReadFile,
                                           MAVFTPProtocol::MaxDataLength, 0x01020304,
                                           QByteArray(), payload, sizeof(payload), &error), qPrintable(error));

    QCOMPARE(static_cast<uchar>(payload[0]), static_cast<uchar>(42));
    QCOMPARE(static_cast<uchar>(payload[1]), static_cast<uchar>(0));
    QCOMPARE(static_cast<uchar>(payload[2]), static_cast<uchar>(7));
    QCOMPARE(static_cast<uchar>(payload[3]), static_cast<uchar>(MAVFTPProtocol::OpReadFile));
    QCOMPARE(static_cast<uchar>(payload[4]), static_cast<uchar>(MAVFTPProtocol::MaxDataLength));
    QCOMPARE(static_cast<uchar>(payload[8]), static_cast<uchar>(0x04));
    QCOMPARE(static_cast<uchar>(payload[9]), static_cast<uchar>(0x03));
    QCOMPARE(static_cast<uchar>(payload[10]), static_cast<uchar>(0x02));
    QCOMPARE(static_cast<uchar>(payload[11]), static_cast<uchar>(0x01));

    const QByteArray writeData("abc");
    QVERIFY2(MAVFTPProtocol::encodePayload(44, 7, MAVFTPProtocol::OpWriteFile,
                                           writeData.size(), 251, writeData,
                                           payload, sizeof(payload), &error), qPrintable(error));
    MAVFTPProtocol::Packet packet;
    QVERIFY2(MAVFTPProtocol::decodePayload(payload, sizeof(payload), &packet, &error), qPrintable(error));
    QCOMPARE(packet.opcode, static_cast<uint8_t>(MAVFTPProtocol::OpWriteFile));
    QCOMPARE(packet.size, static_cast<uint8_t>(3));
    QCOMPARE(packet.offset, static_cast<quint32>(251));
    QCOMPARE(packet.data, writeData);

    memset(payload, 0, sizeof(payload));
    payload[0] = 45;
    payload[2] = 7;
    payload[3] = MAVFTPProtocol::OpAck;
    payload[4] = 1;
    payload[5] = MAVFTPProtocol::OpOpenFileRO;
    payload[MAVFTPProtocol::HeaderLength] = 99;
    QVERIFY2(MAVFTPProtocol::decodePayload(payload, sizeof(payload), &packet, &error), qPrintable(error));
    QCOMPARE(packet.sequence, static_cast<quint16>(45));
    QCOMPARE(packet.opcode, static_cast<uint8_t>(MAVFTPProtocol::OpAck));
    QCOMPARE(packet.requestOpcode, static_cast<uint8_t>(MAVFTPProtocol::OpOpenFileRO));
    QCOMPARE(packet.data, QByteArray(1, static_cast<char>(99)));

    memset(payload, 0, sizeof(payload));
    payload[3] = MAVFTPProtocol::OpNack;
    payload[4] = 1;
    payload[5] = MAVFTPProtocol::OpReadFile;
    payload[MAVFTPProtocol::HeaderLength] = MAVFTPProtocol::ErrEndOfFile;
    QVERIFY2(MAVFTPProtocol::decodePayload(payload, sizeof(payload), &packet, &error), qPrintable(error));
    QCOMPARE(MAVFTPProtocol::responseErrorCode(packet), static_cast<uint8_t>(MAVFTPProtocol::ErrEndOfFile));
    QVERIFY(MAVFTPProtocol::errorString(MAVFTPProtocol::responseErrorCode(packet)).contains(QStringLiteral("end of file")));

    QCOMPARE(MAVFTPProtocol::expectedResponseSequence(0xfffe), static_cast<quint16>(0xffff));
    QCOMPARE(MAVFTPProtocol::expectedResponseSequence(0xffff), static_cast<quint16>(0));
}

void MAVFTPUnitTest::rejectInvalidFtpPayloads()
{
    uint8_t payload[MAVFTPProtocol::PayloadLength];
    QString error;
    const QByteArray tooLarge(MAVFTPProtocol::MaxDataLength + 1, 'x');
    QVERIFY(!MAVFTPProtocol::encodePayload(1, 0, MAVFTPProtocol::OpWriteFile,
                                           tooLarge.size(), 0, tooLarge,
                                           payload, sizeof(payload), &error));
    QVERIFY(error.contains(QStringLiteral("too large")));

    QVERIFY(!MAVFTPProtocol::encodePayload(1, 0, MAVFTPProtocol::OpWriteFile,
                                           1, 0, QByteArray("ab", 2),
                                           payload, sizeof(payload), &error));
    QVERIFY(error.contains(QStringLiteral("too large")));

    memset(payload, 0, sizeof(payload));
    payload[4] = MAVFTPProtocol::MaxDataLength + 1;
    MAVFTPProtocol::Packet packet;
    QVERIFY(!MAVFTPProtocol::decodePayload(payload, sizeof(payload), &packet, &error));
    QVERIFY(error.contains(QStringLiteral("too large")));
}

QTEST_MAIN(MAVFTPUnitTest)
#include "MAVFTPUnitTest.moc"
