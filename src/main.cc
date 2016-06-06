/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009 - 2011 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Main executable
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#include "QGCCore.h"
#include "MainWindow.h"
#include "configuration.h"
#include "logging.h"
#include <QtWidgets/QApplication>
#include <fstream>

/* SDL does ugly things to main() */
#ifdef main
#undef main
#endif


// Install a message handler so you do not need
// the MSFT debug tools installed to se
// qDebug(), qWarning(), qCritical and qAbort
#ifdef Q_OS_WIN
void msgHandler( QtMsgType type, const char* msg )
{
    const char symbols[] = { 'I', 'E', '!', 'X' };
    QString output = QString("[%1] %2").arg( symbols[type] ).arg( msg );
    std::cerr << output.toStdString() << std::endl;
    if( type == QtFatalMsg ) abort();
}
#endif

// Path for file logging
static QString sLogPath;

// Message handler for logging provides console and file output
// The handler itself has to be reentrant and threadsafe!
void loggingMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    // The message handler has to be thread safe
    static QMutex mutex;
    QMutexLocker localLoc(&mutex);

    static std::ofstream logFile(sLogPath.toStdString().c_str(), std::ofstream::out | std::ofstream::app);

    if(logFile)
    {
        logFile << qPrintable(qFormatLogMessage(type, context, message)) << std::endl;
    }

    fprintf(stderr, "%s\n", qPrintable(qFormatLogMessage(type, context, message)));
}


/**
 * @brief Starts the application
 *
 * @param argc Number of commandline arguments
 * @param argv Commandline arguments
 * @return exit code, 0 for normal exit and !=0 for error cases
 */
int main(int argc, char *argv[])
{
// install the message handler
#ifdef Q_OS_WIN
    //qInstallMsgHandler( msgHandler );
#endif


    // init the logging mechanism
#ifdef ENABLE_QS_LOG
    // disabled due to issue #941
    QsLogging::Logger& logger = QsLogging::Logger::instance();
    logger.setLoggingLevel(QsLogging::DebugLevel);

    const QString sLogPath(QDir(QGC::appDataDirectory()).filePath("log.txt"));

    QsLogging::DestinationPtr fileDestination(
    QsLogging::DestinationFactory::MakeFileDestination(sLogPath,
                                                       QsLogging::EnableLogRotation,
                                                       QsLogging::MaxSizeBytes(0),
                                                        QsLogging::MaxOldLogCount(5)) );
    QsLogging::DestinationPtr debugDestination(
       QsLogging::DestinationFactory::MakeDebugOutputDestination() );
    logger.addDestination(debugDestination);
    logger.addDestination(fileDestination);

#else
    // create filename and path for logfile like "apmlog_20160529.txt"
    // one logfile for every day. Size is not limited
    QString logFileName("apmlog_");
    logFileName.append(QDateTime::currentDateTime().toString("yyyyMMdd"));
    logFileName.append(".txt");
    sLogPath = QString(QDir(QGC::appDataDirectory()).filePath(logFileName));

    // Just keep the 5 recent logfiles and delete the rest.
    QDir logDirectory(QGC::appDataDirectory(), "apmlog*", QDir::Name, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QStringList logFileList(logDirectory.entryList());
    // As the file list is sorted we can delete index 0 cause its the oldest one
    while(logFileList.size() > 5)
    {
        logDirectory.remove(logFileList.at(0));
        logFileList.pop_front();
    }

    // Add sperator for better orientation in Logfiles
    std::ofstream logFile(sLogPath.toStdString().c_str(), std::ofstream::out | std::ofstream::app);
    if (logFile)
    {
        logFile << std::endl << std::endl << "**************************************************" << std::endl << std::endl;
        logFile.close();
    }

    // set up logging pattern
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    // QT < 5.5.x does not support qInfo() logging macro and no info-formatting too
    QString logPattern("[%{time yyyyMMdd h:mm:ss.zzz} %{if-debug}DEBUG%{endif}%{if-warning}WARN %{endif}%{if-critical}ERROR%{endif}%{if-fatal}FATAL%{endif}] - %{message}");
#else
    QString logPattern("[%{time yyyyMMdd h:mm:ss.zzz} %{if-debug}DEBUG%{endif}%{if-info}INFO %{endif}%{if-warning}WARN %{endif}%{if-critical}ERROR%{endif}%{if-fatal}FATAL%{endif}] - %{message}");
#endif

    qSetMessagePattern(logPattern);

    // install the message handler for logging
    qInstallMessageHandler(loggingMessageHandler);
#endif

    // start the application
    QGCCore core(argc, argv);
    core.initialize();
    return core.exec();
}
