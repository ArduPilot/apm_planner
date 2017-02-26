/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2016 APM_PLANNER PROJECT <http://www.ardupilot.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file
 *   @brief AP2DataPlot log loader thread
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Arne Wischmann <wischmann-a@gmx.de>
 */

#include <QTextBlock>
#include "AP2DataPlotThread.h"
#include "logging.h"
#include "Loghandling/BinLogParser.h"
#include "Loghandling/AsciiLogParser.h"
#include "Loghandling/TlogParser.h"


AP2DataPlotThread::AP2DataPlotThread(LogdataStorage::Ptr storagePtr, QObject *parent) :
    QThread(parent),
    m_stop(false),
    m_dataStoragePtr(storagePtr),
    mp_logParser(0)
{
    QLOG_DEBUG() << "Created AP2DataPlotThread:" << this;
    qRegisterMetaType<MAV_TYPE>("MAV_TYPE");
    qRegisterMetaType<AP2DataPlotStatus>("AP2DataPlotStatus");
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCursor>("QTextCursor");
}

AP2DataPlotThread::~AP2DataPlotThread()
{
    QLOG_DEBUG() << "Destroyed AP2DataPlotThread:" << this;
}

void AP2DataPlotThread::loadFile(const QString &file)
{
    Q_ASSERT(isMainThread());
    m_fileName = file;
    start();
}

bool AP2DataPlotThread::isMainThread()
{
    return QThread::currentThread() == QCoreApplication::instance()->thread();
}

void AP2DataPlotThread::stopLoad()
{
    m_stop = true;
    if(mp_logParser)
    {
        mp_logParser->stopParsing();
    }
}

void AP2DataPlotThread::onProgress(const qint64 pos, const qint64 size)
{
    emit loadProgress(pos, size);
}

void AP2DataPlotThread::onError(const QString &errorMsg)
{
    emit error(errorMsg);
}


void AP2DataPlotThread::run()
{
    Q_ASSERT(!isMainThread());
    emit startLoad();
    AP2DataPlotStatus plotState;
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();

    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file ("  +  m_fileName + ")");
        return;
    }

    QLOG_DEBUG() << "AP2DataPlotThread::run(): Log loading start -" << logfile.size() << "bytes";

    if (m_fileName.toLower().endsWith(".bin"))
    {
        //It's a binary file
        BinLogParser parser(m_dataStoragePtr, this);
        mp_logParser = &parser;
        plotState = parser.parse(logfile);
        mp_logParser = 0;
    }
    else if (m_fileName.toLower().endsWith(".log"))
    {
        //It's a ascii log.
        AsciiLogParser parser(m_dataStoragePtr, this);
        mp_logParser = &parser;
        plotState = parser.parse(logfile);
        mp_logParser = 0;
    }
    else if (m_fileName.toLower().endsWith(".tlog"))
    {
        //It's a tlog
        TlogParser parser(m_dataStoragePtr, this);
        mp_logParser = &parser;
        plotState = parser.parse(logfile);
        mp_logParser = 0;
    }
    else
    {
        emit error("Unable to detect file type from filename. Ensure the file has a .bin or .log extension");
        return;
    }


    if (m_stop)
    {
        QLOG_ERROR() << "Plot Log loading was canceled after" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0
                     << "seconds -" << logfile.pos() << "of" << logfile.size() << "bytes";
        emit error("Log loading Canceled");
    }
    else
    {
        QLOG_INFO() << "Plot Log loading took" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds -"
                    << logfile.pos() << "of" << logfile.size() << "bytes used";
        emit done(plotState);
    }
}




