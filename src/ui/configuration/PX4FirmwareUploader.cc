#include "PX4FirmwareUploader.h"
#include "qserialportinfo.h"
#ifndef Q_OS_WIN
#include <openssl/rsa.h>
#include <openssl/x509.h>
#endif //Q_OS_WIN
#include <QProcess>

#include <QCryptographicHash>
#include <QDateTime>
#include "QsLog.h"

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
    for (unsigned i = 0; i < src.size(); i++)
    {
        state = crctab[(state ^ static_cast<unsigned char>(src[i])) & 0xff] ^ (state >> 8);
    }
    return state;
}


PX4FirmwareUploader::PX4FirmwareUploader(QObject *parent) : QThread(parent)
{
    m_stop = false;
}

bool PX4FirmwareUploader::reqInfo(unsigned char infobyte,unsigned int *reply)
{
    m_port->clear();
    m_port->write(QByteArray().append(PROTO_GET_DEVICE).append(infobyte).append(PROTO_EOC));
    m_port->waitForBytesWritten(-1);
    m_port->flush();
    QByteArray infobuf;
    int read = readBytes(4,5000,infobuf);
    if (read != 4)
    {
        QLOG_ERROR() << "Tried to read 4, only read:" << read;
    }
    else
    {
        *reply = ((unsigned char)infobuf[0]) + ((unsigned char)infobuf[1] << 8) + ((unsigned char)infobuf[2] << 16) + ((unsigned char)infobuf[3] << 24);
    }
    int sync = get_sync(2000);
    if (sync != 0)
    {
        return false;
    }
    return true;
}

int PX4FirmwareUploader::readBytes(int num,int timeout,QByteArray &buf)
{
    if (m_serialBuffer.size() >= num)
    {
        buf.append(m_serialBuffer.mid(0,num));
        m_serialBuffer.remove(0,num);
        return num;
    }
    while (m_port->waitForReadyRead(timeout))
    {
        m_serialBuffer.append(m_port->readAll());
        if (m_serialBuffer.size() >= num)
        {
            buf.append(m_serialBuffer.mid(0,num));
            m_serialBuffer.remove(0,num);
            return num;
        }
    }
    QLOG_DEBUG() << "timeout expired:" << m_serialBuffer.size() << num;
    return -1;
}

void PX4FirmwareUploader::stop()
{
    m_stop = true;
}

bool PX4FirmwareUploader::loadFile(QString file)
{
    QFile json(file);
    json.open(QIODevice::ReadOnly);
    QByteArray jsonbytes = json.readAll();
    QString jsonstring(jsonbytes);
    json.close();

    tempJsonFile = new QTemporaryFile();
    tempJsonFile->open();
    tempJsonFile->write(jsonbytes);
    tempJsonFile->close();

    //This chunk of code is from QUpgrade
    //Available https://github.com/LorenzMeier/qupgrade
    // BOARD ID
    QStringList decode_list = jsonstring.split("\"board_id\":");
    decode_list = decode_list.last().split(",");
    if (decode_list.count() < 1)
    {
        QLOG_ERROR() << "Error parsing BOARD ID from .px4 file";
        return false;
    }
    QString board_id = QString(decode_list.first().toUtf8()).trimmed();
    m_loadedBoardID = board_id.toInt();

    // IMAGE SIZE
    decode_list = jsonstring.split("\"image_size\":");
    decode_list = decode_list.last().split(",");
    if (decode_list.count() < 1)
    {
        QLOG_ERROR() << "Error parsing IMAGE SIZE from .px4 file";
        return false;
    }
    QString image_size = QString(decode_list.first().toUtf8()).trimmed();
    m_loadedFwSize = image_size.toInt();

    // DESCRIPTION
    decode_list = jsonstring.split("\"description\": \"");
    decode_list = decode_list.last().split("\"");
    if (decode_list.count() < 1)
    {
        QLOG_ERROR() << "Error parsing DESCRIPTION from .px4 file";
        return false;
    }
    m_loadedDescription = QString(decode_list.first().toUtf8()).trimmed();
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
        QLOG_ERROR() << "Error in decompressing firmware. Please re-download and try again";
        m_port->close();
        return false;
    }
    //Per QUpgrade, pad it to a 4 byte multiple.
    while ((uncompressed.count() % 4) != 0)
    {
        uncompressed.append((char)0xFF);
    }
    tempFile = new QTemporaryFile();
    tempFile->open();
    tempFile->write(uncompressed);
    tempFile->close();
    start();
    return true;
}

