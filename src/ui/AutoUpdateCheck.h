#ifndef AUTOUPDATECHECK_H
#define AUTOUPDATECHECK_H

#include "configuration.h"
#include <QObject>
#include <QtNetwork>

const QString AUTOUPDATE_VERSION_OBJECT_LOCATION = "http://firmware.diydrones.com/Tools/APMPlanner/";
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
    void processDownloadedVersionObject(const QString& versionObject);
    bool compareVersionStrings(const QString& newVersion, const QString& currentVersion);

private:
    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    bool m_httpRequestAborted;

    bool m_isAutoUpdateEnabled;
    QString m_skipVersion;
    QString m_releaseType;
    bool m_suppressNoUpdateSignal;

};

#endif // AUTOUPDATECHECK_H
