/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 *   @brief DroneshareUpload helper
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#ifndef DRONESHAREUPLOAD_H
#define DRONESHAREUPLOAD_H

#include <QObject>
#include <QtNetwork>

static const QString DroneshareBaseUrl = "https://api.3drobotics.com/api/v1";
static const QString DroneshareAPIKey = "5e5c1859.14f775d41f4c3b21af4b3bceb104728a";

class DroneshareUpload : public QObject
{
    Q_OBJECT
public:
    explicit DroneshareUpload(QObject *parent = 0);
    ~DroneshareUpload();

    void uploadLog(const QString& filename, const QString& user, const QString& password,
                   const QString& vehicleID, const QString& apiKey);

signals:
    void uploadFailed(const QString& jsonRepsonse, const QString& errorString);
    void uploadComplete(const QString& jsonRepsonse);
    void uploadProgress(int bytesRead, int totalBytes);

private slots:
    // http slots
    void cancel();
    void httpFinished();
    void updateDataWriteProgress(qint64 bytesWritten, qint64 totalBytes);

private:
    QUrl m_url;
    QFile* m_uploadFile;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    bool m_httpRequestAborted;
};

#endif // DRONESHAREUPLOAD_H
