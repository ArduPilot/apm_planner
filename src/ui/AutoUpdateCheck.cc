/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.ardupilot.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file
 *   @brief Droneshare API Query Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#include "logging.h"
#include "AutoUpdateCheck.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>
#include "QGC.h"

static const QString VersionCompareRegEx = "(\\d*\\.\\d+\\.?\\d+)-?(rc\\d)?";

AutoUpdateCheck::AutoUpdateCheck(QObject *parent) :
    QObject(parent),
    m_networkReply(NULL),
    m_httpRequestAborted(false),
    m_suppressNoUpdateSignal(false)
{
    loadSettings();
}

void AutoUpdateCheck::forcedAutoUpdateCheck()
{
    loadSettings();
    setSkipVersion("0.0.0");
    autoUpdateCheck();
}

void AutoUpdateCheck::autoUpdateCheck()
{
    autoUpdateCheck(QUrl(AUTOUPDATE_VERSION_OBJECT_LOCATION
                          + AUTOUPDATE_VERSION_OBJECT_NAME));
}

void AutoUpdateCheck::autoUpdateCheck(const QUrl &url)
{
    QLOG_DEBUG() << "retrieve versionobject from server: " + url.toString();

    m_url = QUrl(url);

    if (m_networkReply != NULL){
        delete m_networkReply;
        m_networkReply = NULL;
    }
    m_networkReply = m_networkAccessManager.get(QNetworkRequest(m_url));
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void AutoUpdateCheck::cancelDownload()
{
     m_httpRequestAborted = true;
     m_networkReply->abort();
}

void AutoUpdateCheck::httpFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QLOG_DEBUG() << "AutoUpdateCheck::httpFinished()";
    if (m_httpRequestAborted) {
        reply->deleteLater();
        reply = NULL;
        return;
    }

    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    // Finished donwloading the version information
    if (reply->error()) {
        // [TODO] cleanup download failed
#ifdef QT_DEBUG
        QMessageBox::information(NULL, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(m_networkReply->errorString()));
#endif
    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = reply->url().resolved(redirectionTarget.toUrl());
        QNetworkReply* newReply = m_networkAccessManager.get(QNetworkRequest(newUrl));
        QLOG_DEBUG() << "Redirecting to " << newUrl;

        connect(newReply, SIGNAL(finished()), this, SLOT(httpFinished()));
        connect(newReply, SIGNAL(downloadProgress(qint64,qint64)),
                this, SLOT(updateDataReadProgress(qint64,qint64)));
        reply->deleteLater();
        m_networkReply = newReply;
        return;
    } else {
        // Process downloadeed object
        processDownloadedVersionObject(m_networkReply->readAll());
    }

    m_networkReply->deleteLater();
    m_networkReply = NULL;
}

void AutoUpdateCheck::processDownloadedVersionObject(const QByteArray& versionObject)
{
    QJsonParseError jsonParseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(versionObject, &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError){
        QLOG_ERROR() << "Unable to open json version object: " << jsonParseError.errorString();
        QLOG_ERROR() << "Error evaluating version object";
        return;
    }
    QJsonObject json = jdoc.object();

    QJsonArray releases = json["releases"].toArray();
    foreach(QJsonValue release, releases){
        const QJsonObject& releaseObject = release.toObject();
        QString platform = releaseObject["platform"].toString();
        QString type = releaseObject["type"].toString();
        QString version = releaseObject["version"].toString();
        QString name = releaseObject["name"].toString();
        QString locationUrl = releaseObject["url"].toString();

        if ((platform == define2string(APP_PLATFORM)) && (type == m_releaseType)){
            if (compareVersionStrings(version,QGC_APPLICATION_VERSION)){
                QLOG_DEBUG() << "Found New Version: " << platform << " "
                            << type << " " << version << " " << locationUrl;
                if(m_skipVersion != version){
                    emit updateAvailable(version, type, locationUrl, name);
                } else {
                    QLOG_INFO() << "Version Skipped at user request";
                }
                break;
            } else {
                QLOG_INFO() << "no new update available";
                if (!m_suppressNoUpdateSignal){
                    emit noUpdateAvailable();
                }
                m_suppressNoUpdateSignal = false;
            }
        }
    }
}

void AutoUpdateCheck::httpReadyRead()
{

}

void AutoUpdateCheck::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (m_httpRequestAborted)
        return;
    QLOG_DEBUG() << "Downloading:" << bytesRead << "/" << totalBytes;
}

bool AutoUpdateCheck::compareVersionStrings(const QString& newVersion, const QString& currentVersion)
{
    // [TODO] DRY this out by creating global function for use in APM Firmware as well
    int newMajor = 0,newMinor = 0,newBuild = 0;
    int currentMajor = 0, currentMinor = 0,currentBuild = 0;

    QString newBuildSubMoniker, oldBuildSubMoniker; // holds if the build is a rc or dev build


    QRegExp versionEx(VersionCompareRegEx);
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
        if (versionEx.captureCount() == 2)
            newBuildSubMoniker = versionEx.cap(2);
    }

    QRegExp versionEx2(VersionCompareRegEx);
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
        if (versionEx2.captureCount() == 2)
            oldBuildSubMoniker = versionEx2.cap(2);
    }

    QLOG_DEBUG() << "Verison Compare:" <<QString().sprintf(" New Version %d.%d.%d compared to Old Version %d.%d.%d",
                                                 newMajor,newMinor,newBuild, currentMajor, currentMinor,currentBuild);
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
                        QLOG_DEBUG() << "Detected oldRc:" << versionEx2.capturedTexts();
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

void AutoUpdateCheck::setSkipVersion(const QString& version)
{
    m_skipVersion = version;
    writeSettings();
}

void AutoUpdateCheck::setAutoUpdateEnabled(bool enabled)
{
    m_isAutoUpdateEnabled = enabled;
    writeSettings();
}

bool AutoUpdateCheck::isUpdateEnabled()
{
    return m_isAutoUpdateEnabled;
}

void AutoUpdateCheck::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.beginGroup("AUTO_UPDATE");
    m_isAutoUpdateEnabled = settings.value("ENABLED", true).toBool();
    m_skipVersion = settings.value("SKIP_VERSION", "0.0.0").toString();
    m_releaseType = settings.value("RELEASE_TYPE", define2string(APP_TYPE)).toString();
    settings.endGroup();
}

void AutoUpdateCheck::writeSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("AUTO_UPDATE");
    settings.setValue("ENABLED", m_isAutoUpdateEnabled);
    settings.setValue("SKIP_VERSION", m_skipVersion);
    settings.setValue("RELEASE_TYPE", m_releaseType);
    settings.endGroup();
    settings.sync();
}

void AutoUpdateCheck::suppressNoUpdateSignal()
{
    m_suppressNoUpdateSignal = true;
}
