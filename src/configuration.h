#ifndef QGC_CONFIGURATION_H
#define QGC_CONFIGURATION_H

#include "globalobject.h"
#include <QString>
#include <QDateTime>
#include <QDir>
#include <QCoreApplication>

/** @brief Polling interval in ms */
#define SERIAL_POLL_INTERVAL 100

/** @brief Heartbeat emission rate, in Hertz (times per second) */
#define MAVLINK_HEARTBEAT_DEFAULT_RATE 1
#define WITH_TEXT_TO_SPEECH 1

#define QGC_APPLICATION_NAME "APM Planner"
#define QGC_APPLICATION_VERSION "v2.0.6"
#define APP_DATA_DIRECTORY "/apmplanner2"
#define LOG_DIRECTORY "/dataflashLogs"
#define PARAMETER_DIRECTORY "/parameters"
#define MAVLINK_LOG_DIRECTORY "/tlogs"
#define MAVLINK_LOGFILE_EXT ".tlog"

namespace QGC

{
const QString APPNAME = "APMPLANNER2";
const QString COMPANYNAME = "DIYDRONES";
const int APPLICATIONVERSION = 206; // 2.0.0

    inline void close(){
        GlobalObject* global = GlobalObject::sharedInstance();
        delete global;
        global = NULL;
    }

    inline void loadSettings(){
        GlobalObject::sharedInstance()->loadSettings();
    }

    inline void saveSettings(){
        GlobalObject::sharedInstance()->saveSettings();
    }

    inline QString fileNameAsTime(){
        return GlobalObject::sharedInstance()->fileNameAsTime();
    }

    inline bool makeDirectory(const QString& dir){
        return GlobalObject::sharedInstance()->makeDirectory(dir);
    }

    inline QString appDataDirectory(){
        return GlobalObject::sharedInstance()->appDataDirectory();
    }

    inline void setAppDataDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setAppDataDirectory(dir);
    }

    inline QString MAVLinkLogDirectory(){
        return GlobalObject::sharedInstance()->MAVLinkLogDirectory();
    }

    inline void setMAVLinkLogDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setMAVLinkLogDirectory(dir);
    }

    inline QString logDirectory(){
        return GlobalObject::sharedInstance()->logDirectory();
    }

    inline void setLogDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setLogDirectory(dir);
    }

    inline QString parameterDirectory(){
        return GlobalObject::sharedInstance()->parameterDirectory();
    }

    inline void setParameterDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setParameterDirectory(dir);
    }

    //Returns the absolute parth to the files, data, qml support directories
    //It could be in 1 of 2 places under Linux
    inline QString shareDirectory(){
        return GlobalObject::sharedInstance()->shareDirectory();
    }

}

#endif // QGC_CONFIGURATION_H
