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
    void elevationDataReady(const QList<Waypoint *> waypointList);

    void waypointCountToLow();
    void invalidHomeLocation();

private slots:
    // http slots
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    void processDownloadedObject(const QString& jsonObject);

private:
    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    bool m_httpRequestAborted;

    QList<Waypoint* > m_elevationData;

};

#endif // GOOGLEELEVATIONDATA_H
