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
#include "MAVLinkDecoder.h"
#include "QsLog.h"
#include "QGC.h"


AP2DataPlotThread::AP2DataPlotThread(QObject *parent) :
    QThread(parent)
{
    qRegisterMetaType<MAV_TYPE>("MAV_TYPE");
}
void AP2DataPlotThread::loadFile(QString file,QSqlDatabase *db)
{
    m_fileName = file;
    m_db = db;
    start();
}
QString AP2DataPlotThread::makeCreateTableString(QString tablename, QString formatstr,QString variablestr)
{
    QStringList varchar = variablestr.split(",",QString::SkipEmptyParts);
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
    QString insertvalues = "(:idx,";
    QStringList linesplit = variablestr.split(",",QString::SkipEmptyParts);
    for (int j=0;j<linesplit.size();j++)
    {
        QString name = linesplit[j].trimmed();
        inserttable.append("," + name + "");
        insertvalues.append(":" + name + ((j < linesplit.size()-1) ? "," : ""));
    }
    inserttable.append(")");
    insertvalues.append(")");
    QString final = inserttable + " values " + insertvalues + ";";
    return final;
}
bool AP2DataPlotThread::createFMTTable()
{
    QSqlQuery fmttablecreate(*m_db);
    fmttablecreate.prepare("CREATE TABLE 'FMT' (idx integer PRIMARY KEY, typeid integer,length integer,name varchar(200),format varchar(6000),val varchar(6000));");
    if (!fmttablecreate.exec())
    {
        emit error("Error creating FMT table: " + m_db->lastError().text());
        return false;
    }
    return true;
}
bool AP2DataPlotThread::createFMTInsert(QSqlQuery *query)
{
    if (!query->prepare("INSERT INTO 'FMT' (idx,typeid,length,name,format,val) values (?,?,?,?,?,?);"))
    {
        return false;
    }
    return true;
}
bool AP2DataPlotThread::createIndexTable()
{
    QSqlQuery indextablecreate(*m_db);
    if (!indextablecreate.prepare("CREATE TABLE 'INDEX' (idx integer PRIMARY KEY, value varchar(200));"))
    {
        emit error("Error preparing INDEX table: " + m_db->lastError().text());
        return false;
    }
    if (!indextablecreate.exec())
    {
        emit error("Error creating INDEX table: " + m_db->lastError().text());
        return false;
    }
    return true;
}
bool AP2DataPlotThread::createIndexInsert(QSqlQuery *query)
{
    if (!query->prepare("INSERT INTO 'INDEX' (idx,value) values (:idx,:value);"))
    {
        return false;
    }
    return true;
}

