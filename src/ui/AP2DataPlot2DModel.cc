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
    QAbstractTableModel(parent),
    m_databaseName(QUuid::createUuid().toString()),
    m_allRowsHaveTime(false),
    m_canUseTimeOnX(false),
    m_minTime(0),
    m_maxTime(0),
    m_rowCount(0),
    m_columnCount(0),
    m_currentRow(0),
    m_fmtIndex(0),
    m_firstIndex(0),
    m_lastIndex(0)
{
    m_sharedDb = QSqlDatabase::addDatabase("QSQLITE",m_databaseName);
    m_sharedDb.setDatabaseName(":memory:");

    //  Open DB and start transaction
    if (!m_sharedDb.open())
    {
        setError("Error opening shared database " + m_sharedDb.lastError().text());
        return;
    }
    if (!m_sharedDb.transaction())
    {
        setError("Unable to start database transaction "  + m_sharedDb.lastError().text());
        return;
    }

    // Create FMT Table and prepare insert query
    if (!createFMTTable())
    {
        return; //Error already emitted.
    }
    m_fmtInsertQuery = queryPtr(new QSqlQuery(m_sharedDb));
    if (!createFMTInsert(m_fmtInsertQuery))
    {
        return;  //Error already emitted.
    }

    // Create index table and prepare insert query
    if (!createIndexTable())
    {
        return;   //Error already emitted.
    }
    m_indexinsertquery = queryPtr(new QSqlQuery(m_sharedDb));
    if (!createIndexInsert(m_indexinsertquery))
    {
        return;  //Error already emitted.
    }

    if (!m_sharedDb.commit())
    {
        setError("Unable to commit database transaction "  + m_sharedDb.lastError().text());
        return;
    }
}

AP2DataPlot2DModel::~AP2DataPlot2DModel()
{
    QSqlDatabase::removeDatabase(m_databaseName);
}

QMap<QString,QList<QString> > AP2DataPlot2DModel::getFmtValues()
{
    QMap<QString,QList<QString> > retval;
    QSqlQuery fmtquery(m_sharedDb);
    fmtquery.prepare("SELECT * FROM 'FMT';");
    fmtquery.exec();
    while (fmtquery.next())
    {
        QSqlRecord record = fmtquery.record();
        QString name = record.value(3).toString();
        QSqlQuery indexquery(m_sharedDb);
        if (!indexquery.prepare("SELECT * FROM 'INDEX' WHERE value = '" + name + "';"))
        {
            continue;
        }
        if (!indexquery.exec())
        {
            continue;
        }
        if (!indexquery.next())
        {
            //No records
            continue;
        }
        if (!m_headerStringList.contains(name))
        {
            continue;
        }
        retval.insert(name,QList<QString>());
        for (int i=0;i<m_headerStringList[name].size();i++)
        {
            retval[name].append(m_headerStringList[name].at(i));
        }
    }
    return retval;
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
            else if (format.at(i).toLatin1() == 'Q')
            {
                size += 8;
            }
            else if (format.at(i).toLatin1() == 'q')
            {
                size += 8;
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
            } else {
                QLOG_DEBUG() << "Unknown format character (" << format.at(i).toLatin1() << "); export will be bad";
            }
        }
        QString formatline = "FMT, " + QString::number(record.value(1).toInt()) + ", " + QString::number(size+3) + ", " + name + ", " + format + ", " + vars;
        return formatline;
    }
    return "";
}

void AP2DataPlot2DModel::getMessagesOfType(const QString &type, QMap<quint64, MessageBase::Ptr> &indexToMessageMap)
{
    QSqlQuery query(m_sharedDb);
    query.prepare("SELECT * FROM '" + type + "';");
    if (query.exec())
    {
        while (query.next())
        {
            MessageBase::Ptr p_msg = MessageFactory::getMessageOfType(type);
            QSqlRecord record = query.record();

            if (!p_msg->setFromSqlRecord(record))
            {
                QLOG_DEBUG() << "Not all data could be read from SQL-Record. Schema mismatch?! "
                             << "The data of type " << type << " might be corrupted.";
            }
            indexToMessageMap.insert(p_msg->getIndex(), p_msg);
        }
    }
    else
    {
        //query returned no result
        QLOG_DEBUG() << "Graph loaded with no table of type " << type << ". This is strange!";
    }
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
    Q_UNUSED(parent)
    return m_rowCount;
}

