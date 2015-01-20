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


#include "AP2DataPlot2DModel.h"
#include <QSqlQuery>
#include <QDebug>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QUuid>
#include <QsLog.h>
/*
 * This model holds everything in memory in a sqlite database.
 * There are two system tables, then unlimited number of message tables.
 *
 * System Tables:
 *
 * FMT (idx integer PRIMARY KEY, typeid integer,length integer,name varchar(200),format varchar(6000),val varchar(6000));
 *  idx: Index - Position in the file in relation to other values
 *  typeid: typeid, this is unused in most cases
 *  length: length, this is unused in most cases
 *  name: Name of the type defined by the format: ATUN
 *  format: format of the message: BBfffff
 *  val: Comma deliminted field with names of the message fields: Axis,TuneStep,RateMin,RateMax,RPGain,RDGain,SPGain
 *
 * INDEX (idx integer PRIMARY KEY, value varchar(200));
 *  idx: Index: Index on graph/table
 *  value: The table name (message type name) that the index points to
 *
 * Message tables are formatted as such:
 *
 * ATUN (idx integer PRIMARY KEY, Axis integer, TuneStep integer, RateMin real, RateMax real, RPGain real, RDGain real, SPGain real);
 *  The types are defined by the format (in this case, BBfffff)
 *  inside AP2DataPlot2DModel::makeCreateTableString.
 */
AP2DataPlot2DModel::AP2DataPlot2DModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_databaseName = QUuid::createUuid().toString();
    m_sharedDb = QSqlDatabase::addDatabase("QSQLITE",m_databaseName);
    m_sharedDb.setDatabaseName(":memory:");
    if (!m_sharedDb.open())
    {
     //   QMessageBox::information(0,"error","Error opening shared database " + m_sharedDb.lastError().text());
        return;
    }

    m_fmtIndex=0;
    if (!m_sharedDb.transaction())
    {
       // emit error("Unable to start database transaction 1");
        return;
    }

    if (!createFMTTable())
    {
        //Error already emitted.
        return;
    }
    m_fmtInsertQuery = new QSqlQuery(m_sharedDb);
    if (!createFMTInsert(m_fmtInsertQuery))
    {
        //emit error("Error preparing FMT insert statement: " + fmtinsertquery.lastError().text());
        return;
    }

    if (!createIndexTable())
    {
        //Error already emitted.
        return;
    }
    m_indexinsertquery = new QSqlQuery(m_sharedDb);
    if (!createIndexInsert(m_indexinsertquery))
    {
        //emit error("Error preparing INDEX insert statement: " + indexinsertquery.lastError().text());
        return;
    }

    QMap<QString,QSqlQuery*> nameToInsertQuery;
    QMap<QString,QString> nameToTypeString;
    if (!m_sharedDb.commit())
    {
       // emit error("Unable to commit database transaction 1");
        return;
    }
    //if (!m_sharedDb.transaction())
    //{
        //emit error("Unable to start database transaction 2");
    //    return;
    //}

    QSqlQuery query(m_sharedDb);
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
        //m_rowToTableMap.insert(query.value("idx").toInt(),name);
        quint64 idx = query.value("idx").toLongLong();

        m_rowToTableMap.insert(m_rowCount,QPair<quint64,QString>(idx,name));
        QSqlQuery fmtquery(m_sharedDb);
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
        m_rowCount++;
    }

    //"INSERT INTO 'FMT' (idx,typeid,length,name,format,val)

}
AP2DataPlot2DModel::~AP2DataPlot2DModel()
{
    QSqlDatabase::removeDatabase(m_databaseName);
}

