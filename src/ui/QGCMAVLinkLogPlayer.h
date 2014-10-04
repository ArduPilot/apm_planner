#ifndef QGCMAVLINKLOGPLAYER_H
#define QGCMAVLINKLOGPLAYER_H

#include <QWidget>
#include <QFile>
#include "MAVLinkProtocol.h"
#include "TLogReplayLink.h"
#include "MAVLinkDecoder.h"
#include "QGCMAVLinkInspector.h"
namespace Ui
{
class QGCMAVLinkLogPlayer;
}

/**
 * @brief Replays MAVLink log files
 *
 * This class allows to replay MAVLink logs at varying speeds.
 * captured flights can be replayed, shown to others and analyzed
 * in-depth later on.
 */
class QGCMAVLinkLogPlayer : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMAVLinkLogPlayer(QWidget *parent = 0);
    void setMavlinkDecoder(MAVLinkDecoder *decoder);
    void setMavlinkInspector(QGCMAVLinkInspector *inspector);
    ~QGCMAVLinkLogPlayer();
    bool isPlayingLogFile()
    {
        return m_isPlaying;
    }

    /**
     * @brief Set the last log file name
     * @param filename
     */
/*    void setLastLogFile(const QString& filename) {
        lastLogDirectory = filename;
    }*/

public slots:
    void loadLogButtonClicked();
    void playButtonClicked();
    void logLinkTerminated();
    void speedSliderValueChanged(int value);
    void speed75Clicked();
    void speed100Clicked();
    void speed150Clicked();
    void speed200Clicked();
    void speed500Clicked();
    void speed1000Clicked();
private slots:
    void logProgress(qint64 pos,qint64 total);
    void positionSliderReleased();
    void positionSliderPressed();
    void loadLogDialogAccepted();
protected:
    bool m_sliderDown;
    bool m_isPlaying;
    void changeEvent(QEvent *e);

    void storeSettings();

private:
    Ui::QGCMAVLinkLogPlayer *ui;
    TLogReplayLink *m_logLink;
    bool m_logLoaded;
    MAVLinkDecoder *m_mavlinkDecoder;
    QGCMAVLinkInspector *m_mavlinkInspector;
signals:
    void logFinished();
    void logLoaded();
};

#endif // QGCMAVLINKLOGPLAYER_H