int AP2DataPlot2DModel::columnCount ( const QModelIndex & parent) const
{
    Q_UNUSED(parent)
    return m_columnCount;
}

QVariant AP2DataPlot2DModel::data ( const QModelIndex & index, int role) const
{
    if ((role != Qt::DisplayRole) || !index.isValid())
    {
        return QVariant();
    }
    if (index.row() >= m_rowCount)
    {
        QLOG_ERROR() << "Accessing a Database row that does not exist! Row was: " << index.row();
        return QVariant();
    }

    if (index.column() == 0)
    {
        // Column 0 is the DB index of the log data
        return QVariant(QString::number(m_rowIndexToDBIndex[index.row()].first));
    }
    if (index.column() == 1)
    {
        // Column 1 is the name of the log data (ATT,ATUN...)
        return QVariant(m_rowIndexToDBIndex[index.row()].second);
    }

    // The data is mostly read row by row. This means first all columns of row 1 are read and then
    // all columns of row 2 and than all colums of row 3 and so on.
    // This cache reads a whole row from DB and caches it internally. All consecutive accesses to this
    // row will be answered from the cache.
    if (index.row() != m_prefetchedRowIndex.row())
    {
        m_prefetchedRowData.clear();
        QString tableIndex = QString::number(m_rowIndexToDBIndex[index.row()].first);
        QString tableName  = m_rowIndexToDBIndex[index.row()].second;

        queryPtr selectRowQuery = m_msgNameToPrepearedSelectQuery.value(tableName);
        selectRowQuery->bindValue(":val", tableIndex);
        if (selectRowQuery->exec())
        {
            int recordCount = selectRowQuery->record().count();
            if (!selectRowQuery->next())
            {
                return QVariant();
            }
            for (int i = 0; i < recordCount; ++i)
            {
                m_prefetchedRowData.push_back(selectRowQuery->value(i));
            }
            m_prefetchedRowIndex = index;
            selectRowQuery->finish();   // must be called because we will reuse this query
        }
        else
        {
            qDebug() << "Unable to exec table query:" << selectRowQuery->lastError().text();
        }
    }

    if ((index.column()-1) >= m_prefetchedRowData.size())
    {
        return QVariant();
    }
    return m_prefetchedRowData[index.column()-1];
}

void AP2DataPlot2DModel::selectedRowChanged(QModelIndex current,QModelIndex previous)
{
    Q_UNUSED(previous)
    if (!current.isValid())
    {
        return;
    }
    if (m_currentRow == current.row())
    {
        return;
    }
    m_currentRow = current.row();
    if (current.row() < m_fmtStringList.size())
    {
        m_currentHeaderItems = QList<QString>();
        emit headerDataChanged(Qt::Horizontal,0,9);
        return;
    }
    //Grab the index

    if (current.row() < m_rowCount)
    {
        m_currentHeaderItems = m_headerStringList.value(m_rowIndexToDBIndex[current.row()].second);
    }
    else
    {
        m_currentHeaderItems = QList<QString>();
    }
    emit headerDataChanged(Qt::Horizontal,0,9);
}

bool AP2DataPlot2DModel::hasType(const QString& name)
{
    return m_msgNameToPrepearedInsertQuery.contains(name);
}

