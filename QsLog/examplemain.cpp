#include "QsLog.h"
#include "QsLogDest.h"
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <iostream>

int main(int argc, char *argv[])
{
   QCoreApplication a(argc, argv);

   // init the logging mechanism
   QsLogging::Logger& logger = QsLogging::Logger::instance();
   logger.setLoggingLevel(QsLogging::TraceLevel);
   const QString sLogPath(QDir(a.applicationDirPath()).filePath("log.txt"));

   QsLogging::DestinationPtr fileDestination(
      QsLogging::DestinationFactory::MakeFileDestination(sLogPath, true, 512, 2) );
   QsLogging::DestinationPtr debugDestination(
      QsLogging::DestinationFactory::MakeDebugOutputDestination() );
   logger.addDestination(debugDestination);
   logger.addDestination(fileDestination);

   QLOG_INFO() << "Program started";
   QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

   QLOG_TRACE() << "Here's a" << QString::fromUtf8("trace") << "message";
   QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
   QLOG_WARN()  << "Uh-oh!";
   qDebug() << "This message won't be picked up by the logger";
   QLOG_ERROR() << "An error has occurred";
   qWarning() << "Neither will this one";
   QLOG_FATAL() << "Fatal error!";

   logger.setLoggingLevel(QsLogging::OffLevel);
   for (int i = 0;i < 10000000;++i) {
       QLOG_ERROR() << QString::fromUtf8("logging is turned off");
   }

   return 0;
}
