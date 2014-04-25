#ifndef QGCMAVLINKLOGPLAYER_H
#define QGCMAVLINKLOGPLAYER_H

#include <QWidget>
#include <QFile>
#include "MAVLinkProtocol.h"
#include "TLogReplayLink.h"

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
    explicit QGCMAVLinkLogPlayer(MAVLinkProtocol* mavlink, QWidget *parent = 0);
    ~QGCMAVLinkLogPlayer();
    void loadLog(QString filename);
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
private slots:
    void logProgress(qint64 pos,qint64 total);
protected:
    bool m_isPlaying;
    void changeEvent(QEvent *e);

    void storeSettings();

private:
    MAVLinkProtocol *m_mavlink;
    Ui::QGCMAVLinkLogPlayer *ui;
    TLogReplayLink *m_logLink;
    bool m_logLoaded;
signals:
    void logFinished();
};

#endif // QGCMAVLINKLOGPLAYER_H
