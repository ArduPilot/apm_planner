/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2015 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 */


#ifndef AP2DATAPLOTTHREAD_H
#define AP2DATAPLOTTHREAD_H

#include <QThread>
#include <QVariantMap>
#include <QSqlDatabase>
#include "MAVLinkDecoder.h"
#include "AP2DataPlot2DModel.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

class AP2DataPlotThread : public QThread
{
    Q_OBJECT
public:
    explicit AP2DataPlotThread(AP2DataPlot2DModel *model,QObject *parent = 0);
    ~AP2DataPlotThread();

    void loadFile(const QString& file);
    void stopLoad() { m_stop = true; }

signals:
    void startLoad();
    void loadProgress(qint64 pos,qint64 size);
    void payloadDecoded(int index,QString name,QVariantMap map);
    void done(int errors,MAV_TYPE type);
    void error(QString errorstr);
    void lineRead(QString line);

private:
    void run(); // from QThread;
    bool isMainThread();

    void loadDataFieldsFromValues();
    void loadBinaryLog();
    void loadAsciiLog();
    void loadTLog();

private:
    QString m_fileName;
    bool m_stop;
    int m_fieldCount;
    int m_errorCount;
    MAV_TYPE m_loadedLogType;
    MAVLinkDecoder *m_decoder;
    AP2DataPlot2DModel *m_dataModel;
    QMap<QString,QString> m_msgNameToInsertQuery;
};

#endif // AP2DATAPLOTTHREAD_H
