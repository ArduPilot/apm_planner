#ifndef VERSIONCOMPARATOR_H
#define VERSIONCOMPARATOR_H

#include <QString>

class VersionComparator
{
public:

    static bool isVersionNewer(const QString& newVersion,
                               const QString& currentVersion);
};

#endif // VERSIONCOMPARATOR_H
