#include <QSettings>
#include "AudioWorker.h"
#include "configuration.h"
#include <QThread>

AudioWorker::AudioWorker(QObject *parent) :
    QObject(parent)
{

#ifdef Q_OS_LINUX
    //localAlsaAudio = AlsaAudio::instance();
#endif // Q_OS_LINUX

}

void AudioWorker::setFilename(QString filename)
{
    aw_Filename = filename;
     QLOG_INFO() << "New Filename:" << aw_Filename;
}

bool AudioWorker::isPlaying()
{
    return false;
}

void AudioWorker::play()
{
    QLOG_INFO() << "Playing:" << aw_Filename;
#ifdef Q_OS_LINUX

    //QLOG_INFO() <<  AlsaAudio::instance()->alsa_play(aw_Filename) << "hat gespielt";
#endif // Q_OS_LINUX
    thread()->exit();
}


