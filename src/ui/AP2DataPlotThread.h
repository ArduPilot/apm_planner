#ifndef AP2DATAPLOTTHREAD_H
#define AP2DATAPLOTTHREAD_H

#include <QThread>
#include <QVariantMap>
class AP2DataPlotThread : public QThread
{
    Q_OBJECT
public:
    explicit AP2DataPlotThread(QObject *parent = 0);
    void loadFile(QString file);
    void stopLoad() { m_stop = true; }
private:
    void loadDataFieldsFromValues();
    QString m_fileName;
    bool m_stop;
protected:
    void run();
signals:
    void startLoad();
    void loadProgress(qint64 pos,qint64 size);
    void payloadDecoded(int index,QString name,QVariantMap map);
    void done();
    void error(QString errorstr);
    void lineRead(QString line);
public slots:

};

#endif // AP2DATAPLOTTHREAD_H
