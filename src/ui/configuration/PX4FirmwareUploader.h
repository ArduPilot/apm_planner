#ifndef PX4FIRMWAREUPLOADER_H
#define PX4FIRMWAREUPLOADER_H

#include <QThread>
#include <qserialport.h>
#include <QFile>
#include <QTemporaryFile>
#include <QDebug>
//#include <qjson/parser.h>
#include <QStringList>
class PX4FirmwareUploader : public QThread
{
    Q_OBJECT
public:
    explicit PX4FirmwareUploader(QObject *parent = 0);
    bool loadFile(QString file);
    void stop();
protected:
    void run();
private:
    bool m_stop;
    QSerialPort *m_port;
    QByteArray m_serialBuffer;
    int get_sync(int timeout=1000);
    bool reqInfo(unsigned char infobyte,unsigned int *reply);
    int readBytes(int num,int timeout,QByteArray &buf);
    unsigned int m_loadedBoardID;
    unsigned int m_loadedFwSize;
    QString m_loadedDescription;
    QTemporaryFile *tempFile;
    QTemporaryFile *tempJsonFile;
signals:
    void requestDevicePlug();
    void devicePlugDetected();
    void done();
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
public slots:

};

#endif // PX4FIRMWAREUPLOADER_H
