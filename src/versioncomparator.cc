#include "versioncomparator.h"

#include <QStringList>

#include "QsLog.h"

bool VersionComparator::isVersionNewer(const QString& newVersion,
                                       const QString& currentVersion)
{
    int newMajor = 0,newMinor = 0,newBuild = 0, newRc = 0;
    int currentMajor = 0, currentMinor = 0,currentBuild = 0, oldRc = 0;

    QString newBuildSubMoniker, oldBuildSubMoniker; // holds if the build is a rc or dev build

    splitVersionString(newVersion, newMajor, newMinor, newBuild,
                       newBuildSubMoniker, newRc);

    splitVersionString(currentVersion, currentMajor, currentMinor, currentBuild,
                       oldBuildSubMoniker, oldRc);

    QLOG_DEBUG() << "Verison Compare:" <<QString().sprintf(" New Version %d.%d.%d > Old Version %d.%d.%d",
                                                 newMajor,newMinor,newBuild,currentMajor, currentMinor,currentBuild);
    if (newMajor>currentMajor){
        // A Major release
        return true;
    } else if (newMajor == currentMajor){
        if (newMinor >  currentMinor){
            // A minor release
            return true;
        } else if (newMinor ==  currentMinor){
            if (newBuild > currentBuild)
                // new build (or tiny release)
                return true;
            else if (newBuild == currentBuild) {
                // Check if RC is newer
                // If the version isn't newer, it might be a new release candidate

                if (newBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)
                        && oldBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)) {

                    if (newRc > oldRc)
                        return true;
                }

                if (newBuildSubMoniker.length() == 0
                        && oldBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)) {
                    QLOG_DEBUG() << "Stable build newer that last unstable release candidate ";
                    return true; // this means a new stable build of the unstable rc is available
                }
            }
        }
    }

    return false;
}

void VersionComparator::splitVersionString(const QString& versionString,
                                           int& majorNumber, int& minorNumber,
                                           int& buildNumber, QString& buildSubMoniker,
                                           int& rcVersion)
{
    const QRegExp versionEx("((\\d)*\\.(\\d)+\\.?(\\d)?)-?(rc(\\d))?");

    const int pos = versionEx.indexIn(versionString);
    if (pos > -1) {

        QLOG_DEBUG() << "Detected version:" << versionEx.capturedTexts()<< " count:"
                     << versionEx.captureCount();

        majorNumber = versionEx.cap(2).toInt();
        minorNumber = versionEx.cap(3).toInt();
        buildNumber = versionEx.cap(4).toInt();

        // fifth subelement is either rcX candidate or developement build
        buildSubMoniker = versionEx.cap(5);
        rcVersion = versionEx.cap(6).toInt();
    }
}