void PX4FirmwareUploader::run()
{
    QLOG_INFO() << "Waiting for device to be plugged in...";
    emit requestDevicePlug();
    bool found = false;
    QList<QString> portlist;
    QString portnametouse = "";
    int size = 0;
    foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
    {
        portlist.append(info.portName());
    }
    size = portlist.size();
    while (!found)
    {
        foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
        {
            if (!portlist.contains(info.portName()))
            {
                portnametouse = info.portName();
                found = true;
                break;
            }
        }
        if (size > QSerialPortInfo::availablePorts().size())
        {
            //Something has been removed. Rescan.
            portlist.clear();
            foreach (QSerialPortInfo info,QSerialPortInfo::availablePorts())
            {
                portlist.append(info.portName());
            }
            size = portlist.size();
        }
        if (m_stop)
        {
            return;
        }
        msleep(10);
    }
    emit devicePlugDetected();
#ifdef Q_OS_WIN
    //Windows stuff is all QProcess based for px4uploader
    tempJsonFile->setAutoRemove(false);
    QString filename = tempJsonFile->fileName();
    delete tempJsonFile;
    tempJsonFile = 0;
    QProcess *proc = new QProcess(this);

    //Set the working directory to the temp directory, should be universally writable.
    proc->setWorkingDirectory(tempFile->fileName().mid(0,tempFile->fileName().lastIndexOf("/")));
    proc->start("uploader\\px4uploader.exe",QStringList() << filename.replace("/","\\"));
    emit statusUpdate("Loading file: " + filename);
    proc->waitForStarted(2000);
    int total = 0;
    int show = 0;
    bool finished = false;
    while ((proc->state() == QProcess::Running) || !finished)
    {
        proc->waitForReadyRead(1);
        QString bytes = proc->readLine();
        if (bytes.contains("Trying Port"))
        {
            emit statusUpdate("Attempting to find COM port...");
        }
        else if (bytes.contains("Valid Key"))
        {
            emit statusUpdate("OTP Verified");
        }
        else if (bytes.contains("erase"))
        {
            emit statusUpdate("Erasing...");
        }
        else if (bytes.contains("Program"))
        {
            show++;
            if (!(show % 100))
            {
                emit statusUpdate("Programming...");
                QStringList numsplit = bytes.mid(bytes.indexOf(" ")).split("/");
                if (numsplit.size() > 1)
                {
                    emit flashProgress(numsplit[0].toInt(),numsplit[1].toInt());
                }
            }
        }
        else if (bytes.contains("Same Firmware. Not uploading"))
        {
            emit statusUpdate("No need to update, identical firmware already loaded");
            proc->kill();
            return;
        }
        else if (bytes.contains("Programming packet total:"))
        {
            QString totalstr = bytes.mid(bytes.indexOf("Programming packet total:")+25,bytes.indexOf("\n",bytes.indexOf("Programming packet total:")+25));
            bool ok = false;
            total = totalstr.toInt(&ok);
            if (!ok)
            {
                total = 0;
            }

        }
        else if (bytes.contains("done, rebooting") || bytes.contains("Press Any Key"))
        {
            //Finished
            proc->write("\r");
            proc->write("\n");
            proc->waitForBytesWritten();
            finished = true;
        }
        else if (bytes != "")
        {
            emit debugUpdate(bytes);
        }
    }
    emit debugUpdate(proc->readAllStandardError());
    if (finished)
    {
        emit statusUpdate("Process Success");
    }
    else
    {
        emit statusUpdate("Process ended: " + proc->errorString());
    }
    delete tempFile;
    tempFile = 0;
    emit done();
    return;
#endif
    m_port = new QSerialPort();
    msleep(500);
    m_port->setPortName(portnametouse);
    if (!m_port->open(QIODevice::ReadWrite))
    {
        QLOG_ERROR() << "Unable to open port" << m_port->errorString() << m_port->portName();
#ifdef Q_OS_LINUX
        if(m_port->errorString().contains("busy"))
        {
            emit statusUpdate("ERROR: Port " + m_port->portName() + " is locked by an external process. Run: \"sudo lsof /dev/" + m_port->portName() + "\" to determine the associated programs. They can usually be uninstalled.");
        }
#endif
        return;
    }
    m_port->setBaudRate(QSerialPort::Baud115200);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setParity(QSerialPort::NoParity);
    m_port->setFlowControl(QSerialPort::NoFlowControl);
    m_port->setTextModeEnabled(false);

    //Clear out the port if anything was in it
    for (int i=0;i<128;i++)
    {
        m_port->write(QByteArray().append((char)0x0));
    }
    m_port->waitForBytesWritten(100);
    msleep(1000);
    while(m_port->bytesAvailable())
    {
        m_port->read(1)[0];
    }

    if (m_stop)
    {
        m_port->close();
        delete m_port;
        return;
    }

    //5 retries
    for (int retry=0;retry<5;retry++)
    {
        QLOG_INFO() << "Sending SYNC command, loop" << retry << "of" << 5;
        m_port->write(QByteArray().append(0x21).append(0x20));
        m_port->waitForBytesWritten(-1);
        m_port->flush();
        int sync = get_sync();
        if (sync == 0)
        {
            QLOG_INFO() << "Initial Sync successful";
            int bootloaderrev=0;
            int boardid=0;
            int boardrev=0;
            int flashsize=0;
            QString otpstr = "";
            QString snstr = "";
            //we're synced
            unsigned int read = 0;
            emit statusUpdate("Requesting bootloader rev");
            bool reply = reqInfo(PROTO_DEVICE_BL_REV,&read);
            if (!reply)
            {
                QLOG_WARN() << "Bad sync";
                emit statusUpdate("Bad sync, retrying from start");
                continue;
            }
            bootloaderrev = read;
            QLOG_INFO() << "Bootloader rev:" << read;
            emit statusUpdate("Bootloader Rev: " + QString::number(read));
            if (bootloaderrev >= 4)
            {
                //QLOG_FATAL() << "PX4Firmware Uploader does not yet support V4 bootloaders";
                //emit statusUpdate("PX4 Firmware Uploader does not yet support V4 bootloaders");
                //emit error("PX4FirmwareUploader does not yet support V4 bootloaders");
                //return;
            }
            msleep(500);
            emit statusUpdate("Requesting board ID");
            reply = reqInfo(PROTO_DEVICE_BOARD_ID,&read);
            if (!reply)
            {
                QLOG_WARN() << "Bad sync";
                emit statusUpdate("Bad sync, retrying from start");
                continue;
            }
            boardid = read;
            QLOG_INFO() << "Board ID:" << read;
            emit statusUpdate("Board ID: " + QString::number(read));
            msleep(500);
            emit statusUpdate("Requesting board rev");
            reply = reqInfo(PROTO_DEVICE_BOARD_REV,&read);
            if (!reply)
            {
                QLOG_WARN() << "Bad sync";
                emit statusUpdate("Bad sync, retrying from start");
                continue;
            }
            QLOG_INFO() << "Board rev:" << read;
            emit statusUpdate("Board Rev: " + QString::number(read));
            boardrev = read;
            msleep(500);
            emit statusUpdate("Requesting firmware size");
            reply = reqInfo(PROTO_DEVICE_FW_SIZE,&read);
            if (!reply)
            {
                QLOG_WARN() << "Bad sync";
                emit statusUpdate("Bad sync, retrying from start");
                continue;
            }
            QLOG_INFO() << "Flash size:" << read;
            emit statusUpdate("Flash size: " + QString::number(read));
            flashsize = read;

            while(m_port->bytesAvailable())
            {
                m_port->read(1)[0];
            }
            if (m_stop)
            {
                m_port->close();
                delete m_port;
                return;
            }

            //Create an empty buffer
            msleep(250);
            unsigned char otpbuf[512];
            memset(otpbuf,0,512);
            bool bad = false;
            int timeout = 0;
            if (bootloaderrev >= 4)
            {
                QLOG_INFO() << "Requesting OTP";
                emit statusUpdate("Requesting OTP");
                for (int i=0;i<512;i+=4)
                {
                    m_port->clear();
                    m_port->write(QByteArray().append(0x2A).append(i & 0xFF).append(((i >> 8) & 0xFF)).append((char)0).append((char)0));//.append(PROTO_EOC));
                    m_port->waitForBytesWritten(-1);
                    m_port->flush();
                    timeout = 0;

                    QByteArray bytes;
                    int count = readBytes(4,2000,bytes);
                    if (count < 4)
                    {
                        QLOG_ERROR() << "wrong bytes available:" << count;
                        //bad = true;
                        i-=4;
                        m_port->waitForReadyRead(1000);
                        while(m_port->bytesAvailable())
                        {
                            m_port->read(1)[0];
                        }
                        //Clear the port
                        m_port->clear();
                        continue;
                    }

                    otpbuf[i] = bytes[0];
                    otpbuf[i+1] = bytes[1];
                    otpbuf[i+2] = bytes[2];
                    otpbuf[i+3] = bytes[3];
                    int sync = get_sync(2000);
                    if (sync != 0)
                    {
                        QLOG_ERROR() << "Bad sync";
                        i-=4;
                        m_port->waitForReadyRead(1000);
                        while(m_port->bytesAvailable())
                        {
                            m_port->read(1)[0];
                        }
                        continue;
                        //bad = true;
                        //break;
                    }
                    if (m_stop)
                    {
                        m_port->close();
                        delete m_port;
                        return;
                    }
                }
                if (bad)
                {
                    continue;
                }
                QLOG_INFO() << "OTP read";
                if (otpbuf[0] != 80 && otpbuf[1] != 88 && otpbuf[2] != 52 && otpbuf[3] != 0)
                {
                    QLOG_ERROR() << "OTP header failure";
                    continue;
                }
                //Let's format this like MP does
                QString otpoutput = "";
                for (int i=0;i<512;i++)
                {
                    otpoutput += (otpbuf[i] <= 0xF ? "0" : "") + QString::number(otpbuf[i],16).toUpper() + " ";
                    if (i % 16 == 15)
                    {
                        QLOG_TRACE() << otpoutput;
                        otpstr += otpoutput + "\n";
                        otpoutput = "";
                    }
                }
                QLOG_INFO() << "OTP:" << otpstr;


                //Create an empty buffer for the serialnumber
                emit statusUpdate("Requesting board SN");
                unsigned char snbuf[12];
                memset(snbuf,0,12);
                for (int i=0;i<12;i+=4)
                {
                    m_port->clear();
                    m_port->write(QByteArray().append(0x2B).append(i).append((char)0).append((char)0).append((char)0).append(PROTO_EOC));
                    m_port->flush();
                    timeout = 0;
                    QByteArray bytes;
                    int count = readBytes(4,2000,bytes);
                    if (count < 4)
                    {
                        QLOG_ERROR() << "wrong bytes available:" << count;
                        //bad = true;
                        i-=4;
                        m_port->waitForReadyRead(1000);
                        while(m_port->bytesAvailable())
                        {
                            m_port->read(1)[0];
                        }
                        continue;
                    }
                    snbuf[i] = bytes[3];
                    snbuf[i+1] = bytes[2];
                    snbuf[i+2] = bytes[1];
                    snbuf[i+3] = bytes[0];
                    int sync = get_sync(2000);
                    if (sync != 0)
                    {
                        QLOG_ERROR() << "Bad sync";
                        bad = true;
                        break;
                    }
                    while(m_port->bytesAvailable())
                    {
                        m_port->read(1)[0];
                    }

                }
                if (bad)
                {
                    continue;
                }
                QString SN = "";
                for (int i=0;i<12;i++)
                {
                    //(otpbuf[i] <= 0xF ? "0" : "") + QString::number(otpbuf[i],16).toUpper() + " ";
                    SN += (snbuf[i] <= 0xF ? "0" : "") + QString::number(snbuf[i],16).toUpper() + " ";
                    //snstr += SN + "\n";
                }

                QLOG_INFO() << "Board SN:" << SN;
                snstr = SN;
                QByteArray signature;
                for (int i=32;i<(128+32);i++)
                {
                    signature.append(otpbuf[i]);
                }
                QByteArray serial;
                for (int i=0;i<12;i++)
                {
                    serial.append(snbuf[i]);
                }
                for (int i=0;i<8;i++)
                {
                    serial.append((char)0);
                }
                serial[0] = serial[1];
                qDebug() << "Serial size:" << serial.size();
                emit statusUpdate("Verifying OTP");

#ifdef Q_OS_WIN
                //QProcess *proc = new QProcess();
                QLOG_FATAL() << "PX4Firmware Uploader does not yet support uploading on Windows";
                emit statusUpdate("PX4Firmware Uploader does not yet support uploading on Windows");
                emit error("PX4Firmware Uploader does not yet support uploading on Windows");
                m_port->close();
                delete m_port;
                return;

#else

                QString test = "\r\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDqi8E6EdZ11iE7nAc95bjdUTwd\r\n/gLetSAAx8X9jgjInz5j47DIcDqFVFKEFZWiAc3AxJE/fNrPQey16SfI0FyDAX/U\t\n4jyGIv9w+M1dKgUPI8UdpEMS2w1YnfzW0GO3PX0SBL6pctEIdXr0NGsFFaqU9Yz4\r\nDbgBdR6wBz9qdfRRoQIDAQAB";
                QByteArray bytes = QByteArray::fromBase64(test.toAscii());

                BIO *bi = BIO_new(BIO_s_mem());
                BIO_write(bi, bytes.data(), bytes.size());
                EVP_PKEY *pkey = d2i_PUBKEY_bio(bi, NULL);
                BIO_free(bi);
                if(!pkey)
                {
                    QLOG_FATAL() << "PX4Firmware Uploader failed OTP check! Internal public key is not valid. Possible corrupted install?";
                    emit statusUpdate("PX4Firmware Uploader failed OTP check! Internal public key is not valid. Possible corrupted install?");
                    emit error("PX4Firmware Uploader failed OTP check! Internal public key is not valid. Possible corrupted install?");
                    m_port->close();
                    delete m_port;
                    return;
                }

                int verify = RSA_verify(NID_sha1,(unsigned char*)serial.data(),serial.size(),(unsigned char*)signature.data(),signature.size(),pkey->pkey.rsa);
                if (verify)
                {
                    //Failed!
                    QLOG_FATAL() << "PX4Firmware Uploader failed OTP check";
                    emit statusUpdate("PX4Firmware Uploader failed OTP check");
                    emit error("PX4Firmware Uploader failed OTP check! Are you sure this is a legimiate 3DR PX4?");
                    m_port->close();
                    delete m_port;
                    return;
                }
#endif //Q_OS_WIN
                QLOG_DEBUG() << "OTP verification successful";
                emit statusUpdate("OTP verification successful");
                //QLOG_INFO() << "OTP Successful";
                //emit statusUpdate("OTP Verification successful!");
                //qDebug() << "Sig size:" << signature.size();
                //qDebug() << "First three of sig:" << QString::number(signature[0],16) << QString::number(signature[1],16) << QString::number(signature[2],16);
                //qDebug() << "Last three of sig:" << QString::number(signature[125],16) << QString::number(signature[126],16) << QString::number(signature[127],16);
                //qDebug() << "Serial size:" << serial.size();
            } //if bootloaderrev >= 4
            if (m_stop)
            {
                m_port->close();
                delete m_port;
                return;
            }


            emit boardRev(boardrev);
            emit boardId(boardid);
            emit bootloaderRev(bootloaderrev);
            emit flashSize(flashsize);
            emit serialNumber(snstr);
            emit OTP(otpstr);

            //Erase
            QLOG_INFO() << "Requesting erase";
            emit statusUpdate("Erasing flash, this may take up to a minute");
            m_port->write(QByteArray().append(0x23).append(0x20));
            m_port->flush();
            //msleep(20000);
            sync = get_sync(60000);
            if (sync)
            {
                QLOG_DEBUG() << "never returned from erase.";
                emit statusUpdate("Flash erase never completed, please restart autopilot board and retry.");
                emit error("Flash erase never completed, please restart autopilot board and retry.");
                m_port->close();
                delete tempFile;
                delete m_port;
                return;
            }
            if (m_stop)
            {
                m_port->close();
                delete tempFile;
                delete m_port;
                return;
            }


            {

                //unsigned int fwsize = fwmap["image_size"].toUInt();
                //Lorenz says that this is a more reliable way of parsing out the image, I agree.
                //Clear out the m_port->
                m_port->clear();
                msleep(1000);

                QLOG_INFO() << "Starting flash process";
                emit statusUpdate("Flashing firmware");
                tempFile->open();
                int counter = 0;
                int failure = 0;
                QByteArray writtenbuf = tempFile->readAll();
                tempFile->seek(0);
                while (!tempFile->atEnd())
                {
                    QByteArray buf = tempFile->read(60);
                    if (buf.size() > 0)
                    {
                        QByteArray tosend;
                        tosend.append(0x27);
                        tosend.append(buf.size());
                        tosend.append(buf);
                        tosend.append(0x20);
                        m_port->clear();
                        m_port->write(tosend);
                        m_port->waitForBytesWritten(-1);
                        m_port->flush();
                        //msleep(1000);
                        int sync = get_sync(1000);
                        if (sync != 0)
                        {
                            failure++;
                            if (failure > 2)
                            {
                                QLOG_FATAL() << "error writing firmware" << tempFile->pos() << tempFile->size();
                                emit error("Error writing firmware, invalid sync. Please retry");
                                m_port->close();
                                tempFile->close();
                                delete tempFile;
                                return;
                            }
                            msleep(1000);
                            QLOG_INFO() << "Requesting erase";
                            emit statusUpdate("Erasing flash, this may take up to a minute");
                            m_port->clear();
                            m_port->write(QByteArray().append(0x23).append(0x20));
                            m_port->flush();
                            //msleep(20000);
                            sync = get_sync(60000);
                            if (sync)
                            {
                                QLOG_DEBUG() << "never returned from erase.";
                                emit statusUpdate("Flash erase never completed, please restart autopilot board and retry.");
                                emit error("Flash erase never completed, please restart autopilot board and retry.");
                                m_port->close();
                                tempFile->close();
                                delete tempFile;
                                delete m_port;
                                return;
                            }
                            tempFile->seek(0);
                            continue;
                        }
                        if (counter++ % 50 == 0)
                        {
                            emit flashProgress(tempFile->pos(),tempFile->size());
                            QLOG_INFO() << "flashing:" << tempFile->pos() << "/" << tempFile->size();
                        }
                        if (m_stop)
                        {
                            m_port->close();
                            tempFile->close();
                            delete tempFile;
                            delete m_port;
                            return;
                        }

                    }
                    else
                    {
                        QLOG_ERROR() << "Something went wrong, couldn't read from tmp file";
                        m_port->close();
                        tempFile->close();
                        delete tempFile;
                        delete m_port;
                        return;
                    }
                }
                QLOG_DEBUG() << "Done";
                emit statusUpdate("Flashing complete, verifying");
                m_port->clear();

                m_port->write(QByteArray().append(0x29).append(0x20));
                m_port->waitForBytesWritten(-1);
                m_port->flush();



                QByteArray infobuf;
                int read = readBytes(4,5000,infobuf);
                if (read != 4)
                {
                    QLOG_ERROR() << "Tried to read 4, only read:" << read;
                }
                int sync = get_sync(2000);
                if (sync != 0)
                {
                    return;
                }

                //infobuf has our expected CRC, calculate it ourselves.
                //writtenbuf contains the data to CRC verify.
                int oldsize = writtenbuf.size();
                for (int i=oldsize;i<flashsize;i++)
                {
                    writtenbuf.append(0xFF);
                }
                quint32 remotecrc = crc32(writtenbuf);
                quint32 localcrc = 0;
                localcrc += static_cast<unsigned char>(infobuf[0]);
                localcrc += static_cast<unsigned char>(infobuf[1]) << 8;
                localcrc += static_cast<unsigned char>(infobuf[2]) << 16;
                localcrc += static_cast<unsigned char>(infobuf[3]) << 24;
                QLOG_DEBUG() << "Remote CRC:" << QString::number(remotecrc,16).toUpper();
                QLOG_DEBUG() << "Local CRC:" << QString::number(localcrc,16).toUpper();
                if (remotecrc != localcrc)
                {
                    emit error("CRC mismatch! Firmware write failed, please try again");
                    emit statusUpdate("CRC mismatch! Firmware write failed, please try again");
                    m_port->close();
                    tempFile->close();
                    delete tempFile;
                    delete m_port;
                    return;
                }

                m_port->write(QByteArray().append(0x30).append(0x20));
                m_port->flush();
                m_port->waitForBytesWritten(1000);
                m_port->close();
                tempFile->close();
                emit statusUpdate("Verification successful, rebooting...");
                delete tempFile;
                delete m_port;
                emit done();
                return;
            }
            m_port->close();
        }
        else
        {
            msleep(500);
            continue;
        }
    }
    QLOG_DEBUG() << "Retry timeout";
    m_port->close();
    delete m_port;
    emit statusUpdate("Unable to flash board, 5 retries attempted. Please check hardware and try again");
    emit error("Unable to flash board, 5 retries attempted. Please check hardware and try again");

}
int PX4FirmwareUploader::get_sync(int timeout)
{
    QByteArray infobuf;
    int read = readBytes(2,timeout,infobuf);
    if (read != 2)
    {
        QLOG_ERROR() << "Wrong number of bytes read on sync:" << read;
        return -1;
    }
    else
    {
        if (infobuf[0] != (char)0x12  || infobuf[1] != (char)0x10)
        {
            QLOG_ERROR() << "Bad sync return:" << QString::number(infobuf[0],16) << QString::number(infobuf[1],16);
            return -1;
        }
        return 0;
    }
}
