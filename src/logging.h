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

// Defines for configuring the used loglevels
// just uncomment if you want to hide one level
#define NO_TRACE
//#define NO_DEBUG
//#define NO_INFO
//#define NO_WARN
//#define NO_ERROR


// logging macros
#ifdef NO_TRACE
    #define QLOG_TRACE() if(1){} else qDebug()
#else
    #define QLOG_TRACE() qDebug()
#endif

#ifdef NO_DEBUG
    #define QLOG_DEBUG() if(1){} else qDebug()
#else
    #define QLOG_DEBUG() qDebug()
#endif

#ifdef NO_INFO
    #define QLOG_INFO()  if(1){} else qInfo()
#else
    #if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
        #define QLOG_INFO() qDebug()
    #else
        #define QLOG_INFO() qInfo()
    #endif
#endif

#ifdef NO_WARN
    #define QLOG_WARN()  if(1){} else qWarning()
#else
    #define QLOG_WARN() qWarning()
#endif

#ifdef NO_ERROR
    #define QLOG_ERROR() if(1){} else qCritical()
#else
    #define QLOG_ERROR() qCritical()
#endif

#ifdef NO_ERROR
    #define QLOG_FATAL() if(1){} else qCritical()
#else
    #define QLOG_FATAL() qCritical()
#endif

#endif // LOGGING_H

