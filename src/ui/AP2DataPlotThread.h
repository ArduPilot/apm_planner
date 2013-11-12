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
private:
    void loadDataFieldsFromValues();
    QString m_fileName;
protected:
    void run();
signals:
    void payloadDecoded(int index,QString name,QVariantMap map);
    void done();
public slots:

};

#endif // AP2DATAPLOTTHREAD_H
