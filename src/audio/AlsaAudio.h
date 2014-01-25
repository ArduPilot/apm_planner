#include <QApplication>
#include <QSettings>
#include "QsLog.h"
#include <QThread>
#ifndef ALSAAUDIO_H
#define ALSAAUDIO_H

#include <QQueue>

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <sndfile.h>
#define BUFFER_LEN (2048)
#endif // Q_OS_LINUX

class AlsaAudio : public QThread
{
//   Q_OBJECT
public:
    AlsaAudio(QObject *parent=NULL);

    /** @brief Get the singleton instance */
    static AlsaAudio* instance(QObject *par);

    /** @brief enqueue new filename */
    void enqueueFilname(QString name);

    /** @brief set volume float 0.0f - 1.0f */
    float getAAVolume(){
        return aa_volume;
    }

    /** @brief return volume float 0.0f - 1.0f */
    float setAAVolume( float volume){
        aa_volume = volume;
    }

private:
    QQueue<QString> aa_fileNameQueue;
    float aa_volume;

protected:
#ifdef Q_OS_LINUX
    bool alsa_play( QString filename );
    snd_pcm_t * alsa_open( int channels, int srate );
    int alsa_write_float( snd_pcm_t *alsa_dev, float *data, int frames, int channels );
#endif // Q_OS_LINUX


public slots:
    void run();

};

#endif // ALSAAUDIO_H
