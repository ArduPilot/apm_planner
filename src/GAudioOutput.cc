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
 *   @brief Implementation of audio output
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#include "QsLog.h"
#include "configuration.h"
#include "GAudioOutput.h"
#include "MG.h"

#include <QApplication>
#include <QSettings>
#include <QTemporaryFile>

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#endif

// Speech synthesis is only supported with MSVC compiler
#if _MSC_VER2
// Documentation: http://msdn.microsoft.com/en-us/library/ee125082%28v=VS.85%29.aspx
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
//You may derive a class from CComModule and use it if you want to override something,
//but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>

#include <sapi.h>

//using System;
//using System.Speech.Synthesis;
#endif

#if defined(FLITE_AUDIO_ENABLED)
extern "C" {
#include <flite/flite.h>
    cst_voice* register_cmu_us_kal(const char* voxdir);
};
#endif



/**
 * This class follows the singleton design pattern
 * @see http://en.wikipedia.org/wiki/Singleton_pattern
 * A call to this function thus returns the only instance of this object
 * the call can occur at any place in the code, no reference to the
 * GAudioOutput object has to be passed.
 */
GAudioOutput* GAudioOutput::instance()
{
    static GAudioOutput* _instance = 0;
    if(_instance == 0)
    {
        _instance = new GAudioOutput();
        // Set the application as parent to ensure that this object
        // will be destroyed when the main application exits
        _instance->setParent(qApp);
    }
    return _instance;
}

#define QGC_GAUDIOOUTPUT_KEY QString("QGC_AUDIOOUTPUT_")

GAudioOutput::GAudioOutput(QObject* parent) : QObject(parent),
    voiceIndex(0),
    emergency(false),
    muted(false)
{
    // Load settings
    QSettings settings;
    settings.sync();
    muted = settings.value(QGC_GAUDIOOUTPUT_KEY+"muted", muted).toBool();

#ifdef FLITE_AUDIO_ENABLED
    // Remove Phonon Audio for linux and use alsa
    flite_init();

    QLOG_INFO() << "Using Alsa Audio driver";

    // Create shared dir tmp_audio
    // we create new spoken audio files here. we don't delete them as befor.
    // we save audiofiles like message inside.
    // if new messages will create in code this new messages will saved as audio file on first call
    // this save time and also it will possible to queue audio messages later because the are not temporary
    QDir dir(QString("%1/%2").arg( QGC::appDataDirectory() ).arg( "tmp_audio" ));
    if (!dir.exists()) {
        QLOG_WARN() << "Create directory tmp_audio";
        dir.mkpath(".");
    }else
    {
        QLOG_WARN() << "Dir directory tmp_audio exists";
    }

#endif

#ifdef Q_OS_MAC
    m_speech_channel = new SpeechChannel;
    OSErr theErr = NewSpeechChannel(NULL, m_speech_channel);
    if (theErr != noErr)
    {
        QLOG_WARN() << "Creating speech channel failed!";
        delete m_speech_channel;
        m_speech_channel = NULL;
    }
#endif

#if _MSC_VER2

    ISpVoice * pVoice = NULL;
    if (FAILED(::CoInitialize(NULL)))
    {
        QLOG_WARN() << "Creating COM object for audio output failed!";
    }
    else
    {

        HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice;);
        if( SUCCEEDED( hr ) )
        {
            hr = pVoice->Speak(L"Hello world", 0, NULL);
            pVoice->Release();
            pVoice = NULL;
        }
    }
#endif

    // Prepare regular emergency signal, will be fired off on calling startEmergency()
    emergencyTimer = new QTimer();
    connect(emergencyTimer, SIGNAL(timeout()), this, SLOT(beep()));

    switch (voiceIndex) {
    case 0:
        selectFemaleVoice();
        break;
    default:
        selectMaleVoice();
        break;
    }
}


GAudioOutput::~GAudioOutput()
{
    QLOG_INFO() << "~GAudioOutput()";
#ifdef Q_OS_LINUX
    // wait until thread is running before terminate AlsaAudio thread
    AlsaAudio::instance(this)->wait();
#endif
#ifdef Q_OS_MAC
    if(m_speech_channel)
    {
        DisposeSpeechChannel(*m_speech_channel);
    };
    delete m_speech_channel;
    m_speech_channel = NULL;
#endif
//#ifdef _MSC_VER2
// ::CoUninitialize();
//#endif
}

void GAudioOutput::mute(bool mute)
{
    if (mute != muted)
    {
        this->muted = mute;
        QSettings settings;
        settings.setValue(QGC_GAUDIOOUTPUT_KEY+"muted", this->muted);
        settings.sync();
        emit mutedChanged(muted);
    }
}

bool GAudioOutput::isMuted()
{
    return this->muted;
}

