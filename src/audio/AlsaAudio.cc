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
**
**  aufruf von alsaplayer alsa_play (argc, argv) ;
*/

#include <QApplication>
#include <QSettings>

#ifdef Q_OS_LINUX
#include "AlsaAudio.h"

AlsaAudio::AlsaAudio()
{
}

AlsaAudio* AlsaAudio::instance()
{
    static AlsaAudio* _instance = 0;
    if(_instance == 0)
    {
        _instance = new AlsaAudio();
        // Set the application as parent to ensure that this object
        // will be destroyed when the main application exits
        _instance->setParent(qApp);
    }
    return _instance;
}

void AlsaAudio::alsa_play( QString filename )
{

    static float buffer [BUFFER_LEN] ;
    SNDFILE *sndfile ;
    SF_INFO sfinfo ;
    snd_pcm_t * alsa_dev ;
    int k, readcount, subformat ;

    //puts (__func__) ;


    memset (&sfinfo, 0, sizeof (sfinfo)) ;

    QLOG_INFO() << "Playing:" << filename;
    if (! (sndfile = sf_open (filename.toLocal8Bit(), SFM_READ, &sfinfo)))
    {
        QLOG_INFO() << sf_strerror;
    }

    if (sfinfo.channels < 1 || sfinfo.channels > 2)
    {
        QLOG_INFO() << "Error : channels = " << sfinfo.channels;

    }

    alsa_dev = alsa_open (sfinfo.channels, sfinfo.samplerate) ;

    subformat = sfinfo.format & SF_FORMAT_SUBMASK ;

    if (subformat == SF_FORMAT_FLOAT || subformat == SF_FORMAT_DOUBLE)
    {
        double scale;
        int m;

        sf_command (sndfile, SFC_CALC_SIGNAL_MAX, &scale, sizeof (scale)) ;
        if (scale < 1e-10)
            scale = 1.0 ;
        else
            scale = 32700.0 / scale ;

        while ((readcount = sf_read_float (sndfile, buffer, BUFFER_LEN)))
        {
            for (m = 0 ; m < readcount ; m++)
                buffer [m] *= scale ;
            alsa_write_float (alsa_dev, buffer, BUFFER_LEN / sfinfo.channels, sfinfo.channels) ;
        }
    }
    else
    {     while ((readcount = sf_read_float (sndfile, buffer, BUFFER_LEN)))
            alsa_write_float (alsa_dev, buffer, BUFFER_LEN / sfinfo.channels, sfinfo.channels) ;
    }

    snd_pcm_close (alsa_dev) ;

    sf_close (sndfile) ;


    return ;
} /* alsa_play */

