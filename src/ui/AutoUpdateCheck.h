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
 *   @brief AutoUpdate Query Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#ifndef AUTOUPDATECHECK_H
#define AUTOUPDATECHECK_H

#include "configuration.h"
#include <QObject>
#include <QtNetwork>

const QString AUTOUPDATE_VERSION_OBJECT_LOCATION = "http://firmware.ardupilot.org/Tools/APMPlanner/";
const QString AUTOUPDATE_VERSION_OBJECT_NAME = "apm_planner_version.json";

class AutoUpdateCheck : public QObject
{
    Q_OBJECT
public:
    explicit AutoUpdateCheck(QObject *parent = 0);
    void suppressNoUpdateSignal();

signals:
    void updateAvailable(QString version, QString releaseType, QString url, QString name);
    void noUpdateAvailable();

public slots:
    void forcedAutoUpdateCheck();
    void autoUpdateCheck();
    void autoUpdateCheck(const QUrl& url);
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    void setSkipVersion(const QString& version);
    void setAutoUpdateEnabled(bool enabled);
    bool isUpdateEnabled();

private:
    void loadSettings();
    void writeSettings();
    void processDownloadedVersionObject(const QByteArray& versionObject);
    bool compareVersionStrings(const QString& newVersion, const QString& currentVersion);

private:
    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    bool m_httpRequestAborted;

    bool m_isAutoUpdateEnabled;
    QString m_skipVersion;
    QString m_releaseType; // 'stable', 'beta', 'daily'
    bool m_suppressNoUpdateSignal;

};

#endif // AUTOUPDATECHECK_H
