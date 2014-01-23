#include <QApplication>
#include <QSettings>
#include "QsLog.h"
#ifdef Q_OS_LINUX
#ifndef ALSAAUDIO_H
#define ALSAAUDIO_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include <alsa/asoundlib.h>
#include <sys/time.h>
#include    <sndfile.h>

#define ALSA_PCM_NEW_HW_PARAMS_API
#define ALSA_PCM_NEW_SW_PARAMS_API
#define SIGNED_SIZEOF(x)  ((int) sizeof (x))
#define BUFFER_LEN (2048)

class AlsaAudio : public QObject
{
public:
    AlsaAudio();
    /** @brief Get the singleton instance */
    static AlsaAudio* instance();

    void alsa_play( QString filename );

private:
    snd_pcm_t * alsa_open( int channels, int srate );
    int alsa_write_float( snd_pcm_t *alsa_dev, float *data, int frames, int channels );
};

#endif // ALSAAUDIO_H
#endif // Q_OS_LINUX


