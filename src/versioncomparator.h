#ifndef VERSIONCOMPARATOR_H
#define VERSIONCOMPARATOR_H

#include <QString>

class VersionComparator
{
public:

    static bool isVersionNewer(const QString& newVersion,
                               const QString& currentVersion);

private:

    static void splitVersionString(const QString& versionString,
                                   int& majorNumber, int& minorNumber,
                                   int& buildNumber, QString& buildSubMoniker,
                                   int& rcVersion);
};

#endif // VERSIONCOMPARATOR_H
