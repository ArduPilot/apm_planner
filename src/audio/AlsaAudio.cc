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

////////////////////////////////// INFO ////////////////////////////////////////
//                                                                            //
/// some driver parts are copied from mega-nerd.com(libsndfile) example page ///
//                                                                            //

/*
** Copyright (C) 1999-2004 Erik de Castro Lopo <erikd@mega-nerd.com>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

//                                                                            //
//                                                                            //
///////////////////////////////// ~INFO ////////////////////////////////////////

/**
 * @file
 *   @brief Definition of audio output
 *
 *   @author Michael Wolkstein
 *
 */

#include "AlsaAudio.h"

AlsaAudio::AlsaAudio(QObject *parent) :
    QThread(parent),
    aa_Volume(1.0f)
{
}

AlsaAudio* AlsaAudio::instance(QObject *par)
{
    static AlsaAudio* _instance = 0;
    if(_instance == 0)
    {
        _instance = new AlsaAudio();
        // Set the application as parent to ensure that this object
        // will be destroyed when the main application exits
        _instance->setParent(par);
    }
    return _instance;
}

void AlsaAudio::enqueueFilname(QString name)
{
    aa_fileNameQueue.enqueue(name);
}

// main qthread
void AlsaAudio::run()
{
    while (!aa_fileNameQueue.isEmpty()){
        QString filetoplay = aa_fileNameQueue.dequeue();
        alsa_play( filetoplay );
    }

}


bool AlsaAudio::alsa_play( QString filename )
{
#ifndef Q_OS_LINUX
    Q_UNUSED(filename);
#endif

#ifdef Q_OS_LINUX
    static float buffer [BUFFER_LEN];
    SNDFILE *sndfile;
    SF_INFO sfinfo;
    snd_pcm_t * alsa_dev;
    int readcount, subformat;

    memset (&sfinfo, 0, sizeof (sfinfo));


    //QLOG_INFO() << "Playing:" << filename;
    if (! (sndfile = sf_open (filename.toLocal8Bit(), SFM_READ, &sfinfo)))
    {
        //QLOG_INFO() << sf_strerror;
        QLOG_INFO() << " ERROR OPEN FILE: " << filename;
        return false;
    }


    if (sfinfo.channels < 1 || sfinfo.channels > 2)
    {
        QLOG_INFO() << "Error : channels = " << sfinfo.channels;
        return false;

    }

    alsa_dev = alsa_open (sfinfo.channels, sfinfo.samplerate);

    if (!alsa_dev)
    {
        QLOG_ERROR() << "Failure playing audio file" << filename;
        return false;
    }

    subformat = sfinfo.format & SF_FORMAT_SUBMASK;

    if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
    {
        double scale;
        int m;

        sf_command (sndfile, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale));
        if (scale < 1e-10)
            scale = 1.0;
        else
            scale = 32700.0 / scale;

        while ((readcount = sf_read_float (sndfile, buffer, BUFFER_LEN)))
        {
            for (m = 0; m < readcount; m++)
                buffer [m] *= scale * aa_Volume;
            alsa_write_float (alsa_dev, buffer, BUFFER_LEN / sfinfo.channels, sfinfo.channels);
        }
    }
    else
    {
        int m;
        while ((readcount = sf_read_float (sndfile, buffer, BUFFER_LEN)))
        {
            for (m = 0; m < readcount; m++)
                buffer [m] *= aa_Volume;
            alsa_write_float (alsa_dev, buffer, BUFFER_LEN / sfinfo.channels, sfinfo.channels);
        }
    }

    snd_pcm_close (alsa_dev);

    sf_close (sndfile);


    return true;
} /* alsa_play */

