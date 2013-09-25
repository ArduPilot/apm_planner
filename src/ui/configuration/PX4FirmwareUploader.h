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
protected:
    void run();
private:
    QSerialPort *port;
    int get_sync(int timeout=1000);
    bool reqInfo(unsigned char infobyte,unsigned int *reply);
    int readBytes(int num,int timeout,QByteArray &buf);
    unsigned int m_loadedBoardID;
    unsigned int m_loadedFwSize;
    QString m_loadedDescription;
    QTemporaryFile *tempFile;
signals:
    void requestDevicePlug();
    void done();
    void serialNumber(QString sn);
    void OTP(QString otp);
    void boardRev(int rev);
    void boardId(int id);
    void bootloaderRev(int rev);
    void flashSize(int size);
    void flashProgress(int current,int total);
public slots:

};

#endif // PX4FIRMWAREUPLOADER_H
