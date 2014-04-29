#include "TLogReplayLink.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include "UASManager.h"
#include "UAS.h"
#include "MainWindow.h"
TLogReplayLink::TLogReplayLink() :
    m_toBeDeleted(false),
    m_threadRun(false)
{
    m_speedVar = 50;
}
int TLogReplayLink::getId() const
{
    return 1;
}
QString TLogReplayLink::getName() const
{
    return "AP2SimulationLink";
}
void TLogReplayLink::requestReset()
{

}
bool TLogReplayLink::isConnected() const
{
    return false;
}
qint64 TLogReplayLink::getConnectionSpeed() const
{
    return 115200;
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
    MainWindow::instance()->toolBar().disableConnectWidget(true);
    MainWindow::instance()->toolBar().overrideDisableConnectWidget(true);
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
    if (m_threadRun)
    {
        m_toBeDeleted = true;
    }
    MainWindow::instance()->toolBar().overrideDisableConnectWidget(false);
    MainWindow::instance()->toolBar().disableConnectWidget(false);
    emit disconnected(this);
    emit disconnected();
    emit connected(false);
    UASManager::instance()->removeUAS(UASManager::instance()->getActiveUAS());
}
void TLogReplayLink::setLog(QString logfile)
{
    m_logFile = logfile;
}
void TLogReplayLink::stop()
{
    m_toBeDeleted = false;
    m_threadRun = false;
}

bool TLogReplayLink::toBeDeleted()
{
    return m_toBeDeleted;
}
