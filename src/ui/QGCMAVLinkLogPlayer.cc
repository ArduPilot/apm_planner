#include "QsLog.h"
#include "MainWindow.h"
#include "SerialLink.h"
#include "QGCMAVLinkLogPlayer.h"
#include "QGC.h"
#include "ui_QGCMAVLinkLogPlayer.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>

QGCMAVLinkLogPlayer::QGCMAVLinkLogPlayer(MAVLinkProtocol* mavlink, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCMAVLinkLogPlayer),
    m_logLink(NULL),
    m_mavlink(mavlink),
    m_logLoaded(false),
    m_isPlaying(false)
{
    ui->setupUi(this);
    ui->horizontalLayout->setAlignment(Qt::AlignTop);
    // Setup buttons
    connect(ui->selectFileButton, SIGNAL(clicked()), this, SLOT(loadLogButtonClicked()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedSliderValueChanged(int)));
    //connect(ui->positionSlider, SIGNAL(valueChanged(int)), this, SLOT(jumpToSliderVal(int)));
    //connect(ui->positionSlider, SIGNAL(sliderPressed()), this, SLOT(pause()));

    //setAccelerationFactorInt(49);
    //ui->positionSlider->setValue(ui->positionSlider->minimum());

    ui->playButton->setEnabled(true);
    ui->speedSlider->setEnabled(true);
    //ui->positionSlider->setEnabled(true);
    ui->speedLabel->setEnabled(false);
    ui->logStatsLabel->setEnabled(true);
    ui->playButton->setVisible(true);



}

QGCMAVLinkLogPlayer::~QGCMAVLinkLogPlayer()
{
    storeSettings();
    delete ui;
}
void QGCMAVLinkLogPlayer::storeSettings()
{
    // Nothing to store
}
void QGCMAVLinkLogPlayer::loadLog(QString filename)
{
    if (m_logLoaded)
    {
        if (m_logLink)
        {
            //Stop the mavlink, schedule for deletion
            if (m_logLink->isRunning())
            {
                m_logLink->stop();
            }
            else
            {
                m_logLink->deleteLater();
                m_logLink = 0;
                m_logLoaded = false;
            }
        }
        else
        {
            m_logLoaded = false;
        }
        return;
    }
    m_logLoaded = true;
    m_mavlink->throwAwayGCSPackets(true);
    m_logLink = new TLogReplayLink();
    connect(m_logLink,SIGNAL(logProgress(qint64,qint64)),this,SLOT(logProgress(qint64,qint64)));
    connect(m_logLink,SIGNAL(finished()),this,SLOT(logLinkTerminated()));


    m_logLink->setLog(filename);
    connect(m_logLink,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlink,SLOT(receiveBytes(LinkInterface*,QByteArray)));
   // connect(m_logLink,SIGNAL(terminated()),this,SLOT(logLinkTerminated()));
    m_logLink->connect();
    m_isPlaying = true;
    ui->logStatsLabel->setText(filename.mid(filename.lastIndexOf("/")+1));
    ui->playButton->setIcon(QIcon(":/files/images/actions/media-playback-stop.svg"));
}

void QGCMAVLinkLogPlayer::loadLogButtonClicked()
{
    if (m_logLoaded)
    {
        if (m_logLink)
        {
            //Stop the mavlink, schedule for deletion
            if (m_logLink->isRunning())
            {
                m_logLink->stop();
            }
            else
            {
                m_logLink->deleteLater();
                m_logLink = 0;
                m_logLoaded = false;
            }
        }
        else
        {
            m_logLoaded = false;
        }
        return;
    }


    QString fileName = QFileDialog::getOpenFileName(this, tr("Specify MAVLink log file name to replay"), QGC::MAVLinkLogDirectory(), tr("MAVLink Telemetry log (*.tlog)"));
    if (fileName == "")
    {
        //No file selected/cancel clicked
        return;
    }
    m_logLoaded = true;
    m_mavlink->throwAwayGCSPackets(true);
    m_logLink = new TLogReplayLink();
    connect(m_logLink,SIGNAL(logProgress(qint64,qint64)),this,SLOT(logProgress(qint64,qint64)));
    connect(m_logLink,SIGNAL(finished()),this,SLOT(logLinkTerminated()));


    m_logLink->setLog(fileName);
    connect(m_logLink,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlink,SLOT(receiveBytes(LinkInterface*,QByteArray)));
    //connect(m_logLink,SIGNAL(terminated()),this,SLOT(logLinkTerminated()));
    m_logLink->connect();
    ui->logStatsLabel->setText(fileName.mid(fileName.lastIndexOf("/")+1));
    ui->playButton->setIcon(QIcon(":/files/images/actions/media-playback-stop.svg"));
}
void QGCMAVLinkLogPlayer::logProgress(qint64 pos,qint64 total)
{
    //ui->positionSlider->setValue(((double)pos / (double)total) * 100);
    ui->positionProgressBar->setValue(((double)pos / (double)total) * 100);
}

void QGCMAVLinkLogPlayer::playButtonClicked()
{
    if (m_logLink)
    {
        if (m_logLink->isPaused())
        {
            m_logLink->play();
            ui->playButton->setIcon(QIcon(":/files/images/actions/media-playback-stop.svg"));
        }
        else
        {
            m_logLink->pause();
            ui->playButton->setIcon(QIcon(":/files/images/actions/media-playback-start.svg"));
        }
    }
}
void QGCMAVLinkLogPlayer::logLinkTerminated()
{
    m_isPlaying = false;
    if (m_logLink->toBeDeleted())
    {
        //Log loop has terminated with the intention of unloading the sim link
        m_logLink->deleteLater();
        m_logLink = 0;
        m_logLoaded = false;
        m_mavlink->throwAwayGCSPackets(false);
        emit logFinished();
    }
}

void QGCMAVLinkLogPlayer::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void QGCMAVLinkLogPlayer::speedSliderValueChanged(int value)
{
    if (m_logLink)
    {
        m_logLink->setSpeed(value);
    }
}
