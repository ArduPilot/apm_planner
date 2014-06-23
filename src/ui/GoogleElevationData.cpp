#include "QsLog.h"
#include "GoogleElevationData.h"
#include "Waypoint.h"

#include <QString>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QMessageBox>

GoogleElevationData::GoogleElevationData(QObject *parent) :
    QObject(parent),
    m_networkReply(NULL),
    m_httpRequestAborted(false)
{
}

GoogleElevationData::~GoogleElevationData()
{
    delete m_networkReply;
}

void GoogleElevationData::cancelDownload()
{
     m_httpRequestAborted = true;
     m_networkReply->abort();
}

void GoogleElevationData::httpFinished()
{
    QLOG_DEBUG() << "GoogleElevationData::httpFinished()";
    if (m_httpRequestAborted) {
        m_networkReply->deleteLater();
        m_networkReply = NULL;
        return;
    }

    // Finished donwloading the elevation information
    if (m_networkReply->error()) {
        // [TODO] cleanup download failed
#ifdef QT_DEBUG
        QMessageBox::information(NULL, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(m_networkReply->errorString()));
#endif
    } else {
        // Process downloadeed object
        processDownloadedObject(QString(m_networkReply->readAll()));
    }

    m_networkReply->deleteLater();
    m_networkReply = NULL;
}

void GoogleElevationData::httpReadyRead()
{

}

void GoogleElevationData::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (m_httpRequestAborted)
        return;
    QLOG_DEBUG() << "Downloading: " << m_url << " :"<< bytesRead << "/" << totalBytes;
}

void GoogleElevationData::requestElevationData(const QList<Waypoint *> &waypointList, int distance,
                                               int samples)
{
    // Create the google request from the currentWaypointList
    if (waypointList.count() < 2){
        QLOG_ERROR() << "Not enough waypoints to request elevation data.";
        emit waypointCountToLow();
        return;
    }

    if ((waypointList.at(0)->getLatitude() == 0.0)
       ||(waypointList.at(0)->getLatitude() == 0.0)){
       QLOG_ERROR() << "Need valid home location.";
       emit invalidHomeLocation();
    }

    m_url.clear();

    QString path;
    foreach(Waypoint* wp, waypointList){
        path.append(QString::number(wp->getLatitude()) + ","
                              + QString::number(wp->getLongitude()));
        path.append('|');
    }

    path.chop(1); // removes the last extra '|'

    m_url = GoogleElevationBaseUrl;
    m_url.addQueryItem("path", path);
    m_url.addQueryItem("samples", QString::number(samples));
    QLOG_DEBUG() << "elevation query :" << m_url;

    if (m_networkReply != NULL){
        delete m_networkReply;
        m_networkReply = NULL;
    }
    m_networkReply = m_networkAccessManager.get(QNetworkRequest(m_url));
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void GoogleElevationData::processDownloadedObject(const QString &jsonObject)
{
    QLOG_TRACE() << "Elevation Response: " << jsonObject;
    QScriptSyntaxCheckResult syntaxCheck = QScriptEngine::checkSyntax(jsonObject);
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+jsonObject+")");

    if (engine.hasUncaughtException()){
        QLOG_ERROR() << "Error evaluating version object";
        QLOG_ERROR() << "Error @line#" << engine.uncaughtExceptionLineNumber();
        QLOG_ERROR() << "Backtrace:" << engine.uncaughtExceptionBacktrace();
        QLOG_ERROR() << "Syntax Check:" << syntaxCheck.errorMessage();
        QLOG_ERROR() << "Syntax Check line:" << syntaxCheck.errorLineNumber()
                     << " col:" << syntaxCheck.errorColumnNumber();
        return;
    }

    QList<Waypoint*> elevationWaypoints;

    QScriptValue entries = result.property("results");
    QScriptValueIterator it(entries);
    while (it.hasNext()){
        it.next();
        QScriptValue entry = it.value();

        double elevation = entry.property("elevation").toNumber();
        double latitude = entry.property("location").property("lat").toNumber();
        double longitude = entry.property("location").property("lng").toNumber();
//        double resolution = entry.property("resolution").toNumber();

        QLOG_DEBUG() << "elevation loc: " << latitude << "," << longitude;
        QLOG_DEBUG() << "elevation alt" << elevationWaypoints.count() << ":" << elevation;

        Waypoint* wp = new Waypoint(elevationWaypoints.count(), latitude, longitude, elevation,
                                    0.0,0.0,0.0,0.0,true,false,MAV_FRAME_GLOBAL);
        elevationWaypoints.append(wp);
    }

    elevationWaypoints.removeLast(); // the last one seems defunct.

    emit elevationDataReady(elevationWaypoints);
}
