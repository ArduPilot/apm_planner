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
    bool createFMTTable();
    bool createFMTInsert(QSqlQuery *query);
    bool createIndexTable();
    bool createIndexInsert(QSqlQuery *query);
    void setError(QString error);
    QString makeCreateTableString(QString tablename, QString formatstr,QStringList variablestr);
    QString makeInsertTableString(QString tablename, QStringList variablestr);

private:
    QString m_error;
    QString m_databaseName;
    QSqlDatabase m_sharedDb;
    QMap<int,QPair<quint64,QString> > m_rowToTableMap;
    QMap<QString,QList<QString> > m_headerStringList;
    QList<QString> m_currentHeaderItems;
    QList<QList<QString> > m_fmtStringList;
    QMap<QString,QString> m_msgNameToInsertQuery;

    int m_rowCount;
    int m_columnCount;
    int m_currentRow;
    int m_fmtIndex;

    quint64 m_firstIndex;
    quint64 m_lastIndex;

    QSqlQuery *m_indexinsertquery;
    QSqlQuery *m_fmtInsertQuery;


};

#endif // AP2DATAPLOT2DMODEL_H
