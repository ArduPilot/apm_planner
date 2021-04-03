#ifndef PX4FIRMWAREUPLOADER_H
#define PX4FIRMWAREUPLOADER_H

#include <QThread>
#include <QSerialPort>
#include <QTimer>
#include <QTemporaryFile>
class PX4FirmwareUploader : public QThread
{
    Q_OBJECT
public:
    explicit PX4FirmwareUploader(QObject *parent = nullptr);
    enum State
    {
        INIT,
        REQ_DEVICE_INFO,
        REQ_SN,
        SEND_FW,
        REQ_CHECKSUM
    };
    void stop();
    void loadFile(QString filename);

private:
    QList<QString> m_portlist;
    QString m_portToUse;
    QScopedPointer<QSerialPort, QScopedPointerDeleteLater> mp_port;

    bool m_waitingForSync;

    QList<unsigned char> m_devInfoList;
    QScopedPointer<QTimer, QScopedPointerDeleteLater> mp_checkTimer;
    State m_currentState;

    bool getSync();

    bool reqNextSNAddress();
    void getSNAddress(int address);
    bool readSN();
    int m_currentSNAddress;
    QByteArray m_snBytes;
    int m_fwBytesCounter;

    bool reqNextDeviceInfo();
    void getDeviceInfo(unsigned char infobyte);
    bool readDeviceInfo();
    int m_waitingDeviceInfoVar;

    bool sendNextFwBytes();

    void reqReboot();

    void reqChecksum();
    bool readChecksum();
    quint32 m_checksum;
    quint32 m_localChecksum;
    int m_flashSize;
    QByteArray m_toflashbytes;



    void reqErase();
    QScopedPointer<QTimer, QScopedPointerDeleteLater> mp_eraseTimeoutTimer;
    int m_eraseTimerCounter;


    void reqFlash();
    QTemporaryFile *tempFile;



signals:
    void kickOff();
    void gotDeviceInfo(unsigned char devinfo,unsigned int reply);


    void requestDevicePlug();
    void devicePlugDetected();
    void complete();
    void serialNumber(QString sn);
    void boardRev(int rev);
    void boardId(int id);
    void bootloaderRev(int rev);
    void flashSize(int size);
    void startFlashing();
    void flashProgress(qint64 current,qint64 total);
    void error(QString error);
    void statusUpdate(QString status);
    void debugUpdate(QString debug);
    void warning(QString message);



public slots:
private slots:
    void kickOffTriggered();
    void portReadyRead();
    void checkForPort();
    void eraseSyncCheck();

};

#endif // PX4FIRMWAREUPLOADER_H
