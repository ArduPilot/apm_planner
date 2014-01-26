/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#ifndef ALSAAUDIO_H
#define ALSAAUDIO_H
#include <QApplication>
#include <QSettings>
#include "QsLog.h"
#include <QThread>

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
    void setAAVolume( float volume){
        aa_volume = volume;
    }

public slots:
    void run();

private:
    QQueue<QString> aa_fileNameQueue;
    float aa_volume;

protected:
#ifdef Q_OS_LINUX
    bool alsa_play( QString filename );
    snd_pcm_t * alsa_open( int channels, int srate );
    int alsa_write_float( snd_pcm_t *alsa_dev, float *data, int frames, int channels );
#endif // Q_OS_LINUX

};

#endif // ALSAAUDIO_H
