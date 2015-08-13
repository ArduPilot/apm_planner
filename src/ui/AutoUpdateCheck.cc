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
 *   @brief Droneshare API Query Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#include "QsLog.h"
#include "AutoUpdateCheck.h"
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QMessageBox>
#include <QSettings>
#include "QGC.h"
#include "versioncomparator.h"

AutoUpdateCheck::AutoUpdateCheck(QObject *parent) :
    QObject(parent),
    m_networkReply(NULL),
    m_httpRequestAborted(false),
    m_suppressNoUpdateSignal(false)
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

        if ((platform == define2string(APP_PLATFORM)) && (type == m_releaseType)){
            if (compareVersionStrings(version,QGC_APPLICATION_VERSION)){
                QLOG_DEBUG() << "Found New Version: " << platform << " "
                            << type << " " << version << " " << locationUrl;
                if(m_skipVersion != version){
                    emit updateAvailable(version, type, locationUrl, name);
                } else {
                    QLOG_INFO() << "Version Skipped at user request";
                }
                break;
            } else {
                QLOG_INFO() << "no new update available";
                if (!m_suppressNoUpdateSignal){
                    emit noUpdateAvailable();
                }
                m_suppressNoUpdateSignal = false;
            }
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
    return VersionComparator::isVersionNewer(newVersion, currentVersion);
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
