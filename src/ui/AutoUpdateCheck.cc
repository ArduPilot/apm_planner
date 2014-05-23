#include "QsLog.h"
#include "AutoUpdateCheck.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QMessageBox>
#include <QSettings>
#include "QGC.h"
AutoUpdateCheck::AutoUpdateCheck(QObject *parent) :
    QObject(parent),
    m_networkReply(NULL),
    m_httpRequestAborted(false)
{
    loadSettings();
}

void AutoUpdateCheck::forcedAutoUpdateCheck()
{
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
    QLOG_DEBUG() << "AutoUpdateCheck::httpFinished()";
    if (m_httpRequestAborted) {
        m_networkReply->deleteLater();
        m_networkReply = NULL;
        return;
    }

    // Finished donwloading the version information
    if (m_networkReply->error()) {
        // [TODO] cleanup download failed
#ifdef QT_DEBUG
        QMessageBox::information(NULL, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(m_networkReply->errorString()));
#endif
    } else {
        // Process downloadeed object
        processDownloadedVersionObject(QString(m_networkReply->readAll()));
    }

    m_networkReply->deleteLater();
    m_networkReply = NULL;
}

void AutoUpdateCheck::processDownloadedVersionObject(const QString &versionObject)
{
    QScriptSyntaxCheckResult syntaxCheck = QScriptEngine::checkSyntax(versionObject);
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+versionObject+")");

    if (engine.hasUncaughtException()){
        QLOG_ERROR() << "Error evaluating version object";
        QLOG_ERROR() << "Error @line#" << engine.uncaughtExceptionLineNumber();
        QLOG_ERROR() << "Backtrace:" << engine.uncaughtExceptionBacktrace();
        QLOG_ERROR() << "Syntax Check:" << syntaxCheck.errorMessage();
        QLOG_ERROR() << "Syntax Check line:" << syntaxCheck.errorLineNumber()
                     << " col:" << syntaxCheck.errorColumnNumber();
        return;
    }

    QScriptValue entries = result.property("releases");
    QScriptValueIterator it(entries);
    while (it.hasNext()){
        it.next();
        QScriptValue entry = it.value();

        QString platform = entry.property("platform").toString();
        QString type = entry.property("type").toString();
        QString version = entry.property("version").toString();
        QString name = entry.property("name").toString();
        QString locationUrl = entry.property("url").toString();

        if ((platform == define2string(APP_PLATFORM)) && (type == define2string(APP_TYPE))
            && (compareVersionStrings(version,QGC_APPLICATION_VERSION))){
            QLOG_DEBUG() << "Found New Version: " << platform << " "
                        << type << " " << version << " " << locationUrl;
            if(m_skipVerison != version){
                emit updateAvailable(version, type, locationUrl, name);
            } else {
                QLOG_DEBUG() << "Version Skipped at user request";
            }
            break;
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
    int newMajor,newMinor,newBuild = 0;
    int currentMajor, currentMinor,currentBuild = 0;

    QString newBuildSubMoniker, oldBuildSubMoniker; // holds if the build is a rc or dev build

    QRegExp versionEx("(\\d*\\.\\d+\\.?\\d?)-?(rc\\d)?");
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

    QRegExp versionEx2("(\\d*\\.\\d+\\.?\\d?)-?(rc\\d)?");
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
                        QLOG_DEBUG() << "Detected oldRc:" << versionEx.capturedTexts();
                        oldRc = releaseNumber.cap(0).toInt();
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
    m_skipVerison = version;
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
    m_skipVerison = settings.value("SKIP_VERSION", "0.0.0").toString();
    m_releaseType = settings.value("RELEASE_TYPE", APP_TYPE).toString();
    settings.endGroup();
}

void AutoUpdateCheck::writeSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("AUTO_UPDATE");
    settings.setValue("ENABLED", m_isAutoUpdateEnabled);
    settings.setValue("SKIP_VERSION", m_skipVerison);
    settings.setValue("RELEASE_TYPE", m_releaseType);
    settings.endGroup();
    settings.sync();
}
