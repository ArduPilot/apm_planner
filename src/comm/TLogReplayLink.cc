#include "TLogReplayLink.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include "UASManager.h"
#include "UAS.h"
#include "MainWindow.h"
TLogReplayLink::TLogReplayLink(QObject *parent) :
    LinkInterface(parent),
    m_toBeDeleted(false),
    m_threadRun(false)
{
    m_speedVar = 50;
}
int TLogReplayLink::getId()
{
    return 1;
}
QString TLogReplayLink::getName()
{
    return "AP2SimulationLink";
}
void TLogReplayLink::requestReset()
{

}
bool TLogReplayLink::isConnected()
{
    return false;
}
qint64 TLogReplayLink::getNominalDataRate()
{
    return 115200;
}
bool TLogReplayLink::isFullDuplex()
{
    return true;
}
int TLogReplayLink::getLinkQuality()
{
    return 100;
}
qint64 TLogReplayLink::getTotalUpstream()
{
    return 0;
}
qint64 TLogReplayLink::getCurrentUpstream()
{
    return 0;
}
qint64 TLogReplayLink::getMaxUpstream()
{
    return 0;
}
qint64 TLogReplayLink::getBitsSent()
{
    return 0;
}
qint64 TLogReplayLink::getBitsReceived()
{
    return 0;
}
bool TLogReplayLink::connect()
{
    start();
    return true;
}
bool TLogReplayLink::disconnect()
{
    return false;
}
qint64 TLogReplayLink::bytesAvailable()
{
    return 0;
}
void TLogReplayLink::writeBytes(const char *bytes, qint64 length)
{

}
void TLogReplayLink::readBytes()
{

}
void TLogReplayLink::setSpeed(int speed)
{
    m_variableAccessMutex.lock();
    m_speedVar = speed;
    m_variableAccessMutex.unlock();
}
void TLogReplayLink::play()
{
    m_pause = false;
}

void TLogReplayLink::pause()
{
    m_pause = true;
}
bool TLogReplayLink::isPaused()
{
    return m_pause;
}

void TLogReplayLink::run()
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
    MainWindow::instance()->toolBar().disableConnectWidget(true);
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
    UASManager::instance()->removeUAS(UASManager::instance()->getActiveUAS());
    MainWindow::instance()->toolBar().disableConnectWidget(false);
}
void TLogReplayLink::setLog(QString logfile)
{
    m_logFile = logfile;
}
void TLogReplayLink::stop()
{
    m_toBeDeleted = true;
    m_threadRun = false;
}

bool TLogReplayLink::toBeDeleted()
{
    return m_toBeDeleted;
}
