#ifndef QGCMAVLINKLOGPLAYER_H
#define QGCMAVLINKLOGPLAYER_H

#include <QWidget>
#include <QFile>
#include "MAVLinkProtocol.h"
#include "TLogReplyLink.h"

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
/*    bool isPlayingLogFile()
    {
        return isPlaying;
    }

    bool isLogFileSelected()
    {
        return logFile.isOpen();
    }
*/
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
    /*int lineCounter;
    int totalLines;
    quint64 startTime;
    quint64 endTime;
    quint64 currentStartTime;
    float accelerationFactor;
    MAVLinkProtocol* mavlink;
    MAVLinkSimulationLink* logLink;
    QFile logFile;
    QTimer loopTimer;
    int loopCounter;
    bool mavlinkLogFormat;
    int binaryBaudRate;
    bool isPlaying;
    unsigned int currPacketCount;
    static const int packetLen = MAVLINK_MAX_PACKET_LEN;
    static const int timeLen = sizeof(quint64);
    QString lastLogDirectory;
    */
    void changeEvent(QEvent *e);

    void storeSettings();

private:
    MAVLinkProtocol *m_mavlink;
    Ui::QGCMAVLinkLogPlayer *ui;
    TLogReplyLink *m_logLink;
    bool m_logLoaded;
};

#endif // QGCMAVLINKLOGPLAYER_H