snd_pcm_t * AlsaAudio::alsa_open (int channels, int samplerate)
{
    const char * device = "plughw:0";
    snd_pcm_t *alsa_dev;
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_uframes_t buffer_size, xfer_align, start_threshold;
    snd_pcm_uframes_t alsa_period_size, alsa_buffer_frames;
    snd_pcm_sw_params_t *sw_params;

    int err;

    alsa_period_size = 512;
    alsa_buffer_frames = 3 * alsa_period_size;

    if ((err = snd_pcm_open (&alsa_dev, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        QLOG_INFO() << "cannot open audio device " << device << snd_strerror(err);
        return NULL;
    }

    snd_pcm_nonblock (alsa_dev, 0);

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {
        QLOG_INFO() << "cannot allocate hardware parameter structure "<< snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params_any (alsa_dev, hw_params)) < 0)
    {
        QLOG_INFO() << "cannot initialize hardware parameter structure " << snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_access (alsa_dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        QLOG_INFO() << "cannot set access type ", snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_format (alsa_dev, hw_params, SND_PCM_FORMAT_FLOAT)) < 0)
    {
        QLOG_INFO() << "cannot set sample format " << snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_rate_near (alsa_dev, hw_params, (uint*)&samplerate, 0)) < 0)
    {
        QLOG_INFO() << "cannot set sample rate " << snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_channels (alsa_dev, hw_params, channels)) < 0)
    {
        QLOG_INFO() << "cannot set channel count " << snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params_set_buffer_size_near (alsa_dev, hw_params, &alsa_buffer_frames)) < 0)
    {
        QLOG_INFO() << "cannot set buffer size " << snd_strerror (err);
        return NULL;
    }


    if ((err = snd_pcm_hw_params_set_period_size_near (alsa_dev, hw_params, &alsa_period_size, 0)) < 0)
    {
        QLOG_INFO() << "cannot set period size " << snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_hw_params (alsa_dev, hw_params)) < 0)
    {
        QLOG_INFO() << "cannot set parameters " << snd_strerror (err);
        return NULL;
    }

    /* extra check: if we have only one period, this code won't work */
    snd_pcm_hw_params_get_period_size (hw_params, &alsa_period_size, 0);
    snd_pcm_hw_params_get_buffer_size (hw_params, &buffer_size);
    if (alsa_period_size == buffer_size)
    {
        QLOG_INFO() << "Can't use period equal to buffer size " << alsa_period_size << buffer_size;
        return NULL;
    }

    snd_pcm_hw_params_free (hw_params);

    if ((err = snd_pcm_sw_params_malloc (&sw_params)) != 0)
    {
        QLOG_INFO() << "%s: snd_pcm_sw_params_malloc: " << __func__<< snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_sw_params_current (alsa_dev, sw_params)) != 0)
    {
        QLOG_INFO() << "%s: snd_pcm_sw_params_current: " << __func__ << snd_strerror (err);
        return NULL;
    }

    /* note: set start threshold to delay start until the ring buffer is full */
    snd_pcm_sw_params_current (alsa_dev, sw_params);
    if ((err = snd_pcm_sw_params_get_xfer_align (sw_params, &xfer_align)) < 0)
    {
        QLOG_INFO() << "cannot get xfer align " << snd_strerror (err);
        return NULL;
    }

    /* round up to closest transfer boundary */
    start_threshold = (buffer_size / xfer_align) * xfer_align;
    if (start_threshold < 1)
        start_threshold = 1;
    if ((err = snd_pcm_sw_params_set_start_threshold (alsa_dev, sw_params, start_threshold)) < 0)
    {
        QLOG_INFO() << "cannot set start threshold " << snd_strerror (err);
        return NULL;
    }

    if ((err = snd_pcm_sw_params (alsa_dev, sw_params)) != 0)
    {
        QLOG_INFO() << "%s: snd_pcm_sw_params: " << __func__ << snd_strerror (err);
        return NULL;
    }
    snd_pcm_sw_params_free (sw_params);

    snd_pcm_reset (alsa_dev);

    return alsa_dev;
} /* alsa_open */

int AlsaAudio::alsa_write_float(snd_pcm_t *alsa_dev, float *data, int frames, int channels)
{
    static int epipe_count = 0;

    snd_pcm_status_t *status;
    int total = 0;
    int retval;

    if (epipe_count > 0)
        epipe_count --;

    while (total < frames)
    {
        retval = snd_pcm_writei (alsa_dev, data + total * channels, frames - total);

        if (retval >= 0)
        {     total += retval;
            if (total == frames)
                return total;

            continue;
        }

        switch (retval)
        {
        case -EAGAIN :
            puts ("alsa_write_float: EAGAIN");
            continue;
            break;

        case -EPIPE :
            if (epipe_count > 0)
            {
                QLOG_INFO() << "alsa_write_float: EPIPE " << epipe_count;
                if (epipe_count > 140)
                    return retval;
            }
            epipe_count += 100;

            if (0)
            {
                snd_pcm_status_alloca (&status);
                if ((retval = snd_pcm_status (alsa_dev, status)) < 0){
                    QLOG_INFO() << "alsa_out: xrun. can't determine length";
                }
                else if (snd_pcm_status_get_state (status) == SND_PCM_STATE_XRUN)
                {
                    struct timeval now, diff, tstamp;

                    gettimeofday (&now, 0);
                    snd_pcm_status_get_trigger_tstamp (status, &tstamp);
                    timersub (&now, &tstamp, &diff);

                    QLOG_INFO() << "alsa_write_float xrun: of at least " << (diff.tv_sec * 1000 + diff.tv_usec / 1000.0) << " msecs. resetting stream";
                }
                else
                    QLOG_INFO() << "alsa_write_float: xrun. can't determine length";
            }

            snd_pcm_prepare (alsa_dev);
            break;

        case -EBADFD :
            QLOG_INFO() << "alsa_write_float: Bad PCM state.n";
            return 0;
            break;

        case -ESTRPIPE :
            QLOG_INFO() << "alsa_write_float: Suspend event.n";
            return 0;
            break;

        case -EIO :
            QLOG_INFO() << "alsa_write_float: EIO";
            return 0;

        default :
            QLOG_INFO() << "alsa_write_float: retval = " << retval;
            return 0;
            break;
        } /* switch */
    } /* while */

    return total;
#else
    return 0;
#endif // Q_OS_LINUX

}



