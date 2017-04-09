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


#ifndef AP2DATAPLOTTHREAD_H
#define AP2DATAPLOTTHREAD_H

#include <QThread>
#include "Loghandling/IParserCallback.h"
#include "Loghandling/ILogParser.h"

#include "Loghandling/LogdataStorage.h"

/**
 * @brief The AP2DataPlotThread class provides the infrastucture for log parsing.
 *        It handles the thread and the signals and selects the parser type.
 */
class AP2DataPlotThread : public QThread, public IParserCallback
{
    Q_OBJECT
public:

    /**
     * @brief AP2DataPlotThread - CTOR
     * @param model - Pointer to the AP2DataPlot2DModel data stucture
     */
    explicit AP2DataPlotThread(LogdataStorage::Ptr storagePtr, QObject *parent = 0);

    /**
     * @brief ~AP2DataPlotThread - DTOR
     */
    ~AP2DataPlotThread();

    /**
     * @brief loadFile starts the parsing of file
     * @param file - filename of the file to parse
     */
    void loadFile(const QString& file);

    /**
     * @brief stopLoad stops the parsing process and forces
     *        the parser to return immediately
     */
    void stopLoad();

    /**
     * @copydoc IParserCallback::onProgress
     */
    virtual void onProgress(const qint64 pos, const qint64 size);

    /**
     * @copydoc IParserCallback::onError
     */
    virtual void onError(const QString &errorMsg);

signals:
    void startLoad();                           /// Emited as soon as log parsing starts
    void loadProgress(qint64 pos, qint64 size); /// Emited to show parsing progress
    void done(AP2DataPlotStatus state);         /// Emited as soon as the parsing is done
    void error(QString errorstr);               /// Emited on parsing error

private:

    QString m_fileName;     /// Filename of the file to be parsed
    bool m_stop;            /// true if parsing shall be stopped

    LogdataStorage::Ptr m_dataStoragePtr;   /// Pointer to the datamodel for storing the data

    ILogParser         *mp_logParser;   /// Pointer to active parser use to pass stop command

    void run();             /// from QThread - the thread;
    bool isMainThread();

};




#endif // AP2DATAPLOTTHREAD_H
