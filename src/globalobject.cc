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

    //    autoReconnect = settings.value("AUTO_RECONNECT", autoReconnect).toBool();
    //    currentStyle = (QGC_MAINWINDOW_STYLE)settings.value("CURRENT_STYLE", currentStyle).toInt();
    //    lowPowerMode = settings.value("LOW_POWER_MODE", lowPowerMode).toBool();

    settings.endGroup();
}

void GlobalObject::saveSettings()
{
    QSettings settings;
    settings.beginGroup("GLOBAL_SETTINGS");
    settings.setValue("APP_DATA_DIRECTORY", m_appDataDirectory);
    settings.setValue("LOG_DIRECTORY", m_logDirectory);
    settings.setValue("MAVLINK_LOG_DIRECTORY", m_MAVLinklogDirectory);
    settings.setValue("PARAMETER_DIRECTORY", m_parameterDirectory);

    //    settings.setValue("AUTO_RECONNECT", autoReconnect);
    //    settings.setValue("CURRENT_STYLE", currentStyle);
    //    settings.setValue("LOW_POWER_MODE", lowPowerMode);

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
    QString homeDir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
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
    m_appDataDirectory = dir;
}

//
// Log Data Directory
//

QString GlobalObject::defaultLogDirectory()
{
    QString homeDir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
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
    m_logDirectory = dir;
}

//
// MAVLink Log Data Directory
//

QString GlobalObject::defaultMAVLinkLogDirectory()
{
    QString homeDir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
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
    m_MAVLinklogDirectory = dir;
}

//
// Parameter Data Directory
//

QString GlobalObject::defaultParameterDirectory()
{
    QString homeDir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
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
    m_parameterDirectory = dir;
}