QMap<QString,QList<QString> > AP2DataPlot2DModel::getFmtValues()
{
    return m_headerStringList;
}
QString AP2DataPlot2DModel::getFmtLine(const QString& name)
{
    QSqlQuery fmtquery(m_sharedDb);
    fmtquery.prepare("SELECT * FROM 'FMT' WHERE name = '" + name + "';");
    if (!fmtquery.exec())
    {
        //QMessageBox::information(0,"Error","Error selecting from table 'FMT' " + m_sharedDb.lastError().text());
        //return;
    }
    if (fmtquery.next())
    {
        QSqlRecord record = fmtquery.record();
        QString name = record.value(3).toString();
        QString vars = record.value(5).toString();
        QString format = record.value(4).toString();
        int size = 0;
        for (int i=0;i<format.size();i++)
        {
            if (format.at(i).toLatin1() == 'n')
            {
                size += 4;
            }
            else if (format.at(i).toLatin1() == 'N')
            {
                size += 16;
            }
            else if (format.at(i).toLatin1() == 'Z')
            {
                size += 64;
            }
            else if (format.at(i).toLatin1() == 'f')
            {
                size += 4;
            }
            else if ((format.at(i).toLatin1() == 'i') || (format.at(i).toLatin1() == 'I') || (format.at(i).toLatin1() == 'e') || (format.at(i).toLatin1() == 'E')  || (format.at(i).toLatin1() == 'L'))
            {
                size += 4;
            }
            else if ((format.at(i).toLatin1() == 'h') || (format.at(i).toLatin1() == 'H') || (format.at(i).toLatin1() == 'c') || (format.at(i).toLatin1() == 'C'))
            {
                size += 2;
            }
            else if ((format.at(i).toLatin1() == 'b') || (format.at(i).toLatin1() == 'B') || (format.at(i).toLatin1() == 'M'))
            {
                size += 1;
            }
        }
        QString formatline = "FMT, " + QString::number(record.value(1).toInt()) + ", " + QString::number(size+3) + ", " + name + ", " + format + ", " + vars;
        return formatline;
    }
    return "";
}
QMap<int,QString> AP2DataPlot2DModel::getModeValues()
{
    QMap<int,QString> retval;
    QSqlQuery modequery(m_sharedDb);
    modequery.prepare("SELECT * FROM 'MODE';");
    if (!modequery.exec())
    {
        //No mode?
        QLOG_DEBUG() << "Graph loaded with no mode table. Running anyway, but text modes will not be available";
    }
    else
    {
        while (modequery.next())
        {
            QSqlRecord record = modequery.record();
            int index = record.value(0).toInt();
            QString mode = "";
            if (record.contains("Mode"))
            {
                mode = record.value("Mode").toString();
            }
            bool ok = false;
            int modeint = mode.toInt(&ok);
            if (!ok)
            {
                if (record.contains("ModeNum"))
                {
                    mode = record.value("ModeNum").toString();
                }
                else
                {
                    QLOG_DEBUG() << "Unable to determine Mode number in log" << record.value("Mode").toString();
                    mode = record.value("Mode").toString();
                }
            }
            retval.insert(index,mode);
        }
    }
    return retval;
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
    quint64 tableindex = index.row();
    if (m_rowToTableMap.contains(index.row()))
    {
        tableindex = m_rowToTableMap.value(index.row()).first;
    }
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
        return QString::number(tableindex);
    }
    if (index.row() < m_fmtStringList.size())
    {
        return m_fmtStringList.at(index.row()).value(index.column()-1);
    }

    QString tablename = m_rowToTableMap.value(index.row()).second;
    QSqlQuery tablequery(m_sharedDb);
    QString val = QString::number(tableindex,'f',0);
    tablequery.prepare("SELECT * FROM " + tablename + " WHERE idx = " + val);
    if (!tablequery.exec())
    {
        qDebug() << "Unable to exec table query:" << tablequery.lastError().text();
        return QVariant();
    }
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

    if (m_rowToTableMap.contains(current.row()))
    {
        m_currentHeaderItems = m_headerStringList.value(m_rowToTableMap[current.row()].second);
    }
    else
    {
        m_currentHeaderItems = QList<QString>();
    }
    emit headerDataChanged(Qt::Horizontal,0,9);
}
bool AP2DataPlot2DModel::hasType(const QString& name)
{
    return m_msgNameToInsertQuery.contains(name);
}

