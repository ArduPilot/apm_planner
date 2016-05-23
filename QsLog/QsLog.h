// Copyright (c) 2013, Razvan Petru
// All rights reserved.

// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:

// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this
//   list of conditions and the following disclaimer in the documentation and/or other
//   materials provided with the distribution.
// * The name of the contributors may not be used to endorse or promote products
//   derived from this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef QSLOG_H
#define QSLOG_H

#include "QsLogLevel.h"
#include "QsLogDest.h"
#include <QDebug>
#include <QString>
#include <QDateTime>

#define QS_LOG_VERSION "2.0b3"

namespace QsLogging
{
class Destination;
class LoggerImpl; // d pointer

class QSLOG_SHARED_OBJECT Logger
{
public:
    static Logger& instance();
    static void destroyInstance();
    static Level levelFromLogMessage(const QString& logMessage, bool* conversionSucceeded = 0);

    ~Logger();

    //! Adds a log message destination. Don't add null destinations.
    void addDestination(DestinationPtr destination);
    //! Logging at a level < 'newLevel' will be ignored
    void setLoggingLevel(Level newLevel);
    //! The default level is INFO
    Level loggingLevel() const;
    //! Set to false to disable timestamp inclusion in log messages
    void setIncludeTimestamp(bool e);
    //! Default value is true.
    bool includeTimestamp() const;
    //! Set to false to disable log level inclusion in log messages
    void setIncludeLogLevel(bool l);
    //! Default value is true.
    bool includeLogLevel() const;

    //! The helper forwards the streaming to QDebug and builds the final
    //! log message.
    class QSLOG_SHARED_OBJECT Helper
    {
    public:
        explicit Helper(Level logLevel) :
            level(logLevel),
            qtDebug(&buffer)
        {}
        ~Helper();
        QDebug& stream(){ return qtDebug; }

    private:
        void writeToLog();

        Level level;
        QString buffer;
        QDebug qtDebug;
	};

private:
    Logger();
    Logger(const Logger&);            // not available
    Logger& operator=(const Logger&); // not available

    void enqueueWrite(const QString& message, Level level);
    void write(const QString& message, Level level);

    LoggerImpl* d;

    friend class LogWriterRunnable;
};

} // end namespace

//! Logging macros: define QS_LOG_LINE_NUMBERS to get the file and line number
//! in the log output.
#ifndef QS_LOG_LINE_NUMBERS
#define QLOG_TRACE() \
    if(1){} \
    else qDebug() << "TRACE" << QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz")
#define QLOG_DEBUG() \
    qDebug() << "DEBUG" << QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz")
#define QLOG_INFO()  \
    qInfo() << "INFO " << QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz")
#define QLOG_WARN()  \
    qWarning() << "WARN " << QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz")
#define QLOG_ERROR() \
    qCritical() << "ERROR" << QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz")
#define QLOG_FATAL() \
    qCritical() << "FATAL" << QDateTime::currentDateTime().toString("yyyy-MM-ddThh:mm:ss.zzz")
#else
#define QLOG_TRACE() \
    qDebug() << __FILE__ << '@' << __LINE__
#define QLOG_DEBUG() \
    qDebug() << __FILE__ << '@' << __LINE__
#define QLOG_INFO()  \
    qInfo() << __FILE__ << '@' << __LINE__
#define QLOG_WARN()  \
    qWarning() << __FILE__ << '@' << __LINE__
#define QLOG_ERROR() \
    qCritical() << __FILE__ << '@' << __LINE__
#define QLOG_FATAL() \
    qCritical() << __FILE__ << '@' << __LINE__
#endif

#ifdef QS_LOG_DISABLE
#include "QsLogDisableForThisFile.h"
#endif

#endif // QSLOG_H
