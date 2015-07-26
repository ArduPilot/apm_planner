#include "versioncomparator.h"

#include <QStringList>

#include "QsLog.h"

bool VersionComparator::isVersionNewer(const QString& newVersion,
                                       const QString& currentVersion)
{
    int newMajor,newMinor,newBuild = 0;
    int currentMajor, currentMinor,currentBuild = 0;

    QString newBuildSubMoniker, oldBuildSubMoniker; // holds if the build is a rc or dev build

    QRegExp versionEx("(\\d*\\.\\d+\\.?\\d?)-?(rc\\d)?");
    QString versionstr = "";
    int pos = versionEx.indexIn(newVersion);
    if (pos > -1) {
        // Split first sub-element to get numercal major.minor.build version
        QLOG_DEBUG() << "Detected newVersion:" << versionEx.capturedTexts()<< " count:"
                     << versionEx.captureCount();
        versionstr = versionEx.cap(1);
        QStringList versionList = versionstr.split(".");
        newMajor = versionList[0].toInt();
        newMinor = versionList[1].toInt();
        if (versionList.size() > 2){
            newBuild = versionList[2].toInt();
        }
        // second subelement is either rcX candidate or developement build
        newBuildSubMoniker = versionEx.cap(2);
    }

    QRegExp versionEx2("(\\d*\\.\\d+\\.?\\d?)-?(rc\\d)?");
    versionstr = "";
    pos = versionEx2.indexIn(currentVersion);
    if (pos > -1) {
        QLOG_DEBUG() << "Detected currentVersion:" << versionEx2.capturedTexts() << " count:"
                     << versionEx2.captureCount();
        versionstr = versionEx2.cap(1);
        QStringList versionList = versionstr.split(".");
        currentMajor = versionList[0].toInt();
         currentMinor = versionList[1].toInt();
        if (versionList.size() > 2){
            currentBuild = versionList[2].toInt();
        }
        // second subelement is either rcX candidate or developement build
        oldBuildSubMoniker = versionEx2.cap(2);
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
                int newRc = 0, oldRc = 0;

                if (newBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)
                        && oldBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)) {
                    QRegExp releaseNumber("\\d+");
                    pos = releaseNumber.indexIn(newBuildSubMoniker);
                    if (pos > -1) {
                        QLOG_DEBUG() << "Detected newRc:" << versionEx.capturedTexts();
                        newRc = releaseNumber.cap(0).toInt();
                    }

                    QRegExp releaseNumber2("\\d+");
                    pos = releaseNumber2.indexIn(oldBuildSubMoniker);
                    if (pos > -1) {
                        QLOG_DEBUG() << "Detected oldRc:" << versionEx.capturedTexts();
                        oldRc = releaseNumber2.cap(0).toInt();
                    }

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