bool AP2DataPlot2DModel::addType(QString name,int type,int length,QString types,QStringList names)
{
    if (!m_msgNameToInsertQuery.contains(name))
    {
        QString createstring = makeCreateTableString(name,types,names);
        QString variablenames = "";
        for (int i=0;i<names.size();i++)
        {
            variablenames += names.at(i) + ((i < names.size()-1) ? "," : "");
        }
        QString insertstring = makeInsertTableString(name,names);

        //if (!query->prepare("INSERT INTO 'FMT' (idx,typeid,length,name,format,val) values (:idx,:typeid,:length,:name,:format,:val);"))
        m_fmtInsertQuery->bindValue(":idx",m_fmtIndex++);
        m_fmtInsertQuery->bindValue(":typeid",type);
        m_fmtInsertQuery->bindValue(":length",length);
        m_fmtInsertQuery->bindValue(":name",name);
        m_fmtInsertQuery->bindValue(":format",types);
        m_fmtInsertQuery->bindValue(":val",variablenames);
        QList<QString> list;
        list.append("FMT");
        list.append(QString::number(m_fmtIndex-1));
        list.append(QString::number(type));
        list.append(QString::number(length));
        list.append(types);
        list.append(variablenames);
        m_fmtStringList.append(list);
        if (!m_fmtInsertQuery->exec())
        {
            setError("FAILED TO FMT: " + m_fmtInsertQuery->lastError().text());
            return false;
        }
        m_indexinsertquery->bindValue(":idx",m_fmtIndex-1);
        m_indexinsertquery->bindValue(":value","FMT");
        if (!m_indexinsertquery->exec())
        {
            setError("Error execing index: " + name + " " + m_indexinsertquery->lastError().text());
            return false;
        }

        m_msgNameToInsertQuery.insert(name,insertstring.replace("insert or replace","insert"));
        QSqlQuery create(m_sharedDb);
        if (!create.prepare(createstring))
        {
            setError("Unable to create: " + create.lastError().text());
            return false;
        }
        if (!create.exec())
        {
            setError("Unable to exec create: " + create.lastError().text());
            return false;
        }
    }
    if (!m_headerStringList.contains(name))
    {
        m_headerStringList.insert(name,QList<QString>());
        //QStringList valsplit = fmtquery.value("val").toString().split(",");
        foreach (QString val,names)
        {
            m_headerStringList[name].append(val);
        }
    }
    return true;
}
QMap<int,QVariant> AP2DataPlot2DModel::getValues(const QString& parent,const QString& child)
{
    int index = getChildIndex(parent,child);
    QSqlQuery itemquery(m_sharedDb);
    itemquery.prepare("SELECT * FROM '" + parent + "';");
    itemquery.exec();
    QMap<int,QVariant> retval;
    while (itemquery.next())
    {
        QSqlRecord record = itemquery.record();
        quint64 graphindex = record.value(0).toLongLong();
        QVariant graphvalue= record.value(index+1);
        retval.insert(graphindex,graphvalue);
    }
    return retval;
}

int AP2DataPlot2DModel::getChildIndex(const QString& parent,const QString& child)
{

    QSqlQuery tablequery(m_sharedDb);
    //tablequery.prepare("SELECT * FROM '" + parent + "';");
    if (!tablequery.prepare("SELECT * FROM 'FMT' WHERE name = '" + parent + "';"))
    {
        QLOG_DEBUG() << "Error preparing select:" + tablequery.lastError().text();
        return -1;
    }
    if (!tablequery.exec())
    {
        QLOG_DEBUG() << "Error execing select:" + tablequery.lastError().text();
        return -1;
    }
    if (!tablequery.next())
    {
        return -1;
    }
    QSqlRecord record = tablequery.record();
    QStringList valuessplit = record.value(5).toString().split(","); //comma delimited list of names
    bool found = false;
    int index = 0;
    for (int i=0;i<valuessplit.size();i++)
    {
        if (valuessplit.at(i) == child)
        {
            found = true;
            index = i;
            i = valuessplit.size();
        }
    }
    if (!found)
    {
        return -1;
    }
    return index;
}
bool AP2DataPlot2DModel::startTransaction()
{
    if (!m_sharedDb.transaction())
    {
        setError("Unable to start transaction to database: " + m_sharedDb.lastError().text());
        return false;
    }
    return true;
}
bool AP2DataPlot2DModel::endTransaction()
{
    if (!m_sharedDb.commit())
    {
        setError("Unable to commit to database: " + m_sharedDb.lastError().text());
        return false;
    }
    return true;
}

