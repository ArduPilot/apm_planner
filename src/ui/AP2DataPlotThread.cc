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

#include <QTextBlock>


AP2DataPlotThread::AP2DataPlotThread(AP2DataPlot2DModel *model,QObject *parent) :
    QThread(parent),
    m_stop(false),
    m_dataModel(model)
{
    QLOG_DEBUG() << "Created AP2DataPlotThread:" << this;
    qRegisterMetaType<MAV_TYPE>("MAV_TYPE");
    qRegisterMetaType<AP2DataPlotStatus>("AP2DataPlotStatus");
    qRegisterMetaType<QTextBlock>("QTextBlock");
    qRegisterMetaType<QTextCursor>("QTextCursor");


    // flash logs and exported logs can have different timestamps
    m_possibleTimestamps.push_back(timeStampType("TimeUS", 1000000.0));
    m_possibleTimestamps.push_back(timeStampType("TimeMS", 1000.0));
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
    typedef QPair<unsigned int, typeDescriptor> typeToDescPair; // Pair holding message type and format descriptor
    QMap<unsigned int, typeDescriptor> typeToDescriptorMap;     // Map to get a format descriptor for every message type
    QList<typeToDescPair> typesWithoutTimeStamp;        // list storing all descriptors without a timestamp field
    QList<unsigned int> timeStampHasToBeAdded;          // list holding all message types without a timestamp
    QStringList tables;

    QByteArray block;
    int paramtype = -1;
    int index = 0;
    quint64 lastValidTS = 0;
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

                        unsigned char msg_type = static_cast<unsigned char>(packet.at(0)); //Message type defined in the format struct

                        typeDescriptor desc;
                        desc.m_length = static_cast<int>(static_cast<unsigned>(packet.at(1)));    //Message length
                        desc.m_name   = packet.mid(2,4);    //Name of the message
                        desc.m_format = packet.mid(6,16);   //Format of the variables
                        desc.m_labels = packet.mid(22,64);  //comma delimited list of variable names.
                        typeToDescriptorMap.insert(msg_type, desc); // store descriptor for parsing

                        if (desc.m_name == "PARM")
                        {
                            paramtype = msg_type;
                        }
                        if (msg_type == 0x80)
                        {
                            //Message is a format type, we don't want to include it
                            continue;
                        }
                        if (desc.m_format == "" || desc.m_labels == "")
                        {
                            // "STRT" message is a special case in older logs - ignore
                            if (desc.m_name != "STRT")
                            {
                                QLOG_DEBUG() << "AP2DataPlotThread::run(): empty format string or labels string for type" << msg_type << desc.m_name;
                                m_plotState.corruptFMTRead(index, desc.m_name + " format data: Corrupt or missing. Message type is:0x" +
                                                           QString::number(msg_type, 16));
                                continue;
                            }
                        }

                        // All parsing is done now. following code shall detect the name of the timestamp field
                        // and add such a field to all lines that do not have one.
                        if (!tables.contains(desc.m_name))
                        {
                            // try to find the name of timestamp column
                            if (m_timeStamp.m_name.size() == 0)
                            {
                                foreach(const timeStampType &ts, m_possibleTimestamps)
                                {
                                    if (desc.m_labels.contains(ts.m_name))
                                    {
                                        // found
                                        m_timeStamp = ts;
                                        break;
                                    }
                                }
                                // check again and if not found store descriptor for delayed transfer to data model
                                // as we haven't detected the right name
                                if(m_timeStamp.m_name.size() == 0)
                                {
                                    typesWithoutTimeStamp.push_back(typeToDescPair(msg_type, desc));
                                }
                            }
                            if (m_timeStamp.m_name.size() != 0)
                            {   // we have a valid timestamp column name
                                // first check if we have to process delayed messages without a timestamp
                                foreach (typeToDescPair typeDescPair, typesWithoutTimeStamp)
                                {
                                    addTimeToDescriptor(typeDescPair.second);
                                    // store message type for later processing
                                    timeStampHasToBeAdded.push_back(typeDescPair.first);
                                    // store adapted message
                                    if (!m_dataModel->addType(typeDescPair.second.m_name, typeDescPair.first,
                                                              typeDescPair.second.m_length, typeDescPair.second.m_format,
                                                              typeDescPair.second.m_labels.split(",")))
                                    {
                                        QString actualerror = m_dataModel->getError();
                                        m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                        emit error(actualerror);
                                        return;
                                    }
                                    tables.append(typeDescPair.second.m_name);
                                    index++;
                                }
                                typesWithoutTimeStamp.clear();

                                // Now check if the actual message conains a timestamp if not add it
                                if (!desc.m_labels.contains(m_timeStamp.m_name))
                                {
                                    // Add timestamp to descriptor
                                    addTimeToDescriptor(desc);
                                    timeStampHasToBeAdded.push_back(msg_type);// store message type for later processing
                                }
                                // Special handling for "GPS" messages that have a "TimeMS" timestamp but scaling
                                // and value does not mach all other time stamps
                                if ((desc.m_name == "GPS") && desc.m_labels.contains("TimeMS"))
                                {
                                    // replace the original timestamp name
                                    adaptGPSDescriptor(typeToDescriptorMap, desc, msg_type);
                                    timeStampHasToBeAdded.push_back(msg_type);// store message type for later processing
                                }

                                // store in datamodel
                                if (!m_dataModel->addType(desc.m_name, msg_type, desc.m_length, desc.m_format, desc.m_labels.split(",")))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
                                }
                                tables.append(desc.m_name);
                                index++;
                            }
                        }
                    }
                    else
                    {
                        //Data packet
                        if (!typeToDescriptorMap.contains(type))
                        {
                            QLOG_DEBUG() << "AP2DataPlotThread::run(): No entry in typeToDescriptorMap for type:" << type;
                            m_plotState.corruptDataRead(index, "No length information for message type:0x" + QString::number(type, 16));
                            break;
                        }
                        if (i+3+typeToDescriptorMap.value(type).m_length >= block.size())
                        {
                            //Not enough data yet, read more from the file
                            break;
                        }
                        QByteArray packet = block.mid(i + 3, typeToDescriptorMap.value(type).m_length - 3);
                        block.remove(i,packet.size()+3); //Remove both the 3 byte header and the packet
                        i--;
                        QDataStream packetstream(packet);
                        packetstream.setByteOrder(QDataStream::LittleEndian);
                        packetstream.setFloatingPointPrecision(QDataStream::SinglePrecision);

                        QString name = typeToDescriptorMap.value(type).m_name;
                        if (tables.contains(name))
                        {
                            index++;
                            QList<QPair<QString,QVariant> > valuepairlist;
                            QString formatstr = typeToDescriptorMap.value(type).m_format;
                            QString labelstr = typeToDescriptorMap.value(type).m_labels;
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
                            // check if a synthetic timestamp has to added
                            handleMissingTimeStamps(timeStampHasToBeAdded, type, valuepairlist, lastValidTS, index);

                            if (noCorruptDataFound && (valuepairlist.size() >= 1))
                            {
                                if (!m_dataModel->addRow(name, valuepairlist, index, m_timeStamp.m_name))
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
    m_dataModel->setAllRowsHaveTime(true, m_timeStamp.m_name, m_timeStamp.m_divisor);
}

void AP2DataPlotThread::loadAsciiLog(QFile &logfile)
{
    m_loadedLogType = MAV_TYPE_GENERIC;
    int index = 500;

    typedef QPair<unsigned int, typeDescriptor> typeToDescPair; // Pair holding message type and format descriptor
    QMap<QString, typeDescriptor> nameToDescriptorMap;     // Map to get a format descriptor for every message type
    QList<typeToDescPair> typesWithoutTimeStamp;        // list storing all descriptors without a timestamp field
    QStringList timeStampHasToBeAdded;          // list holding all message types without a timestamp
    quint64 lastValidTS = 0;

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
                    typeDescriptor desc;
                    desc.m_name = linesplit[3].trimmed();
                    if (desc.m_name != "FMT")
                    {
                        desc.m_format = linesplit[4].trimmed();
                        nameToDescriptorMap.insert(desc.m_name, desc);
                        if (desc.m_format == "")
                        {
                            continue;
                        }
                        QStringList valuestr;
                        for (int i=5;i<linesplit.size();i++)
                        {
                            QString name = linesplit[i].trimmed();
                            valuestr += name;
                        }

                        desc.m_labels = valuestr.join(",");
                        unsigned int type_id = linesplit[1].trimmed().toUInt();
                        desc.m_length = linesplit[2].trimmed().toInt();
                        nameToDescriptorMap.insert(desc.m_name, desc);  // Update descriptor

                        // Check for rows having a timestamp
                        if (m_timeStamp.m_name.size() == 0)
                        {
                            foreach(const timeStampType &ts, m_possibleTimestamps)
                            {
                                if (desc.m_labels.contains(ts.m_name))
                                {
                                    // found
                                    m_timeStamp = ts;
                                    break;
                                }
                            }
                            // check again and if not found store descriptor for delayed transfer to data model
                            // as we haven't detected the right name
                            if(m_timeStamp.m_name.size() == 0)
                            {
                                typesWithoutTimeStamp.push_back(typeToDescPair(type_id, desc));
                            }
                        }
                        if (m_timeStamp.m_name.size() != 0)
                        {   // we have a valid timestamp column name
                            // first check if we have to process delayed messages without a timestamp
                            foreach (typeToDescPair typeDescPair, typesWithoutTimeStamp)
                            {
                                addTimeToDescriptor(typeDescPair.second);
                                // store message type for later processing
                                timeStampHasToBeAdded.push_back(typeDescPair.second.m_name);
                                // store adapted message
                                if (!m_dataModel->addType(typeDescPair.second.m_name, typeDescPair.first,
                                                          typeDescPair.second.m_length, typeDescPair.second.m_format,
                                                          typeDescPair.second.m_labels.split(",")))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
                                }
                                index++;
                            }
                            typesWithoutTimeStamp.clear();

                            // Now check if the actual message conains a timestamp if not add it
                            if (!desc.m_labels.contains(m_timeStamp.m_name))
                            {
                                // Add timestamp to descriptor
                                addTimeToDescriptor(desc);
                                timeStampHasToBeAdded.push_back(desc.m_name);// store message type for later processing
                            }
                            // Special handling for "GPS" messages that have a "TimeMS" timestamp but scaling
                            // and value does not mach all other time stamps
                            if ((desc.m_name == "GPS") && desc.m_labels.contains("TimeMS"))
                            {
                                // replace the original timestamp name only if not already done
                                if (adaptGPSDescriptor(nameToDescriptorMap, desc))
                                {
                                    timeStampHasToBeAdded.push_back(desc.m_name);// store message type for later processing
                                }
                            }
                        }

                        if (!m_dataModel->addType(desc.m_name, type_id, desc.m_length, desc.m_format, desc.m_labels.split(",")))
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
                    if (nameToDescriptorMap.contains(name))
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
                        QString typestr = nameToDescriptorMap.value(name).m_format;
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
                            QStringList valuestrlist = nameToDescriptorMap.value(name).m_labels.split(",");

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
                                    qint64 val = valStr.toLongLong(&ok);
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
                                // check if a synthetic timestamp has to added
                                handleMissingTimeStamps(timeStampHasToBeAdded, name, valuepairlist, lastValidTS, index);

                                if (valuepairlist.size() >= 1)
                                {
                                    if (!m_dataModel->addRow(name,valuepairlist,index++, m_timeStamp.m_name))
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
    m_dataModel->setAllRowsHaveTime(true, m_timeStamp.m_name , m_timeStamp.m_divisor);
}


void AP2DataPlotThread::loadTLog(QFile &logfile)
{
    m_loadedLogType = MAV_TYPE_GENERIC;
    int nrOfEmptyMsg = 0;
    int bytesize = 0;
    int index = 100;
    quint8 lastModeVal = 255;
    QStringList timeStampHasToBeAdded;
    quint64 lastValidTS = 0;
    m_timeStamp = timeStampType("time_boot_ms", 1000.0); // tlogs only have one possible time stamp

    mavlink_message_t message;
    mavlink_status_t status;
    QScopedPointer<MAVLinkDecoder> decoder(new MAVLinkDecoder());

    if (!m_dataModel->startTransaction())
    {
        emit error(m_dataModel->getError());
        return;
    }

    // Tlog does not contain MODE messages the mode information ins transmitted in
    // a heartbeat message. So we create the datatype for MODE here and put it into data model
    QStringList modeVarNames;
    modeVarNames.push_back(QString(m_timeStamp.m_name));
    modeVarNames.push_back(QString("Mode"));
    modeVarNames.push_back(QString("ModeNum"));
    modeVarNames.push_back(QString("Info"));
    if (!m_dataModel->addType(ModeMessage::TypeName,0,0,"QMBZ",modeVarNames))
    {
        QString actualerror = m_dataModel->getError();
        m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
        emit error(actualerror);
        return;
    }

    // Tlog does not contain MSG messages. The information is gathered from STATUSTEXT tlog
    // messages. So we create the datatype for MSG here and put it into data model.
    QStringList msgVarNames;
    msgVarNames.push_back(QString(m_timeStamp.m_name));
    msgVarNames.push_back(QString("Message"));
    msgVarNames.push_back(QString("Info"));
    if (!m_dataModel->addType(MsgMessage::TypeName,0,0,"QZZ",msgVarNames))
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
                typeDescriptor desc;
                desc.m_name = decoder->getMessageName(message.msgid);
                if (desc.m_name != "EMPTY")
                {
                    if (message.sysid != 255) // [TODO] GCS packet is not always 255 sysid.
                    {
                        QList<QPair<QString,QVariant> > retvals = decoder->receiveMessage(0,message);
                        if (!m_dataModel->hasType(desc.m_name))
                        {
                            // If we don't have a format description by now extract and store it
                            QList<QString> fieldnames = decoder->getFieldList(desc.m_name);
                            QStringList variablenames;
                            for (int i=0;i<fieldnames.size();i++)
                            {
                                mavlink_field_info_t fieldinfo = decoder->getFieldInfo(desc.m_name,fieldnames.at(i));
                                variablenames <<  QString(fieldinfo.name);
                                switch (fieldinfo.type)
                                {
                                    case MAVLINK_TYPE_CHAR:
                                    {
                                        if (fieldinfo.array_length == 0)
                                        {
                                            desc.m_format += "b";   // it is a single byte
                                        }
                                        else
                                        {
                                            desc.m_format += "Z";   // everything else is a string
                                        }
                                    }
                                    break;
                                    case MAVLINK_TYPE_UINT8_T:
                                    {
                                        desc.m_format += "B";
                                    }
                                    break;
                                    case MAVLINK_TYPE_INT8_T:
                                    {
                                        desc.m_format += "b";
                                    }
                                    break;
                                    case MAVLINK_TYPE_UINT16_T:
                                    {
                                        desc.m_format += "H";
                                    }
                                    break;
                                    case MAVLINK_TYPE_INT16_T:
                                    {
                                        desc.m_format += "h";
                                    }
                                    break;
                                    case MAVLINK_TYPE_UINT32_T:
                                    {
                                        desc.m_format += "I";
                                    }
                                        break;
                                    case MAVLINK_TYPE_INT32_T:
                                    {
                                        desc.m_format += "i";
                                    }
                                    break;
                                    case MAVLINK_TYPE_FLOAT:
                                    {
                                        desc.m_format += "f";
                                    }
                                    break;
                                    case MAVLINK_TYPE_UINT64_T:
                                    {
                                        desc.m_format += "Q";
                                    }
                                    break;
                                    case MAVLINK_TYPE_INT64_T:
                                    {
                                        desc.m_format += "q";
                                    }
                                    break;
                                    default:
                                    {
                                        QLOG_ERROR() << "Unknown type:" << QString::number(fieldinfo.type);
                                        m_plotState.corruptDataRead(i, desc.m_name + " data: Unknown data type:" + QString::number(fieldinfo.type));
                                    }
                                    break;
                                }
                            }
                            desc.m_labels = variablenames.join(",");    // complete descriptor

                            // Handle time stamps
                            if (m_timeStamp.m_name.size() != 0)
                            {
                                // Now check if the actual message conains a timestamp if not add it
                                if (!desc.m_labels.contains(m_timeStamp.m_name))
                                {
                                    addTimeToDescriptor(desc);
                                    // store message type for later processing
                                    timeStampHasToBeAdded.push_back(desc.m_name);
                                }

                                if (!m_dataModel->addType(desc.m_name,0,0, desc.m_format, desc.m_labels.split(",")))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
                                }
                            }
                        }

                        // Read packet data if there is something
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

                        // Check if we have some valid data to store
                        if (valuepairlist.size() >= 1)
                        {
                            // check if a synthetic timestamp has to added
                            handleMissingTimeStamps(timeStampHasToBeAdded, desc.m_name, valuepairlist, lastValidTS, index);

                            if (!m_dataModel->addRow(desc.m_name,valuepairlist, index++, m_timeStamp.m_name))
                            {
                                QString actualerror = m_dataModel->getError();
                                m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                emit error(actualerror);
                                return;
                            }

                            // Tlog does not contain MODE messages the mode information ins transmitted in
                            // a heartbeat message. So here we extract MODE data from hertbeat
                            if ((message.msgid == MAVLINK_MSG_ID_HEARTBEAT) && (valuepairlist.size() > 1))
                            {
                                // Only if mode val has canged
                                if (lastModeVal != static_cast<quint8>(valuepairlist[1].second.toInt()))
                                {
                                    QList<QPair<QString,QVariant> > specialValuepairlist;
                                    // Extract MODE messages from heratbeat messages
                                    lastModeVal = static_cast<quint8>(valuepairlist[1].second.toInt());

                                    specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[0], lastValidTS));
                                    specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[1], lastModeVal));
                                    specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[2], lastModeVal));
                                    specialValuepairlist.append(QPair<QString, QVariant>(modeVarNames[3], "Generated Value"));
                                    if (!m_dataModel->addRow(ModeMessage::TypeName, specialValuepairlist, index++, m_timeStamp.m_name))
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
                                        for (int i = 0; i < valuepairlist.size(); ++i)
                                        {
                                            if (valuepairlist[i].first == "type")   // "type" field holds MAV_TYPE
                                            {
                                                m_loadedLogType = static_cast<MAV_TYPE>(valuepairlist[i].second.toInt());
                                                break;
                                            }
                                        }
                                    }
                                }
                            }

                            if((message.msgid == MAVLINK_MSG_ID_STATUSTEXT) && (valuepairlist.size() > 2))
                            {
                                // Create a MsgMessage from STATUSTEXT
                                QList<QPair<QString,QVariant> > specialValuepairlist;
                                specialValuepairlist.append(QPair<QString, QVariant>(msgVarNames[0], lastValidTS));
                                specialValuepairlist.append(QPair<QString, QVariant>(msgVarNames[1], valuepairlist[2].second));
                                specialValuepairlist.append(QPair<QString, QVariant>(msgVarNames[2], "Generated Value"));
                                if (!m_dataModel->addRow(MsgMessage::TypeName, specialValuepairlist, index++, m_timeStamp.m_name))
                                {
                                    QString actualerror = m_dataModel->getError();
                                    m_dataModel->endTransaction(); //endTransaction can re-set the error if it errors, but we should try it anyway.
                                    emit error(actualerror);
                                    return;
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
    m_dataModel->setAllRowsHaveTime(true, m_timeStamp.m_name, m_timeStamp.m_divisor);
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

void AP2DataPlotThread::addTimeToDescriptor(typeDescriptor &desc)
{
    // Add name of the timestamp column adding a "," only if needed
    desc.m_labels = desc.m_labels.size() != 0 ? m_timeStamp.m_name + ',' + desc.m_labels : m_timeStamp.m_name;
    // Add timestamp format code to format string
    desc.m_format.prepend('Q');
    // and increase the length by 8 bytes ('Q' is a quint_64)
    desc.m_length += 8;
}

void AP2DataPlotThread::adaptGPSDescriptor(QMap<unsigned int, typeDescriptor> &typeToDescriptorMap, typeDescriptor &desc, const unsigned char msg_type)
{
    QStringList labels = desc.m_labels.split(",");
    for (QStringList::Iterator iter = labels.begin(); iter != labels.end(); ++iter)
    {
        if (*iter == "TimeMS")
        {
            *iter = "GPSTimeMS";
            break;
        }
    }
    // Very special - manipulate parsing info here to force
    // parser to use the new time stamp name.
    typeToDescriptorMap.find(msg_type)->m_labels = labels.join(",");
    // add default time stamp name
    labels.prepend(m_timeStamp.m_name);
    desc.m_labels = labels.join(",");
    desc.m_format.prepend("Q");     // Add timestamp format code to format string
    desc.m_length += 8;
}

bool AP2DataPlotThread::adaptGPSDescriptor(QMap<QString, typeDescriptor> &nameToDescriptorMap, typeDescriptor &desc)
{
    if (desc.m_labels.contains("GPSTimeMS"))
    {
        // descriptor is already patched
        return false;
    }

    QStringList labels = desc.m_labels.split(",");
    for (QStringList::Iterator iter = labels.begin(); iter != labels.end(); ++iter)
    {
        if (*iter == "TimeMS")
        {
            *iter = "GPSTimeMS";
            break;
        }
    }
    // Very special - manipulate parsing info here to force
    // parser to use the new time stamp name.
    nameToDescriptorMap.find(desc.m_name)->m_labels = labels.join(",");
    // add default time stamp name
    labels.prepend(m_timeStamp.m_name);
    desc.m_labels = labels.join(",");
    desc.m_format.prepend("Q");     // Add timestamp format code to format string
    desc.m_length += 8;
    return true;
}

void AP2DataPlotThread::handleMissingTimeStamps(const QStringList &timeStampHasToBeAdded, const QString &name,
                                                QList<QPair<QString,QVariant> > &valuepairlist,
                                                quint64 &lastValidTS, const int index)
{
    if (timeStampHasToBeAdded.size() > 0)
    {
        if (timeStampHasToBeAdded.contains(name))
        {
            valuepairlist.prepend(QPair<QString, QVariant>(m_timeStamp.m_name, lastValidTS));
        }
        // if not store actual time stamp
        else
        {
            getTimeStamp(valuepairlist, index, lastValidTS);
        }
    }
}

void AP2DataPlotThread::handleMissingTimeStamps(const QList<unsigned int> &timeStampHasToBeAdded, const unsigned char type,
                                                QList<QPair<QString,QVariant> > &valuepairlist,
                                                quint64 &lastValidTS, const int index)
{
    if (timeStampHasToBeAdded.size() > 0)
    {
        if (timeStampHasToBeAdded.contains(type))
        {
            valuepairlist.prepend(QPair<QString, QVariant>(m_timeStamp.m_name, lastValidTS));
        }
        // if not store actual time stamp
        else
        {
            getTimeStamp(valuepairlist, index, lastValidTS);
        }
    }
}

void AP2DataPlotThread::getTimeStamp(QList<QPair<QString,QVariant> > &valuepairlist, const int index, quint64 &lastValidTS)
{
    // find value pair with time stamp name
    QList<QPair<QString,QVariant> >::Iterator iter;
    for (iter = valuepairlist.begin(); iter != valuepairlist.end(); ++iter)
    {
        if (iter->first == m_timeStamp.m_name)
        {   // found!
            quint64 tempVal = static_cast<quint64>(iter->second.toULongLong());
            // check if time is increasing
            if (tempVal >= lastValidTS)
            {
                lastValidTS = tempVal;
            }
            else
            {
                QLOG_ERROR() << "Corrupt data read: Time is not increasing! Last valid time stamp:"
                             << QString::number(lastValidTS) << " actual read time stamp is:"
                             << QString::number(tempVal);
                m_plotState.corruptTimeRead(index, "Log time is not increasing! Last Time:" +
                                            QString::number(lastValidTS) + " new Time:" +
                                            QString::number(tempVal));
                // if not increasing set to last valid value
                iter->second = lastValidTS;
            }
            break;
        }
    }
}

//*************

AP2DataPlotStatus::AP2DataPlotStatus() : m_lastParsingState(OK), m_globalState(OK)
{
}

void AP2DataPlotStatus::corruptDataRead(const int index, const QString &errorMessage)
{
    // When here we just know that we have an error but not if it is at the end
    // or in the middle of the logfile. So we set truncation error. Data error will
    // be set as soon as valid data is read again.
    m_globalState = m_globalState == OK ? TruncationError : m_globalState;
    m_lastParsingState = DataError;
    errorEntry entry(m_lastParsingState, index, errorMessage);
    m_errors.push_back(entry);
}

void AP2DataPlotStatus::corruptFMTRead(const int index, const QString &errorMessage)
{
    m_globalState = m_globalState == OK ? FmtError : m_globalState;
    m_lastParsingState = FmtError;
    errorEntry entry(m_lastParsingState, index, errorMessage);
    m_errors.push_back(entry);
}

void AP2DataPlotStatus::corruptTimeRead(const int index, const QString &errorMessage)
{
    m_globalState = m_globalState == OK ? TimeError : m_globalState;
    m_lastParsingState = TimeError;
    errorEntry entry(m_lastParsingState, index, errorMessage);
    m_errors.push_back(entry);
}

AP2DataPlotStatus::parsingState AP2DataPlotStatus::getParsingState() const
{
    return m_globalState;
}


QString AP2DataPlotStatus::getErrorOverview() const
{
    int timeErrorCount = 0;
    int dataErrorCount = 0;
    int fmtErrorCount  = 0;
    int unknownErrorCount = 0;
    QString out;
    QTextStream outStream(&out);

    foreach (const errorEntry &entry, m_errors)
    {
        switch (entry.m_state)
        {
        case OK:
            break;
        case FmtError:
            fmtErrorCount++;
            break;
        case TimeError:
            timeErrorCount++;
            break;
        case DataError:
            dataErrorCount++;
            break;
        default:
            unknownErrorCount++;
            break;
        }
    }

    if (fmtErrorCount     > 0) outStream << fmtErrorCount << " format corruptions" << endl;
    if (timeErrorCount    > 0) outStream << timeErrorCount << " time corruptions" << endl;
    if (dataErrorCount    > 0) outStream << dataErrorCount << " data corruptions" << endl;
    if (unknownErrorCount > 0) outStream << unknownErrorCount << " unspecific corruptions" << endl;

    return out;
}

QString AP2DataPlotStatus::getDetailedErrorText() const
{
    QString out;
    QTextStream outStream(&out);

    foreach (const errorEntry &entry, m_errors)
    {
        if (entry.m_state != OK)    // Only if a error
        {
            outStream << "Logline " << entry.m_index << ": " << entry.m_errortext << endl;
        }
    }
    outStream << endl << " There were " << m_errors.size() << " errors during log parsing." << endl;
    return out;
}
