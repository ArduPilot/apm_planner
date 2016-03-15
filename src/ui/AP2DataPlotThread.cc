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
#include <QByteArray>
#include <QDataStream>
#include "MAVLinkDecoder.h"
#include "QsLog.h"
#include "QGC.h"


const QString AP2DataPlotThread::c_timeStampSearchKey("TimeUS");


AP2DataPlotThread::AP2DataPlotThread(AP2DataPlot2DModel *model,QObject *parent) :
    QThread(parent),
    m_stop(false),
    m_dataModel(model)
{
    QLOG_DEBUG() << "Created AP2DataPlotThread:" << this;
    qRegisterMetaType<MAV_TYPE>("MAV_TYPE");
    qRegisterMetaType<AP2DataPlotStatus>("AP2DataPlotStatus");
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

    bool allRowsHaveTime = true;
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
                            //Message is a format type, we don't want to include it
                            continue;
                        }
                        if (format == "" || labels == "")
                        {
                            QLOG_DEBUG() << "AP2DataPlotThread::run(): empty format string or labels string for type" << msg_type << name;
                            m_plotState.corruptFMTRead(index, name + " format data: Corrupt or missing. Message type is:0x" +
                                                       QString::number(msg_type, 16));
                            continue;
                        }
                        if (!tables.contains(name)) {
                            // Check for rows having a timestamp
                            allRowsHaveTime &= labels.contains(c_timeStampSearchKey, Qt::CaseInsensitive);

                            if (!m_dataModel->addType(name,msg_type,msg_length,format, labels.split(",")))
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
                            m_plotState.corruptDataRead(index, "No length information for message type:0x" + QString::number(type, 16));
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
                            index++;
                            QList<QPair<QString,QVariant> > valuepairlist;
                            QString formatstr = typeToFormatMap.value(type);
                            QString labelstr = typeToLabelMap.value(type);
                            QStringList labelstrsplit = labelstr.split(",");
                            bool noCorruptDataFound = true;

                            for (int j=0;j<formatstr.size();j++)
                            {
                                QChar typeCode = formatstr.at(j);
                                if (typeCode == 'b') //int8_t
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'B') //uint8_t
                                {
                                    quint8 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'h') //int16_t
                                {
                                    qint16 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'H') //uint16_t
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'i') //int32_t
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'I') //uint32_t
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'f') //float
                                {
                                    float f;
                                    packetstream >> f;
                                    if (f != f) // This tests for not a number
                                    {
                                        QLOG_WARN() << "Corrupted log data found - Graphing may not work as expected for data of type" << name;
                                        noCorruptDataFound = false;
                                        m_plotState.corruptDataRead(index, "Corrupt data element found when decoding " + name + " data.");
                                    }
                                    else
                                    {
                                        valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),f));
                                    }
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
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'c') //int16_t * 100
                                {
                                    qint16 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'C') //uint16_t * 100
                                {
                                    quint16 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'e') //int32_t * 100
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'E') //uint32_t * 100
                                {
                                    quint32 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 100.0));
                                }
                                else if (typeCode == 'L') //uint32_t GPS Lon/Lat * 10000000
                                {
                                    qint32 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val / 10000000.0));
                                }
                                else if (typeCode == 'M')
                                {
                                    qint8 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'q')
                                {
                                    qint64 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else if (typeCode == 'Q')
                                {
                                    quint64 val;
                                    packetstream >> val;
                                    valuepairlist.append(QPair<QString,QVariant>(labelstrsplit.at(j),val));
                                }
                                else
                                {
                                    //Unknown!
                                    QLOG_DEBUG() << "AP2DataPlotThread::run(): ERROR UNKNOWN DATA TYPE" << typeCode;
                                    m_plotState.corruptDataRead(index, "Unknown data type: " + QString(typeCode) + " when decoding " + name);
                                }
                            }
                            if (noCorruptDataFound && (valuepairlist.size() >= 1))
                            {
                                if (!m_dataModel->addRow(name, valuepairlist, index, c_timeStampSearchKey))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
                                }
                                m_plotState.validDataRead();
                            }

                            if (type == paramtype && m_loadedLogType == MAV_TYPE_GENERIC)
                            {
                                // Name field is not always on same Index. So first search for the right position...
                                int nameIndex = 0;
                                for (int i = 0; i < valuepairlist.size(); ++i)
                                {
                                    if (valuepairlist[i].first == "Name")
                                    {
                                        nameIndex = i;
                                        break;
                                    }
                                }
                                //...and then use it to check the values.
                                if (valuepairlist[nameIndex].second == "RATE_RLL_P" || valuepairlist[nameIndex].second == "H_SWASH_PLATE")
                                {
                                    m_loadedLogType = MAV_TYPE_QUADROTOR;
                                }
                                else if (valuepairlist[nameIndex].second == "PTCH2SRV_P")
                                {
                                    m_loadedLogType = MAV_TYPE_FIXED_WING;
                                }
                                else if (valuepairlist[nameIndex].second == "SKID_STEER_OUT")
                                {
                                    m_loadedLogType = MAV_TYPE_GROUND_ROVER;
                                }
                            }
                        }
                        else
                        {
                            QLOG_DEBUG() << "AP2DataPlotThread::run(): No query available for param category" << name;
                            m_plotState.corruptDataRead(index, "No format information available for message type:0x" + QString::number(type, 16));
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
            m_plotState.corruptDataRead(index, "Non packet bytes found in log file " + QString::number(nonpacketcounter) + " bytes filtered out");
        }
    }
    if (!m_dataModel->endTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }
    m_dataModel->setAllRowsHaveTime(allRowsHaveTime, c_timeStampSearchKey);
}

