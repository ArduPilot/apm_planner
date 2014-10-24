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
#include "GStreamerPlayer.h"
#include <QUrl>
#include <QDebug>
#include <QGlib/Connect>
#include <QGlib/Error>
#include <QGst/ElementFactory>
#include <QGst/Bus>
#include <QGst/Pad>
#include <QGst/Event>
#include <QQuickView>
#include <QDialog>

GStreamerPlayer::GStreamerPlayer(QObject *parent)
    : QObject(parent)
{
    m_playing = false;
    m_stopped = true;
    m_paused = false;

    m_stopTimer.setSingleShot(true);
    connect(&m_stopTimer, SIGNAL(timeout()), this, SLOT(onStopTimer()));

}

GStreamerPlayer::~GStreamerPlayer()
{
    stop();
}

void GStreamerPlayer::setVideoSink(const QGst::ElementPtr & sink)
{
    m_videoSink = sink;
}

void GStreamerPlayer::play()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StatePlaying);
    }
}

void GStreamerPlayer::pause()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StatePaused);
    }
}

void GStreamerPlayer::stop()
{
    if (m_pipeline) {
        m_pipeline->setState(QGst::StateNull);
    }
}

void GStreamerPlayer::toggleFullScreen()
{
	QQuickView *pView = dynamic_cast<QQuickView*>(parent());
    if (pView != NULL)
        {
        if (pView->visibility() == QWindow::FullScreen)
        {
            pView->showNormal();
        }
        else
        {
            pView->showFullScreen();
        }
    }
    else
    {
        for (QObject *pParent = parent(); pParent != NULL; pParent = pParent->parent())
        {
            QDialog *pView = dynamic_cast<QDialog*>(pParent);
            if (pView != NULL)
            {
                if (pView->isFullScreen())
                {
                    pView->showNormal();
                }
                else
                {
                    pView->showFullScreen();
                }
                break;
            }
        }
    }
}

void GStreamerPlayer::initialize(const QString & pipelineString)
{
    if (!m_pipeline)
    {
        QByteArray data = pipelineString.toUtf8();
        const char *pData = data.constData();

        m_pipeline = QGst::ElementFactory::parseLaunch(pData).dynamicCast<QGst::Pipeline>();
        if (m_pipeline)
        {
            QGst::PadPtr src = m_pipeline->findUnlinkedPad(QGst::PadSrc);
            if (!src.isNull())
            {
                src->parentElement()->setProperty("caps", QGst::Caps::fromString("video/x-raw, format=I420"));
                m_videoSink->setProperty("sync", false);
                m_pipeline->add(m_videoSink);
                src->parentElement()->link(m_videoSink);

                //watch the bus for messages
                QGst::BusPtr bus = m_pipeline->bus();
                bus->addSignalWatch();
                QGlib::connect(bus, "message", this, &GStreamerPlayer::onBusMessage);
            }
            else
            {
               qCritical() << "The Pipeline command string has no unlinked video source element, cannot link pipeline. String = " << pipelineString;
            }
        }
        else
        {
            qCritical() << "Failed to create the pipeline";
        }
    }
}

void GStreamerPlayer::onBusMessage(const QGst::MessagePtr & message)
{
    switch (message->type()) {
    case QGst::MessageEos: //End of stream. We reached the end of the file.
        stop();
        break;
    case QGst::MessageError: //Some error occurred.
        qCritical() << message.staticCast<QGst::ErrorMessage>()->error();
        stop();
        break;
    case QGst::MessageStateChanged:
        if (message->source() == m_pipeline)
        {
            handlePipelineStateChange(message.staticCast<QGst::StateChangedMessage>());
        }
        break;
    default:
        break;
    }
}

void GStreamerPlayer::handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm)
{
    switch (scm->newState()) {
    case QGst::StatePlaying:
        m_playing = true;
        emit playingChanged(m_playing);
        m_paused = false;
        emit pausedChanged(m_paused);
        m_stopped = false;
        emit stoppedChanged(m_stopped);
        break;
    case QGst::StatePaused:
        m_playing = false;
        emit playingChanged(m_playing);
        m_paused = true;
        emit pausedChanged(m_paused);
        m_stopped = false;
        emit stoppedChanged(m_stopped);
        break;
    case QGst::StateNull:
        m_playing = false;
        emit playingChanged(m_playing);
        m_paused = false;
        emit pausedChanged(m_paused);
        m_stopped = true;
        emit stoppedChanged(m_stopped);
        break;
    }
}

void GStreamerPlayer::sendEOS()
{
    // Need this to save a running stream
    QGst::EosEventPtr eos = QGst::EosEvent::create();
    m_pipeline->sendEvent(eos);
}

void GStreamerPlayer::onStopTimer()
{
    m_stopTimer.stop();
    m_stopped = true;

    emit stoppedChanged(m_stopped);

    m_paused = false;
    emit pausedChanged(m_paused);
    m_playing = false;
    emit playingChanged(m_playing);

    this->stop();
}

#include "moc_GStreamerPlayer.cpp"
