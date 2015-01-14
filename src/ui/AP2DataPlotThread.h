#ifndef AP2DATAPLOTTHREAD_H
#define AP2DATAPLOTTHREAD_H

#include <QThread>
#include <QVariantMap>
#include <QSqlDatabase>
#include "MAVLinkDecoder.h"
#include "AP2DataPlot2DModel.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
class AP2DataPlotThread : public QThread
{
    Q_OBJECT
public:
    explicit AP2DataPlotThread(AP2DataPlot2DModel *model,QObject *parent = 0);
    void loadFile(QString file);
    void stopLoad() { m_stop = true; }
private:
    void loadDataFieldsFromValues();
    QString m_fileName;
    bool m_stop;
    int m_fieldCount;
    MAVLinkDecoder *decoder;
    int m_errorCount;
    QMap<QString,QString> m_msgNameToInsertQuery;
    void loadBinaryLog();
    void loadAsciiLog();
    void loadTLog();
    MAV_TYPE m_loadedLogType;
    AP2DataPlot2DModel *m_dataModel;
protected:
    void run();
signals:
    void startLoad();
    void loadProgress(qint64 pos,qint64 size);
    void payloadDecoded(int index,QString name,QVariantMap map);
    void done(int errors,MAV_TYPE type);
    void error(QString errorstr);
    void lineRead(QString line);
};

#endif // AP2DATAPLOTTHREAD_H