bool AP2DataPlot2DModel::addType(const QString &name, int type, int length, const QString &types, const QStringList &names)
{
    if (!m_msgNameToPrepearedInsertQuery.contains(name))
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

        // Create table for measurement of type "name"
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
        // Prepare an insert query for the created table above and store it to speed up the inserting process later
        queryPtr prepQuery = queryPtr(new QSqlQuery(m_sharedDb));
        if (!prepQuery->prepare(insertstring.replace("insert or replace","insert")))
        {
            setError("Error preparing insertquery: " + name + " " + prepQuery->lastError().text());
            return false;
        }
        m_msgNameToPrepearedInsertQuery.insert(name, prepQuery);
        // And prepare a select query for rows of this table
        prepQuery = queryPtr(new QSqlQuery(m_sharedDb));
        if (!prepQuery->prepare("SELECT * FROM " + name + " WHERE idx = :val"))
        {
            setError("Error preparing select query: " + name + " " + prepQuery->lastError().text());
            return false;
        }
        m_msgNameToPrepearedSelectQuery.insert(name, prepQuery);

    }
    if (!m_headerStringList.contains(name))
    {
        m_headerStringList.insert(name,QList<QString>());
        foreach (QString val,names)
        {
            m_headerStringList[name].append(val);
        }
    }
    return true;
}
QMap<quint64,QVariant> AP2DataPlot2DModel::getValues(const QString& parent, const QString& child, bool useTimeAsIndex)
{
    int valueColum = getChildColum(parent,child);
    int indexColum = 0; // Default index is always colum 0
    QMap<quint64,QVariant> retval;

    if (useTimeAsIndex)
    {
        indexColum = getChildColum(parent, m_timeStampColumName);
    }
    QSqlQuery itemquery(m_sharedDb);
    itemquery.prepare("SELECT * FROM '" + parent + "';");
    itemquery.exec();
    while (itemquery.next())
    {
        QSqlRecord record = itemquery.record();
        quint64 graphindex = record.value(indexColum).toLongLong();
        QVariant graphvalue = record.value(valueColum);
        retval.insert(graphindex,graphvalue);
    }

    return retval;
}

