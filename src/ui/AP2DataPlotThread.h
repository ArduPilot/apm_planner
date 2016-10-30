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
#include <QSemaphore>
#include "MAVLinkDecoder.h"
#include "AP2DataPlot2DModel.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

#include "LogParser/IParserCallback.h"
#include "LogParser/ILogParser.h"



class AP2DataPlotThread : public QThread, public IParserCallback
{
    Q_OBJECT
public:
    explicit AP2DataPlotThread(AP2DataPlot2DModel *model,QObject *parent = 0);
    ~AP2DataPlotThread();

    void loadFile(const QString& file);

    void stopLoad();

    virtual void onProgress(const qint64 pos, const qint64 size);

    virtual void onError(const QString &errorMsg);

    /**
     * @brief allowToTerminate is part of a workaround for a crash resulting
     *        from an unknown effect in QFontEngineFT which randomly happens
     *        if this thread terminates before the method is finished which
     *        is triggered through a signal. allowToTerminate is used by the
     *        method which signalled to tell that its done.
     * @attention This method has to be called to allow the thread to terminate
     *            the thread will wait forever if not called.
     */
    void allowToTerminate() {m_workAroundSemaphore.release(1);}

signals:
    void startLoad();
    void loadProgress(qint64 pos,qint64 size);
    void payloadDecoded(int index,QString name,QVariantMap map);
    void done(AP2DataPlotStatus state,MAV_TYPE type);
    void error(QString errorstr);
    void lineRead(QString line);

private:
    /**
     * @brief The timeStampType struct
     *        Used to hold the name and the scaling of a time stamp.
     */
    struct timeStampType
    {
        QString m_name;     /// Name of the time stamp
        double  m_divisor;  /// Divisor to scale time stamp to seconds

        timeStampType() : m_divisor(0.0) {}
        timeStampType(const QString &name, const double divisor) : m_name(name), m_divisor(divisor) {}
    };

    /**
     * @brief The typeDescriptor struct
     *        Used to hold all data needed to describe a message type
     */
    struct typeDescriptor
    {
        int m_length;       /// Length of the message
        QString m_name;     /// Name of the message
        QString m_format;   /// Format string like "QbbI"
        QString m_labels;   /// Name of each value in message. Comma seperated like "lat,lon,time"

        typeDescriptor() : m_length(0){}
        typeDescriptor(int length, const QString &name, const QString &format, const QString &labels) :
            m_length(length), m_name(name), m_format(format), m_labels(labels){}
    };

    void run(); // from QThread;
    bool isMainThread();

    void loadBinaryLog(QFile &logfile);
    void loadAsciiLog(QFile &logfile);
    void loadTLog(QFile &logfile);

    /**
     * @brief addTimeToDescriptor - helper function for parsing. Extends a type descriptor to hold
     *        a timestamp
     */
    void addTimeToDescriptor(typeDescriptor &desc);

    /**
     * @brief adaptGPSDescriptor - helper function for parsing. Manipulates a GPS type descriptor
     *        by renaming old time stamp name and adding a new one. Needed cause the GPS time does not
     *        match other times
     */
    void adaptGPSDescriptor(QMap<unsigned int, typeDescriptor> &typeToDescriptorMap, typeDescriptor &desc, unsigned char msg_type);

    /**
     * @brief adaptGPSDescriptor - helper function for parsing. Manipulates a GPS type descriptor
     *        by renaming old time stamp name and adding a new one. Needed cause the GPS time does not
     *        match other times
     */
    bool adaptGPSDescriptor(QMap<QString, typeDescriptor> &nameToDescriptorMap, typeDescriptor &desc);

    /**
     * @brief handleMissingTimeStamps - helper function for parsing. Checks if a timestamp has to be added
     *        and adds it if needed. If the data already contains a valid time stamp its value is stored
     *        in lastValidTS. lastValidTS is used to set the time stamp of the data which lacks it.
     *
     * @param timeStampHasToBeAdded - Collection holding all message keys of the messages wothout a time stamp
     * @param desc - typeDescriptor of the actual message
     * @param valuepairlist - extracted data pairs of the message
     * @param lastValidTS - last valid time stamp
     * @param index - actual parsing index
     */
    void handleMissingTimeStamps(const QStringList &timeStampHasToBeAdded, const QString &name, QList<QPair<QString,QVariant> > &valuepairlist,
                                 quint64 &lastValidTS, const int index);

    /**
     * @brief handleMissingTimeStamps - helper for parsing.
     * @see handleMissingTimeStamps
     */
    void handleMissingTimeStamps(const QList<unsigned int> &timeStampHasToBeAdded, const unsigned char type, QList<QPair<QString,QVariant> > &valuepairlist,
                                 quint64 &lastValidTS, const int index);

    /**
     * @brief getTimeStamp - extracts a valid time stamp from valuepair list and sets lastValidTS.
     *        checks if the time stamps are increasing. Used by handleMissingTimeStamps methods
     */
    void getTimeStamp(QList<QPair<QString,QVariant> > &valuepairlist, const int index, quint64 &lastValidTS);
private:

    QString m_fileName;
    bool m_stop;
    MAV_TYPE m_loadedLogType;
    AP2DataPlot2DModel *m_dataModel;
    ILogParser         *mp_logParser;

    AP2DataPlotStatus m_plotState;
    QList<timeStampType> m_possibleTimestamps;
    timeStampType m_timeStamp;

    /**
     * @brief m_workAroundSemaphore is part of the workaround @see allowToTerminate
     *        is used for too. It is used to block the thread until allowToTerminate
     *        is called
     */
    QSemaphore m_workAroundSemaphore;



};




#endif // AP2DATAPLOTTHREAD_H
