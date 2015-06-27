#include "PX4FirmwareUploader.h"
#include <QSerialPort>
#include <QDebug>
#include <QSerialPortInfo>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QApplication>
#ifndef Q_OS_WIN
#include <openssl/ssl.h>
#endif
#include "QsLog.h"

#define CERT_OF_A_FAILED "Certificate of Authenticity check failed! Please check with your autopilot hardware supplier for support."
#define CERT_OF_A_PUB_KEY_FAILED "Certificate of Authenticity failed COA check! Public Key is not valid."

#define PROTO_OK 0x10
#define PROTO_GET_DEVICE 0x22
#define PROTO_EOC 0x20
#define PROTO_DEVICE_BL_REV 0x01
#define PROTO_DEVICE_BOARD_ID 0x02
#define PROTO_DEVICE_BOARD_REV 0x03
#define PROTO_DEVICE_FW_SIZE 0x04
#define PROTO_DEVICE_VEC_AREA 0x05

static const quint32 crctab[] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

static quint32 crc32(const QByteArray src)
{
    quint32 state = 0;
    for (int i = 0; i < src.size(); i++) //Limited to half of 32bits, since QByteArray::size() is an integer. Shouldn't ever be larger anyway
    {
        state = crctab[(state ^ static_cast<unsigned char>(src[i])) & 0xff] ^ (state >> 8);
    }
    return state;
}

PX4FirmwareUploader::PX4FirmwareUploader(QObject *parent) :
    QThread(parent),
    m_port(NULL),
    m_waitingForSync(false),
    m_checkTimer(NULL),
    m_currentOtpAddress(0),
    m_currentSNAddress(0),
    m_eraseTimeoutTimer(NULL)
{
}

void PX4FirmwareUploader::loadFile(QString filename)
{
    foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        m_portlist.append(info.portName());
    }
    connect(this,SIGNAL(kickOff()),this,SLOT(kickOffTriggered()));
    m_checkTimer = new QTimer(this);
    connect(m_checkTimer,SIGNAL(timeout()),this,SLOT(checkForPort()));
    m_checkTimer->start(250);

    m_waitingForSync = false;

    QFile json(filename);
    json.open(QIODevice::ReadOnly);
    QByteArray jsonbytes = json.readAll();
    QString jsonstring(jsonbytes);
    json.close();


    //This chunk of code is from QUpgrade
    //Available https://github.com/LorenzMeier/qupgrade
    // BOARD ID
    QStringList decode_list = jsonstring.split("\"board_id\":");
    decode_list = decode_list.last().split(",");
    if (decode_list.count() < 1)
    {
        QLOG_INFO() << "Error parsing BOARD ID from .px4 file";
        return;
    }
    QString board_id = QString(decode_list.first().toUtf8()).trimmed();
    //int m_loadedBoardID = board_id.toInt();

    // IMAGE SIZE
    decode_list = jsonstring.split("\"image_size\":");
    decode_list = decode_list.last().split(",");
    if (decode_list.count() < 1)
    {
        QLOG_INFO() << "Error parsing IMAGE SIZE from .px4 file";
        return ;
    }
    QString image_size = QString(decode_list.first().toUtf8()).trimmed();
    int m_loadedFwSize = image_size.toInt();

    // DESCRIPTION
    decode_list = jsonstring.split("\"description\": \"");
    decode_list = decode_list.last().split("\"");
    if (decode_list.count() < 1)
    {
        QLOG_INFO() << "Error parsing DESCRIPTION from .px4 file";
        return ;
    }
    QString m_loadedDescription = QString(decode_list.first().toUtf8()).trimmed();
    QStringList list = jsonstring.split("\"image\": \"");
    list = list.last().split("\"");

    //Create the "image" to be written to the board
    //from loaded data

    QByteArray fwimage;

    fwimage.append((unsigned char)((m_loadedFwSize >> 24) & 0xFF));
    fwimage.append((unsigned char)((m_loadedFwSize >> 16) & 0xFF));
    fwimage.append((unsigned char)((m_loadedFwSize >> 8) & 0xFF));
    fwimage.append((unsigned char)((m_loadedFwSize >> 0) & 0xFF));

    QByteArray raw64 = list.first().toUtf8();

    fwimage.append(QByteArray::fromBase64(raw64));
    QByteArray uncompressed = qUncompress(fwimage);

    QLOG_INFO() << "Firmware size:" << uncompressed.size() << "expected" << m_loadedFwSize << "bytes";
    if (uncompressed.size() != m_loadedFwSize)
    {
        QLOG_INFO() << "Error in decompressing firmware. Please re-download and try again";
        return;
    }
    //Per QUpgrade, pad it to a 4 byte multiple.
    while ((uncompressed.count() % 4) != 0)
    {
        uncompressed.append((char)0xFF);
    }
    m_localChecksum = crc32(uncompressed);
    tempFile = new QTemporaryFile();
    tempFile->open();
    tempFile->write(uncompressed);
    tempFile->close();

    emit requestDevicePlug();
}

