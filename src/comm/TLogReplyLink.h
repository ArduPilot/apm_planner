#ifndef TLOGREPLYLINK_H
#define TLOGREPLYLINK_H

#include "LinkInterface.h"
#include <QMutex>

class TLogReplyLink : public LinkInterface
{
    Q_OBJECT
public:
    explicit TLogReplyLink(QObject *parent = 0);
    void play();
    void pause();
    bool isPaused();
    int getId();
    QString getName();
    void requestReset();
    bool isConnected();
    qint64 getNominalDataRate();
    bool isFullDuplex();
    int getLinkQuality();
    qint64 getTotalUpstream();
    qint64 getCurrentUpstream();
    qint64 getMaxUpstream();
    qint64 getBitsSent();
    qint64 getBitsReceived();
    bool connect();
    bool disconnect();
    qint64 bytesAvailable();
    void writeBytes(const char *bytes, qint64 length);
    void setLog(QString logfile);
    void stop();
    bool toBeDeleted();

    //Speed is 1-100, being slowest to fastest
    void setSpeed(int speed);
signals:
    /*void bytesReceived(LinkInterface* link, QByteArray data);
    void connected();
    void connected(LinkInterface* linkInterface);
    void disconnected();
    void disconnected(LinkInterface* linkInterface);
    void connected(bool connected);
    void nameChanged(QString name);
    void communicationError(const QString& linkname, const QString& error);
    void communicationUpdate(const QString& linkname, const QString& text);
    void deleteLink(LinkInterface* const link);*/
    void logProgress(qint64 pos,qint64 total);
public slots:
private slots:
    void run();
    void readBytes();
private:
    QString m_logFile;
    bool m_toBeDeleted;
    bool m_threadRun;
    QMutex m_variableAccessMutex;
    int m_speedVar;
    bool m_pause;
};

#endif // TLOGREPLYLINK_H
