
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

    void setFilename(QString filename);
    bool isPlaying();

signals:

public slots:

    void play();

private:

    QString aw_Filename;
#ifdef Q_OS_LINUX

    //AlsaAudio *localAlsaAudio;
#endif // Q_OS_LINUX

};

#endif // AUDIOWORKER_H
