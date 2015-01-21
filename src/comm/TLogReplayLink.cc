#include "TLogReplayLink.h"
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include "UASManager.h"
#include "UAS.h"
#include "MainWindow.h"
#include "QGCMAVLinkUASFactory.h"
TLogReplayLink::TLogReplayLink(QObject *parent) :
    LinkInterface(),
    m_toBeDeleted(false),
    m_threadRun(false),
    m_speedVar(50),
    m_posVar(0),
    m_mavlinkDecoder(new MAVLinkDecoder()),
    m_mavlinkInspector(NULL)
{
    Q_UNUSED(parent);
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
    Q_UNUSED(bytes);
    Q_UNUSED(length);
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
void TLogReplayLink::setPosition(qint64 pos)
{
    m_variableAccessMutex.lock();
    m_posVar = pos;
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
void TLogReplayLink::setMavlinkDecoder(MAVLinkDecoder *decoder)
{
    m_mavlinkDecoder = decoder;
}
void TLogReplayLink::setMavlinkInspector(QGCMAVLinkInspector *inspector)
{
    m_mavlinkInspector = inspector;
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
    int privSpeedVar = 100;
    mavlink_message_t message;
    mavlink_status_t status;
    QByteArray timebuf;
    bool firsttime = true;
    int delay = 0;
    qint64 lastLogTime = 0;
    qint64 lastPcTime = 0;
    bool nexttime = false;
    qint64 privatepos = 0;
    while (!file.atEnd() && m_threadRun)
    {
        if (QDateTime::currentMSecsSinceEpoch() - msecs > 1000)
        {
            msecs = QDateTime::currentMSecsSinceEpoch();
            m_variableAccessMutex.lock();

            //m_speedVar is a value, between 25 and 175. These are speed percentages.
            privSpeedVar = m_speedVar; //50 - ((m_speedVar) / 4);
            if (privatepos != m_posVar)
            {
                privatepos = m_posVar;
                if (privatepos > 0 && privatepos < 100)
                {
                    file.seek((privatepos / 100.0) * file.size());
                }
            }
            m_variableAccessMutex.unlock();
        }
        emit logProgress(file.pos(),file.size());
        QByteArray bytes = file.read(128);
        bytesize+=128;

        for (int i=0;i<bytes.size();i++)
        {
            unsigned int decodeState = mavlink_parse_char(14, (uint8_t)(bytes[i]), &message, &status);
            if (decodeState != 1)
            {
                //Not a mavlink byte!
                if (nexttime)
                {
                    timebuf.append(bytes[i]);
                }
                if (timebuf.size() == 8)
                {
                    nexttime = false;

                    //Should be a timestamp for the next packet.
                    quint64 logmsecs = quint64(static_cast<unsigned char>(timebuf.at(0))) << 56;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(1))) << 48;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(2))) << 40;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(3))) << 32;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(4))) << 24;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(5))) << 16;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(6))) << 8;
                    logmsecs += quint64(static_cast<unsigned char>(timebuf.at(7))) << 0;

                    timebuf.clear();

                    if (firsttime)
                    {
                        firsttime = false;
                        lastLogTime = logmsecs;
                        lastPcTime = QDateTime::currentMSecsSinceEpoch();
                    }
                    else
                    {
                        //Difference in time between the last time we read a timestamp, and this time
                        qint64 pcdiff = QDateTime::currentMSecsSinceEpoch() - lastPcTime;
                        lastPcTime = QDateTime::currentMSecsSinceEpoch();

                        //Difference in time between the last timestamp we fired off, and this one
                        qint64 logdiff = logmsecs - lastLogTime;
                        lastLogTime = logmsecs;
                        logdiff /= 1000;

                        if (logdiff < pcdiff)
                        {
                            //The next mavlink packet was fired faster than our loop is running, send it immediatly
                            //Fire immediatly
                            delay = 0;
                        }
                        else
                        {
                            //The next mavlink packet was sent logdiff-pcdiff millseconds after the current time
                            delay = logdiff-pcdiff;

                        }
                    }

                }
            }
            else if (decodeState == 1)
            {
                nexttime = true;
                //Good decode
                if (message.sysid == 255)
                {
                    //GCS packet, ignore it
                }
                else
                {
                    UASInterface* uas = UASManager::instance()->getUASForId(message.sysid);
                    if (!uas && message.msgid == MAVLINK_MSG_ID_HEARTBEAT)
                    {
                        mavlink_heartbeat_t heartbeat;
                        // Reset version field to 0
                        heartbeat.mavlink_version = 0;
                        mavlink_msg_heartbeat_decode(&message, &heartbeat);


                        // Create a new UAS object
                        if (heartbeat.autopilot == MAV_AUTOPILOT_ARDUPILOTMEGA)
                        {
                            ArduPilotMegaMAV* mav = new ArduPilotMegaMAV(0, message.sysid);
                            mav->setSystemType((int)heartbeat.type);
                            uas = mav;
                            // Make UAS aware that this link can be used to communicate with the actual robot
                            uas->addLink(this);
                            UASObject *obj = new UASObject();
                            LinkManager::instance()->addSimObject(message.sysid,obj);

                            // Now add UAS to "official" list, which makes the whole application aware of it
                            UASManager::instance()->addUAS(uas);

                        }
                    }
                    else if (uas)
                    {
                        if (delay > 0 && delay < 10000)
                        {
                            //Split the delay into 100msec chunks, to allow for canceling.
                            int realdelay = delay / ((double)privSpeedVar / 100.0);
                            int repeat = realdelay / 100;
                            for (int i=0;i<repeat;i++)
                            {
                                msleep(100);
                                if (!m_threadRun)
                                {
                                    //Break out
                                    MainWindow::instance()->toolBar().overrideDisableConnectWidget(false);
                                    MainWindow::instance()->toolBar().disableConnectWidget(false);
                                    emit disconnected(this);
                                    emit disconnected();
                                    emit connected(false);
                                    UASManager::instance()->removeUAS(UASManager::instance()->getActiveUAS());
                                    return;
                                }
                            }
                            msleep(realdelay - repeat);
                            //msleep(delay / ((double)privSpeedVar / 100.0));
                        }
                        else
                        {
                            msleep(1);
                        }
                        uas->receiveMessage(this,message);
                        m_mavlinkDecoder->receiveMessage(this,message);
                        if (m_mavlinkInspector)
                        {
                            m_mavlinkInspector->receiveMessage(this,message);
                        }
                    }
                    else
                    {
                        //no UAS, and not a heartbeat
                    }
                }

            }
        }
        while (m_pause)
        {
            msleep(100);
        }
    }
    if (m_threadRun)
    {
        m_toBeDeleted = true;
    }
    LinkManager *lm = LinkManager::instance();
    if (lm){
        LinkManager::instance()->removeSimObject(UASManager::instance()->getActiveUAS()->getSystemId());
    } else {
        QLOG_ERROR() << "TLogReplayLink: failed to get Linkmanager instance";
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
