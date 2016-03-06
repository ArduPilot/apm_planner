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
 *   @brief AP2DataPlot internal data model
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */


#ifndef AP2DATAPLOT2DMODEL_H
#define AP2DATAPLOT2DMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>
#include <ArduPilotMegaMAV.h>


class AP2DataPlot2DModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AP2DataPlot2DModel(QObject *parent = 0);
    ~AP2DataPlot2DModel();

    int rowCount(const QModelIndex& parent = QModelIndex() ) const;
    int columnCount(const QModelIndex& parent = QModelIndex() ) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    bool addType(const QString &name, const int type, const int length, const QString &types, const QStringList &names);
    bool addRow(const QString &name, const QList<QPair<QString,QVariant> >  &values, const quint64 index, const QString &timeColName);
    QMap<QString,QList<QString> > getFmtValues();
    QString getFmtLine(const QString& name);
    void getMessagesOfType(const QString &type, QMap<quint64, MessageBase::Ptr> &indexToMessageMap);
    bool hasType(const QString& name);
    QMap<quint64,QVariant> getValues(const QString& parent, const QString& child, bool useTimeAsIndex);
    int getChildColum(const QString& parent,const QString& child);
    QString getError() { return m_error; }
    bool endTransaction();
    bool startTransaction();
    quint64 getLastIndex();
    quint64 getFirstIndex();



    /**
     * @brief setAllRowsHaveTime should be called at the end of dataparsing
     *        telling the model that all rows have a valid timestamp which has
     *        to be determined by the parser as the model should know as less as
     *        possible about the data stored inside. Due to the fact that the
     *        parser knows about the name of the timefield he can also deliver
     *        its name keeping the model as dynamic as possible.
     *
     * @param allHaveTime - set to true if parser found a timestamp in all rows.
     * @param timeColumName - name of the DB coulum containing the timestamp.
     */
    void setAllRowsHaveTime(bool allHaveTime, const QString &timeColumName);

    /**
     * @brief getAllRowsHaveTime returns true if all rows in model have a
     *        timestamp which does NOT imply that it is possible to use time
     *        on x axis cause therefore you need min and max time too.
     *
     * @return true if all rows in model have a valid timestamp, false otherwise.
     */
    bool getAllRowsHaveTime();

    /**
     * @brief canUseTimeOnX returns true if the stored model has a valid
     *        timebase eg. all rows have a valid timestamp AND min/max time
     *        are valid. Should be used to determine whether it is possible
     *        to use time on x axis. If this delivers true all methods taking
     *        a 'useTimeAsIndex' parameter, like getValues(...) will deliver
     *        a timestamp as index if 'useTimeAsIndex' was set to true.
     *
     * @return true - Using time values on x axis is possible, false otherwise
     */
    bool canUseTimeOnX();

    /**
     * @brief getMinTime delivers the minimum timestamp held in model if
     *        the model has a valid timeline meaning all rows have a timestamp.
     *
     * @return - min timestamp if model has a valid timebase, 0 otherwise
     */
    quint64 getMinTime();

    /**
     * @brief getMinTime delivers the maximum timestamp held in model if
     *        the model has a valid timeline meaning all rows have a timestamp.
     *
     * @return - max timestamp if model has a valid timebase, 0 otherwise
     */
    quint64 getMaxTime();


    /**
     * @brief getNearestIndexForTimestamp delivers the row index which has the
     *        smallest deviation in its timeStamp to the delivered timeValue.
     *
     * @param timevalue - The timeStamp to search for
     * @return The index with the best timestamp match.
     */
    quint64 getNearestIndexForTimestamp(double timevalue);


public slots:
    void selectedRowChanged(QModelIndex current,QModelIndex previous);

signals:

private slots:

private: //helpers
    typedef QSharedPointer<QSqlQuery> queryPtr;              /// Shared pointer type for QSqlQueries

    bool createFMTTable();
    bool createFMTInsert(queryPtr &query);
    bool createIndexTable();
    bool createIndexInsert(queryPtr &query);
    void setError(QString error);
    QString makeCreateTableString(QString tablename, QString formatstr,QStringList variablestr);
    QString makeInsertTableString(QString tablename, QStringList variablestr);
    bool setUpMinTime();
    bool setUpMaxTime();

private:
    QString m_error;
    QString m_databaseName;
    QSqlDatabase m_sharedDb;
    QVector<QPair<quint64,QString> > m_rowIndexToDBIndex;   /// stores relation between Table row index
                                                            /// and DB index and DB table name
    QMap<QString,QList<QString> > m_headerStringList;
    QList<QString> m_currentHeaderItems;
    QList<QList<QString> > m_fmtStringList;

    QMap<QString,queryPtr> m_msgNameToPrepearedInsertQuery;  /// Map holding prepared insert queries to speed up inserts
    QMap<QString,queryPtr> m_msgNameToPrepearedSelectQuery;  /// Map holding prepared select queries to speed up selects

    bool m_allRowsHaveTime;         /// True if all rows have a timestamp
    bool m_canUseTimeOnX;           /// True if all rows have time and min & max time could be selected
    quint64 m_minTime;              /// smallest timestamp im model
    quint64 m_maxTime;              /// biggest timestamp im model
    QString m_timeStampColumName;   /// Name of the table colum holding the timestamp

    QList<QPair<quint64, quint64> > m_TimeIndexList;    /// List holding pairs of time stamp and table row index


    int m_rowCount;                 /// Stores the number of rows held in model.
    int m_columnCount;
    int m_currentRow;
    int m_fmtIndex;

    quint64 m_firstIndex;
    quint64 m_lastIndex;

    queryPtr m_indexinsertquery;
    queryPtr m_fmtInsertQuery;

    mutable QVector<QVariant> m_prefetchedRowData;  /// holds the cached data used in data(...) method
    mutable QModelIndex m_prefetchedRowIndex;       /// Stores the index which which is actually in cache
};



#endif // AP2DATAPLOT2DMODEL_H
