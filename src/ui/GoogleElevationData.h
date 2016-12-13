/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>
(c) author: Bill Bonney <billbonney@communistech.com>

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
#ifndef GOOGLEELEVATIONDATA_H
#define GOOGLEELEVATIONDATA_H

#include <QObject>
#include <QtNetwork>

// see docs at http://code.google.com/apis/maps/documentation/elevation/
static const QString GoogleElevationBaseUrl = "http://maps.googleapis.com/maps/api/elevation/json";

class Waypoint;

class GoogleElevationData : public QObject
{
    Q_OBJECT
public:
    explicit GoogleElevationData(QObject *parent = 0);
    ~GoogleElevationData();

    void requestElevationData(const QList<Waypoint *> &waypointList, int distance, int samples);

signals:
    void downloadFailed();
    void elevationDataReady(const QList<Waypoint *> waypointList, double averageResolution);

    void waypointCountToLow();
    void invalidHomeLocation();

private slots:
    // http slots
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    void processDownloadedObject(const QByteArray &data);

private:
    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    bool m_httpRequestAborted;

    QList<Waypoint* > m_elevationData;

};

#endif // GOOGLEELEVATIONDATA_H
