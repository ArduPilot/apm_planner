/****************************************************************************
 *
 *   (c) 2009-2016 APMPLANNER PROJECT <http://www.qgroundcontrol.org>
 *
 * APM Planner is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 ****************************************************************************/


/// @file
/// @author Bill Bonney <bill@communistech.com>
/// @author Don Gagne <don@thegagnes.com>
///

#include "logging.h"
#include "ArduPilotMegaMAV.h"
#include "APMFirmwareVersion.h"
#include <QRegularExpression>

APMFirmwareVersion::APMFirmwareVersion(): _major(0),_minor(0),_patch(0)
{
}

APMFirmwareVersion::APMFirmwareVersion(const QString &versionText):
    _major(0),_minor(0),_patch(0)
{
    parseVersion(versionText);
}

bool APMFirmwareVersion::isValid() const
{
    return !_versionString.isEmpty();
}

bool APMFirmwareVersion::isBeta() const
{
    return _versionString.contains(QStringLiteral(".rc"));
}

bool APMFirmwareVersion::isDev() const
{
    return _versionString.contains(QStringLiteral(".dev"));
}

bool APMFirmwareVersion::operator <(const APMFirmwareVersion& other) const
{
    int myVersion = _major << 16 | _minor << 8 | _patch ;
    int otherVersion = other.majorNumber() << 16 | other.minorNumber() << 8 | other.patchNumber();
    return myVersion < otherVersion;
}

void APMFirmwareVersion::parseVersion(const QString &versionText)
{
    if (versionText.isEmpty()) {
        return;
    }


    QRegularExpressionMatch versionMatch = VERSION_REXP.match(versionText);
    if (!versionMatch.hasMatch()) {
        QLOG_WARN() << "firmware version regex didn't match anything"
                                        << "version text to be parsed" << versionText;
        return;
    }

    QStringList capturedTexts = versionMatch.capturedTexts();

    if (capturedTexts.count() < 5) {
        QLOG_WARN() << "something wrong with parsing the version text, not hitting anything"
                                        << VERSION_REXP.captureCount() << versionMatch.capturedTexts();
        return;
    }

    // successful extraction of version numbers
    // even though we could have collected the version string atleast
    // but if the parsing has faild, not much point
    _versionString = versionText;
    _vehicleType   = capturedTexts[1];
    _major         = capturedTexts[2].toInt();
    _minor         = capturedTexts[3].toInt();
    _patch         = capturedTexts[4].toInt();
}
