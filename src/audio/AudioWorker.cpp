#include <QSettings>
#include "AudioWorker.h"

AudioWorker::AudioWorker(QObject *parent) :
    QObject(parent)
{
}

#ifdef Q_OS_LINUX
void AudioWorker::setFilename(QString filename)
{
    aw_Filename = filename;
}

bool AudioWorker::isPlaying()
{
    return false;
}

void AudioWorker::play()
{
    QLOG_INFO() << "Playing:" << aw_Filename;
}

#endif // Q_OS_LINUX
