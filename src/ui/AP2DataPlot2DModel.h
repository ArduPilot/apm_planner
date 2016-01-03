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
    bool addType(QString name,int type,int length,QString types,QStringList names);
    bool addRow(QString name,QList<QPair<QString,QVariant> >  values,quint64 index);
    QMap<QString,QList<QString> > getFmtValues();
    QString getFmtLine(const QString& name);
    QMap<quint64,QString> getModeValues();
    QMap<quint64, ErrorType> getErrorValues();
    bool hasType(const QString& name);
    QMap<quint64,QVariant> getValues(const QString& parent,const QString& child);
    int getChildIndex(const QString& parent,const QString& child);
    QString getError() { return m_error; }
    bool endTransaction();
    bool startTransaction();
    quint64 getLastIndex();
    quint64 getFirstIndex();

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

    int m_rowCount;         /// Stores the number of rows held in model.
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
