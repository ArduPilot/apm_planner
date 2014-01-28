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

/**
 * @file
 *   @brief Definition of audio output
 *
 *   @author Michael Wolkstein
 *
 */

#ifndef ALSAAUDIO_H
#define ALSAAUDIO_H
#include <QSettings>
#include <QThread>
#include <QQueue>
#include "QsLog.h"

#ifdef Q_OS_LINUX
#include <alsa/asoundlib.h>
#include <sys/time.h>
#include <sndfile.h>
#define BUFFER_LEN (2048)
#endif // Q_OS_LINUX

class AlsaAudio : public QThread
{
//    Q_OBJECT
public:
    AlsaAudio(QObject *parent=NULL);

    /** @brief Get the singleton instance */
    static AlsaAudio* instance(QObject *par);

    /** @brief enqueue new filename */
    void enqueueFilname(QString name);

    /** @brief set volume double 0.0f - 1.0f */
    double getAAVolume(){
        return aa_Volume;
    }

    /** @brief return volume float 0.0f - 1.0f */
    void setAAVolume( double volume){
        aa_Volume = volume;
    }

public slots:
    void run();

private:
    QQueue<QString> aa_fileNameQueue;
    double aa_Volume;

protected:

    bool alsa_play( QString filename );
#ifdef Q_OS_LINUX
    snd_pcm_t * alsa_open( int channels, int srate );
    int alsa_write_float( snd_pcm_t *alsa_dev, float *data, int frames, int channels );
#endif // Q_OS_LINUX

};

#endif // ALSAAUDIO_H