void AP2DataPlotThread::loadBinaryLog()
{
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }
    bool firstactual = true;
    QByteArray block;
    int paramtype = -1;
    QMap<unsigned char,unsigned char> typeToLengthMap;
    QMap<unsigned char,QString > typeToNameMap;
    QMap<unsigned char,QString > typeToFormatMap;
    QMap<unsigned char,QString > typeToLabelMap;
    QStringList tables;

    if (!m_db->transaction())
    {
        emit error("Unable to start database transaction 1");
        return;
    }

    if (!createFMTTable())
    {
        //Error already emitted.
        return;
    }
    QSqlQuery fmtinsertquery(*m_db);
    if (!createFMTInsert(&fmtinsertquery))
    {
        emit error("Error preparing FMT insert statement: " + fmtinsertquery.lastError().text());
        return;
    }

    if (!createIndexTable())
    {
        //Error already emitted.
        return;
    }
    QSqlQuery indexinsertquery(*m_db);
    if (!createIndexInsert(&indexinsertquery))
    {
        emit error("Error preparing INDEX insert statement: " + indexinsertquery.lastError().text());
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



    int index = 0;
    m_loadedLogType = MAV_TYPE_GENERIC;

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
                        if (name == "PARM")
                        {
                            paramtype = msg_type;
                        }
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
                        fmtinsertquery.bindValue(1,msg_type);
                        fmtinsertquery.bindValue(2,0);
                        fmtinsertquery.bindValue(3,name);
                        fmtinsertquery.bindValue(4,format);
                        fmtinsertquery.bindValue(5,labels);
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
                        QDataStream packetstream(packet);
                        packetstream.setByteOrder(QDataStream::LittleEndian);
                        packetstream.setFloatingPointPrecision(QDataStream::SinglePrecision);

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

                            nameToInsertQuery[name]->bindValue(":idx",index);
                            indexinsertquery.bindValue(0,index);
                            indexinsertquery.bindValue(1,name);
                            if (!indexinsertquery.exec())
                            {
                                emit error("Error execing:" + indexinsertquery.executedQuery() + " error was " + indexinsertquery.lastError().text());
                                return;
                            }
                            index++;
                            QString formatstr = typeToFormatMap.value(type);

                            for (int j=0;j<formatstr.size();j++)
                            {
                                QChar typeCode = formatstr.at(j);
                                if (typeCode == 'b') //int8_t
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else if (typeCode == 'B') //uint8_t
                                {
                                    quint8 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else if (typeCode == 'h') //int16_t
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else if (typeCode == 'H') //uint16_t
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else if (typeCode == 'i') //int32_t
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else if (typeCode == 'I') //uint32_t
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else if (typeCode == 'f') //float
                                {
                                    float f;
                                    packetstream >> f;
                                    nameToInsertQuery[name]->bindValue(j+1,f);
                                    linetoemit += "," + QString::number(f,'f',4);
                                }
                                else if (typeCode == 'n') //char(4)
                                {

                                    QString val = "";
                                    for (int i=0;i<4;i++)
                                    {
                                        quint8 ch;
                                        packetstream >> ch;
                                        val += static_cast<char>(ch);
                                    }
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + val;
                                }
                                else if (typeCode == 'N') //char(16)
                                {
                                    QString val = "";
                                    for (int i=0;i<16;i++)
                                    {
                                        quint8 ch;
                                        packetstream >> ch;
                                        val += static_cast<char>(ch);
                                    }
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + val;
                                }
                                else if (typeCode == 'Z') //char(64)
                                {
                                    QString val = "";
                                    for (int i=0;i<64;i++)
                                    {
                                        quint8 ch;
                                        packetstream >> ch;
                                        val += static_cast<char>(ch);
                                    }
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + val;
                                }
                                else if (typeCode == 'c') //int16_t * 100
                                {
                                    qint16 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                }
                                else if (typeCode == 'C') //uint16_t * 100
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                }
                                else if (typeCode == 'e') //int32_t * 100
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                }
                                else if (typeCode == 'E') //uint32_t * 100
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val / 100.0);
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                }
                                else if (typeCode == 'L') //uint32_t GPS Lon/Lat * 10000000
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val / 10000000.0);
                                    linetoemit += "," + QString::number(val / 10000000.0,'f',6);
                                }
                                else if (typeCode == 'M')
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    nameToInsertQuery[name]->bindValue(j+1,val);
                                    linetoemit += "," + QString::number(val,'f',0);
                                }
                                else
                                {
                                    //Unknown!
                                    QLOG_DEBUG() << "AP2DataPlotThread::run(): ERROR UNKNOWN DATA TYPE" << typeCode;
                                }
                            }
                            if (type == paramtype && m_loadedLogType == MAV_TYPE_GENERIC)
                            {
                                if (linetoemit.contains("RATE_RLL_P") || linetoemit.contains("H_SWASH_PLATE"))
                                {
                                    m_loadedLogType = MAV_TYPE_QUADROTOR;
                                }
                                if (linetoemit.contains("PTCH2SRV_P"))
                                {
                                    m_loadedLogType = MAV_TYPE_FIXED_WING;
                                }
                                if (linetoemit.contains("SKID_STEER_OUT"))
                                {
                                    m_loadedLogType = MAV_TYPE_GROUND_ROVER;
                                }
                            }
                            //emit lineRead(linetoemit);

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
}
void AP2DataPlotThread::loadAsciiLog()
{

    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }
    if (!m_db->transaction())
    {
        emit error("Unable to start database transaction 1");
        return;
    }

    if (!createFMTTable())
    {
        //Error already emitted.
        return;
    }
    QSqlQuery fmtinsertquery(*m_db);
    if (!createFMTInsert(&fmtinsertquery))
    {
        emit error("Error preparing FMT insert statement: " + fmtinsertquery.lastError().text());
        return;
    }

    if (!createIndexTable())
    {
        //Error already emitted.
        return;
    }
    QSqlQuery indexinsertquery(*m_db);
    if (!createIndexInsert(&indexinsertquery))
    {
        emit error("Error preparing INDEX insert statement: " + indexinsertquery.lastError().text());
        return;
    }



    bool firstactual = true;
    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 0;
    int errorcount = 0;
    QMap<QString,QSqlQuery*> nameToInsertQuery;
    QMap<QString,QString> nameToTypeString;


    while (!logfile.atEnd() && !m_stop)
    {
        emit loadProgress(logfile.pos(),logfile.size());
        QString line = logfile.readLine();
        emit lineRead(line);
        if (m_loadedLogType == MAV_TYPE_GENERIC)
        {
            if ((line.contains("ArduCopter") || (line.contains("PARM") && (line.contains("RATE_RLL_P") || line.contains("H_SWASH_PLATE")))))
            {
                m_loadedLogType = MAV_TYPE_QUADROTOR;
            }
            if (line.contains("ArduPlane") || (line.contains("PARM") && line.contains("PTCH2SRV_P")))
            {
                m_loadedLogType = MAV_TYPE_FIXED_WING;
            }
            if (line.contains("ArduRover") || (line.contains("PARM") && line.contains("SKID_STEER_OUT")))
            {
                m_loadedLogType = MAV_TYPE_GROUND_ROVER;
            }
        }
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
                        fmtinsertquery.bindValue(1,linesplit[1].trimmed().toInt());
                        fmtinsertquery.bindValue(2,0);
                        fmtinsertquery.bindValue(3,type);
                        fmtinsertquery.bindValue(4,descstr);
                        fmtinsertquery.bindValue(5,valuestr);
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
                        nameToInsertQuery[name]->bindValue(":idx",index);
                        indexinsertquery.bindValue(0,index);
                        indexinsertquery.bindValue(1,name);
                        if (!indexinsertquery.exec())
                        {
                            emit error("Error execing:" + indexinsertquery.executedQuery() + " error was " + indexinsertquery.lastError().text());
                            return;
                        }
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
}
void AP2DataPlotThread::loadTLog()
{
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }

    if (!m_db->transaction())
    {
        emit error("Unable to start database transaction 1");
        return;
    }

    if (!createFMTTable())
    {
        //Error already emitted.
        return;
    }
    QSqlQuery fmtinsertquery(*m_db);
    if (!createFMTInsert(&fmtinsertquery))
    {
        emit error("Error preparing FMT insert statement: " + fmtinsertquery.lastError().text());
        return;
    }

    if (!createIndexTable())
    {
        //Error already emitted.
        return;
    }
    QSqlQuery indexinsertquery(*m_db);
    if (!createIndexInsert(&indexinsertquery))
    {
        emit error("Error preparing INDEX insert statement: " + indexinsertquery.lastError().text());
        return;
    }



    bool firstactual = true;
    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 0;
    int errorcount = 0;
    QMap<QString,QSqlQuery*> nameToInsertQuery;
    QMap<QString,QString> nameToTypeString;


    int bytesize = 0;
    QByteArray timebuf;
    bool firsttime = true;
    qint64 lastLogTime = 0;
    mavlink_message_t message;
    mavlink_status_t status;
    bool nexttime = false;
    int delay;
    int fmtindex = 0;
    index = 500;
    QList<uint64_t*> mavlinkList;
    m_fieldCount=0;
    qint64 lastPcTime = QDateTime::currentMSecsSinceEpoch();
    decoder = new MAVLinkDecoder();
    QList<quint64> lastunixtimemseclist;
    //connect(decoder,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)),Qt::DirectConnection);

    while (!logfile.atEnd() && !m_stop)
    {
        emit loadProgress(logfile.pos(),logfile.size());
        QByteArray bytes = logfile.read(128);
        bytesize+=128;

        for (int i=0;i<bytes.size();i++)
        {
            unsigned int decodeState = mavlink_parse_char(14, (uint8_t)(bytes[i]), &message, &status);
            if (decodeState != 1)
            {
                //Not a mavlink byte!
                if (nexttime)
                {
                    timebuf.append(bytes[i]);
                }
                if (timebuf.size() == 8)
                {
                    nexttime = false;

                    //Should be a timestamp for the next packet.
                    unsigned long logmsecs = static_cast<unsigned char>(timebuf.at(0)) << 56;
                    logmsecs += static_cast<unsigned char>(timebuf.at(1)) << 48;
                    logmsecs += static_cast<unsigned char>(timebuf.at(2)) << 40;
                    logmsecs += static_cast<unsigned char>(timebuf.at(3)) << 32;
                    logmsecs = static_cast<unsigned char>(timebuf.at(4)) << 24;
                    logmsecs += static_cast<unsigned char>(timebuf.at(5)) << 16;
                    logmsecs += static_cast<unsigned char>(timebuf.at(6)) << 8;
                    logmsecs += static_cast<unsigned char>(timebuf.at(7)) << 0;

                    timebuf.clear();

                    if (firsttime)
                    {
                        firsttime = false;
                        lastLogTime = logmsecs;
                        lastPcTime = QDateTime::currentMSecsSinceEpoch();
                    }
                    else
                    {
                        //Difference in time between the last time we read a timestamp, and this time
                        qint64 pcdiff = QDateTime::currentMSecsSinceEpoch() - lastPcTime;
                        lastPcTime = QDateTime::currentMSecsSinceEpoch();

                        //Difference in time between the last timestamp we fired off, and this one
                        qint64 logdiff = logmsecs - lastLogTime;
                        lastLogTime = logmsecs;
                        logdiff /= 1000;

                        if (logdiff < pcdiff)
                        {
                            //The next mavlink packet was fired faster than our loop is running, send it immediatly
                            //Fire immediatly
                            delay = 0;
                        }
                        else
                        {
                            //The next mavlink packet was sent logdiff-pcdiff millseconds after the current time
                            delay = logdiff-pcdiff;

                        }
                    }

                }
            }
            else if (decodeState == 1)
            {
                nexttime = true;
                //Good decode
                if (message.sysid == 255)
                {
                    //GCS packet, ignore it
                }
                else
                {

                    uint64_t *target = (uint64_t*)malloc(message.len * 4);
                    memcpy(target,message.payload64,message.len * 4);
                    mavlinkList.append(target);
                    QList<QPair<QString,QVariant> > retvals = decoder->receiveMessage(0,message);
                    QString name = decoder->getMessageName(message.msgid);

                    if (!m_msgNameToInsertQuery.contains(name))
                    {

                        QList<QString> fieldnames = decoder->getFieldList(name);
                        QString variablenames;
                        QString typechars;
                        for (int i=0;i<fieldnames.size();i++)
                        {
                            mavlink_field_info_t fieldinfo = decoder->getFieldInfo(name,fieldnames.at(i));
                            variablenames += QString(fieldinfo.name) + ",";
                            switch (fieldinfo.type)
                            {
                                case MAVLINK_TYPE_CHAR:
                                {
                                    typechars += "b";
                                }
                                break;
                                case MAVLINK_TYPE_UINT8_T:
                                {
                                    typechars += "B";
                                }
                                break;
                                case MAVLINK_TYPE_INT8_T:
                                {
                                    typechars += "b";
                                }
                                break;
                                case MAVLINK_TYPE_UINT16_T:
                                {
                                    typechars += "H";
                                }
                                break;
                                case MAVLINK_TYPE_INT16_T:
                                {
                                    typechars += "h";
                                }
                                break;
                                case MAVLINK_TYPE_UINT32_T:
                                {
                                    typechars += "I";
                                }
                                break;
                                case MAVLINK_TYPE_INT32_T:
                                {
                                    typechars += "i";
                                }
                                break;
                                case MAVLINK_TYPE_FLOAT:
                                {
                                    typechars += "f";
                                }
                                break;
                                case MAVLINK_TYPE_UINT64_T:
                                {
                                    typechars += "I";
                                }
                                break;
                                case MAVLINK_TYPE_INT64_T:
                                {
                                    typechars += "i";
                                }
                                break;
                                default:
                                {
                                    qDebug() << "Unknown type:" << QString::number(fieldinfo.type);
                                }
                                break;
                            }
                        }
                        QString createstring = makeCreateTableString(name,typechars,variablenames);
                        QString insertstring = makeInsertTableString(name,variablenames);
                        //if (!fmtinsertquery.prepare("INSERT INTO 'FMT' (idx,typeid,length,name,format,val) values (?,?,?,?,?,?);"))
                        fmtinsertquery.bindValue(":idx",fmtindex++);
                        fmtinsertquery.bindValue(1,0);
                        fmtinsertquery.bindValue(2,0);
                        fmtinsertquery.bindValue(3,name);
                        fmtinsertquery.bindValue(4,typechars);
                        fmtinsertquery.bindValue(5,variablenames);
                        fmtinsertquery.exec();
                        indexinsertquery.bindValue(":idx",fmtindex-1);
                        indexinsertquery.bindValue(1,"FMT");
                        if (!indexinsertquery.exec())
                        {
                            qDebug() << "Error execing index:" << lastLogTime << name << indexinsertquery.lastError().text();
                            emit error("Error execing:" + indexinsertquery.executedQuery() + " error was " + indexinsertquery.lastError().text());
                            return;
                        }

                        //qDebug() << "Create:" << createstring;
                        //qDebug() << "Insert:" << insertstring;

                        /*QSqlQuery *query = new QSqlQuery(*m_db);
                        query->prepare(insertstring.replace("insert or replace","insert"));
                        for (int i=0;i<fieldnames.size();i++)
                        {
                            qDebug() << "Binding:" << QString(":") + fieldnames.at(i);
                            query->bindValue(QString(":") + fieldnames.at(i),0);
                        }
                        m_msgNameToInsertQuery.insert(name,query);*/
                        m_msgNameToInsertQuery.insert(name,insertstring.replace("insert or replace","insert"));
                        QSqlQuery create(*m_db);
                        if (!create.prepare(createstring))
                        {
                            qDebug() << "Unable to create:" << create.lastError().text();
                            return;
                        }
                        if (!create.exec())
                        {
                            qDebug() << "Unable to exec create:" << create.lastError().text();
                        }
                    }
                    //QSqlQuery *query = m_msgNameToInsertQuery.value(name);
                    QSqlQuery query(*m_db);
                    if (!query.prepare(m_msgNameToInsertQuery.value(name)))
                    {
                        qDebug() << "Unable to prepare query:" << query.lastError().text();
                    }
                    //QList<QString> fieldlist = decoder->getFieldList();
                    //qDebug() << "Values:" << query.boundValues().keys().size();
                    quint64 unixtimemsec = (quint64)decoder->getUnixTimeFromMs(message.sysid, lastLogTime);
                    while (lastunixtimemseclist.contains(unixtimemsec))
                    {
                        unixtimemsec++;
                    }
                    lastunixtimemseclist.append(unixtimemsec);
                    query.bindValue(":idx",unixtimemsec);
                    for (int i=0;i<retvals.size();i++)
                    {
                        //if (retvals.at(i).first.contains())
                        query.bindValue(QString(":") + retvals.at(i).first.split(".")[1],retvals.at(i).second.toInt());
                        //qDebug() << "Bound:" << retvals.at(i).first.split(".")[1] << retvals.at(i).second;
                    }
                    if (retvals.size() > 0)
                    {
                        if (!query.exec())
                        {
                            qDebug() << "Error execing insert query:" << query.lastError().text();
                        }
                        else
                        {
                            indexinsertquery.bindValue(":idx",unixtimemsec);
                            indexinsertquery.bindValue(1,name);
                            if (!indexinsertquery.exec())
                            {
                                qDebug() << "Error execing index:" << lastLogTime << name << indexinsertquery.lastError().text();
                                emit error("Error execing:" + indexinsertquery.executedQuery() + " error was " + indexinsertquery.lastError().text());
                                return;
                            }
                        }
                    }

                    //UASInterface* uas = UASManager::instance()->getUASForId(message.sysid);
                    /*if (!uas && message.msgid == MAVLINK_MSG_ID_HEARTBEAT)
                    {
                        mavlink_heartbeat_t heartbeat;
                        // Reset version field to 0
                        heartbeat.mavlink_version = 0;
                        mavlink_msg_heartbeat_decode(&message, &heartbeat);


                        // Create a new UAS object
                        if (heartbeat.autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA)
                        {
                            ArduPilotMegaMAV* mav = new ArduPilotMegaMAV(0, message.sysid);
                            mav->setSystemType((int)heartbeat.type);
                            uas = mav;
                            // Make UAS aware that this link can be used to communicate with the actual robot
                            uas->addLink(this);
                            UASObject *obj = new UASObject();
                            LinkManager::instance()->addSimObject(message.sysid,obj);

                            // Now add UAS to "official" list, which makes the whole application aware of it
                            UASManager::instance()->addUAS(uas);

                        }
                    }
                    else if (uas)
                    {
                        if (delay > 0 && delay < 10000)
                        {
                            //Split the delay into 100msec chunks, to allow for canceling.
                            int realdelay = delay / ((double)privSpeedVar / 100.0);
                            int repeat = realdelay / 100;
                            for (int i=0;i<repeat;i++)
                            {
                                msleep(100);
                                if (!m_threadRun)
                                {
                                    //Break out
                                    MainWindow::instance()->toolBar().overrideDisableConnectWidget(false);
                                    MainWindow::instance()->toolBar().disableConnectWidget(false);
                                    emit disconnected(this);
                                    emit disconnected();
                                    emit connected(false);
                                    UASManager::instance()->removeUAS(UASManager::instance()->getActiveUAS());
                                    return;
                                }
                            }
                            msleep(realdelay - repeat);
                            //msleep(delay / ((double)privSpeedVar / 100.0));
                        }
                        else
                        {
                            msleep(1);
                        }
                        uas->receiveMessage(this,message);
                        m_mavlinkDecoder->receiveMessage(this,message);
                        if (m_mavlinkInspector)
                        {
                            m_mavlinkInspector->receiveMessage(this,message);
                        }
                    }
                    else
                    {
                        //no UAS, and not a heartbeat
                    }*/
                }

            }
        }
    }
    bool stopper = true;
}

