#include "AP2DataPlotThread.h"
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlError>
#include "QsLog.h"

AP2DataPlotThread::AP2DataPlotThread(QObject *parent) :
    QThread(parent)
{

}
void AP2DataPlotThread::loadFile(QString file,QSqlDatabase *db)
{
    m_fileName = file;
    m_db = db;
    start();
}
void AP2DataPlotThread::run()
{
    int errorcount = 0;
    m_stop = false;
    emit startLoad();
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }
    int index = 0;

    if (!m_db->transaction())
    {
        emit error("Unable to start database transaction");
        return;
    }

    QSqlQuery fmttablecreate(*m_db);
    fmttablecreate.prepare("CREATE TABLE 'FMT' (typeID integer PRIMARY KEY,length integer,name varchar(200),format varchar(6000),val varchar(6000));");
    if (!fmttablecreate.exec())
    {
        emit error("Error creating FMT table: " + m_db->lastError().text());
        return;
    }
    QSqlQuery fmtinsertquery;
    if (!fmtinsertquery.prepare("INSERT INTO 'FMT' (typeID,length,name,format,val) values (?,?,?,?,?);"))
    {
        emit error("Error preparing FMT insert statement: " + fmtinsertquery.lastError().text());
        return;
    }
    QMap<QString,QSqlQuery*> nameToInsertQuery;
    QMap<QString,QString> nameToTypeString;
    if (!m_db->commit())
    {
        emit error("Unable to commit database transaction 1");
        return;
    }
    bool firstactual = true;
    while (!logfile.atEnd() && !m_stop)
    {
        emit loadProgress(logfile.pos(),logfile.size());
        QString line = logfile.readLine();
        emit lineRead(line);
        QStringList linesplit = line.replace("\r","").replace("\n","").split(",");
        if (linesplit.size() > 0)
        {
            if (index == 0)
            {
                //First record
                if (!m_db->transaction())
                {
                    emit error("Unable to start database transaction");
                    return;
                }
            }
            index++;
            if (line.startsWith("FMT"))
            {
                //Format line
                if (linesplit.size() > 4)
                {
                    QString type = linesplit[3].trimmed();
                    if (type != "FMT")
                    {
                        QString descstr = linesplit[4].trimmed();
                        nameToTypeString[type] = descstr;
                        if (descstr == "")
                        {
                            continue;
                        }
                        QString valuestr = "";
                        QString inserttable = "insert or replace into '" + type + "' (idx";
                        QString insertvalues = "(?";
                        QString mktable = "CREATE TABLE '" + type + "' (idx integer PRIMARY KEY";
                        for (int i=5;i<linesplit.size();i++)
                        {
                            QString name = linesplit[i].trimmed();
                            char type = descstr.at(i-5).toAscii();
                            valuestr += name + ",";
                            qDebug() << name << type;
                            if (type == 'I') //uint32_t
                            {
                                mktable.append("," + name + " integer");
                            }
                            else if (type == 'f') //float
                            {
                                mktable.append("," + name + " real");
                            }
                            else if (type == 'h') //int16_t
                            {
                                mktable.append("," + name + " real");
                            }
                            else if (type == 'L') //int32_t (lat/long)
                            {
                                mktable.append("," + name + " integer");
                            }
                            else if (type == 'e') //int32_t * 100
                            {
                                mktable.append("," + name + " real");
                            }
                            else if (type == 'c') //int16_t * 100
                            {
                                mktable.append("," + name + " real");
                            }
                            else
                            {
                                mktable.append("," + name + " real");
                            }
                            inserttable.append("," + name);
                            insertvalues.append(",?");

                            //fieldnames.append(linesplit[i].trimmed());
                        }
                        inserttable.append(")");
                        insertvalues.append(")");
                        valuestr = valuestr.mid(0,valuestr.length()-1);
                        QString final = inserttable + " values " + insertvalues + ";";


                        mktable.append(");");
                        QSqlQuery mktablequery(*m_db);
                        mktablequery.prepare(mktable);
                        if (!mktablequery.exec())
                        {
                            emit error("Error creating table for: " + type + " : " + m_db->lastError().text());
                            return;
                        }
                        QSqlQuery *query = new QSqlQuery(*m_db);
                        if (!query->prepare(final))
                        {
                            emit error("Error preparing inserttable: " + final + " Error is: " + query->lastError().text());
                            return;
                        }
                        //typeID,length,name,format
                        fmtinsertquery.bindValue(0,index);
                        fmtinsertquery.bindValue(1,0);
                        fmtinsertquery.bindValue(2,type);
                        fmtinsertquery.bindValue(3,descstr);
                        fmtinsertquery.bindValue(4,valuestr);
                        fmtinsertquery.exec();
                        nameToInsertQuery[type] = query;
                    }
                }
                else
                {
                    QLOG_ERROR() << "Error with line in plot log file:" << line;
                }
            }
            else
            {
                if (linesplit.size() > 1)
                {
                    QString name = linesplit[0].trimmed();
                    if (nameToInsertQuery.contains(name))
                    {
                        if (firstactual)
                        {
                            if (!m_db->commit())
                            {
                                emit error("Unable to commit database transaction 2");
                                return;
                            }
                            if (!m_db->transaction())
                            {
                                emit error("Unable to start database transaction 3");
                                return;
                            }
                            firstactual = false;
                        }
                        QString typestr = nameToTypeString[name];
                        nameToInsertQuery[name]->bindValue(0,index);
                        if (typestr.size() != linesplit.size() - 1)
                        {
                            QLOG_DEBUG() << "Bound values for" << name << "count:" << nameToInsertQuery[name]->boundValues().values().size() << "actual" << linesplit.size() << typestr.size();
                            QLOG_DEBUG() << "Error in line:" << index << "param" << name << "parameter mismatch";
                            errorcount++;
                        }
                        else
                        {
                            for (int i=1;i<linesplit.size();i++)
                            {
                                if (typestr.at(i-1).toAscii() == 'I')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toInt());
                                }
                                else if (typestr.at(i-1).toAscii() == 'f')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toFloat());
                                }
                                else if (typestr.at(i-1).toAscii() == 'h')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toInt());
                                }
                                else if (typestr.at(i-1).toAscii() == 'c')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toInt() * 100);
                                }
                                else if (typestr.at(i-1).toAscii() == 'C')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toInt() * 100);
                                }
                                else if (typestr.at(i-1).toAscii() == 'e')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toInt() * 100);
                                }
                                else if (typestr.at(i-1).toAscii() == 'E')
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toInt() * 100);
                                }
                                else if (typestr.at(i-1).toAscii() == 'L')
                                {
                                    nameToInsertQuery[name]->bindValue(i,(qlonglong)linesplit[i].toLong());
                                }
                                else
                                {
                                    nameToInsertQuery[name]->bindValue(i,linesplit[i].toFloat());
                                }
                            }
                            if (!nameToInsertQuery[name]->exec())
                            {
                                emit error("Error execing:" + nameToInsertQuery[name]->executedQuery() + " error was " + nameToInsertQuery[name]->lastError().text());
                                return;
                            }
                        }
                    }
                }

            }
        }
    }
    if (!m_db->commit())
    {
        emit error("Unable to commit database transaction 2");
        return;
    }
    qDebug() << logfile.pos() << logfile.size() << logfile.atEnd();
    if (m_stop)
    {
        QLOG_ERROR() << "Plot Log loading was canceled after" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds";
        emit error("Log loading Canceled");
    }
    else
    {
        QLOG_INFO() << "Plot Log loading took" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds";
        emit done(errorcount);
    }
}
