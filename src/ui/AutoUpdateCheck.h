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
 *   @brief AutoUpdate Query Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *	 @author Arne Wischamnn <wischmann-a@gmx.de>
 */

#pragma once

#include <QObject>
#include <QtNetwork>

class AutoUpdateCheck : public QObject
{
    Q_OBJECT
public:
    explicit AutoUpdateCheck(QObject *parent = nullptr);
    void suppressNoUpdateSignal();

signals:
    void updateAvailable(QString version, QString releaseType, QString url, QString name);
    void noUpdateAvailable();

public slots:
    void forcedAutoUpdateCheck();
    void autoUpdateCheck();
    void autoUpdateCheck(const QString &url);
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);
 
    void setSkipVersion(const QString& version);
    void setAutoUpdateEnabled(bool enabled);
    bool isUpdateEnabled();

private:

    static constexpr const char* c_AutoUpdateVersionObjectLocation {"https://firmware.ardupilot.org/Tools/APMPlanner/"};
    static constexpr const char* c_AutoUpdateVersionObjectName {"apm_planner_version.json"};
    static constexpr const char* c_VersionCompareRegEx {"(\\d*\\.\\d+\\.?\\d+)-?(rc\\d)?"};


    void loadSettings();
    void writeSettings();
    void processDownloadedVersionObject(const QByteArray& versionObject);
    bool compareVersionStrings(const QString& newVersion, const QString& currentVersion);
    void extractVersion(const QString& versionString, int& major, int& minor, int& build, int& rc);

private:
    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QScopedPointer<QNetworkReply, QScopedPointerDeleteLater> m_networkReplyPtr;

    QString m_skipVersion;
    QString m_releaseType; // 'stable', 'beta', 'daily'

    bool m_isAutoUpdateEnabled {false};
    bool m_httpRequestAborted {false};
    bool m_suppressNoUpdateSignal {false};

};
