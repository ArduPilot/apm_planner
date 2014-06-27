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
#include "QGC.h"

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
QString AP2DataPlotThread::makeCreateTableString(QString tablename, QString formatstr,QString variablestr)
{
    QStringList varchar = variablestr.split(",");
    QString mktable = "CREATE TABLE '" + tablename + "' (idx integer PRIMARY KEY";
    for (int j=0;j<varchar.size();j++)
    {
        QString name = varchar[j].trimmed();
        name = "\"" + name + "\"";
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
            QLOG_DEBUG() << "AP2DataPlotThread::makeCreateTableString(): NEW UNKNOWN VALUE" << typeCode;
        }
    }
    mktable.append(");");
    return mktable;
}

QString AP2DataPlotThread::makeInsertTableString(QString tablename, QString variablestr)
{
    QString inserttable = "insert or replace into '" + tablename + "' (idx";
    QString insertvalues = "(?";
    QStringList linesplit = variablestr.split(",");
    for (int j=0;j<linesplit.size();j++)
    {
        QString name = linesplit[j].trimmed();
        inserttable.append(",\"" + name + "\"");
        insertvalues.append(",?");
    }
    inserttable.append(")");
    insertvalues.append(")");
    QString final = inserttable + " values " + insertvalues + ";";
    return final;
}