bool AP2DataPlot2DModel::addRow(QString name,QList<QPair<QString,QVariant> >  values,quint64 index)
{
    //Add a row to a previously defined message type, NAME.Jy   Th
    QSqlQuery query(m_sharedDb);
    if (m_msgNameToInsertQuery.contains(name))
    {
        if (!query.prepare(m_msgNameToInsertQuery.value(name)))
        {
           setError("Unable to prepare query: " + query.lastError().text());
           return false;
        }
    }
    query.bindValue(QString(":idx"),index);
    for (int i=0;i<values.size();i++)
    {
        query.bindValue(QString(":") + values.at(i).first,values.at(i).second);
    }
    if (!query.exec())
    {
        setError("Error execing insert query: " + query.lastError().text());
        return false;
    }
    else
    {
        m_indexinsertquery->bindValue(":idx",index);
        m_indexinsertquery->bindValue(":value",name);
        if (!m_indexinsertquery->exec())
        {
            setError("Error execing:" + m_indexinsertquery->executedQuery() + " error was " + m_indexinsertquery->lastError().text());
            return false;
        }
    }
    m_rowToTableMap.insert(m_rowCount++,QPair<quint64,QString>(index,name));
   // if (!m_sharedDb.commit())
   // {
   //     setError("Error commiting to database " + m_sharedDb.lastError().text());
   //     return false;
   // }
    return true;

}
QString AP2DataPlot2DModel::makeCreateTableString(QString tablename, QString formatstr,QStringList variablestr)
{
    QString mktable = "CREATE TABLE '" + tablename + "' (idx integer PRIMARY KEY";
    for (int j=0;j<variablestr.size();j++)
    {
        QString name = variablestr[j].trimmed();
        //name = "\"" + name + "\"";
        QChar typeCode = formatstr.at(j);
        if (typeCode == 'b') //int8_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'B') //uint8_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'h') //int16_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'H') //uint16_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'i') //int32_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'I') //uint32_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'f') //float
        {
            mktable.append("," + name + " real");
        }
        else if (typeCode == 'N') //char(16)
        {
            mktable.append("," + name + " text");
        }
        else if (typeCode == 'Z') //char(64)
        {
            mktable.append("," + name + " text");
        }
        else if (typeCode == 'c') //int16_t * 100
        {
            mktable.append("," + name + " real");
        }
        else if (typeCode == 'C') //uint16_t * 100
        {
            mktable.append("," + name + " real");
        }
        else if (typeCode == 'e') //int32_t * 100
        {
            mktable.append("," + name + " real");
        }
        else if (typeCode == 'E') //uint32_t * 100
        {
            mktable.append("," + name + " real");
        }
        else if (typeCode == 'L') //uint32_t lon/lat
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'M') //uint8_t
        {
            mktable.append("," + name + " integer");
        }
        else
        {
            //QLOG_DEBUG() << "AP2DataPlotThread::makeCreateTableString(): NEW UNKNOWN VALUE" << typeCode;
        }
    }
    mktable.append(");");
    return mktable;
}
QString AP2DataPlot2DModel::makeInsertTableString(QString tablename, QStringList variablestr)
{
    QString inserttable = "insert or replace into '" + tablename + "' (idx";
    QString insertvalues = "(:idx,";
    for (int j=0;j<variablestr.size();j++)
    {
        QString name = variablestr[j].trimmed();
        inserttable.append("," + name + "");
        insertvalues.append(":" + name + ((j < variablestr.size()-1) ? "," : ""));
    }
    inserttable.append(")");
    insertvalues.append(")");
    QString final = inserttable + " values " + insertvalues + ";";
    return final;
}
bool AP2DataPlot2DModel::createFMTTable()
{
    QSqlQuery fmttablecreate(m_sharedDb);
    if (!fmttablecreate.prepare("CREATE TABLE 'FMT' (idx integer PRIMARY KEY, typeid integer,length integer,name varchar(200),format varchar(6000),val varchar(6000));"))
    {
        setError("Prapre create FMT table failed: " + fmttablecreate.lastError().text());
        return false;
    }
    if (!fmttablecreate.exec())
    {
        QLOG_ERROR() << "Error creating FMT table: " + fmttablecreate.lastError().text();
        setError("Exec create FMT table failed: " + fmttablecreate.lastError().text());
        return false;
    }
    return true;
}
bool AP2DataPlot2DModel::createFMTInsert(QSqlQuery *query)
{
    if (!query->prepare("INSERT INTO 'FMT' (idx,typeid,length,name,format,val) values (:idx,:typeid,:length,:name,:format,:val);"))
    {
        setError("Insert into FMT prepare failed: " + query->lastError().text());
        return false;
    }
    return true;
}
bool AP2DataPlot2DModel::createIndexTable()
{
    QSqlQuery indextablecreate(m_sharedDb);
    if (!indextablecreate.prepare("CREATE TABLE 'INDEX' (idx integer PRIMARY KEY, value varchar(200));"))
    {
        setError("Error preparing INDEX table: " + m_sharedDb.lastError().text());
        return false;
    }
    if (!indextablecreate.exec())
    {
        setError("Error creating INDEX table: " + m_sharedDb.lastError().text());
        return false;
    }
    return true;
}
bool AP2DataPlot2DModel::createIndexInsert(QSqlQuery *query)
{
    if (!query->prepare("INSERT INTO 'INDEX' (idx,value) values (:idx,:value);"))
    {
        setError("Insert into Index prepare failed: " + query->lastError().text());
        return false;
    }
    return true;
}
void AP2DataPlot2DModel::setError(QString error)
{
    QLOG_ERROR() << error;
    m_error = error;
}
