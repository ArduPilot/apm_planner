#include <QApplication>
#include <QSettings>
#include "QsLog.h"
#include <QThread>
#ifndef ALSAAUDIO_H
#define ALSAAUDIO_H

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <sndfile.h>

#define BUFFER_LEN (2048)
#endif // Q_OS_LINUX

class AlsaAudio : public QThread
{

public:
    AlsaAudio(QObject *parent=NULL);
    /** @brief Get the singleton instance */
    static AlsaAudio* instance(QObject *par);
    void setFilname(QString name);

#ifdef Q_OS_LINUX

    bool alsa_play( QString filename );
#endif // Q_OS_LINUX

private:

    QString a_fileName;
#ifdef Q_OS_LINUX
    snd_pcm_t * alsa_open( int channels, int srate );
    int alsa_write_float( snd_pcm_t *alsa_dev, float *data, int frames, int channels );

#endif // Q_OS_LINUX


public slots:

    void run();

};

#endif // ALSAAUDIO_H