void AP2DataPlotThread::run()
{
    emit startLoad();
    int type = -1;
    if (m_fileName.toLower().endsWith(".bin"))
    {
        //It's a binary file
        type = 1;
    }
    else if (m_fileName.toLower().endsWith(".log"))
    {
        //It's a ascii log.
        type = 2;
    }
    else
    {
        emit error("Unable to detect file type from filename. Ensure the file has a .bin or .log extension");
        return;
    }
    int errorcount = 0;
    m_stop = false;

    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }
    QByteArray block;
    QMap<unsigned char,unsigned char> typeToLengthMap;
    QMap<unsigned char,QString > typeToNameMap;
    QMap<unsigned char,QString > typeToFormatMap;
    QMap<unsigned char,QString > typeToLabelMap;
    QStringList tables;
    int index = 0;

    if (!m_db->transaction())
    {
        emit error("Unable to start database transaction 1");
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
    if (!m_db->transaction())
    {
        emit error("Unable to start database transaction 2");
        return;
    }
    if (type == 1)
    {
        bool firstactual = true;
        while (!logfile.atEnd())
        {
            int nonpacketcounter = 0;
            emit loadProgress(logfile.pos(),logfile.size());
            block.append(logfile.read(8192));
            for (int i=0;i<block.size();i++)
            {
                if (i+3 < block.size()) //Enough room for a header
                {
                    if (static_cast<unsigned char>(block.at(i)) == 0xA3 && static_cast<unsigned char>(block.at(i+1)) == 0x95)
                    {
                        //It's a valid header.
                        unsigned char type = static_cast<unsigned char>(block.at(i+2));
                        if (type == 0x80)
                        {
                            //Message format packet
                            if (i+92 >= block.size())
                            {
                                //Not enough data in the block
                                break; //Break out of the for loop to let the file load more
                            }

                            QByteArray packet = block.mid(i+3,86);
                            block = block.remove(i,89); //Remove both the 3 byte header and the packet
                            i--;

                            unsigned char msg_type = packet.at(0); //Message type defined in the format struct
                            unsigned char msg_length = packet.at(1);  //Message length
                            QString name = packet.mid(2,4); //Name of the message
                            QString format = packet.mid(6,16); //Format of the variables
                            QString labels = packet.mid(22,64); //comma delimited list of variable names.
                            typeToFormatMap[msg_type] = format;
                            typeToLabelMap[msg_type] = labels;
                            typeToLengthMap[msg_type] = msg_length;
                            typeToNameMap[msg_type] = name;

                            if (msg_type == 0x80)
                            {
                                //Mesage is a format type, we don't want to include it
                                continue;
                            }
                            if (format == "" || labels == "")
                            {
                                QLOG_DEBUG() << "AP2DataPlotThread::run(): empty format string or labels string for type" << msg_type << name;
                                continue;
                            }
                            if (!tables.contains(name)) {
                                QSqlQuery mktablequery(*m_db);
                                mktablequery.prepare(makeCreateTableString(name,format,labels));
                                if (!mktablequery.exec())
                                {
                                    emit error("Error creating table for: " + name + " : " + m_db->lastError().text());
                                    return;
                                }
                                tables.append(name);
                            }
                            QSqlQuery *query = new QSqlQuery(*m_db);
                            QString final = makeInsertTableString(name,labels);
                            if (!query->prepare(final))
                            {
                                emit error("Error preparing inserttable: " + final + " Error is: " + query->lastError().text());
                                return;
                            }
                            //typeID,length,name,format
                            fmtinsertquery.bindValue(0,index);
                            fmtinsertquery.bindValue(1,0);
                            fmtinsertquery.bindValue(2,name);
                            fmtinsertquery.bindValue(3,format);
                            fmtinsertquery.bindValue(4,labels);
                            if (!fmtinsertquery.exec())
                            {
                                emit error("Error execing insertquery" + fmtinsertquery.lastError().text());
                                return;
                            }
                            nameToInsertQuery[name] = query;
                            index++;
                        }
                        else
                        {
                            //Data packet
                            if (!typeToLengthMap.contains(type))
                            {
                                QLOG_DEBUG() << "AP2DataPlotThread::run(): No entry in typeToLengthMap for type:" << type;
                                break;
                            }
                            if (i+3+typeToLengthMap.value(type) >= block.size())
                            {
                                //Not enough data yet.
                                QLOG_DEBUG() << "AP2DataPlotThread::run(): Not enough data from file, reading more...." << i-block.size() << typeToLengthMap.value(type);
                                break;
                            }
                            QByteArray packet = block.mid(i+3,typeToLengthMap.value(type)-3);
                            block.remove(i,packet.size()+3); //Remove both the 3 byte header and the packet
                            i--;

                            QString name = typeToNameMap.value(type);
                            if (nameToInsertQuery.contains(name))
                            {
                                QString linetoemit = typeToNameMap.value(type);
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

                                nameToInsertQuery[name]->bindValue(0,index);
                                index++;
                                QString formatstr = typeToFormatMap.value(type);

                                int formatpos = 0;
                                for (int j=0;j<formatstr.size();j++)
                                {
                                    QChar typeCode = formatstr.at(j);
                                    if (typeCode == 'b') //int8_t
                                    {
                                        char val = static_cast<char>(packet.at(formatpos));
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=1;
                                    }
                                    else if (typeCode == 'B') //uint8_t
                                    {
                                        //unsigned char
                                        unsigned char val = static_cast<unsigned char>(packet.at(formatpos));
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=1;
                                    }
                                    else if (typeCode == 'h') //int16_t
                                    {
                                        //signed short
                                        short val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=2;
                                    }
                                    else if (typeCode == 'H') //uint16_t
                                    {
                                        //unsigned short
                                        unsigned short val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=2;
                                    }
                                    else if (typeCode == 'i') //int32_t
                                    {
                                        int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        val += static_cast<unsigned char>(packet.at(formatpos+2)) << 16;
                                        val += static_cast<unsigned char>(packet.at(formatpos+3)) << 24;
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=4;
                                    }
                                    else if (typeCode == 'I') //uint32_t
                                    {
                                        unsigned int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        val += static_cast<unsigned char>(packet.at(formatpos+2)) << 16;
                                        val += static_cast<unsigned char>(packet.at(formatpos+3)) << 24;
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=4;
                                    }
                                    else if (typeCode == 'f') //float
                                    {
                                        unsigned long val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        val += static_cast<unsigned char>(packet.at(formatpos+2)) << 16;
                                        val += static_cast<unsigned char>(packet.at(formatpos+3)) << 24;
                                        float f = *(float*)&val;
                                        //float val = packet.mid(formatpos,16).at(0);
                                        nameToInsertQuery[name]->bindValue(j+1,f);
                                        linetoemit += "," + QString::number(f,'f',4);
                                        formatpos+=4;
                                    }
                                    else if (typeCode == 'n') //char(4)
                                    {
                                        QString val = packet.mid(formatpos,4);
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + val;
                                        formatpos += 4;
                                    }
                                    else if (typeCode == 'N') //char(16)
                                    {
                                        QString val = packet.mid(formatpos,16);
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + val;
                                        formatpos += 16;
                                    }
                                    else if (typeCode == 'Z') //char(64)
                                    {
                                        QString val = packet.mid(formatpos,64);
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + val;
                                        formatpos += 64;
                                    }
                                    else if (typeCode == 'c') //int16_t * 100
                                    {
                                        //unsigned short
                                        int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                        linetoemit += "," + QString::number(val / 100.0,'f',4);
                                        formatpos+=2;
                                    }
                                    else if (typeCode == 'C') //uint16_t * 100
                                    {
                                        unsigned int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                        linetoemit += "," + QString::number(val / 100.0,'f',4);
                                        formatpos+=2;
                                    }
                                    else if (typeCode == 'e') //int32_t * 100
                                    {
                                        int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        val += static_cast<unsigned char>(packet.at(formatpos+2)) << 16;
                                        val += static_cast<unsigned char>(packet.at(formatpos+3)) << 24;
                                        nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                        linetoemit += "," + QString::number(val / 100.0,'f',4);
                                        formatpos+=4;
                                    }
                                    else if (typeCode == 'E') //uint32_t * 100
                                    {
                                        unsigned int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        val += static_cast<unsigned char>(packet.at(formatpos+2)) << 16;
                                        val += static_cast<unsigned char>(packet.at(formatpos+3)) << 24;
                                        nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                        linetoemit += "," + QString::number(val / 100.0,'f',4);
                                        formatpos+=4;
                                    }
                                    else if (typeCode == 'L') //uint32_t GPS Lon/Lat * 10000000
                                    {
                                        unsigned int val = static_cast<unsigned char>(packet.at(formatpos));
                                        val += static_cast<unsigned char>(packet.at(formatpos+1)) << 8;
                                        val += static_cast<unsigned char>(packet.at(formatpos+2)) << 16;
                                        val += static_cast<unsigned char>(packet.at(formatpos+3)) << 24;
                                        nameToInsertQuery[name]->bindValue(j+1,val / 10000000.0);
                                        linetoemit += "," + QString::number(val / 10000000.0,'f',6);
                                        formatpos+=4;
                                    }
                                    else if (typeCode == 'M')
                                    {
                                        char val = static_cast<char>(packet.at(formatpos));
                                        nameToInsertQuery[name]->bindValue(j+1,val);
                                        linetoemit += "," + QString::number(val,'f',0);
                                        formatpos+=1;

                                    }
                                    else
                                    {
                                        //Unknown!
                                        QLOG_DEBUG() << "AP2DataPlotThread::run(): ERROR UNKNOWN DATA TYPE" << typeCode;
                                    }
                                }
                                emit lineRead(linetoemit);

                                if (!nameToInsertQuery[name]->exec())
                                {
                                    emit error("Error execing:" + nameToInsertQuery[name]->executedQuery() + " error was " + nameToInsertQuery[name]->lastError().text());
                                    return;
                                }
                            }
                            else
                            {
                                QLOG_DEBUG() << "AP2DataPlotThread::run(): No query available for param category" << name;
                            }
                        }

                    }
                    else
                    {
                        //Non packet
                        nonpacketcounter++;
                    }
                }
            }
            if (nonpacketcounter > 0)
            {
                QLOG_DEBUG() << "AP2DataPlotThread::run(): Non packet bytes found in log file" << nonpacketcounter << "bytes filtered out. This may be a corrupt log";
            }
        }
    } // if (type == 1) //binary datalog file
    else if (type == 2) //ascii datalog file
    {
        bool firstactual = true;
        while (!logfile.atEnd() && !m_stop)
        {
            emit loadProgress(logfile.pos(),logfile.size());
            QString line = logfile.readLine();
            emit lineRead(line);
            QStringList linesplit = line.replace("\r","").replace("\n","").split(",");
            if (linesplit.size() > 0)
            {
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
                            for (int i=5;i<linesplit.size();i++)
                            {
                                QString name = linesplit[i].trimmed();
                                valuestr += name + ",";
                            }
                            valuestr = valuestr.mid(0,valuestr.length()-1);

                            QSqlQuery mktablequery(*m_db);
                            mktablequery.prepare(makeCreateTableString(type,descstr,valuestr));
                            if (!mktablequery.exec())
                            {
                                emit error("Error creating table for: " + type + " : " + m_db->lastError().text());
                                return;
                            }
                            QSqlQuery *query = new QSqlQuery(*m_db);
                            QString final = makeInsertTableString(type,valuestr);
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
                                    emit error("Unable to start database transaction 4");
                                    return;
                                }
                                firstactual = false;
                            }
                            QString typestr = nameToTypeString[name];
                            nameToInsertQuery[name]->bindValue(0,index);
                            static QString intdef("bBhHiI");
                            static QString floatdef("cCeEfL");
                            static QString chardef("nNZM");
                            if (typestr.size() != linesplit.size() - 1)
                            {
                                QLOG_DEBUG() << "Bound values for" << name << "count:" << nameToInsertQuery[name]->boundValues().values().size() << "actual" << linesplit.size() << typestr.size();
                                QLOG_DEBUG() << "Error in line:" << index << "param" << name << "parameter mismatch";
                                errorcount++;
                            }
                            else
                            {
                                bool foundError = false;
                                for (int i = 1; i < linesplit.size(); i++)
                                {
                                    bool ok;
                                    QChar typeCode = typestr.at(i - 1);
                                    QString valStr = linesplit[i].trimmed();
                                    if (intdef.contains(typeCode))
                                    {
                                        int val = valStr.toInt(&ok);
                                        if (ok)
                                        {
                                            nameToInsertQuery[name]->bindValue(i, val);
                                        }
                                        else
                                        {
                                            QLOG_DEBUG() << "Failed to convert " << valStr << " to an integer number.";
                                            foundError = true;
                                        }
                                    }
                                    else if (chardef.contains(typeCode))
                                    {
                                        nameToInsertQuery[name]->bindValue(i, valStr);
                                    }
                                    else if (floatdef.contains(typeCode))
                                    {
                                        double val = valStr.toDouble(&ok);
                                        if (ok && !isinf(val) && !isnan(val))
                                        {
                                            nameToInsertQuery[name]->bindValue(i, val);
                                        }
                                        else
                                        {
                                            QLOG_DEBUG() << "Failed to convert " << valStr << " to a floating point number.";
                                            foundError = true;
                                        }
                                    }
                                    else
                                    {
                                        QLOG_DEBUG() << "AP2DataPlotThread::run(): Unknown data value found" << typeCode;
                                        emit error(QString("Unknown data value found: %1").arg(typeCode));
                                        return;
                                    }
                                }
                                if (foundError)
                                {
                                    QLOG_DEBUG() << "Found an error on line " << index << ", skipping it.";
                                    ++errorcount;
                                }
                                else if (!nameToInsertQuery[name]->exec())
                                {
                                    emit error("Error execing:" + nameToInsertQuery[name]->executedQuery() + " error was " + nameToInsertQuery[name]->lastError().text());
                                    return;
                                }
                            }
                        }
                        else
                        {
                            QLOG_DEBUG() << "Found line " << index << " with unknown command " << name << ", skipping...";
                        }
                    }

                }
            }
        }
    } //else if (type == 2) //ascii datalog file




    if (!m_db->commit())
    {
        emit error("Unable to commit database transaction 4");
        return;
    }
    QLOG_DEBUG() << "AP2DataPlotThread::run(): Log loading finished, pos:" << logfile.pos() << "filesize:" << logfile.size();
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