snd_pcm_t * AlsaAudio::alsa_open (int channels, int samplerate)
{
    const char * device = "plughw:0" ;
    snd_pcm_t *alsa_dev ;
    snd_pcm_hw_params_t *hw_params ;
    snd_pcm_uframes_t buffer_size, xfer_align, start_threshold ;
    snd_pcm_uframes_t alsa_period_size, alsa_buffer_frames ;
    snd_pcm_sw_params_t *sw_params ;

    int err ;

    alsa_period_size = 512 ;
    alsa_buffer_frames = 3 * alsa_period_size ;

    if ((err = snd_pcm_open (&alsa_dev, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {     fprintf (stderr, "cannot open audio device \"%s\" (%s)\n", device, snd_strerror (err)) ;
        return NULL ;
    } ;

    snd_pcm_nonblock (alsa_dev, 0) ;

    if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0)
    {     fprintf (stderr, "cannot allocate hardware parameter structure (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params_any (alsa_dev, hw_params)) < 0)
    {     fprintf (stderr, "cannot initialize hardware parameter structure (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params_set_access (alsa_dev, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {     fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params_set_format (alsa_dev, hw_params, SND_PCM_FORMAT_FLOAT)) < 0)
    {     fprintf (stderr, "cannot set sample format (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params_set_rate_near (alsa_dev, hw_params, (uint*)&samplerate, 0)) < 0)
    {     fprintf (stderr, "cannot set sample rate (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params_set_channels (alsa_dev, hw_params, channels)) < 0)
    {     fprintf (stderr, "cannot set channel count (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params_set_buffer_size_near (alsa_dev, hw_params, &alsa_buffer_frames)) < 0)
    {     fprintf (stderr, "cannot set buffer size (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;


    if ((err = snd_pcm_hw_params_set_period_size_near (alsa_dev, hw_params, &alsa_period_size, 0)) < 0)
    {     fprintf (stderr, "cannot set period size (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_hw_params (alsa_dev, hw_params)) < 0)
    {     fprintf (stderr, "cannot set parameters (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    /* extra check: if we have only one period, this code won't work */
    snd_pcm_hw_params_get_period_size (hw_params, &alsa_period_size, 0) ;
    snd_pcm_hw_params_get_buffer_size (hw_params, &buffer_size) ;
    if (alsa_period_size == buffer_size)
    {     fprintf (stderr, "Can't use period equal to buffer size (%lu == %lu)", alsa_period_size, buffer_size) ;
        return NULL ;
    } ;

    snd_pcm_hw_params_free (hw_params) ;

    if ((err = snd_pcm_sw_params_malloc (&sw_params)) != 0)
    {     fprintf (stderr, "%s: snd_pcm_sw_params_malloc: %s", __func__, snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_sw_params_current (alsa_dev, sw_params)) != 0)
    {     fprintf (stderr, "%s: snd_pcm_sw_params_current: %s", __func__, snd_strerror (err)) ;
        return NULL ;
    } ;

    /* note: set start threshold to delay start until the ring buffer is full */
    snd_pcm_sw_params_current (alsa_dev, sw_params) ;
    if ((err = snd_pcm_sw_params_get_xfer_align (sw_params, &xfer_align)) < 0)
    {     fprintf (stderr, "cannot get xfer align (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    /* round up to closest transfer boundary */
    start_threshold = (buffer_size / xfer_align) * xfer_align ;
    if (start_threshold < 1)
        start_threshold = 1 ;
    if ((err = snd_pcm_sw_params_set_start_threshold (alsa_dev, sw_params, start_threshold)) < 0)
    {     fprintf (stderr, "cannot set start threshold (%s)\n", snd_strerror (err)) ;
        return NULL ;
    } ;

    if ((err = snd_pcm_sw_params (alsa_dev, sw_params)) != 0)
    {     fprintf (stderr, "%s: snd_pcm_sw_params: %s", __func__, snd_strerror (err)) ;
        return NULL ;
    } ;
    snd_pcm_sw_params_free (sw_params) ;

    snd_pcm_reset (alsa_dev) ;

    return alsa_dev ;
} /* alsa_open */

int AlsaAudio::alsa_write_float(snd_pcm_t *alsa_dev, float *data, int frames, int channels)
{
    static int epipe_count = 0 ;

    snd_pcm_status_t *status ;
    int total = 0 ;
    int retval ;

    if (epipe_count > 0)
        epipe_count -- ;

    while (total < frames)
    {     retval = snd_pcm_writei (alsa_dev, data + total * channels, frames - total) ;

        if (retval >= 0)
        {     total += retval ;
            if (total == frames)
                return total ;

            continue ;
        } ;

        switch (retval)
        {     case -EAGAIN :
            puts ("alsa_write_float: EAGAIN") ;
            continue ;
            break ;

        case -EPIPE :
            if (epipe_count > 0)
            {     printf ("alsa_write_float: EPIPE %d\n", epipe_count) ;
                if (epipe_count > 140)
                    return retval ;
            } ;
            epipe_count += 100 ;

            if (0)
            {     snd_pcm_status_alloca (&status) ;
                if ((retval = snd_pcm_status (alsa_dev, status)) < 0)
                    fprintf (stderr, "alsa_out: xrun. can't determine length\n") ;
                else if (snd_pcm_status_get_state (status) == SND_PCM_STATE_XRUN)
                {     struct timeval now, diff, tstamp ;

                    gettimeofday (&now, 0) ;
                    snd_pcm_status_get_trigger_tstamp (status, &tstamp) ;
                    timersub (&now, &tstamp, &diff) ;

                    fprintf (stderr, "alsa_write_float xrun: of at least %.3f msecs. resetting stream\n",
                             diff.tv_sec * 1000 + diff.tv_usec / 1000.0) ;
                }
                else
                    fprintf (stderr, "alsa_write_float: xrun. can't determine length\n") ;
            } ;

            snd_pcm_prepare (alsa_dev) ;
            break ;

        case -EBADFD :
            fprintf (stderr, "alsa_write_float: Bad PCM state.n") ;
            return 0 ;
            break ;

        case -ESTRPIPE :
            fprintf (stderr, "alsa_write_float: Suspend event.n") ;
            return 0 ;
            break ;

        case -EIO :
            puts ("alsa_write_float: EIO") ;
            return 0 ;

        default :
            fprintf (stderr, "alsa_write_float: retval = %d\n", retval) ;
            return 0 ;
            break ;
        } ; /* switch */
    } ; /* while */

    return total ;
}

#endif // Q_OS_LINUX