void PX4FirmwareUploader::stop()
{
    //Stop has been requested, close out the port, kill the thread.
    if (m_port)
    {
        if (m_checkTimer)
        {
            m_checkTimer->stop();
            delete m_checkTimer;
            m_checkTimer = 0;
        }
        if (m_eraseTimeoutTimer)
        {
            m_eraseTimeoutTimer->stop();
            delete m_eraseTimeoutTimer;
            m_eraseTimeoutTimer = 0;
        }
        m_port->close();
        delete m_port;
        m_port = 0;
    }
}

void PX4FirmwareUploader::reqChecksum()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called reqChecksum with a null port!";
        return;
    }
    m_currentState = REQ_CHECKSUM;
    m_port->write(QByteArray().append(0x29).append(0x20));
}
bool PX4FirmwareUploader::readChecksum()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called readChecksum with a null port!";
        return false;
    }
    m_checksum = 0;
    if (m_port->bytesAvailable() >= 4)
    {
        QByteArray infobuf = m_port->read(4);
        QLOG_INFO() << "Got Checksum Bytes:" << infobuf.toHex();
        m_checksum = 0;
        m_checksum += static_cast<unsigned char>(infobuf[0]);
        m_checksum += static_cast<unsigned char>(infobuf[1]) << 8;
        m_checksum += static_cast<unsigned char>(infobuf[2]) << 16;
        m_checksum += static_cast<unsigned char>(infobuf[3]) << 24;
        for (int i=m_toflashbytes.size();i<m_flashSize;i++)
        {
            m_toflashbytes.append((char)0xFF);
        }
        m_localChecksum = crc32(m_toflashbytes);
        return true;
    }
    return false;
}

void PX4FirmwareUploader::checkForPort()
{
    foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        if (!m_portlist.contains(info.portName()))
        {
#ifdef Q_OS_LINUX
            //Needed to weird issue where ports reorder and re-appear in linux.
            if (info.portName().contains("ttyACM"))
            {
#endif
            m_portToUse = info.portName();
            //Found a port!
            QLOG_INFO() << "Port found!" << m_portToUse;
            m_devInfoList.append(PROTO_DEVICE_BL_REV);
            m_devInfoList.append(PROTO_DEVICE_BOARD_ID);
            m_devInfoList.append(PROTO_DEVICE_BOARD_REV);
            m_devInfoList.append(PROTO_DEVICE_FW_SIZE);
            emit devicePlugDetected();
            emit kickOff();
            m_checkTimer->stop();
            m_checkTimer->deleteLater();
            m_checkTimer = 0;
            break;
#ifdef Q_OS_LINUX
            }
            else
            {
                QLOG_INFO() << "Invalid port found:" << info.portName();
                continue;
            }
#endif
        }
    }
    m_portlist.clear();
    foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        m_portlist.append(info.portName());
    }
}
void PX4FirmwareUploader::kickOffTriggered()
{
    if (m_port)
    {
        m_port->close();
        delete m_port;
        m_port = NULL;
    }
    m_waitingForSync = false;
    m_currentState = INIT;
    m_port = new QSerialPort();
    connect(m_port,SIGNAL(readyRead()),this,SLOT(portReadyRead()));

#ifdef Q_OS_MACX
    // temp fix Qt5.4.1 issue on OSX
    // http://code.qt.io/cgit/qt/qtserialport.git/commit/?id=687dfa9312c1ef4894c32a1966b8ac968110b71e
    m_port->setPortName("/dev/cu." + m_portToUse);
#else
    m_port->setPortName(m_portToUse);
#endif

    if (!m_port->open(QIODevice::ReadWrite))
    {
        QLOG_ERROR() << "Unable to open port:" << m_port->errorString();
    }
    m_port->write(QByteArray().append(0x21).append(0x20));

}
void PX4FirmwareUploader::getDeviceInfo(unsigned char infobyte)
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called getDeviceInfo with a null port!";
        return;
    }
    m_waitingDeviceInfoVar = infobyte;
    m_port->write(QByteArray().append(PROTO_GET_DEVICE).append(infobyte).append(PROTO_EOC));
}

