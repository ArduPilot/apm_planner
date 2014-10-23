#include "AP2DataPlot2DModel.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
AP2DataPlot2DModel::AP2DataPlot2DModel(QSqlDatabase *db,QObject *parent) :
    QAbstractTableModel(parent)
{
    m_sharedDb = db;
    QSqlQuery query(*m_sharedDb);
    if (!query.prepare("SELECT * FROM 'INDEX'"))
    {
        qDebug() << "Error preparing:" << query.lastError();
    }
    if (!query.exec())
    {
        qDebug() << "Error execing:" << query.lastError();
    }
    m_rowCount = 0;
    while (query.next())
    {
        QString name = query.value("value").toString();
        m_rowToTableMap.insert(query.value("idx").toInt(),name);
        m_rowCount++;
        QSqlQuery fmtquery(*m_sharedDb);
        if (!m_headerStringList.contains(name))
        {
            if (!fmtquery.prepare("SELECT * FROM 'FMT' WHERE name = '" + name + "'"))
            {
                qDebug() << "Error selecting from fmt";
            }
            fmtquery.exec();
            if (fmtquery.next())
            {
                m_headerStringList.insert(name,QList<QString>());
                QStringList valsplit = fmtquery.value("val").toString().split(",");
                foreach (QString val,valsplit)
                {
                    m_headerStringList[name].append(val);
                }
                QList<QString> list;
                list.append(QString::number(m_rowCount));
                list.append("FMT");
                for (int i=1;i<fmtquery.record().count();i++)
                {
                    list.append(fmtquery.value(fmtquery.record().field(i).name()).toString());
                }
                m_fmtStringList.append(list);
            }
        }
    }

    //"INSERT INTO 'FMT' (idx,typeid,length,name,format,val)

}

QVariant AP2DataPlot2DModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
    if (section == -1)
    {
        return QVariant();
    }
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }
    if (orientation == Qt::Vertical)
    {
        return QVariant();
    }
    if (section == 0)
    {
        return "Index";
    }
    if (section == 1)
    {
        return "MSG Type";
    }
    if ((section-2) >= m_currentHeaderItems.size())
    {
        return QVariant("");
    }
    return m_currentHeaderItems.at(section-2);
}
int AP2DataPlot2DModel::rowCount( const QModelIndex & parent) const
{
     return m_rowCount;
}
int AP2DataPlot2DModel::columnCount ( const QModelIndex & parent) const
{
    return 10;
}
QVariant AP2DataPlot2DModel::data ( const QModelIndex & index, int role) const
{
    if (role != Qt::DisplayRole)
    {
        return QVariant();
    }
    if (!index.isValid())
    {
        return QVariant();
    }
    if (index.column() == 0)
    {
        return QString::number(index.row());
    }
    if (index.row() < m_fmtStringList.size())
    {
        return m_fmtStringList.at(index.row()).value(index.column()-1);
    }
    if (!m_rowToTableMap.contains(index.row()))
    {
        return QVariant();
    }
    QString tablename = m_rowToTableMap.value(index.row());
    QSqlQuery tablequery(*m_sharedDb);
    tablequery.prepare("SELECT * FROM '" + tablename + "' WHERE idx = " + QString::number(index.row()));
    tablequery.exec();
    if (!tablequery.next())
    {
        return QVariant();
    }
    if ((index.column()-1) >= tablequery.record().count())
    {
        return QVariant();
    }
    if (index.column() == 0)
    {
        return tablequery.value(0);
    }
    if (index.column() == 1)
    {
        return tablename;
    }
    return tablequery.value((index.column()-1));
}
void AP2DataPlot2DModel::selectedRowChanged(QModelIndex current,QModelIndex previous)
{
    if (m_currentRow == current.row())
    {
        return;
    }
    m_currentRow = current.row();
    //Grab the index
    int rowid = data(createIndex(current.row(),0)).toString().toInt();
    qDebug() << "row id:" << rowid << "current:" << current.row();
    if (m_rowToTableMap.contains(rowid))
    {
        m_currentHeaderItems = m_headerStringList.value(m_rowToTableMap[rowid]);
    }
    else
    {
        m_currentHeaderItems = QList<QString>();
    }
    emit headerDataChanged(Qt::Horizontal,0,9);
}
