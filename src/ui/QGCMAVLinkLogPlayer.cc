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
    m_isPlaying(false),
    m_sliderDown(false),
    m_mavlinkDecoder(NULL)
{
    ui->setupUi(this);
    ui->horizontalLayout->setAlignment(Qt::AlignTop);
    // Setup buttons
    connect(ui->selectFileButton, SIGNAL(clicked()), this, SLOT(loadLogButtonClicked()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    //connect(ui->speedSlider, SIGNAL(valueChanged(int)), this, SLOT(speedSliderValueChanged(int)));

    connect(ui->positionSlider,SIGNAL(sliderReleased()),this,SLOT(positionSliderReleased()));
    connect(ui->positionSlider,SIGNAL(sliderPressed()),this,SLOT(positionSliderPressed()));

    ui->positionSlider->setTracking(true);
    ui->playButton->setEnabled(true);
    //ui->speedSlider->setEnabled(true);
    //ui->speedLabel->setEnabled(false);
    ui->logStatsLabel->setEnabled(true);
    ui->playButton->setVisible(true);
    connect(ui->speedButton75,SIGNAL(clicked()),this,SLOT(speed75Clicked()));
    connect(ui->speedButton100,SIGNAL(clicked()),this,SLOT(speed100Clicked()));
    connect(ui->speedButton150,SIGNAL(clicked()),this,SLOT(speed150Clicked()));
    connect(ui->speedButton200,SIGNAL(clicked()),this,SLOT(speed200Clicked()));
    connect(ui->speedButton500,SIGNAL(clicked()),this,SLOT(speed500Clicked()));
    ui->speedButton75->setEnabled(false);
    ui->speedButton100->setEnabled(false);
    ui->speedButton150->setEnabled(false);
    ui->speedButton200->setEnabled(false);
    ui->speedButton500->setEnabled(false);
}
void QGCMAVLinkLogPlayer::speed75Clicked()
{
    m_logLink->setSpeed(75);
    ui->speedButton100->setChecked(false);
    ui->speedButton150->setChecked(false);
    ui->speedButton200->setChecked(false);
    ui->speedButton500->setChecked(false);
}

void QGCMAVLinkLogPlayer::speed100Clicked()
{
    ui->speedButton75->setChecked(false);
    m_logLink->setSpeed(100);
    ui->speedButton150->setChecked(false);
    ui->speedButton200->setChecked(false);
    ui->speedButton500->setChecked(false);
}

void QGCMAVLinkLogPlayer::speed150Clicked()
{
    ui->speedButton75->setChecked(false);
    ui->speedButton100->setChecked(false);
    m_logLink->setSpeed(150);
    ui->speedButton200->setChecked(false);
    ui->speedButton500->setChecked(false);
}

void QGCMAVLinkLogPlayer::speed200Clicked()
{
    ui->speedButton75->setChecked(false);
    ui->speedButton100->setChecked(false);
    ui->speedButton150->setChecked(false);
    m_logLink->setSpeed(200);
    ui->speedButton500->setChecked(false);
}

void QGCMAVLinkLogPlayer::speed500Clicked()
{
    ui->speedButton75->setChecked(false);
    ui->speedButton100->setChecked(false);
    ui->speedButton150->setChecked(false);
    ui->speedButton200->setChecked(false);
    m_logLink->setSpeed(500);
}

void QGCMAVLinkLogPlayer::positionSliderReleased()
{
    m_sliderDown = false;
    if (m_logLink)
    {
        m_logLink->setPosition(ui->positionSlider->value());

    }
}

void QGCMAVLinkLogPlayer::positionSliderPressed()
{
    //Deactivate signals here
    m_sliderDown = true;

}

QGCMAVLinkLogPlayer::~QGCMAVLinkLogPlayer()
{
    storeSettings();
    if (m_logLink)
    {
        if (m_logLink->isRunning())
        {
            m_logLink->stop();
            m_logLink->wait(1000);
            delete m_logLink;
        }
    }
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
                ui->speedButton75->setEnabled(false);
                ui->speedButton100->setEnabled(false);
                ui->speedButton150->setEnabled(false);
                ui->speedButton200->setEnabled(false);
                ui->speedButton500->setEnabled(false);
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
    m_logLink->connect();
    m_isPlaying = true;
    ui->logStatsLabel->setText(filename.mid(filename.lastIndexOf("/")+1));
    ui->playButton->setIcon(QIcon(":/files/images/actions/media-playback-stop.svg"));
    ui->speedButton75->setEnabled(true);
    ui->speedButton100->setEnabled(true);
    ui->speedButton150->setEnabled(true);
    ui->speedButton200->setEnabled(true);
    ui->speedButton500->setEnabled(true);
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
                ui->speedButton75->setEnabled(false);
                ui->speedButton100->setEnabled(false);
                ui->speedButton150->setEnabled(false);
                ui->speedButton200->setEnabled(false);
                ui->speedButton500->setEnabled(false);
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
    m_logLink = new TLogReplayLink(this);
    m_logLink->setMavlinkDecoder(m_mavlinkDecoder);
    m_logLink->setMavlinkInspector(m_mavlinkInspector);
    connect(m_logLink,SIGNAL(logProgress(qint64,qint64)),this,SLOT(logProgress(qint64,qint64)));
    connect(m_logLink,SIGNAL(finished()),this,SLOT(logLinkTerminated()));


    m_logLink->setLog(fileName);
    connect(m_logLink,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlink,SLOT(receiveBytes(LinkInterface*,QByteArray)));
    //connect(m_logLink,SIGNAL(terminated()),this,SLOT(logLinkTerminated()));
    m_logLink->connect();
    ui->logStatsLabel->setText(fileName.mid(fileName.lastIndexOf("/")+1));
    ui->playButton->setIcon(QIcon(":/files/images/actions/media-playback-stop.svg"));
    ui->speedButton75->setEnabled(true);
    ui->speedButton100->setEnabled(true);
    ui->speedButton150->setEnabled(true);
    ui->speedButton200->setEnabled(true);
    ui->speedButton500->setEnabled(true);
}
void QGCMAVLinkLogPlayer::logProgress(qint64 pos,qint64 total)
{
    //ui->positionSlider->setValue(((double)pos / (double)total) * 100);
    if (!m_sliderDown)
    {
        //ui->positionProgressBar->setValue(((double)pos / (double)total) * 100);
        ui->positionLabel->setText(QString::number(pos) + "/" + QString::number(total));
        ui->positionSlider->setValue(((double)pos / (double)total) * 100);
    }
}
void QGCMAVLinkLogPlayer::setMavlinkDecoder(MAVLinkDecoder *decoder)
{
    m_mavlinkDecoder = decoder;
}
void QGCMAVLinkLogPlayer::setMavlinkInspector(QGCMAVLinkInspector *inspector)
{
    m_mavlinkInspector = inspector;
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
        ui->speedButton75->setEnabled(false);
        ui->speedButton100->setEnabled(false);
        ui->speedButton150->setEnabled(false);
        ui->speedButton200->setEnabled(false);
        ui->speedButton500->setEnabled(false);
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
        double newval = ((value / 100.0) * 130) + 70;
        m_logLink->setSpeed(newval);
        //ui->speedLabel->setText(QString::number(newval) + "%");
    }
}
