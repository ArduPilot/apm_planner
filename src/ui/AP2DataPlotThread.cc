#include "AP2DataPlotThread.h"
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QDateTime>

AP2DataPlotThread::AP2DataPlotThread(QObject *parent) :
    QThread(parent)
{
    m_badChecksums = 0;
    m_falseStarts = 0;
    m_outOfPacketBytes = 0;
    m_lengthMismatch = 0;
    m_badLengthHeader = 0;
    m_badEscapeChar = 0;
    m_locationIdInfoReq = 0;
    m_locationIdInfoReply = 0;
}

AP2DataPlotThread::Packet AP2DataPlotThread::parseBuffer(QByteArray buffer)
{
    if (buffer.size() <= 2)
    {
        qDebug() << "Not long enough to even contain a header!";
        emit decoderFailure(buffer);
        return Packet(false);
    }


    Packet retval;
    QByteArray header;
    //Parse the packet here
    int headersize = 3;
    int iloc = 0;
    bool seq = false;
    bool len = false;
    if (buffer[iloc] & 0x100)
    {
        //Has header
        seq = true;
        headersize += 1;
    }
    if (buffer[iloc] & 0x1)
    {
        //Has length
        len = true;
        headersize += 2;
    }
    header = buffer.mid(0,headersize);
    iloc++;
    unsigned int payloadid = (unsigned int)buffer[iloc] << 8;

    payloadid += (unsigned char)buffer[iloc+1];
    retval.payloadid = payloadid;
    iloc += 2;
    if (seq)
    {
        iloc += 1;
        retval.hasseq = true;
    }
    else
    {
        retval.hasseq = false;
    }
    QByteArray payload;
    if (len)
    {
        retval.haslength = true;
        unsigned int length = buffer[iloc] << 8;
        length += (unsigned char)buffer[iloc+1];
        retval.length = length;
        iloc += 2;
        if ((unsigned int)buffer.length() > (unsigned int)(length + iloc))
        {
            qDebug() << "Packet length should be:" << length + iloc << "But it is" << buffer.length();
            emit decoderFailure(buffer);
            m_lengthMismatch++;
            return Packet(false);
        }
        payload.append(buffer.mid(iloc,length));
    }
    else
    {
        retval.haslength = false;
        payload.append(buffer.mid(iloc),(buffer.length()-iloc));
    }
    QString output;
    for (int i=0;i<payload.size();i++)
    {
        int num = (unsigned char)payload[i];
        output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
    }
    output.clear();
    for (int i=0;i<header.size();i++)
    {
        int num = (unsigned char)header[i];
        output.append(" ").append((num < 0xF) ? "0" : "").append(QString::number(num,16));
    }
    //Last byte of currPacket should be out checksum.
    retval.header = header;
    retval.payload = payload;
    if (header[0] & 0x02)
    {
        retval.isNAK = true;
    }
    else
    {
        retval.isNAK = false;
    }
    if (retval.header.size() >= 3)
    {
        return retval;
    }
    else
    {
        m_badLengthHeader++;
        return Packet(false);
    }
}
void AP2DataPlotThread::loadFile(QString file)
{
    m_fileName = file;
    start();
}
void AP2DataPlotThread::run()
{
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    //dataflash log
    QFile logfile(m_fileName);
    logfile.open(QIODevice::ReadOnly);
    QMap<QString,QList<QString> > typeToFieldnameMap;
    QList<QString> typelist;
    QVariantMap currentmap;
    QMap<QString,double> lastvalues;
    QList<QString> acceptedTypeList;
    while (!logfile.atEnd())
    {
        QString line = logfile.readLine();
        QStringList linesplit = line.replace("\r","").replace("\n","").split(",");
        if (linesplit.size() > 0)
        {
            if (line.startsWith("FMT"))
            {
                //Format line
                QString type = linesplit[3].trimmed();
                QString descstr = linesplit[4].trimmed();
                QList<QString> fieldnames;
                for (int i=5;i<linesplit.size();i++)
                {
                    fieldnames.append(linesplit[i].trimmed());
                }
                if (!typelist.contains(type))
                {
                    typelist.append(type);
                }
                typeToFieldnameMap[type] = fieldnames;

                /*if (line.split(",").size() == descstr.length()+5)
                {
                    //Valid line
                    qDebug() << "Valid Line";
                }
                else
                {
                    qDebug() << "Invalid line:" << line;
                }*/

            }
            else if (typelist.contains(linesplit[0].trimmed()) && linesplit[0].trimmed() != "PARM")
            {
                //It's a known type.
                if (acceptedTypeList.contains(linesplit[0].trimmed()))
                {
                    //We need to fire off the log and increment.
                    for (int i=0;i<typelist.size();i++)
                    {
                        QList<QString> list = typeToFieldnameMap[typelist[i]];
                        for (int j=0;j<list.size();j++)
                        {
                            if (!currentmap.contains(typelist[i] +"."+list[j]))
                            {
                                if (lastvalues.contains(typelist[i] +"."+list[j]))
                                {
                                    currentmap[typelist[i] +"."+list[j]] = lastvalues[typelist[i] +"."+list[j]];
                                }
                                else
                                {
                                    currentmap[typelist[i] +"."+list[j]] = 0;
                                }
                            }
                            else
                            {
                                lastvalues[typelist[i] +"."+list[j]] = currentmap[typelist[i] +"."+list[j]].toDouble();
                            }
                        }
                    }
                    emit payloadDecoded(currentmap);
                    currentmap.clear();
                    acceptedTypeList.clear();
                    //qDebug() << "Record loaded" << logfile.pos() << logfile.size();
                }
                acceptedTypeList.append(linesplit[0].trimmed());
                QList<QString> list = typeToFieldnameMap[linesplit[0].trimmed()];
                if (linesplit.size() != list.size() + 1)
                {
                    qDebug() << "Error with line:" << line;
                }
                for (int i=0;i<list.size();i++)
                {
                    currentmap[linesplit[0].trimmed() + "." + list[i]] = linesplit[i+1].trimmed().toDouble();
                }
            }
        }
    }
    qDebug() << "Log loading took" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds";
    emit done();
}
