/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2023 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 *	 @author Arne Wischamnn <wischmann-a@gmx.de>
 */

#include "logging.h"
#include "AutoUpdateCheck.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QMessageBox>
#include <QSettings>
#include "QGC.h"
#include "configuration.h"


AutoUpdateCheck::AutoUpdateCheck(QObject *parent) :
    QObject(parent)
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
    QString url(c_AutoUpdateVersionObjectLocation);
    url.append(c_AutoUpdateVersionObjectName);

    autoUpdateCheck(url);
}

void AutoUpdateCheck::autoUpdateCheck(const QString &url)
{
    QLOG_DEBUG() << "Retrieve versionobject from server: " + url;

    m_url = QUrl(url);
    m_networkReplyPtr.reset(m_networkAccessManager.get(QNetworkRequest(m_url)));

    connect(m_networkReplyPtr.data(), &QNetworkReply::finished, this, &AutoUpdateCheck::httpFinished);
    //connect(m_networkReplyPtr.data(), QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::errorOccurred), this, QOverload<QNetworkReply::NetworkError>::of(&AutoUpdateCheck::networkError));
    connect(m_networkReplyPtr.data(), QOverload<qint64,qint64>::of(&QNetworkReply::downloadProgress), this, QOverload<qint64,qint64>::of(&AutoUpdateCheck::updateDataReadProgress));
}

void AutoUpdateCheck::cancelDownload()
{
    QLOG_INFO() << "AutoUpdateCheck download canceled by user.";
    m_httpRequestAborted = true;
    m_networkReplyPtr->abort();
    m_networkReplyPtr.reset();
}

void AutoUpdateCheck::httpFinished()
{
    QLOG_DEBUG() << "AutoUpdateCheck::httpFinished()";
    if (m_httpRequestAborted)
    {
        QLOG_DEBUG() << "AutoUpdateCheck was aborted";
        m_httpRequestAborted = false;
        return;
    }
    if (!m_networkReplyPtr)
    {
        QLOG_DEBUG() << "m_networkReplyPtr is null!!";
        return;
    }

    QVariant redirectionTarget = m_networkReplyPtr->attribute(QNetworkRequest::RedirectionTargetAttribute);

    // Finished donwloading the version information
    if (m_networkReplyPtr->error())
    {
        // [TODO] cleanup download failed
        QLOG_WARN() << "AutoUpdateCheck::httpFinished() received an error: " << m_networkReplyPtr->errorString();
#ifdef QT_DEBUG
        QMessageBox::information(NULL, tr("HTTP"), tr("Download failed: %1.").arg(m_networkReplyPtr->errorString()));
#endif
    }
    else if (!redirectionTarget.isNull())
    {
        // we have a redirection - disconnect the signals
        disconnect(m_networkReplyPtr.data(), &QNetworkReply::finished, this, &AutoUpdateCheck::httpFinished);
        disconnect(m_networkReplyPtr.data(), QOverload<qint64,qint64>::of(&QNetworkReply::downloadProgress), this, QOverload<qint64,qint64>::of(&AutoUpdateCheck::updateDataReadProgress));

        // create new netowrk request
        QUrl newUrl = m_networkReplyPtr->url().resolved(redirectionTarget.toUrl());
        QLOG_DEBUG() << "Redirecting to " << newUrl;
        m_networkReplyPtr.reset(m_networkAccessManager.get(QNetworkRequest(newUrl)));
        m_httpRequestAborted = false;

        // and connect to new request
        connect(m_networkReplyPtr.data(), &QNetworkReply::finished, this, &AutoUpdateCheck::httpFinished);
        connect(m_networkReplyPtr.data(), QOverload<qint64,qint64>::of(&QNetworkReply::downloadProgress), this, QOverload<qint64,qint64>::of(&AutoUpdateCheck::updateDataReadProgress));
        return;
    }
    else
    {
        // Process downloaded object
        processDownloadedVersionObject(m_networkReplyPtr->readAll());
    }

    // the request is not needed anymore
    m_networkReplyPtr.reset();
}

