#ifndef AP2DATAPLOTTHREAD_H
#define AP2DATAPLOTTHREAD_H

#include <QThread>
#include <QVariantMap>
#include <QSqlDatabase>
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
class AP2DataPlotThread : public QThread
{
    Q_OBJECT
public:
    explicit AP2DataPlotThread(QObject *parent = 0);
    void loadFile(QString file,QSqlDatabase *db);
    void stopLoad() { m_stop = true; }
private:
    void loadDataFieldsFromValues();
    QString m_fileName;
    bool m_stop;
    QSqlDatabase *m_db;
    QString makeInsertTableString(QString tablename, QString variablestr);
    QString makeCreateTableString(QString tablename, QString formatstr,QString variablestr);
protected:
    void run();
signals:
    void startLoad();
    void loadProgress(qint64 pos,qint64 size);
    void payloadDecoded(int index,QString name,QVariantMap map);
    void done(int errors,MAV_TYPE type);
    void error(QString errorstr);
    void lineRead(QString line);
public slots:

};

#endif // AP2DATAPLOTTHREAD_H
