/*
   Copyright (C) 2010 Marco Ballesio <gibrovacco@gmail.com>
   Copyright (C) 2011-2013 Collabora Ltd.
     @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

   This library is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef GStreamerPlayer_H
#define GStreamerPlayer_H

#include <QObject>
#include <QTimer>
#include <QGst/Pipeline>
#include <QGst/Message>

class GStreamerPlayer : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(int brightness READ getBrightness WRITE setBrightness NOTIFY brightnessChanged)
    Q_PROPERTY(int contrast READ getContrast WRITE setContrast NOTIFY contrastChanged)
    Q_PROPERTY(int hue READ getHue WRITE setHue NOTIFY hueChanged)
    Q_PROPERTY(int saturation READ getSaturation WRITE setSaturation NOTIFY saturationChanged)
    Q_PROPERTY(bool playing READ getPlaying WRITE setPlaying NOTIFY playingChanged)
    Q_PROPERTY(bool paused READ getPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool stopped READ getStopped WRITE setStopped NOTIFY stoppedChanged)

    explicit GStreamerPlayer(QObject *parent = 0);
    ~GStreamerPlayer();

    void setVideoSink(const QGst::ElementPtr & sink);

    int getBrightness()
    {
        m_brightness = m_videoSink->property("brightness").toInt();
        return m_brightness;
    }

    int getContrast()
    {
        m_contrast = m_videoSink->property("contrast").toInt();
        return m_contrast;
    }

    int getHue()
    {
        m_hue = m_videoSink->property("hue").toInt();
        return m_hue;
    }

    int getSaturation()
    {
        m_saturation = m_videoSink->property("saturation").toInt();
        return m_saturation;
    }

    bool getPlaying()
    {
        return m_playing;
    }

    bool getPaused()
    {
        return m_paused;
    }

    bool getStopped()
    {
        return m_stopped;
    }

    void setPlaying(bool play)
    {
        m_playing = play;
        emit playingChanged(m_playing);
        this->play();
    }

    void setPaused(bool pause)
    {
        m_paused = pause;
        emit pausedChanged(m_paused);
        this->pause();
    }

    void setStopped(bool)
    {
        // First send EOS in case the stream is going to a file
        if (!m_stopTimer.isActive())
        {
            sendEOS();
            m_stopTimer.start(1000);
        }
    }

    void setBrightness(int brightness)
    {
        m_brightness = brightness;
        emit brightnessChanged(brightness);
        m_videoSink->setProperty("brightness", brightness);
    }

    void setContrast(int contrast)
    {
        m_contrast = contrast;
        emit contrastChanged(contrast);
        m_videoSink->setProperty("contrast", contrast);
    }

    void setHue(int hue)
    {
        m_hue = hue;
        emit hueChanged(hue);
        m_videoSink->setProperty("hue", hue);
    }

    void setSaturation(int saturation)
    {
        m_saturation = saturation;
        emit saturationChanged(saturation);
        m_videoSink->setProperty("saturation", saturation);
    }

public Q_SLOTS:
    void play();
    void pause();
    void stop();
	void toggleFullScreen();
    void onStopTimer();

    void initialize(const QString & pipelineString);

signals:
    void brightnessChanged(int);
    void contrastChanged(int);
    void hueChanged(int);
    void saturationChanged(int);
    void playingChanged(bool);
    void pausedChanged(bool);
    void stoppedChanged(bool);

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);
    void sendEOS();

    QTimer m_stopTimer;

    QGst::PipelinePtr m_pipeline;
    QGst::ElementPtr m_videoSink;

    int m_brightness;
    int m_contrast;
    int m_hue;
    int m_saturation;

    bool m_playing;
    bool m_paused;
    bool m_stopped;


};

#endif // GStreamerPlayer_H