void AP2DataPlotThread::loadAsciiLog(QFile &logfile)
{
    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 500;
    QMap<QString,QString> nameToTypeString;
    QMap<QString,QStringList> nameToValueList;
    bool allRowsHaveTime = true;

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
            if ((line.contains(APM_COPTER_REXP) || (line.contains("PARM") && (line.contains("RATE_RLL_P") || line.contains("H_SWASH_PLATE")))))
            {
                m_loadedLogType = MAV_TYPE_QUADROTOR;
            }
            if (line.contains(APM_PLANE_REXP) || (line.contains("PARM") && line.contains("PTCH2SRV_P")))
            {
                m_loadedLogType = MAV_TYPE_FIXED_WING;
            }
            if (line.contains(APM_ROVER_REXP) || (line.contains("PARM") && line.contains("SKID_STEER_OUT")))
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

                        // Check for rows having a timestamp
                        allRowsHaveTime &= valuestr.contains(c_timeStampSearchKey, Qt::CaseInsensitive);
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
                    m_plotState.corruptFMTRead(index, "Too short FMT line in log file: " + line);
                }
            }
            else
            {
                if (linesplit.size() > 1)
                {
                    QString name = linesplit[0].trimmed();
                    if (nameToTypeString.contains(name))
                    {
                        /* from https://github.com/diydrones/ardupilot/blob/master/libraries/DataFlash/DataFlash.h#L737
                        Format characters in the format string for binary log messages
                          b   : int8_t
                          B   : uint8_t
                          h   : int16_t
                          H   : uint16_t
                          i   : int32_t
                          I   : uint32_t
                          f   : float
                          n   : char[4]
                          N   : char[16]
                          Z   : char[64]
                          c   : int16_t * 100
                          C   : uint16_t * 100
                          e   : int32_t * 100
                          E   : uint32_t * 100
                          L   : int32_t latitude/longitude
                          M   : uint8_t flight mode
                          q   : int64_t
                          Q   : uint64_t
                        */
                        QString typestr = nameToTypeString[name];
                        static QString intdef("bBhHiI"); // 32 bit max types.
                        static QString floatdef("cCeEfL");
                        static QString chardef("nNZM");
                        if (typestr.size() != linesplit.size() - 1)
                        {
                            QLOG_DEBUG() << "Error in line:" << index << "param" << name << "parameter mismatch";
                            m_plotState.corruptDataRead(index, "Error in line:" + QString::number(index) +  " parameter mismatch for " + name + " data.");
                        }
                        else
                        {
                            QList<QPair<QString,QVariant> > valuepairlist;
                            QStringList valuestrlist = nameToValueList.value(name);

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
                                        m_plotState.corruptDataRead(index, name + " data: Failed to convert " + valStr + " to an integer number.");
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
                                        m_plotState.corruptDataRead(index, name + " data: Failed to convert " + valStr + " to a floating point number.");
                                        foundError = true;
                                    }
                                }
                                else if (QString('q').contains(typeCode) )
                                {
                                    quint64 val = valStr.toLongLong(&ok);
                                    if (ok)
                                    {
                                        valuepairlist.append(QPair<QString,QVariant>(subname,val));
                                    }
                                    else
                                    {
                                        QLOG_DEBUG() << "Failed to convert " << valStr << " to an qint64 number.";
                                        m_plotState.corruptDataRead(index, name + " data: Failed to convert " + valStr + " to an qint64 number.");
                                        foundError = true;
                                    }
                                }
                                else if (QString('Q').contains(typeCode) )
                                {
                                    quint64 val = valStr.toULongLong(&ok);
                                    if (ok)
                                    {
                                        valuepairlist.append(QPair<QString,QVariant>(subname,val));
                                    }
                                    else
                                    {
                                        QLOG_DEBUG() << "Failed to convert " << valStr << " to an quint64 number.";
                                        m_plotState.corruptDataRead(index, name + " data: Failed to convert " + valStr + " to an quint64 number.");
                                        foundError = true;
                                    }
                                }
                                else
                                {
                                    QLOG_DEBUG() << "AP2DataPlotThread::run(): Unknown data value found" << typeCode;
                                    m_plotState.corruptDataRead(index, name + " data: Unknown data value found: %1" + QString(typeCode));
                                    return;
                                }
                            }
                            if (!foundError)
                            {
                                if (valuepairlist.size() >= 1)
                                {
                                    if (!m_dataModel->addRow(name,valuepairlist,index++,c_timeStampSearchKey))
                                    {
                                        QString actualerror = m_dataModel->getError();
                                        m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                        emit error(actualerror);
                                        return;
                                    }
                                    m_plotState.validDataRead();
                                }
                            }
                        }
                    }
                    else
                    {
                        QLOG_DEBUG() << "Found line " << index << " with unknown command " << name << ", skipping...";
                        m_plotState.corruptDataRead(index, name + " data: Found line with unknown command");
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
    m_dataModel->setAllRowsHaveTime(allRowsHaveTime, c_timeStampSearchKey);
}

