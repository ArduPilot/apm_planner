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

/**
 * @brief The AP2DataPlotStatus class is a helper class desinged as status type for
 *        the log parsing.
 *        It contains the final state of parsing as well as all error strings inserted
 *        with the corruptDataRead() or corruptFMTRead() methods during the parsing process.
 */
class AP2DataPlotStatus
{
public:

    /**
     * @brief The parsingState enum
     *        All possible parsing states
     */
    enum parsingState
    {
        OK,                 /// Perfect result
        FmtError,           /// Corrupt Format description.
        TruncationError,    /// The log was truncated due to errors @ the end
        DataError           /// Data can be corrupted or incomplete
    };

    /**
     * @brief AP2DataPlotStatus CTOR
     */
    AP2DataPlotStatus();

    /**
     * @brief validDataRead
     *        Shall be called if a logline was read successful. Used to
     *        determine if the error(s) are only at the end of the log.
     *        Should be inline due to the high calling frequency.
     */
    inline void validDataRead()
    {
        if (m_parsingState == TruncationError)
        {
            m_parsingState = DataError;
        }
    }

    /**
     * @brief corruptDataRead
     *        Shall be called when ever an error occurs while parsing
     *        a data package.
     *
     * @param index - The log index the error occured
     * @param errorMessage - Error message describing the error reason
     */
    void corruptDataRead(const int index, const QString &errorMessage);

    /**
     * @brief corruptFMTRead
     *        Shall be called when ever an error occurs while parsing
     *        a format package.
     *
     * @param index - The log index the error occured
     * @param errorMessage - Error message describing the error reason
     */
    void corruptFMTRead(const int index, const QString &errorMessage);

    /**
     * @brief getParsingState
     *        Delivers the final state of the log parsing. The value
     *        is only valid if parsing is finished.
     *
     * @return - The parsing state - @see parsingState
     */
    parsingState getParsingState();

    /**
     * @brief getErrorText
     *        Creates a text containing all errormessages inserted during
     *        parsing. One line for each error.
     *
     * @return - multi line string with all error messages.
     */
    QString getErrorText();

private:
    typedef QPair<int, QString> errorEntry; /// Type for storing error index and text
    parsingState m_parsingState;            /// The internal parsing state
    QVector<errorEntry> m_errors;           /// For storing all error entries
};


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
    void done(AP2DataPlotStatus state,MAV_TYPE type);
    void error(QString errorstr);
    void lineRead(QString line);

private:
    void run(); // from QThread;
    bool isMainThread();

    void loadDataFieldsFromValues();
    void loadBinaryLog(QFile &logfile);
    void loadAsciiLog(QFile &logfile);
    void loadTLog(QFile &logfile);

private:
    static const QString c_timeStampSearchKey;    /// Search Key when searching for timestamps

    QString m_fileName;
    bool m_stop;
    MAV_TYPE m_loadedLogType;
    QSharedPointer<MAVLinkDecoder> m_decoder;
    AP2DataPlot2DModel *m_dataModel;

    AP2DataPlotStatus m_plotState;
};




#endif // AP2DATAPLOTTHREAD_H
