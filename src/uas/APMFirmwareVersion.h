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

#ifndef APMFIRMWAREVERSION_H
#define APMFIRMWAREVERSION_H

#include <QString>

class APMFirmwareVersion
{
public:
    APMFirmwareVersion();
    APMFirmwareVersion(const QString &versionText);
    void parseVersion(const QString &versionText);

    bool isValid() const;
    bool isBeta() const;
    bool isDev() const;
    bool operator<(const APMFirmwareVersion& other) const;
    QString versionString() const { return _versionString; }
    QString vehicleType() const { return _vehicleType; }
    int majorNumber() const { return _major; }
    int minorNumber() const { return _minor; }
    int patchNumber() const { return _patch; }

private:
    QString _versionString;
    QString _vehicleType;
    int     _major;
    int     _minor;
    int     _patch;
};

#endif // APMFIRMWAREVERSION_H
