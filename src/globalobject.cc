#include "QsLog.h"
#include "configuration.h"
#include "globalobject.h"
#include <QSettings>
#include <QDateTime>
#include <QDir>
#include <QDesktopServices>

GlobalObject* GlobalObject::sharedInstance()
{
    static GlobalObject* _globalInstance = NULL;
    if (_globalInstance) {
        return _globalInstance;
    }
    // Create the global object
    _globalInstance = new GlobalObject();
    return _globalInstance;
}

GlobalObject::GlobalObject()
{
    loadSettings();
}

GlobalObject::~GlobalObject()
{
}

void GlobalObject::loadSettings()
{
    QSettings settings;
    settings.beginGroup("GLOBAL_SETTINGS");
    m_appDataDirectory = settings.value("APP_DATA_DIRECTORY", defaultAppDataDirectory()).toString();
    m_logDirectory = settings.value("LOG_DIRECTORY", defaultLogDirectory()).toString();
    m_MAVLinklogDirectory = settings.value("MAVLINK_LOG_DIRECTORY", defaultMAVLinkLogDirectory()).toString();
    m_parameterDirectory = settings.value("PARAMETER_DIRECTORY", defaultParameterDirectory()).toString();

    settings.endGroup();
}

void GlobalObject::saveSettings()
{
    QSettings settings;
    settings.beginGroup("GLOBAL_SETTINGS");
    settings.setValue("APP_DATA_DIRECTORY", m_appDataDirectory);
    settings.setValue("LOG_DIRECTORY", m_logDirectory);
    settings.setValue("MAVLINK_LOG_DIRECTORY", m_MAVLinklogDirectory);
    QLOG_DEBUG() << "save tlog dir to:" << m_MAVLinklogDirectory;
    settings.setValue("PARAMETER_DIRECTORY", m_parameterDirectory);

    settings.sync();
}

QString GlobalObject::fileNameAsTime()
{
    QDateTime timeNow;
    timeNow = timeNow.currentDateTime();
    return "/" + timeNow.toString("yyyy-MM-dd hh-mm-ss") + MAVLINK_LOGFILE_EXT;
}

bool GlobalObject::makeDirectory(const QString& dir)
{
    QDir newDir(dir);
    if (!newDir.exists()){
        return newDir.mkpath(dir);
    }
    return true;
}

//
// App Data Directory
//

QString GlobalObject::defaultAppDataDirectory()
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString appHomeDir = homeDir + APP_DATA_DIRECTORY;
    makeDirectory(appHomeDir);
    return appHomeDir;
}

QString GlobalObject::appDataDirectory()
{
    makeDirectory(m_appDataDirectory);
    return m_appDataDirectory;
}

void GlobalObject::setAppDataDirectory(const QString &dir)
{
    QLOG_DEBUG() << "Set app dir to:" << dir;
    m_appDataDirectory = dir;
}

//
// Log Data Directory
//

QString GlobalObject::defaultLogDirectory()
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString logHomeDir = homeDir + APP_DATA_DIRECTORY + LOG_DIRECTORY;
    makeDirectory(logHomeDir);
    return logHomeDir;
}

QString GlobalObject::logDirectory()
{
    makeDirectory(m_logDirectory);
    return m_logDirectory;
}

void GlobalObject::setLogDirectory(const QString &dir)
{
    QLOG_DEBUG() << "Set dataflash dir to:" << dir;
    m_logDirectory = dir;
}

//
// MAVLink Log Data Directory
//

QString GlobalObject::defaultMAVLinkLogDirectory()
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString logHomeDir = homeDir + APP_DATA_DIRECTORY + MAVLINK_LOG_DIRECTORY;
    makeDirectory(logHomeDir);
    return logHomeDir;
}

QString GlobalObject::MAVLinkLogDirectory()
{
    makeDirectory(m_MAVLinklogDirectory);
    return m_MAVLinklogDirectory;
}

void GlobalObject::setMAVLinkLogDirectory(const QString &dir)
{
    QLOG_DEBUG() << "Set tlog dir to:" << dir;
    m_MAVLinklogDirectory = dir;
}

//
// Parameter Data Directory
//

QString GlobalObject::defaultParameterDirectory()
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString paramHomeDir = homeDir + APP_DATA_DIRECTORY + PARAMETER_DIRECTORY;
    makeDirectory(paramHomeDir);   return paramHomeDir;
}

QString GlobalObject::parameterDirectory()
{
    makeDirectory(m_parameterDirectory);
    return m_parameterDirectory;
}

void GlobalObject::setParameterDirectory(const QString &dir)
{
    QLOG_DEBUG() << "Set param dir to:" << dir;
    m_parameterDirectory = dir;
}

QString GlobalObject::shareDirectory()
{
#ifdef Q_OS_WIN
    QDir settingsDir = QDir(QDir::currentPath());
    return  settingsDir.absolutePath();
#elif defined(Q_OS_MAC)
    return QCoreApplication::applicationDirPath();
#else
    QDir settingsDir = QDir(QDir::currentPath());
    if(settingsDir.exists("data") && settingsDir.exists("qml"))
    {
        return  settingsDir.absolutePath();
    }
    settingsDir.cdUp();
    settingsDir.cd("./share/APMPlanner2");
    if(settingsDir.exists("data") && settingsDir.exists("qml"))
    {
        QString tmp = settingsDir.absolutePath();
        return  settingsDir.absolutePath();
    }
    settingsDir = QDir("/usr/share/APMPlanner2");
    if(settingsDir.exists("data") && settingsDir.exists("qml"))
    {
        QString tmp = settingsDir.absolutePath();
        return  settingsDir.absolutePath();
    }
    settingsDir = QDir("/usr/local/share/APMPlanner2");
    if(settingsDir.exists("data") && settingsDir.exists("qml"))
    {
        QString tmp = settingsDir.absolutePath();
        return  settingsDir.absolutePath();
    }
    //else
    return QDir(QDir::currentPath()).absolutePath();

#endif
}