void PX4FirmwareUploader::reqErase()
{
    QLOG_INFO() << "Requesting erase";
    m_eraseTimeoutTimer = new QTimer(this);
    m_eraseTimerCounter = 0;
    connect(m_eraseTimeoutTimer,SIGNAL(timeout()),this,SLOT(eraseSyncCheck()));
    m_port->write(QByteArray().append(0x23).append(0x20));
    m_eraseTimeoutTimer->start(250);
}
void PX4FirmwareUploader::eraseSyncCheck()
{
    if (getSync())
    {
        //Done erasing
        QLOG_INFO() << "Erase complete";
        emit statusUpdate("Erase Complete");
        m_eraseTimeoutTimer->stop();
        m_eraseTimeoutTimer->deleteLater(); //Can't do a direct delete since we're in its slot
        m_eraseTimeoutTimer = 0;
        reqFlash();
    }
    else
    {
        m_eraseTimerCounter++;
        if (m_eraseTimerCounter > 240) // 60 seconds
        {
            m_eraseTimeoutTimer->stop();
            m_eraseTimeoutTimer->deleteLater(); //Can't do a direct delete since we're in its slot
            m_eraseTimeoutTimer = 0;
            //Emit error here
            QLOG_INFO() << "Error flashing, never returned from erase";
        }
    }
}
void PX4FirmwareUploader::reqReboot()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called reqReboot with a null port!";
        return;
    }
    m_port->write(QByteArray().append(0x30).append(0x20));
    m_port->flush();
}
void PX4FirmwareUploader::reqFlash()
{
    QLOG_INFO() << "Flash requested, flashing firmware";
    emit statusUpdate("Flashing Firmware");
    tempFile->open();
    m_currentState = SEND_FW;
    m_waitingForSync = true;
    m_fwBytesCounter = 0;
    sendNextFwBytes();
}
bool PX4FirmwareUploader::sendNextFwBytes()
{
    if (!tempFile)
    {
        QLOG_ERROR() << "Called sendNextFwBytes with no firmware file!";
        return false;
    }
    if (!m_port)
    {
        QLOG_ERROR() << "Called sendNextFwBytes with a null port!";
        return false;
    }
    if (tempFile->atEnd())
    {
        tempFile->close();
        delete tempFile;
        tempFile = NULL;
        return false;
    }
    if (m_fwBytesCounter++ % 50 == 0)
    {
        emit flashProgress(tempFile->pos(),tempFile->size());
        QLOG_INFO() << "flashing:" << tempFile->pos() << "/" << tempFile->size();
    }
    QByteArray bytes = tempFile->read(60);
    m_toflashbytes.append(bytes);
    QByteArray tosend;
    tosend.append(0x27);
    tosend.append(bytes.size());
    tosend.append(bytes);
    tosend.append(0x20);
    m_port->write(tosend);
    return true;
}

