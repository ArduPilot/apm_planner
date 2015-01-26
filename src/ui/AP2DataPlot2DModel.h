#ifndef AP2DATAPLOT2DMODEL_H
#define AP2DATAPLOT2DMODEL_H

#include <QAbstractTableModel>
#include <QSqlDatabase>

class AP2DataPlot2DModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit AP2DataPlot2DModel(QSqlDatabase *db,QObject *parent = 0);
    int rowCount( const QModelIndex & parent = QModelIndex() ) const;
    int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
    QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
private:
    QSqlDatabase *m_sharedDb;
    int m_rowCount;
    QMap<int,QPair<quint64,QString> > m_rowToTableMap;
    int m_currentRow;
    QMap<QString,QList<QString> > m_headerStringList;
    QList<QString> m_currentHeaderItems;
    QList<QList<QString> > m_fmtStringList;
signals:

public slots:
    void selectedRowChanged(QModelIndex current,QModelIndex previous);
};

#endif // AP2DATAPLOT2DMODEL_H