bool GAudioOutput::say(QString text, int severity)
{
    if (!muted)
    {
        if (text.compare("system %1") == 0)
            //don't say system %1 [HACK] :(
            return true;

        // TODO Add severity filter
        Q_UNUSED(severity);
        bool res = false;
        if (!emergency)
        {

            // Speech synthesis is only supported with MSVC compiler
#ifdef _MSC_VER2
            SpeechSynthesizer synth = new SpeechSynthesizer();
            synth.SelectVoice("Microsoft Anna");
            synth.SpeakText(text.toStdString().c_str());
            res = true;
#endif

#ifdef FLITE_AUDIO_ENABLED
            // spokenfilename is the filename created from spoken text
            QString spokenFilename = text;
            spokenFilename.replace(QRegExp(" "), "_");
            spokenFilename = QGC::appDataDirectory() + "/tmp_audio/" + spokenFilename + ".wav";

            // alsadriver is a qthread. tmp. files dont work here
            QFile file( spokenFilename );
            if (!file.exists(spokenFilename)){ // if file not exist we create a new one
                if (file.open(QIODevice::ReadWrite))
                {
                    QLOG_INFO() << file.fileName() << " file not exist, create a new one";
                    cst_voice *v = register_cmu_us_kal(NULL);
                    cst_wave *wav = flite_text_to_wave(text.toStdString().c_str(), v);
                    cst_wave_save(wav, file.fileName().toStdString().c_str(), "riff");
                    file.close();
                    AlsaAudio::instance(this)->enqueueFilname(file.fileName());
                    if(!AlsaAudio::instance(this)->isRunning())
                        AlsaAudio::instance(this)->start();
                    res = true;
                }
            }else // we open existing file
            {
                QLOG_INFO() << file.fileName() << " file exist, playing this file";
                AlsaAudio::instance(this)->enqueueFilname(file.fileName());
                if(!AlsaAudio::instance(this)->isRunning())
                    AlsaAudio::instance(this)->start();
                res = true;
            }
#endif

#ifdef Q_OS_MAC
            if(m_speech_channel)
            {
                SpeakCFString(*m_speech_channel, text.toCFString(), NULL);
            }
            res = true;
#endif
        }
        return res;
    }
    else
    {
        return false;
    }
}

/**
 * @param text This message will be played after the alert beep
 */
bool GAudioOutput::alert(QString text)
{
    if (!emergency || !muted)
    {
        // Play alert sound
        beep();
        // Say alert message
        say(text, 2);
        return true;
    }
    else
    {
        return false;
    }
}

void GAudioOutput::notifyPositive()
{
    if (!muted)
    {
        // Use QFile to transform path for all OS
        QFile f(QGC::shareDirectory()+QString("/files/audio/double_notify.wav"));
        //m_media->setCurrentSource(Phonon::MediaSource(f.fileName().toStdString().c_str()));
        //m_media->play();
    }
}

void GAudioOutput::notifyNegative()
{
    if (!muted)
    {
        // Use QFile to transform path for all OS
        QFile f(QGC::shareDirectory()+QString("/files/audio/flat_notify.wav"));
        //m_media->setCurrentSource(Phonon::MediaSource(f.fileName().toStdString().c_str()));
        //m_media->play();
    }
}

/**
 * The emergency sound will be played continously during the emergency.
 * call stopEmergency() to disable it again. No speech synthesis or other
 * audio output is available during the emergency.
 *
 * @return true if the emergency could be started, false else
 */
bool GAudioOutput::startEmergency()
{
    if (!emergency)
    {
        emergency = true;
        // Beep immediately and then start timer
        if (!muted) beep();
        emergencyTimer->start(1500);
        QTimer::singleShot(5000, this, SLOT(stopEmergency()));
    }
    return true;
}

/**
 * Stops the continous emergency sound. Use startEmergency() to start
 * the emergency sound.
 *
 * @return true if the emergency could be stopped, false else
 */
bool GAudioOutput::stopEmergency()
{
    if (emergency) {
        emergency = false;
        emergencyTimer->stop();
    }
    return true;
}

void GAudioOutput::beep()
{
    if (!muted)
    {
        // Use QFile to transform path for all OS
        QFile f(QGC::shareDirectory()+QString("/files/audio/alert.wav"));

#ifdef Q_OS_LINUX
        AlsaAudio::instance(this)->enqueueFilname(f.fileName());
        if(!AlsaAudio::instance(this)->isRunning())
            AlsaAudio::instance(this)->start();
#endif

    }
}

void GAudioOutput::selectFemaleVoice()
{
#ifdef Q_OS_LINUX
    //this->voice = register_cmu_us_slt(NULL);
#endif
}

void GAudioOutput::selectMaleVoice()
{
#ifdef Q_OS_LINUX
    //this->voice = register_cmu_us_rms(NULL);
#endif
}

/*
void GAudioOutput::selectNeutralVoice()
{
#ifdef Q_OS_LINUX
    this->voice = register_cmu_us_awb(NULL);
#endif
}*/

QStringList GAudioOutput::listVoices(void)
{
    QStringList l;
#ifdef FLITE_AUDIO_ENABLED
    cst_voice *voice;
    const cst_val *v;



    printf("Voices available: ");
    for (v=flite_voice_list; v; v=val_cdr(v)) {
        voice = val_voice(val_car(v));
        QString s;
        s.sprintf("%s",voice->name);
        printf("%s",voice->name);
        l.append(s);
    }
    printf("\n");

#endif
    return l;

}