void PX4FirmwareUploader::getSNAddress(int address)
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called getSNAddress with a null port!";
        return;
    }
    m_port->write(QByteArray().append(0x2B).append(address).append((char)0).append((char)0).append((char)0).append(PROTO_EOC));
}
bool PX4FirmwareUploader::reqNextSNAddress()
{
    if (m_currentSNAddress >= 12)
    {
        //All SN is complete
        return false;
    }
    getSNAddress(m_currentSNAddress);
    m_currentSNAddress+=4;
    return true;
}
bool PX4FirmwareUploader::readSN()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called readSN with a null port!";
        return false;
    }
    if (m_port->bytesAvailable() >= 4)
    {
        QByteArray infobuf = m_port->read(4);
        m_snBytes.append(infobuf[3]);
        m_snBytes.append(infobuf[2]);
        m_snBytes.append(infobuf[1]);
        m_snBytes.append(infobuf[0]);
        return true;
    }
    return false;
}

void PX4FirmwareUploader::getOtpAddress(int address)
{
    m_port->write(QByteArray().append(0x2A).append(address & 0xFF).append(((address >> 8) & 0xFF)).append((char)0).append((char)0));//.append(PROTO_EOC));
}

bool PX4FirmwareUploader::reqNextOtpAddress()
{
    if (m_currentOtpAddress >= 512)
    {
        //All OTP is complete
        return false;
    }
    //qDebug() << m_currentOtpAddress << "OTP";
    getOtpAddress(m_currentOtpAddress);
    m_currentOtpAddress+=4;
    return true;

}

bool PX4FirmwareUploader::reqNextDeviceInfo()
{
    if (m_devInfoList.size() == 0)
    {
        return false;
    }

    unsigned char val = m_devInfoList.at(0);
    m_devInfoList.removeAt(0);
    getDeviceInfo(val);
    return true;
}
bool PX4FirmwareUploader::getSync()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called getSync with a null port!";
        return false;
    }
    if (m_port->bytesAvailable() == 2)
    {
        QByteArray infobuf = m_port->readAll();
        if (infobuf[0] != (char)0x12  || infobuf[1] != (char)0x10)
        {
            QLOG_INFO() << "Bad sync return:" << QString::number(infobuf[0],16) << QString::number(infobuf[1],16);
            return false;
        }
        else
        {
            return true;
        }
    }
    return false;
}
bool PX4FirmwareUploader::readOtp()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called readOtp with a null port!";
        return false;
    }
    if (m_port->bytesAvailable() >= 4)
    {
        QByteArray infobuf = m_port->read(4);
        m_otpBytes.append(infobuf);
        return true;
    }
    return false;
}