int AP2DataPlot2DModel::getChildColum(const QString& parent,const QString& child)
{
    // From headerStringList we can determine which index (colum) is used for child data
    // Be aware that the datarow in DB has always one colum more than the header strings
    // cause of the added DB index.
    if (!m_headerStringList.contains(parent))
    {
        return -1;  // looks like we do not have this data.
    }

    // Try to find child in header strings
    QList<QString> headerStrings =  m_headerStringList.value(parent);
    for (int i = 0; i < headerStrings.size(); i++)
    {
        if (headerStrings[i] == child)
        {
            return i + 1;   // found! increase by one cause of index
        }
    }

    return -1;
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

bool AP2DataPlot2DModel::addRow(const QString &name, const QList<QPair<QString,QVariant> >  &values, const quint64 index, const QString &timeColName)
{
    if (m_firstIndex == 0)
    {
        m_firstIndex = index;
    }
    m_lastIndex = index;

    //Add a row to a previously defined message type using the already prepared insert query
    queryPtr insertQuery = m_msgNameToPrepearedInsertQuery.value(name);
    if (!insertQuery)
    {
        setError("No prepared insert query available for message: " + name);
        return false;
    }
    insertQuery->bindValue(":idx", index);
    bool timeFound = false;
    for (int i=0;i<values.size();i++)
    {
        insertQuery->bindValue(":" + values.at(i).first, values.at(i).second);
        if (!timeFound && (values.at(i).first == timeColName))
        {
            m_TimeIndexList.push_back(QPair<quint64, quint64>(values.at(i).second.toInt(), index));
            timeFound = true;
        }
    }
    if (!insertQuery->exec())
    {
        setError("Error execing insert query: " + insertQuery->lastError().text());
        return false;
    }
    else
    {
        insertQuery->finish();  // must be called in case of a reusage of this query
        m_indexinsertquery->bindValue(":idx", index);
        m_indexinsertquery->bindValue(":value", name);
        if (!m_indexinsertquery->exec())
        {
            setError("Error execing:" + m_indexinsertquery->executedQuery() + " error was " + m_indexinsertquery->lastError().text());
            return false;
        }
    }

    // Our table model is larger than the number of columns we insert:
    // +1 for the index column (in column 0)
    // +1 for the table/message type (in column 1)
    if (values.size()+2 > m_columnCount)
    {
        m_columnCount = values.size() +2;
    }

    m_rowIndexToDBIndex.push_back(QPair<quint64,QString>(index,name));
    m_rowCount++;
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
        else if (typeCode == 'd') //double
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
        else if (typeCode == 'q') //int64_t
        {
            mktable.append("," + name + " integer");
        }
        else if (typeCode == 'Q') //uint64_t
        {
            mktable.append("," + name + " integer");
        }
        else
        {
            QLOG_DEBUG() << "AP2DataPlotThread::makeCreateTableString(): NEW UNKNOWN VALUE" << typeCode;
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
bool AP2DataPlot2DModel::createFMTInsert(queryPtr &query)
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
bool AP2DataPlot2DModel::createIndexInsert(queryPtr &query)
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
quint64 AP2DataPlot2DModel::getLastIndex()
{
    return m_lastIndex;
}

quint64 AP2DataPlot2DModel::getFirstIndex()
{
    return m_firstIndex;
}

void AP2DataPlot2DModel::setAllRowsHaveTime(bool allHaveTime, const QString &timeColumName)
{
    m_timeStampColumName = timeColumName;
    m_allRowsHaveTime = allHaveTime;
    m_canUseTimeOnX  = true;
    m_canUseTimeOnX &= setUpMinTime();
    m_canUseTimeOnX &= setUpMaxTime();
}

bool AP2DataPlot2DModel::getAllRowsHaveTime()
{
    return m_allRowsHaveTime;
}

bool AP2DataPlot2DModel::setUpMinTime()
{
    m_minTime = 0;    // force to be 0 in case of a failure
    if (m_allRowsHaveTime)
    {
        QSqlQuery minTimeQuery(m_sharedDb);
        minTimeQuery.prepare("SELECT " + m_timeStampColumName + " from 'STRT';");
        if (!minTimeQuery.exec())
        {
            setError("Unable to exec getMinTime query: " + minTimeQuery.lastError().text());
            return false;
        }
        if (!minTimeQuery.next())
        {
            setError("Result of getMinTime query was empty!");
            return false;
        }
        m_minTime = minTimeQuery.value(0).toLongLong();
        return true;
    }
    return false;
}

bool AP2DataPlot2DModel::setUpMaxTime()
{
    m_maxTime = 0; // Always force to 0
    if (m_allRowsHaveTime)
    {
        QString maxTableName;
        QSqlQuery maxTimeQuery(m_sharedDb);
        maxTimeQuery.prepare("SELECT value FROM 'INDEX' WHERE idx = (select max(idx) from 'INDEX');");
        if (!maxTimeQuery.exec())
        {
            setError("Unable to select max index from 'INDEX': " + maxTimeQuery.lastError().text());
            return false;
        }
        if (!maxTimeQuery.next())
        {
            setError("Result of select max index from 'INDEX' was empty!");
            return false;
        }
        maxTableName = maxTimeQuery.value(0).toString();

        QSqlQuery maxTimeQuery2(m_sharedDb);
        maxTimeQuery2.prepare("SELECT " + m_timeStampColumName + " FROM '" + maxTableName + "' WHERE idx = (select max(idx) from '" + maxTableName + "');");
        if (!maxTimeQuery2.exec())
        {
            setError("Unable to select max " + m_timeStampColumName + " from " + maxTableName + ": " + maxTimeQuery2.lastError().text());
            return false;
        }
        if (!maxTimeQuery2.next())
        {
            setError("Result of select max " + m_timeStampColumName + " from " + maxTableName + " query was empty!");
            return false;
        }
        m_maxTime = maxTimeQuery2.value(0).toLongLong();
        return true;
    }
    return false;
}

quint64 AP2DataPlot2DModel::getMinTime()
{
    return m_minTime;
}

quint64 AP2DataPlot2DModel::getMaxTime()
{
    return m_maxTime;
}

bool AP2DataPlot2DModel::canUseTimeOnX()
{
    return m_canUseTimeOnX && m_allRowsHaveTime;
}

quint64 AP2DataPlot2DModel::getNearestIndexForTimestamp(double timevalue)
{
    if (m_allRowsHaveTime)
    {
        quint64 intervalSize = m_TimeIndexList.size();
        quint64 intervalStart = 0;
        quint64 middle = 0;

        while (intervalSize > 1)
        {
            middle = intervalStart + intervalSize / 2;
            quint64 temp = m_TimeIndexList[middle].first;
            if (timevalue > temp)
            {
                intervalStart = middle;
            }
            else if (timevalue == temp)
            {
                break;
            }
            intervalSize = intervalSize / 2;
        }
        return m_TimeIndexList[middle].second;
    }
    return 0;
}