void AP2DataPlotThread::run()
{
    emit startLoad();
    int type = -1;
    m_stop = false;
    if (m_fileName.toLower().endsWith(".bin"))
    {
        //It's a binary file
        type = 1;
        loadBinaryLog();
    }
    else if (m_fileName.toLower().endsWith(".log"))
    {
        //It's a ascii log.
        type = 2;
        loadAsciiLog();
    }
    else if (m_fileName.toLower().endsWith(".tlog"))
    {
        //It's a tlog
        type = 3;
        loadTLog();
    }
    else
    {
        emit error("Unable to detect file type from filename. Ensure the file has a .bin or .log extension");
        return;
    }
    int errorcount = 0;


    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }

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
        emit done(errorcount,m_loadedLogType);
    }
}
void AP2DataPlotThread::valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec)
{
    //Called as a direct connection, always in-thread.
    //Throw the data into the database here.
    QString finalname = name;
    if (finalname.contains(":"))
    {
        finalname = finalname.split(":")[1];
    }
    if (!finalname.contains("."))
    {
        return;
    }
    QString msg = finalname.split(".")[0];
    QString field = finalname.split(".")[1];

    if (!m_msgNameToInsertQuery.contains(msg))
    {

        QList<QString> fieldnames = decoder->getFieldList(msg);
        QString variablenames;
        QString typechars;
        for (int i=0;i<fieldnames.size();i++)
        {
            mavlink_field_info_t fieldinfo = decoder->getFieldInfo(msg,fieldnames.at(i));
            variablenames += QString(fieldinfo.name) + ",";
            switch (fieldinfo.type)
            {
                case MAVLINK_TYPE_CHAR:
                {
                    typechars += "b,";
                }
                break;
                case MAVLINK_TYPE_UINT8_T:
                {
                    typechars += "B,";
                }
                break;
                case MAVLINK_TYPE_INT8_T:
                {
                    typechars += "b,";
                }
                break;
                case MAVLINK_TYPE_UINT16_T:
                {
                    typechars += "H,";
                }
                break;
                case MAVLINK_TYPE_INT16_T:
                {
                    typechars += "h,";
                }
                break;
                case MAVLINK_TYPE_UINT32_T:
                {
                    typechars += "I,";
                }
                break;
                case MAVLINK_TYPE_INT32_T:
                {
                    typechars += "i,";
                }
                break;
                case MAVLINK_TYPE_FLOAT:
                {
                    typechars += "f,";
                }
                break;
                default:
                {
                    qDebug() << "Unknown type:" << QString::number(fieldinfo.type);
                }
                break;
            }
        }
        QString createstring = makeCreateTableString(msg,typechars,variablenames);
        QString insertstring = makeInsertTableString(msg,variablenames);
        qDebug() << "Create:" << createstring;
        qDebug() << "Insert:" << insertstring;

        QSqlQuery *query = new QSqlQuery(*m_db);
        query->prepare(insertstring);
        for (int i=0;i<variablenames.size();i++)
        {
            query->bindValue(QString(":") + variablenames.at(i),0);
        }
        //m_msgNameToInsertQuery.insert(msg,query);
        QSqlQuery create(*m_db);
        if (!create.prepare(createstring))
        {
            qDebug() << "Unable to create:" << create.lastError().text();
            return;
        }
        if (!create.exec())
        {
            qDebug() << "Unable to exec create:" << create.lastError().text();
        }
    }

    //Check to see if msg has a table, if not, iterate over and populate a table and FMT entry for it before entering data.
    //this way, we only actually create the tables and fill out data that we actually receive, rather than blindly populating
    //all of them.
    //UPDATE msg SET field="value" WHERE idx="index"
    mavlink_field_info_t fieldinfo = decoder->getFieldInfo(msg,field);
    QSqlQuery selectquery(*m_db);
    if (!selectquery.prepare("SELECT * from " + msg + " WHERE idx == " + QString::number(msec)))
    {
        qDebug() << "Unable to prepare selectquery:" << selectquery.lastError().text();

    }
    if (!selectquery.exec())
    {
        qDebug() << "Unable to exec selectquery:" << selectquery.lastError().text();
    }
    if (!selectquery.next())
    {
        //No row! insert a new one
        //QSqlQuery *query = m_msgNameToInsertQuery.value(msg);
        //query->bindValue(":idx",QString::number(msec));
        //query->bindValue(QString(":") + fieldinfo.name,value.toDouble());
        //if (!query->exec())
        //{
        //    qDebug() << "Unable to exec insert:" << query->lastError().text() << query->lastQuery();
        //}

    }
    else
    {
        //Existing row, modify it
        QSqlQuery editquery(*m_db);
        if (!editquery.prepare("UPDATE '" + msg + "' SET '" + fieldinfo.name + "'= \"" + value.toInt() + "\" WHERE idx = \"" + QString::number(msec) + "\""))
        {
            qDebug() << "Unable to prepare editquery:" << editquery.lastError().text();
        }
        if (!editquery.exec())
        {
            qDebug() << "Unable to exec editquery:" << editquery.lastError().text();
        }
    }
    qDebug() << "Field type:" << fieldinfo.type;
    qDebug() << fieldinfo.print_format;
    m_fieldCount++;
}
