#ifndef QGC_CONFIGURATION_H
#define QGC_CONFIGURATION_H

#include "globalobject.h"
#include <QString>
#include <QDateTime>

/** @brief Polling interval in ms */
#define SERIAL_POLL_INTERVAL 100

/** @brief Heartbeat emission rate, in Hertz (times per second) */
#define MAVLINK_HEARTBEAT_DEFAULT_RATE 1
#define WITH_TEXT_TO_SPEECH 1

#define QGC_APPLICATION_NAME "APM Planner"
#define QGC_APPLICATION_VERSION "v2.0.0 (RC2)"
#define APP_DATA_DIRECTORY "/apmplanner2"
#define LOG_DIRECTORY "/dataflashLogs"
#define PARAMETER_DIRECTORY "/parameters"
#define MAVLINK_LOG_DIRECTORY "/tlogs"
#define MAVLINK_LOGFILE_EXT ".tlog"

namespace QGC

{
const QString APPNAME = "APMPLANNER2";
const QString COMPANYNAME = "DIYDRONES";
const int APPLICATIONVERSION = 200; // 2.0.0

    static void close(){
        GlobalObject* global = GlobalObject::sharedInstance();
        delete global;
        global = NULL;
    }

    static void loadSettings(){
        GlobalObject::sharedInstance()->loadSettings();
    }

    static void saveSettings(){
        GlobalObject::sharedInstance()->saveSettings();
    }

    static QString fileNameAsTime(){
        return GlobalObject::sharedInstance()->fileNameAsTime();
    }

    static bool makeDirectory(const QString& dir){
        return GlobalObject::sharedInstance()->makeDirectory(dir);
    }

    static QString appDataDirectory(){
        return GlobalObject::sharedInstance()->appDataDirectory();
    }

    static void setAppDataDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setAppDataDirectory(dir);
    }

    static QString MAVLinkLogDirectory(){
        return GlobalObject::sharedInstance()->MAVLinkLogDirectory();
    }

    static void setMAVLinkLogDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setMAVLinkLogDirectory(dir);
    }

    static QString logDirectory(){
        return GlobalObject::sharedInstance()->logDirectory();
    }

    static void setLogDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setLogDirectory(dir);
    }

    static QString parameterDirectory(){
        return GlobalObject::sharedInstance()->parameterDirectory();
    }

    static void setParameterDirectory(const QString& dir){
        GlobalObject::sharedInstance()->setParameterDirectory(dir);
    }

}

#endif // QGC_CONFIGURATION_H
