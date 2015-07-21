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
 *   @brief AP2DataPlot log loader thread
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */


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
    QThread(parent),
    m_decoder(NULL),
    m_dataModel(model),
    m_logStartTime(0)
{
    QLOG_DEBUG() << "Created AP2DataPlotThread:" << this;
    qRegisterMetaType<MAV_TYPE>("MAV_TYPE");
}

AP2DataPlotThread::~AP2DataPlotThread()
{
    QLOG_DEBUG() << "Destroyed AP2DataPlotThread:" << this;
}

void AP2DataPlotThread::loadFile(const QString &file)
{
    Q_ASSERT(isMainThread());
    m_fileName = file;
    start();
}

bool AP2DataPlotThread::isMainThread()
{
    return QThread::currentThread() == QCoreApplication::instance()->thread();
}

void AP2DataPlotThread::loadBinaryLog(QFile &logfile)
{
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
    while (!logfile.atEnd() && !m_stop)
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
                                    linetoemit += "," + QString::number(static_cast<qint8>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'B') //uint8_t
                                {
                                    quint8 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<quint8>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'h') //int16_t
                                {
                                    qint16 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<qint16>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'H') //uint16_t
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<quint16>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'i') //int32_t
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<qint32>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'I') //uint32_t
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<quint32>(val));
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
                                    val = val/100;
                                    linetoemit += "," + QString::number(static_cast<qint16>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'C') //uint16_t * 100
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    val = val/100;
                                    linetoemit += "," + QString::number(static_cast<quint16>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'e') //int32_t * 100
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    val = val/100;
                                    linetoemit += "," + QString::number(static_cast<qint32>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'E') //uint32_t * 100
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    val = val/100;
                                    linetoemit += "," + QString::number(static_cast<quint32>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'L') //uint32_t GPS Lon/Lat * 10000000
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    double latLonValue = val/1e7;
                                    linetoemit += "," + QString::number(latLonValue,'f',6);
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 10000000.0));
                                }
                                else if (typeCode == 'M')
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<qint8>(val));
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'q')
                                {
                                    qint64 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<qint64>(val));;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'Q')
                                {
                                    quint64 val;
                                    packetstream >> val;
                                    linetoemit += "," + QString::number(static_cast<quint64>(val));;
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
void AP2DataPlotThread::loadAsciiLog(QFile &logfile)
{
    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 500;
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
                                    if (!m_dataModel->addRow(name,valuepairlist,index++))
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
void AP2DataPlotThread::loadTLog(QFile &logfile)
{
    m_loadedLogType = MAV_TYPE_GENERIC;

    int bytesize = 0;
    QByteArray timebuf;
    qint64 lastLogTime = 0;
    mavlink_message_t message;
    mavlink_status_t status;

    QList<uint64_t*> mavlinkList;
    m_fieldCount=0;
    m_decoder = new MAVLinkDecoder();
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
            mavlink_status_t* status = mavlink_get_channel_status(14);
            if (status->parse_state == MAVLINK_PARSE_STATE_GOT_STX)
            {
                //last 8 bytes are our timestamp.
                if (timebuf.length() >= 8)
                {
                    quint64 logmsecs = quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-8))) << 56;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-7))) << 48;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-6))) << 40;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-5))) << 32;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-4))) << 24;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-3))) << 16;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-2))) << 8;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(timebuf.length()-1))) << 0;
                    lastLogTime = logmsecs;
                    //QLOG_DEBUG() << "Attempt timestamp:" << lastLogTime;
                }
            }
            if (decodeState != 1)
            {
                timebuf.append(bytes[i]);
            }
            else if (decodeState == 1) // This 'else' works as you need one more byte that the timestamp to satisfy. as would just with else removed
            {
                lastLogTime = lastLogTime / 100;
                if (m_logStartTime == 0)
                {
                    m_logStartTime = lastLogTime;
                }
                //Valid message, clear the time buffer for next time
                timebuf.clear();

                //Good decode
                if (message.sysid != 255) // [TODO] GCS packet is not always 255 sysid.
                {
                    uint64_t *target = (uint64_t*)malloc(message.len * 4);
                    memcpy(target,message.payload64,message.len * 4);
                    mavlinkList.append(target);
                    QList<QPair<QString,QVariant> > retvals = m_decoder->receiveMessage(0,message);
                    QString name = m_decoder->getMessageName(message.msgid);

                    if (!m_dataModel->hasType(name))
                    {

                        QList<QString> fieldnames = m_decoder->getFieldList(name);
                        QStringList variablenames;
                        QString typechars;
                        for (int i=0;i<fieldnames.size();i++)
                        {
                            mavlink_field_info_t fieldinfo = m_decoder->getFieldInfo(name,fieldnames.at(i));
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

                    quint64 unixtimemsec = (lastLogTime - m_logStartTime);

                    while (lastunixtimemseclist.contains(unixtimemsec))
                    {
                        unixtimemsec += 1;
                    }

                    lastunixtimemseclist.append(unixtimemsec);
                    QList<QPair<QString,QVariant> > valuepairlist;
                    for (int i=0;i<retvals.size();i++)
                    {
                        valuepairlist.append(QPair<QString,QVariant>(retvals.at(i).first.split(".")[1],retvals.at(i).second.toLongLong()));
                    }
                    if (valuepairlist.size() > 1)
                    {
                        if (!m_dataModel->addRow(name,valuepairlist,unixtimemsec + 500)) // [TODO] offset index
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
    Q_ASSERT(!isMainThread());
    emit startLoad();
    m_stop = false;
    m_errorCount = 0;
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();

    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file ("  +  m_fileName + ")");
        return;
    }

    QLOG_DEBUG() << "AP2DataPlotThread::run(): Log loading start -" << logfile.size() << "bytes";

    if (m_fileName.toLower().endsWith(".bin"))
    {
        //It's a binary file
        loadBinaryLog(logfile);
    }
    else if (m_fileName.toLower().endsWith(".log"))
    {
        //It's a ascii log.
        loadAsciiLog(logfile);
    }
    else if (m_fileName.toLower().endsWith(".tlog"))
    {
        //It's a tlog
        loadTLog(logfile);
    }
    else
    {
        emit error("Unable to detect file type from filename. Ensure the file has a .bin or .log extension");
        return;
    }


    if (m_stop)
    {
        QLOG_ERROR() << "Plot Log loading was canceled after" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds -" << logfile.pos() << "of" << logfile.size() << "bytes";
        emit error("Log loading Canceled");
    }
    else
    {
        QLOG_INFO() << "Plot Log loading took" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds -" << logfile.pos() << "of" << logfile.size() << "bytes used";
        emit done(m_errorCount,m_loadedLogType);
    }
}
