#include "TLogReplyLink.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>
TLogReplyLink::TLogReplyLink(QObject *parent) :
    LinkInterface(parent),
    m_toBeDeleted(false),
    m_threadRun(false)
{
    m_speedVar = 50;
}
int TLogReplyLink::getId()
{
    return 1;
}
QString TLogReplyLink::getName()
{
    return "AP2SimulationLink";
}
void TLogReplyLink::requestReset()
{

}
bool TLogReplyLink::isConnected()
{
    return false;
}
qint64 TLogReplyLink::getNominalDataRate()
{
    return 115200;
}
bool TLogReplyLink::isFullDuplex()
{
    return true;
}
int TLogReplyLink::getLinkQuality()
{
    return 100;
}
qint64 TLogReplyLink::getTotalUpstream()
{
    return 0;
}
qint64 TLogReplyLink::getCurrentUpstream()
{
    return 0;
}
qint64 TLogReplyLink::getMaxUpstream()
{
    return 0;
}
qint64 TLogReplyLink::getBitsSent()
{
    return 0;
}
qint64 TLogReplyLink::getBitsReceived()
{
    return 0;
}
bool TLogReplyLink::connect()
{
    start();
    return true;
}
bool TLogReplyLink::disconnect()
{
    return false;
}
qint64 TLogReplyLink::bytesAvailable()
{
    return 0;
}
void TLogReplyLink::writeBytes(const char *bytes, qint64 length)
{

}
void TLogReplyLink::readBytes()
{

}
void TLogReplyLink::setSpeed(int speed)
{
    m_variableAccessMutex.lock();
    m_speedVar = speed;
    m_variableAccessMutex.unlock();
}
void TLogReplyLink::play()
{
    m_pause = false;
}

void TLogReplyLink::pause()
{
    m_pause = true;
}
bool TLogReplyLink::isPaused()
{
    return m_pause;
}

void TLogReplyLink::run()
{
    m_pause = false;
    m_threadRun = true;
    emit connected(this);
    emit connected(true);
    emit connected();
    QFile file(m_logFile);
    file.open(QIODevice::ReadOnly);
    int bytesize = 0;
    qint64 msecs = QDateTime::currentMSecsSinceEpoch();
    int privSpeedVar = 25;
    while (!file.atEnd() && m_threadRun)
    {
        if (QDateTime::currentMSecsSinceEpoch() - msecs > 1000)
        {
            qDebug() << "Checking reply link timing...";
            msecs = QDateTime::currentMSecsSinceEpoch();
            m_variableAccessMutex.lock();
            //m_speedVar = speed;
            qDebug() << "Old timing:" << privSpeedVar << "New timing:" << 50 - ((m_speedVar) / 4);
            privSpeedVar = 50 - ((m_speedVar) / 4);
            m_variableAccessMutex.unlock();
        }
        emit logProgress(file.pos(),file.size());
        QByteArray bytes = file.read(128);
        bytesize+=128;
        //qDebug() << bytesize << "read";
        emit bytesReceived(this,bytes);
        msleep(privSpeedVar);
        while (m_pause)
        {
            msleep(100);
        }
    }
    emit disconnected(this);
    emit disconnected();
    emit connected(false);
}
void TLogReplyLink::setLog(QString logfile)
{
    m_logFile = logfile;
}
void TLogReplyLink::stop()
{
    m_toBeDeleted = true;
    m_threadRun = false;
}

bool TLogReplyLink::toBeDeleted()
{
    return m_toBeDeleted;
}
