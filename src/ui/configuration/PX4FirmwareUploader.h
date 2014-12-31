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
    explicit PX4FirmwareUploader(QObject *parent = 0);
    enum State
    {
        INIT,
        REQ_DEVICE_INFO,
        REQ_OTP,
        REQ_SN,
        SEND_FW,
        REQ_CHECKSUM
    };
    void stop();
    void loadFile(QString filename);

private:
    bool checkCOA(const QByteArray& serial, const QByteArray& signature, const QString& publicKey);

private:
    QList<QString> m_portlist;
    QString m_portToUse;
    QSerialPort *m_port;

    bool m_waitingForSync;

    QList<unsigned char> m_devInfoList;
    QTimer *m_checkTimer;
    State m_currentState;

    bool getSync();
    bool verifyOtp();

    bool reqNextOtpAddress();
    void getOtpAddress(int address);
    bool readOtp();
    int m_currentOtpAddress;
    QByteArray m_otpBytes;

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
    QTimer *m_eraseTimeoutTimer;
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
    void OTP(QString otp);
    void boardRev(int rev);
    void boardId(int id);
    void bootloaderRev(int rev);
    void flashSize(int size);
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
