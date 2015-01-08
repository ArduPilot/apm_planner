#ifndef AP2DATAPLOT2DMODEL_H
#define AP2DATAPLOT2DMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

class AP2DataPlot2DModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AP2DataPlot2DModel(QObject *parent = 0);
    int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    void addType(QString name,QString types,QStringList names);
    void addRow(QString name,QList<QPair<QString,QVariant> >  values,quint64 index);
    QMap<QString,QList<QString> > getFmtValues();
    QMap<int,QString> getModeValues();
    bool hasType(QString name);
    QMap<int,double> getValues(QString parent,QString child);
    int getChildIndex(QString parent,QString child);
private:
    QSqlDatabase m_sharedDb;
    int m_rowCount;
    QMap<int,QPair<quint64,QString> > m_rowToTableMap;
    int m_currentRow;
    QMap<QString,QList<QString> > m_headerStringList;
    QList<QString> m_currentHeaderItems;
    QList<QList<QString> > m_fmtStringList;
    QMap<QString,QString> m_msgNameToInsertQuery;
    QString makeCreateTableString(QString tablename, QString formatstr,QStringList variablestr);
    QString makeInsertTableString(QString tablename, QStringList variablestr);
    bool createFMTTable();
    bool createFMTInsert(QSqlQuery *query);
    bool createIndexTable();
    bool createIndexInsert(QSqlQuery *query);
    QSqlQuery *indexinsertquery;
    QSqlQuery *m_fmtInsertQuery;
    int m_fmtIndex;
signals:

public slots:
    void selectedRowChanged(QModelIndex current,QModelIndex previous);
};

#endif // AP2DATAPLOT2DMODEL_H
