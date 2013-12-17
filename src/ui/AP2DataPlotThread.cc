#include "AP2DataPlotThread.h"
#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QDateTime>
#include "QsLog.h"

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
    m_stop = false;
    emit startLoad();
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    QFile logfile(m_fileName);
    if (!logfile.open(QIODevice::ReadOnly))
    {
        emit error("Unable to open log file");
        return;
    }
    QMap<QString,QList<QString> > typeToFieldnameMap;
    QList<QString> typelist;
    QVariantMap currentmap;
    int index = 0;
    while (!logfile.atEnd() && !m_stop)
    {
        emit loadProgress(logfile.pos(),logfile.size());
        QString line = logfile.readLine();
        emit lineRead(line);
        QStringList linesplit = line.replace("\r","").replace("\n","").split(",");
        if (linesplit.size() > 0)
        {
            if (line.startsWith("FMT"))
            {
                //Format line
                if (linesplit.size() > 4)
                {
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
                else
                {
                    QLOG_ERROR() << "Error with line in plot log file:" << line;
                }
            }
            else if (typelist.contains(linesplit[0].trimmed()) && linesplit[0].trimmed() != "PARM")
            {

                QList<QString> list = typeToFieldnameMap[linesplit[0].trimmed()];
                if (linesplit.size() != list.size() + 1)
                {
                    QLOG_ERROR() << "Error with line in plot log file:" << line;
                    // [TODO] may want to log the valid values, and show some
                    // kind of error with the frame in the viewer, instead of just
                    // dropping the frame (will impact timing)
                } else {
                    for (int i=0;i<list.size();i++)
                    {
                        currentmap[linesplit[0].trimmed() + "." + list[i]] = linesplit[i+1].trimmed().toDouble();
                    }
                    emit payloadDecoded(index++,linesplit[0].trimmed(),currentmap);
                }
                currentmap.clear();
            }
        }
    }
    if (m_stop)
    {
        QLOG_ERROR() << "Plot Log loading was canceled after" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds";
        emit error("Log loading Canceled");
    }
    else
    {
        QLOG_INFO() << "Plot Log loading took" << (QDateTime::currentMSecsSinceEpoch() - msecs) / 1000.0 << "seconds";
        emit done();
    }
}