bool PX4FirmwareUploader::readDeviceInfo()
{
    if (!m_port)
    {
        QLOG_ERROR() << "Called readDeviceInfo with a null port!";
        return false;
    }
    if (m_port->bytesAvailable() >= 4)
    {
        QByteArray infobuf = m_port->read(4);
        unsigned int reply = ((unsigned char)infobuf[0]) + ((unsigned char)infobuf[1] << 8) + ((unsigned char)infobuf[2] << 16) + ((unsigned char)infobuf[3] << 24);
        emit gotDeviceInfo(m_waitingDeviceInfoVar,reply);
        switch (m_waitingDeviceInfoVar)
        {
            case PROTO_DEVICE_BOARD_ID:
            {
                emit statusUpdate("Requesting Board ID");
                emit boardId(reply);
            }
                break;
            case PROTO_DEVICE_BOARD_REV:
            {
                emit statusUpdate("Requesting Board Rev");
                emit boardRev(reply);
            }
                break;
            case PROTO_DEVICE_FW_SIZE:
            {
                emit statusUpdate("Requesting FW Size");
        emit flashSize(reply);
                m_flashSize = reply;
            }
                break;
            case PROTO_DEVICE_VEC_AREA:
            {
                emit statusUpdate("Requesting Vec Area");
            }
            break;
        }
        return true;
    }
    return false;
}
bool PX4FirmwareUploader::verifyOtp()
{
    QString SIGarg = "";
    QByteArray signature;
    for (int i=32;i<(128+32);i++)
    {
        signature.append(m_otpBytes[i]);
        SIGarg += ((unsigned char)m_otpBytes[i] <= (char)0xF ? "0" : "") + QString::number((unsigned char)m_otpBytes[i],16).toUpper();
    }
    QByteArray serial;
    QString SNarg = "";
    for (int i=0;i<12;i++)
    {
        serial.append(m_snBytes[i]);
        SNarg += ((unsigned char)m_snBytes[i] <= (char)0xF ? "0" : "") + QString::number((unsigned char)m_snBytes[i],16).toUpper();
    }
    QLOG_DEBUG() << "Verifying OTP";
    QLOG_DEBUG() << "Signature:" << signature.toHex();
    QLOG_DEBUG() << "Serial:" << serial.toHex();

#ifdef Q_OS_WIN
    QProcess *proc = new QProcess();
    QLOG_DEBUG() << "Attempting to start" << QApplication::instance()->applicationDirPath() + "/uploader/AP2OTPCheck.exe" << SNarg << SIGarg;
    proc->start(QApplication::instance()->applicationDirPath() + "/uploader/AP2OTPCheck.exe",QStringList() << SNarg << SIGarg);
    proc->waitForStarted();
    proc->waitForFinished();
    QString result = proc->readAll() + " " + proc->readAllStandardError() + " " + proc->readAllStandardOutput();
    QLOG_DEBUG() << "Proc finished with:" << result;
    delete proc;
    if (!result.contains("Valid Key"))
    {
        QLOG_WARN() << CERT_OF_A_FAILED;
        emit statusUpdate(CERT_OF_A_FAILED);
        emit warning(CERT_OF_A_FAILED);
        return false;
    }
    else
    {
        QLOG_DEBUG() << "COA verification successful";
        emit statusUpdate("COA verification successful");
        return true;
    }
#else
    // [TODO] Need to read XML file of list of authorized keys.

    // 3DR COA Public Key
    QStringList publicKeyList;
    // 3DR Public Key 1
    publicKeyList << "\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDqi8E6EdZ11iE7nAc95bjdUTwd\r\n/gLetSAAx8X9jgjInz5j47DIcDqFVFKEFZWiAc3AxJE/fNrPQey16SfI0FyDAX/U\t\n4jyGIv9w+M1dKgUPI8UdpEMS2w1YnfzW0GO3PX0SBL6pctEIdXr0NGsFFaqU9Yz4\r\nDbgBdR6wBz9qdfRRoQIDAQAB";
    // 3DR Public Key 2
    publicKeyList << "\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQCei8vGvc+jPXfbAAUkiu7o9fiQ\r\nhQ6/xdJcrmsJqa2/Onf4xDk6mMoZaNXNaNsEmE/xdeYgLqbPoivCba7A0YiGzonp\r\nOiEZlfUKJPzXvGSNrKbIDsOrvhPQpFwBEU+hO5usHoWCO5VzN5+wKTpGVZ300ny4\r\nNHbIGjZUF/RUz84lVwIDAQAB";
    // Arsov RC Technology USA
    publicKeyList << "\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDJQfXiYHrYIHGadPwDPkUuHnQG\r\nubN4x0UAyvhP1VGzAk/CNYkcSCEKVGbi+Ro3l60xpoiOIR5HfHtzGgzdl+oBu55t\r\n2xjvbJbGIPKkA8pBU10Nj2dg/vjNphWMQpSw2yRdHuqvnrGjOq2fLduS0ITp11ST\r\nRX6cRfeDDXsVkL+XzwIDAQAB";

    bool checkCOAsuccess = false;
    foreach(QString publicKey, publicKeyList){
        checkCOAsuccess = checkCOA(serial, signature, publicKey);
        if (checkCOAsuccess){
            break;
        }
    }

    if (checkCOAsuccess){
        emit statusUpdate("COA Verification Success");
    } else {
        emit statusUpdate(CERT_OF_A_FAILED);
        emit error(CERT_OF_A_FAILED);
    }

    return checkCOAsuccess;
#endif //Q_OS_WIN
}

