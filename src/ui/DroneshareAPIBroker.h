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
 *   @brief Droneshare API Query Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#ifndef DRONESHAREAPIBROKER_H
#define DRONESHAREAPIBROKER_H

#include <QObject>
#include <QtNetwork>

class DroneshareAPIBroker : public QObject
{
    Q_OBJECT
public:
    explicit DroneshareAPIBroker(QObject *parent = 0);
    ~DroneshareAPIBroker();

    void addBaseUrl(const QString& baseUrl);
    void addQuery(const QString& queryString);
    void addQueryItem(const QString& key, const QString& value);
    void sendQueryRequest();
    const QUrl& getUrl() const;

signals:
    void queryFailed(const QString& errorString);
    void queryComplete(const QString& jsonObject);
    void queryProgress(int bytesRead, int totalBytes);

private slots:
    // http slots
    void cancel();
    void httpFinished();
    void downloadProgress(qint64 bytesWritten, qint64 totalBytes);

private:
    QUrl m_url;
    QString m_droneshareBaseUrl;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    bool m_httpRequestAborted;
};

#endif // DRONESHAREAPIBROKER_H
