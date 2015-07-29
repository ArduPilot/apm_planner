#include "versioncomparator.h"

#include <QStringList>

#include "QsLog.h"

bool VersionComparator::isVersionNewer(const QString& newVersion,
                                       const QString& currentVersion)
{
    int newMajor = 0,newMinor = 0,newBuild = 0, newRc = 0;
    int currentMajor = 0, currentMinor = 0,currentBuild = 0, oldRc = 0;

    QString newBuildSubMoniker, oldBuildSubMoniker; // holds if the build is a rc or dev build

    QRegExp versionEx("((\\d)*\\.(\\d)+\\.?(\\d)?)-?(rc(\\d))?");

    int pos = versionEx.indexIn(newVersion);
    if (pos > -1) {

        QLOG_DEBUG() << "Detected newVersion:" << versionEx.capturedTexts()<< " count:"
                     << versionEx.captureCount();

        newMajor = versionEx.cap(2).toInt();
        newMinor = versionEx.cap(3).toInt();
        newBuild = versionEx.cap(4).toInt();

        // fifth subelement is either rcX candidate or developement build
        newBuildSubMoniker = versionEx.cap(5);
        newRc = versionEx.cap(6).toInt();
    }

    pos = versionEx.indexIn(currentVersion);
    if (pos > -1) {

        QLOG_DEBUG() << "Detected currentVersion:" << versionEx.capturedTexts() << " count:"
                     << versionEx.captureCount();
        currentMajor = versionEx.cap(2).toInt();
        currentMinor = versionEx.cap(3).toInt();
        currentBuild = versionEx.cap(4).toInt();

        // fifth subelement is either rcX candidate or developement build
        oldBuildSubMoniker = versionEx.cap(5);
        oldRc = versionEx.cap(6).toInt();
    }

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