void AutoUpdateCheck::processDownloadedVersionObject(const QByteArray& versionObject)
{
    QJsonParseError jsonParseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(versionObject, &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError)
    {
        QLOG_ERROR() << "Unable to open json version object: " << jsonParseError.errorString();
        QLOG_ERROR() << "Error evaluating version object";
        return;
    }

    QJsonObject json = jdoc.object();
    QJsonArray releases = json["releases"].toArray();

    bool foundUpdate = false;

    for (const auto& release : qAsConst(releases))
    {
        const QJsonObject& releaseObject = release.toObject();
        QString platform = releaseObject["platform"].toString();
        QString type = releaseObject["type"].toString();
        QString version = releaseObject["version"].toString();
        QString name = releaseObject["name"].toString();
        QString locationUrl = releaseObject["url"].toString();
        QString platt = define2string(APP_PLATFORM);

        if (platform == platt)
        {
            if (compareVersionStrings(version,QGC_APPLICATION_VERSION))
            {
                foundUpdate = true;
                QLOG_DEBUG() << "Found New Version: " << platform << " " << type << " " << version << " " << locationUrl;
                if(m_skipVersion != version)
                {
                    emit updateAvailable(version, type, locationUrl, name);
                } else
                {
                    QLOG_INFO() << "Version Skipped at user request";
                }
                break;
            }
        }
    }

    if (!foundUpdate)
    {
        QLOG_INFO() << "No new update available";
        if (!m_suppressNoUpdateSignal)
        {
            emit noUpdateAvailable();
        }
    }

    m_suppressNoUpdateSignal = false;
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
    int newMajor = 0;
    int newMinor = 0;
    int newBuild = 0;
    int newRc    = 0;

    int currentMajor = 0;
    int currentMinor = 0;
    int currentBuild = 0;
    int currentRc    = 0;

    extractVersion(newVersion, newMajor, newMinor, newBuild, newRc);
    extractVersion(currentVersion, currentMajor, currentMinor, currentBuild, currentRc);

    QLOG_DEBUG() << "Comparing " <<QString().asprintf("new version %d.%d.%d-rc%d with current Version %d.%d.%d-rc%d",
                                                             newMajor,newMinor,newBuild,newRc, currentMajor, currentMinor,currentBuild, currentRc);
    if (newMajor > currentMajor)
    {
        // A Major release
        return true;
    } else if (newMajor == currentMajor)
    {
        if (newMinor > currentMinor)
        {
            // A minor release
            return true;
        } else if (newMinor ==  currentMinor)
        {
            if (newBuild > currentBuild)
            {
                // new build (or tiny release)
                return true;
            }
            else if (newBuild == currentBuild)
            {
                // Check if RC is newer
                // If the version isn't newer, it might be a new release candidate
                if (newRc > currentRc)
                {
                    return true;
                } else if (newRc == 0)
                {
                    // 2.0.20 is newer than 2.0.20-rc3
                    QLOG_DEBUG() << "Stable build newer that last unstable release candidate ";
                    return true;
                }
            }
        }
    }

    QLOG_DEBUG() << "Current version is still newest one.";
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

void AutoUpdateCheck::extractVersion(const QString& versionString, int& major, int& minor, int& build, int& rc)
{
    QRegExp versionEx(c_VersionCompareRegEx);
    int pos = versionEx.indexIn(versionString);
    if (pos > -1)
    {
        // Split first sub-element to get numercal major.minor.build version
        QLOG_DEBUG() << "parsing version:" << versionEx.capturedTexts();
        QString version = versionEx.cap(1);
        QStringList versionList = version.split(".");
        major = versionList[0].toInt();
        minor = versionList[1].toInt();
        if (versionList.size() > 2)
        {
            build = versionList[2].toInt();
        }
        // second subelement is either rcX candidate or developement build
        if (versionEx.captureCount() == 2)
        {
            QString newBuildSubMoniker = versionEx.cap(2);

            if (newBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive))
            {
                QRegExp releaseNumber("\\d+");
                pos = releaseNumber.indexIn(newBuildSubMoniker);
                if (pos > -1)
                {
                    rc = releaseNumber.cap(0).toInt();
                }
            }
        }
    }
}