void AP2DataPlotThread::loadTLog(QFile &logfile)
{
    m_loadedLogType = MAV_TYPE_GENERIC;
    int nrOfEmptyMsg = 0;
    int bytesize = 0;
    int index = 100;
    quint8 lastModeVal = 255;
    bool allRowsHaveTime = true;
    mavlink_message_t message;
    mavlink_status_t status;
    m_decoder = QSharedPointer<MAVLinkDecoder>(new MAVLinkDecoder());

    if (!m_dataModel->startTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }

    // Tlog does not contain MODE messages the mode information ins transmitted in
    // a heartbeat message. So we create the datatype for MODE here
    QStringList modeVarNames;
    modeVarNames.push_back(QString("Mode"));
    modeVarNames.push_back(QString("ModeNum"));
    modeVarNames.push_back(QString("Info"));
    if (!m_dataModel->addType(ModeMessage::TypeName,0,0,"MBZ",modeVarNames))
    {
        QString actualerror = m_dataModel->getError();
        m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
        emit error(actualerror);
        return;
    }

    while (!logfile.atEnd() && !m_stop)
    {
        emit loadProgress(logfile.pos(),logfile.size());
        QByteArray bytes = logfile.read(8192);
        bytesize += bytes.size();

        for (int i=0;i<bytes.size();i++)
        {
            unsigned int decodeState = mavlink_parse_char(14, static_cast<uint8_t>(bytes[i]), &message, &status);
            if (decodeState == MAVLINK_FRAMING_OK)
            {
                // Good decode. Now check message name. If its "EMPTY" we cannot insert it into datamodel
                // We will count those messages and inform the user.
                QString name = m_decoder->getMessageName(message.msgid);
                if (name != "EMPTY")
                {
                    if (message.sysid != 255) // [TODO] GCS packet is not always 255 sysid.
                    {
                        QList<QPair<QString,QVariant> > retvals = m_decoder->receiveMessage(0,message);
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
                                        typechars += "Q";
                                    }
                                    break;
                                    case MAVLINK_TYPE_INT64_T:
                                    {
                                        typechars += "q";
                                    }
                                    break;
                                    default:
                                    {
                                        QLOG_ERROR() << "Unknown type:" << QString::number(fieldinfo.type);
                                        m_plotState.corruptDataRead(i, name + " data: Unknown data type:" + QString::number(fieldinfo.type));
                                    }
                                    break;
                                }
                            }
                            // Check for rows having a timestamp
                            allRowsHaveTime &= variablenames.contains(c_timeStampSearchKey, Qt::CaseInsensitive);
                            if (!m_dataModel->addType(name,0,0,typechars,variablenames))
                            {
                                QString actualerror = m_dataModel->getError();
                                m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                emit error(actualerror);
                                return;
                            }
                        }

                        QList<QPair<QString,QVariant> > valuepairlist;
                        for (int i=0;i<retvals.size();i++)
                        {
                            QStringList list = retvals.at(i).first.split(".");
                            if (list.size() >= 2)
                            {
                                valuepairlist.append(QPair<QString,QVariant>(list[1],retvals.at(i).second));
                            }
                            else
                            {
                                m_plotState.corruptDataRead(index, "Missing type information. Message:" + retvals.at(i).first + ":" + retvals.at(i).second.toString());
                            }
                        }
                        if (valuepairlist.size() >= 1)
                        {
                            if (!m_dataModel->addRow(name,valuepairlist, index++, c_timeStampSearchKey))
                            {
                                QString actualerror = m_dataModel->getError();
                                m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                emit error(actualerror);
                                return;
                            }
                            // Tlog does not contain MODE messages the mode information ins transmitted in
                            // a heartbeat message. So here we extract MODE data from hertbeat
                            if ((name == "HEARTBEAT") && (lastModeVal != static_cast<quint8>(valuepairlist[0].second.toInt())))
                            {
                                QList<QPair<QString,QVariant> > specialValuepairlist;
                                // Extract MODE messages from heratbeat messages
                                lastModeVal = static_cast<quint8>(valuepairlist[0].second.toInt());

                                specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[0],lastModeVal));
                                specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[1],lastModeVal));
                                specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[2],"Generated Value"));
                                if (!m_dataModel->addRow(ModeMessage::TypeName, specialValuepairlist, index++, c_timeStampSearchKey))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
                                }

                                // Mav type can be extracted from heartbeat too. So lets set the Mav type
                                // if not already set
                                if (m_loadedLogType == MAV_TYPE_GENERIC)
                                {
                                    // Name field is not always on same Index. So first search for the right position...
                                    int typeIndex = 0;
                                    for (int i = 0; i < valuepairlist.size(); ++i)
                                    {
                                        if (valuepairlist[i].first == "type")   // "type" field holds MAV_TYPE
                                        {
                                            typeIndex = i;
                                            break;
                                        }
                                    }
                                    m_loadedLogType = static_cast<MAV_TYPE>(valuepairlist[typeIndex].second.toInt());
                                }
                            }
                            m_plotState.validDataRead();    // tell plot state that we have a valid message
                        }
                    }
                }
                else
                {
                    nrOfEmptyMsg++;
                }
            }
            else if (decodeState == MAVLINK_FRAMING_BAD_CRC)
            {
                m_plotState.corruptDataRead(index, "Bad CRC");
            }

        }
    }
    if (nrOfEmptyMsg != 0) // Did we have messages named "EMPTY" ?
    {
        m_plotState.corruptDataRead(0, "Found " + QString::number(nrOfEmptyMsg) +" 'EMPTY' messages wich could not be processed");
    }
    if (!m_dataModel->endTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }
    m_dataModel->setAllRowsHaveTime(allRowsHaveTime, c_timeStampSearchKey);
}

void AP2DataPlotThread::run()
{
    Q_ASSERT(!isMainThread());
    emit startLoad();
    m_stop = false;
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
        emit done(m_plotState, m_loadedLogType);
    }
}


AP2DataPlotStatus::AP2DataPlotStatus() : m_parsingState(OK)
{
}

void AP2DataPlotStatus::corruptDataRead(const int index, const QString &errorMessage)
{
    m_parsingState = TruncationError;
    m_errors.push_back(errorEntry(index, errorMessage));
}

void AP2DataPlotStatus::corruptFMTRead(const int index, const QString &errorMessage)
{
    m_parsingState = FmtError;
    m_errors.push_back(errorEntry(index, errorMessage));
}

AP2DataPlotStatus::parsingState AP2DataPlotStatus::getParsingState()
{
    return m_parsingState;
}

QString AP2DataPlotStatus::getErrorText()
{
    QString out;
    QTextStream outStream(&out);

    foreach (const errorEntry &entry, m_errors)
    {
        outStream << "Logline " << entry.first << ": " << entry.second << endl;
    }
    outStream << endl << " There were " << m_errors.size() << " errors during log parsing." << endl;
    return out;
}
