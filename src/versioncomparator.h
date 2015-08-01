#ifndef VERSIONCOMPARATOR_H
#define VERSIONCOMPARATOR_H

#include <QString>

/**
 * @brief Compares two version strings.
 *
 * This class can be used to compare two version strings.
 * The strings need to contain a version number in the follwing format:
 *      MAJOR.MINOR[.BUILD][-RELEASE_CANDIDATE]
 * e.g.     2.0.18-rc2 or 2.1
 *
 **/
class VersionComparator
{
public:

    /**
     * @brief compares two version strings
     * @param newVersion: this version is checked if it is newer,
     *                    it is probably fetched from the internet
     * @param currentVersion: this is the current version installed on the system
     * @return returns true if newVersion is newer than currentVersion
     */
    static bool isVersionNewer(const QString& newVersion,
                               const QString& currentVersion);

private:

    static void splitVersionString(const QString& versionString,
                                   int& majorNumber, int& minorNumber,
                                   int& buildNumber, QString& buildSubMoniker,
                                   int& rcVersion);
};

#endif // VERSIONCOMPARATOR_H
