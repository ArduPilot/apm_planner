#include <QSettings>

#ifndef AUDIOWORKER_H
#define AUDIOWORKER_H

#include <QStringList>
#include <QObject>
#include "QsLog.h"
#ifdef Q_OS_LINUX
#include "AlsaAudio.h"
#endif // Q_OS_LINUX

class AudioWorker : public QObject
{
    Q_OBJECT
public:

    AudioWorker(QObject *parent = 0);

#ifdef Q_OS_LINUX
    void setFilename(QString filename);
    bool isPlaying();
#endif // Q_OS_LINUX

signals:

public slots:
    #ifdef Q_OS_LINUX
    void play();
    #endif // Q_OS_LINUX

private:
    #ifdef Q_OS_LINUX
    QString aw_Filename;
    #endif // Q_OS_LINUX
};

#endif // AUDIOWORKER_H