bool PX4FirmwareUploader::checkCOA(const QByteArray& serial, const QByteArray& signature, const QString& publicKey)
{
#ifndef Q_OS_WIN
    QByteArray bytes = QByteArray::fromBase64(publicKey.toUtf8());

    BIO *bi = BIO_new(BIO_s_mem());
    BIO_write(bi, bytes.data(), bytes.size());
    EVP_PKEY *pkey = d2i_PUBKEY_bio(bi, NULL);
    BIO_free(bi);

    if(!pkey)
    {
        QLOG_DEBUG() << "Invalid public key: " << publicKey;
        return false;
    }

    int verify = RSA_verify(NID_sha1,(unsigned char*)serial.data(),serial.size(),(unsigned char*)signature.data(),signature.size(),pkey->pkey.rsa);
    if (verify)
    {
        //Failed!
        QLOG_DEBUG() << "Failed to verify against public key:" << publicKey;
        return false;
    }
    else
    {
        QLOG_DEBUG() << "COA verification successful with public key" << publicKey;
        return true;
    }
#endif
    return false;
}

void PX4FirmwareUploader::portReadyRead()
{
    if (m_currentState == INIT)
    {
        if (getSync())
        {
            m_currentState = REQ_DEVICE_INFO;
            reqNextDeviceInfo();
            return;
        }
    }
    else if (m_currentState == REQ_DEVICE_INFO)
    {
        if (m_waitingForSync)
        {
            if (getSync())
            {
                m_waitingForSync = false;
                if (!reqNextDeviceInfo())
                {
            //Al device info req's are done, request OTP
                    m_currentState = REQ_OTP;
                    emit statusUpdate("Requesting CoA");
                    reqNextOtpAddress();
                }
            }
        }
        else if (readDeviceInfo())
        {
            m_waitingForSync = true;
        portReadyRead(); //Check for sync before popping out.
        }
    }
    else if (m_currentState == REQ_OTP)
    {
        if (m_waitingForSync)
        {
            if (getSync())
            {
                m_waitingForSync = false;
                if (!reqNextOtpAddress())
                {
                    QLOG_INFO() << "OTP read complete";
                    emit statusUpdate("Requesting board Serial Number");
                    emit OTP(m_otpBytes.toHex());
                    m_currentState = REQ_SN;
                    reqNextSNAddress();
                }
            }
        }
        else if (readOtp())
        {
            m_waitingForSync = true;
        portReadyRead();  //Check for sync before popping out.
        }
    }
    else if (m_currentState == REQ_SN)
    {
        if (m_waitingForSync)
        {
            if (getSync())
            {
                m_waitingForSync = false;
                if (!reqNextSNAddress())
                {
                    QLOG_INFO() << "SN read complete";
                    emit serialNumber(m_snBytes.toHex());
                    if (!verifyOtp())
                    {
            //Warnings have already been emitted here, no need for more
                    }
                    emit statusUpdate("Erasing board");
                    reqErase();
                    return;
                }
            }
        }
        else if (readSN())
        {
            m_waitingForSync = true;
        portReadyRead();  //Check for sync before popping out.
        }
    }
    else if (m_currentState == SEND_FW)
    {
        if (m_waitingForSync)
        {
            if (getSync())
            {
                m_waitingForSync = false;
                if (!sendNextFwBytes())
                {
                    //At end
                    QLOG_INFO() << "finished writing firmware";
                    emit statusUpdate("Flashing complete, verifying firmware");
                    m_waitingForSync = false;
                    reqChecksum();
                    return;
                }
                m_waitingForSync = true;
            }
        }
    }
    else if (m_currentState == REQ_CHECKSUM)
    {
        if (m_waitingForSync)
        {
            if (getSync())
            {
                //Everything's happy, reboot and close out
                reqReboot();
                m_port->close();
                m_port->deleteLater(); //We're in a slot for m_port, so don't delete it now.
                m_port = NULL; //But since we called deleteLater, it's safe to clear it out, Qt promises.
                emit complete();
            }
            return;
        }
        if (readChecksum())
        {
            if (m_localChecksum == m_checksum)
            {
                //Good to go!
                QLOG_INFO() << "everything's happy!";
                emit statusUpdate("Verify successful, rebooting");
            }
            else
            {
                QLOG_INFO() << "Error with checksum";
                emit error("CRC mismatch! Firmware write failed, please try again");
                emit statusUpdate("CRC mismatch! Firmware write failed, please try again");
                m_port->close();
                m_port->deleteLater();
                m_port = 0;
                emit complete();
                return;
            }
            //We've read the checksum
            m_waitingForSync = true;
            portReadyRead();  //Check for sync before popping out.
        }
    }
}
