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


AP2DataPlotThread::AP2DataPlotThread(AP2DataPlot2DModel *model,QObject *parent) :
    QThread(parent)
{
    m_dataModel = model;
    qRegisterMetaType<MAV_TYPE>("MAV_TYPE");
}
void AP2DataPlotThread::loadFile(QString file)
{
    m_fileName = file;
    start();
}

void AP2DataPlotThread::loadBinaryLog()
{
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }

    QByteArray block;
    int paramtype = -1;
    QMap<unsigned char,unsigned char> typeToLengthMap;
    QMap<unsigned char,QString > typeToNameMap;
    QMap<unsigned char,QString > typeToFormatMap;
    QMap<unsigned char,QString > typeToLabelMap;
    QStringList tables;

    int index = 0;
    m_loadedLogType = MAV_TYPE_GENERIC;

    if (!m_dataModel->startTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }
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
                            if (!m_dataModel->addType(name,msg_type,msg_length,format,labels.split(",")))
                            {
                                QString actualerror = m_dataModel->getError();
                                m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                emit error(actualerror);
                                return;
                            }
                            tables.append(name);
                        }
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
                            //Not enough data yet, read more from the file
                            break;
                        }
                        QByteArray packet = block.mid(i+3,typeToLengthMap.value(type)-3);
                        block.remove(i,packet.size()+3); //Remove both the 3 byte header and the packet
                        i--;
                        QDataStream packetstream(packet);
                        packetstream.setByteOrder(QDataStream::LittleEndian);
                        packetstream.setFloatingPointPrecision(QDataStream::SinglePrecision);

                        QString name = typeToNameMap.value(type);
                        if (tables.contains(name))
                        {
                            QString linetoemit = typeToNameMap.value(type);
                            index++;
                            QList<QPair<QString,QVariant> > valuepairlist;
                            QString formatstr = typeToFormatMap.value(type);
                            QString labelstr = typeToLabelMap.value(type);
                            QStringList labelstrsplit = labelstr.split(",");

                            for (int j=0;j<formatstr.size();j++)
                            {
                                QChar typeCode = formatstr.at(j);
                                if (typeCode == 'b') //int8_t
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'B') //uint8_t
                                {
                                    quint8 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'h') //int16_t
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'H') //uint16_t
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'i') //int32_t
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'I') //uint32_t
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'f') //float
                                {
                                    float f;
                                    packetstream >> f;
                                    linetoemit += "," + QString::number(f,'f',4);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),f));
                                }
                                else if (typeCode == 'n') //char(4)
                                {

                                    QString val = "";
                                    for (int i=0;i<4;i++)
                                    {
                                        quint8 ch;
                                        packetstream >> ch;
                                        if (ch)
                                        {
                                            val += static_cast<char>(ch);
                                        }
                                    }
                                    linetoemit += "," + val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'N') //char(16)
                                {
                                    QString val = "";
                                    for (int i=0;i<16;i++)
                                    {
                                        quint8 ch;
                                        packetstream >> ch;
                                        if (ch)
                                        {
                                            val += static_cast<char>(ch);
                                        }
                                    }
                                    linetoemit += "," + val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'Z') //char(64)
                                {
                                    QString val = "";
                                    for (int i=0;i<64;i++)
                                    {
                                        quint8 ch;
                                        packetstream >> ch;
                                        if (ch)
                                        {
                                            val += static_cast<char>(ch);
                                        }
                                    }
                                    linetoemit += "," + val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'c') //int16_t * 100
                                {
                                    qint16 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'C') //uint16_t * 100
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'e') //int32_t * 100
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'E') //uint32_t * 100
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val / 100.0,'f',4);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'L') //uint32_t GPS Lon/Lat * 10000000
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val / 10000000.0,'f',6);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 10000000.0));
                                }
                                else if (typeCode == 'M')
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(val,'f',0);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else
                                {
                                    //Unknown!
                                    QLOG_DEBUG() << "AP2DataPlotThread::run(): ERROR UNKNOWN DATA TYPE" << typeCode;
                                }
                            }
                            if (valuepairlist.size() > 1)
                            {
                                if (!m_dataModel->addRow(name,valuepairlist,index))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
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
    if (!m_dataModel->endTransaction())
    {
        emit error(m_dataModel->getError());
        return;
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

    bool firstactual = true;
    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 0;
    QMap<QString,QString> nameToTypeString;
    QMap<QString,QStringList> nameToValueList;

    if (!m_dataModel->startTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }
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
                        QStringList valuestr;
                        for (int i=5;i<linesplit.size();i++)
                        {
                            QString name = linesplit[i].trimmed();
                            valuestr += name;
                        }
                        nameToValueList[type] = valuestr;
                        int type_id = linesplit[1].trimmed().toInt();
                        int length = linesplit[2].trimmed().toInt();
                        if (!m_dataModel->addType(type,type_id,length,descstr,valuestr))
                        {
                            QString actualerror = m_dataModel->getError();
                            m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                            emit error(actualerror);
                            return;
                        }
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
                    if (nameToTypeString.contains(name))
                    {
                        QString typestr = nameToTypeString[name];
                        static QString intdef("bBhHiI");
                        static QString floatdef("cCeEfL");
                        static QString chardef("nNZM");
                        if (typestr.size() != linesplit.size() - 1)
                        {
                            //QLOG_DEBUG() << "Bound values for" << name << "count:" << nameToInsertQuery[name]->boundValues().values().size() << "actual" << linesplit.size() << typestr.size();
                            //QLOG_DEBUG() << "Error in line:" << index << "param" << name << "parameter mismatch";
                            m_errorCount++;
                        }
                        else
                        {
                            QList<QPair<QString,QVariant> > valuepairlist;
                            QStringList valuestrlist = nameToValueList.value(name);
                            //valuelist.append(QPair<QString,QVariant>("idx",unixtimemsec));

                            bool foundError = false;
                            for (int i = 1; i < linesplit.size(); i++)
                            {
                                QString subname = "";
                                if (valuestrlist.size() > i-1)
                                {
                                    subname = valuestrlist.at(i-1);
                                }
                                else
                                {
                                    continue;
                                }
                                bool ok;
                                QChar typeCode = typestr.at(i - 1);
                                QString valStr = linesplit[i].trimmed();
                                if (intdef.contains(typeCode))
                                {
                                    int val = valStr.toInt(&ok);
                                    if (ok)
                                    {
                                        valuepairlist.append(QPair<QString,QVariant>(subname,val));
                                    }
                                    else
                                    {
                                        QLOG_DEBUG() << "Failed to convert " << valStr << " to an integer number.";
                                        foundError = true;
                                    }
                                }
                                else if (chardef.contains(typeCode))
                                {
                                    valuepairlist.append(QPair<QString,QVariant>(subname,valStr));
                                }
                                else if (floatdef.contains(typeCode))
                                {
                                    double val = valStr.toDouble(&ok);
                                    if (ok && !isinf(val) && !isnan(val))
                                    {
                                        valuepairlist.append(QPair<QString,QVariant>(subname,val));
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
                                ++m_errorCount;
                            }
                            else
                            {
                                if (valuepairlist.size() > 0)
                                {
                                    if (!m_dataModel->addRow(name,valuepairlist,index))
                                    {
                                        QString actualerror = m_dataModel->getError();
                                        m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                        emit error(actualerror);
                                        return;
                                    }
                                }
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
    if (!m_dataModel->endTransaction())
    {
        emit error(m_dataModel->getError());
        return;
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


    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 0;

    int bytesize = 0;
    QByteArray timebuf;
    bool firsttime = true;
    qint64 lastLogTime = 0;
    mavlink_message_t message;
    mavlink_status_t status;
    bool nexttime = false;
    int delay;

    index = 500;
    QList<uint64_t*> mavlinkList;
    m_fieldCount=0;
    qint64 lastPcTime = QDateTime::currentMSecsSinceEpoch();
    decoder = new MAVLinkDecoder();
    QList<quint64> lastunixtimemseclist;

    if (!m_dataModel->startTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }
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

                    if (!m_dataModel->hasType(name))
                    {

                        QList<QString> fieldnames = decoder->getFieldList(name);
                        QStringList variablenames;
                        QString typechars;
                        for (int i=0;i<fieldnames.size();i++)
                        {
                            mavlink_field_info_t fieldinfo = decoder->getFieldInfo(name,fieldnames.at(i));
                            variablenames <<  QString(fieldinfo.name);
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
                                    QLOG_ERROR() << "Unknown type:" << QString::number(fieldinfo.type);
                                }
                                break;
                            }
                        }

                        if (!m_dataModel->addType(name,0,0,typechars,variablenames))
                        {
                            QString actualerror = m_dataModel->getError();
                            m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                            emit error(actualerror);
                            return;
                        }
                    }

                    quint64 unixtimemsec = (quint64)decoder->getUnixTimeFromMs(message.sysid, lastLogTime);
                    QVariantList valuelist;

                    while (lastunixtimemseclist.contains(unixtimemsec))
                    {
                        unixtimemsec++;
                    }
                    lastunixtimemseclist.append(unixtimemsec);
                    //query.bindValue(":idx",unixtimemsec);
                    QList<QPair<QString,QVariant> > valuepairlist;
                    //valuelist.append(QPair<QString,QVariant>("idx",unixtimemsec));
                    for (int i=0;i<retvals.size();i++)
                    {
                        valuepairlist.append(QPair<QString,QVariant>(retvals.at(i).first.split(".")[1],retvals.at(i).second.toLongLong()));
                        //query.bindValue(QString(":") + retvals.at(i).first.split(".")[1],retvals.at(i).second.toInt());
                    }
                    if (valuepairlist.size() > 1)
                    {
                        if (!m_dataModel->addRow(name,valuepairlist,unixtimemsec))
                        {
                            QString actualerror = m_dataModel->getError();
                            m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                            emit error(actualerror);
                            return;
                        }
                    }
                }
            }
        }
    }
    if (!m_dataModel->endTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }
}

void AP2DataPlotThread::run()
{
    emit startLoad();
    m_stop = false;
    m_errorCount = 0;
    if (m_fileName.toLower().endsWith(".bin"))
    {
        //It's a binary file
        loadBinaryLog();
    }
    else if (m_fileName.toLower().endsWith(".log"))
    {
        //It's a ascii log.
        loadAsciiLog();
    }
    else if (m_fileName.toLower().endsWith(".tlog"))
    {
        //It's a tlog
        loadTLog();
    }
    else
    {
        emit error("Unable to detect file type from filename. Ensure the file has a .bin or .log extension");
        return;
    }


    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
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
        emit done(m_errorCount,m_loadedLogType);
    }
}
