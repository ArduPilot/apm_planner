#ifndef QGC_CONFIGURATION_H
#define QGC_CONFIGURATION_H

#include <QString>
#include <QDateTime>

/** @brief Polling interval in ms */
#define SERIAL_POLL_INTERVAL 9

/** @brief Heartbeat emission rate, in Hertz (times per second) */
#define MAVLINK_HEARTBEAT_DEFAULT_RATE 1
#define WITH_TEXT_TO_SPEECH 1

#define QGC_APPLICATION_NAME "APM Planner"
#define QGC_APPLICATION_VERSION "v2.0.0 (alpha-RC4)"
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

static QString fileNameAsTime() {
    QDateTime timeNow;
    timeNow = timeNow.currentDateTime();
    return timeNow.toString("yyyy-MM-dd hh-mm-ss") + MAVLINK_LOGFILE_EXT;
    }

}

#endif // QGC_CONFIGURATION_H
