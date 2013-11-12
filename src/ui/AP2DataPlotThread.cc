#include "AP2DataPlotThread.h"
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QDateTime>

AP2DataPlotThread::AP2DataPlotThread(QObject *parent) :
    QThread(parent)
{

}
void AP2DataPlotThread::loadFile(QString file)
{
    m_fileName = file;
    start();
}
void AP2DataPlotThread::run()
{
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    QFile logfile(m_fileName);
    logfile.open(QIODevice::ReadOnly);
    QMap<QString,QList<QString> > typeToFieldnameMap;
    QList<QString> typelist;
    QVariantMap currentmap;

    int index = 0;
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
            }
            else if (typelist.contains(linesplit[0].trimmed()) && linesplit[0].trimmed() != "PARM")
            {

                QList<QString> list = typeToFieldnameMap[linesplit[0].trimmed()];
                if (linesplit.size() != list.size() + 1)
                {
                    qDebug() << "Error with line:" << line;
                }
                for (int i=0;i<list.size();i++)
                {
                    currentmap[linesplit[0].trimmed() + "." + list[i]] = linesplit[i+1].trimmed().toDouble();
                }
                emit payloadDecoded(index++,linesplit[0].trimmed(),currentmap);
                currentmap.clear();
            }
        }
    }
    qDebug() << "Log loading took" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds";
    emit done();
}
