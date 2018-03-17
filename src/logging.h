//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    (c) 2016 Author: Arne Wischmann <wischmann-a@gmx.de>
//

#ifndef LOGGING_H
#define LOGGING_H

#include <QDebug>
#include <QtGlobal>
#include <QLoggingCategory>

// Declare the base logging category - creation is done in main.cc
Q_DECLARE_LOGGING_CATEGORY(apmGeneral);

// Define for enabling trace logging. As trace logging is not supported
// by the Qt logging framework the trace level is handeled by the debug level.
// Disabling the NO_TRACE define enabes the trace logging
#define NO_TRACE

// logging macro
#ifdef NO_TRACE
    #define QLOG_TRACE() if(1){} else qDebug()
#else
    #define QLOG_TRACE() qCDebug(apmGeneral)
#endif


// The loglevels which have a corresponding Qt logging class (debug, info, warning) can be disabled
// by using the Qt defines (QT_NO_DEBUG_OUTPUT, QT_NO_INFO_OUTPUT, QT_NO_WARNING_OUTPUT) at
// compile time.
#define QLOG_DEBUG() qCDebug(apmGeneral)

// Qt versions below 5.5.0 does not support qInfo() or qcinfo() logging
#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
    #define QLOG_INFO() qCDebug(apmGeneral)
#else
    #define QLOG_INFO() qCInfo(apmGeneral)
#endif


#define QLOG_WARN() qCWarning(apmGeneral)

#define QLOG_ERROR() qCCritical(apmGeneral)

#define QLOG_FATAL() qCCritical(apmGeneral)


#endif // LOGGING_H

