/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief ArduinoFlash implementation
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *
 */

#include "arduinoflash.h"
#include <QDebug>
#include "arduino_intelhex.h"
#include "QsLog.h"
#define GET_SIG_ONE QByteArray().append(0x1B).append((char)0x00).append((char)0x00).append((char)0x00).append((char)0x00)
#define GET_SIG_TWO QByteArray().append(0x1B).append((char)0x00).append((char)0x00).append((char)0x00).append((char)0x01)
#define GET_SIG_THREE QByteArray().append(0x1B).append((char)0x00).append((char)0x00).append((char)0x00).append((char)0x02)
#define ARDUINO2560SIG QByteArray().append(0x1e).append(0x98).append(0x01)
#define CMD_LOAD_ADDRESS 0x06
#define CMD_PROGRAM_FLASH_ISP 0x13
#define CMD_READ_FLASH_ISP 0x14
#define CMD_LEAVE_PROGMODE_ISP 0x11
ArduinoFlash::ArduinoFlash(QObject *parent) :
    QThread(parent)
{
    m_sequenceNumber = 0;
}
void ArduinoFlash::loadFirmware(QString comport,QString filename)
{
    m_firmwareFile = filename;
    m_comPort = comport;
    start();
}
void ArduinoFlash::start(Priority priority)
{
    m_running = true;
    QThread::start(priority);
}
void ArduinoFlash::abortLoading()
{
    m_runningMutex.lock();
    m_running = false;
    m_runningMutex.unlock();
}

void ArduinoFlash::run()
{
    emit firmwareUploadStarted();
    emit statusUpdate("Opening firmware file...");
    ArduinoIntelHex hex;
    hex.loadIntelHex(m_firmwareFile);
    if (hex.size() == 0)
    {
        emit firmwareUploadError("Unable to open file: " + m_firmwareFile);
        return;
    }
    else
    {
        emit debugUpdate("Hex file loaded: " + QString::number(hex.size()) + " bytes");
    }
    m_port = new QSerialPort();
#if defined(Q_OS_MACX) && ((QT_VERSION == 0x050402)||(QT_VERSION == 0x0500401))
    // temp fix Qt5.4.1 issue on OSX
    // http://code.qt.io/cgit/qt/qtserialport.git/commit/?id=687dfa9312c1ef4894c32a1966b8ac968110b71e
    m_port->setPortName("/dev/cu." + m_comPort);
#else
    m_port->setPortName(m_comPort);
#endif
    if (!m_port->open(QIODevice::ReadWrite))
    {
        emit firmwareUploadError("Unable to open COM port: " + m_port->portName());
        delete m_port;
        return;
    }
    m_port->setBaudRate(115200);
    m_port->setDataBits(QSerialPort::Data8);
    m_port->setStopBits(QSerialPort::OneStop);
    m_port->setParity(QSerialPort::NoParity);

    int retries = 0;
    bool success = false;
    QByteArray packet;
    while (!success && retries++ < 5)
    {
        m_runningMutex.lock();
        if (!m_running)
        {
            m_port->close();
            delete m_port;
            emit firmwareUploadError("Canceled by user");
            return;
        }
        m_runningMutex.unlock();
        m_port->flush();
        m_port->setDataTerminalReady(true);
        msleep(500);
        m_port->setDataTerminalReady(false);
        msleep(200);
        m_sequenceNumber = 0;

        m_port->write(generateMessage(GET_SIG_ONE));
        m_port->waitForBytesWritten(1);

        packet = readMessage();
        if (packet.size() > 0)
        {
            success = true;
        }
        else
        {
            packet = readMessage();
            if (packet.size() > 0)
            {
                success = true;
            }
        }
        QLOG_INFO() << "ArduinoFlash::readMessage(): Got packet:" << packet.size() << packet.toHex();
        msleep(500);
        m_privBuffer.clear();
    }
    if (!success)
    {
        emit firmwareUploadError("Unable to gain access to bootloader");
        m_port->close();
        delete m_port;
        return;
    }
    packet = readMessage();
    msleep(100);
    m_privBuffer.clear();
    m_port->flush();
    emit statusUpdate("Reading board sig");

    QByteArray sig;
    m_port->write(generateMessage(GET_SIG_ONE));
    m_port->waitForBytesWritten(1);
    packet = readMessage();
    sig.append(packet.at(2));
    msleep(100);

    m_port->write(generateMessage(GET_SIG_TWO));
    m_port->waitForBytesWritten(1);
    packet = readMessage();
    sig.append(packet.at(2));
    msleep(100);

    m_port->write(generateMessage(GET_SIG_THREE));
    m_port->waitForBytesWritten(1);
    packet = readMessage();
    sig.append(packet.at(2));
    msleep(100);

    emit debugUpdate("Board sig: " + sig.toHex());


    m_port->write(generateLoadAddress(0));

    m_port->waitForBytesWritten(1);
    packet = readMessage();
    emit debugUpdate("Setting start address to zero");
    emit debugUpdate("Response: " + packet.toHex());
    unsigned short blocksize = 0x100;


    int blocks = hex.size() / blocksize;
    QByteArray writeheader;

    QByteArray tosend;
    unsigned int pos = 0;
    int bytesremaining = hex.size();
    unsigned short reqsize = 0;
    QByteArray response;
    emit debugUpdate("0x100 Split block count: " + QString::number(blocks));
    emit statusUpdate("Flashing firmware...");

    m_runningMutex.lock();
    if (!m_running)
    {
        m_port->close();
        delete m_port;
        emit firmwareUploadError("Canceled by user");
        return;
    }
    m_runningMutex.unlock();
    for (int i=0;i<=blocks;i++)
    {
        if (bytesremaining > 0x100)
        {
            reqsize = 0x100;
        }
        else
        {
            reqsize = bytesremaining;
        }
        response = sendMessageForResponse(generateProgramFlash(hex.mid(pos,reqsize)));
        if (response.size() > 0)
        {
            if (((unsigned char)response.at(1)) != 0x0)
            {
                //Failure
                emit debugUpdate("Failed writing block " + QString::number(i));
                m_port->close();
                delete m_port;
                emit firmwareUploadError("Failed while writing block " + QString::number(i));
                return;
            }
        }
        else
        {
            emit debugUpdate("Failed writing block " + QString::number(i));
            m_port->close();
            delete m_port;
            emit firmwareUploadError("Failed while writing block " + QString::number(i));
            return;
        }
        pos += blocksize;
        bytesremaining -= blocksize;
        emit flashProgress(i * blocksize,blocks*blocksize);
        m_runningMutex.lock();
        if (!m_running)
        {
            m_port->close();
            delete m_port;
            emit firmwareUploadError("Canceled by user");
            return;
        }
        m_runningMutex.unlock();
    }
    emit flashComplete();


    pos = 0;
    packet = sendMessageForResponse(generateLoadAddress(0));

    QByteArray verifyvalue;
    blocks = hex.size() / 0x100;
    blocksize = 0x100;
    bytesremaining = hex.size();
    reqsize = 0;
    emit statusUpdate("Verifying firmware...");
    for (int i=0;i<=blocks;i++)
    {
        if (bytesremaining > 0x100)
        {
            reqsize = 0x100;
        }
        else
        {
            reqsize = bytesremaining;
        }
        packet = readFlash(reqsize);
        if (packet.size() != reqsize)
        {
            m_port->close();
            delete m_port;
            emit firmwareUploadError("Failed while verifying block " + QString::number(i));
            return;
        }
        verifyvalue.append(packet);
        emit verifyProgress(i * blocksize,blocks*blocksize);
        pos += blocksize;
        bytesremaining-= reqsize;
        m_runningMutex.lock();
        if (!m_running)
        {
            m_port->close();
            delete m_port;
            emit firmwareUploadError("Canceled by user");
            return;
        }
        m_runningMutex.unlock();

    }
    if (verifyvalue != hex)
    {
        m_port->close();
        delete m_port;
        emit debugUpdate("Verification of firmware failed after uploading");
        emit firmwareUploadError("Verification of firmware failed after uploading");
        emit verifyFailed();
        return;
    }
    else
    {
        emit verifyComplete();
    }





    m_port->write(generateMessage(QByteArray().append(0x11)));
    m_port->waitForBytesWritten(1);
    QByteArray closereturn = readMessage();
    if (closereturn.size() != 2)
    {
        QLOG_ERROR() << "ArduinoFlash::readMessage(): Close of Arduino bootloader failed";
    }
    m_port->setDataTerminalReady(false);
    msleep(500);
    m_port->setDataTerminalReady(true);
    msleep(500);
    m_port->close();
    delete m_port;
    emit firmwareUploadComplete();
}
QByteArray ArduinoFlash::sendMessageForResponse(QByteArray message)
{
    m_port->write(message);
    m_port->waitForBytesWritten(1);
    return readMessage();
}

QByteArray ArduinoFlash::readMessage()
{
    QByteArray retbuf;
    QByteArray packet;
    int count = readBytes(retbuf,5);
    if (count != 5)
    {
        //No header
        QLOG_ERROR() << "ArduinoFlash::readMessage(): Bad header return";
        return QByteArray();
    }
    packet.append(retbuf);
    if (((unsigned char)retbuf.at(0)) == 0x1b && ((unsigned char)retbuf.at(4)) == 0x0E)
    {
        unsigned short messagelength = (((unsigned char)retbuf[2]) << 8) + ((unsigned char)retbuf[3]);
        count = readBytes(retbuf,messagelength);
        if (count != messagelength)
        {
            QLOG_ERROR() << "ArduinoFlash::readMessage(): Error reading message" << messagelength << retbuf.toHex();
            return QByteArray();
        }
        packet.append(retbuf);
        count = readBytes(retbuf,1);
        if (count != 1)
        {
            QLOG_ERROR() << "ArduinoFlash::readMessage(): Error reading checksum";
            return QByteArray();
        }
        packet.append(retbuf);
        //qDebug() << "<<" << packet.toHex();
        return packet.mid(5,packet.length()-6);
    }
    QLOG_ERROR() << "ArduinoFlash::readMessage(): Bad packet start:" << retbuf.toHex();
    return QByteArray();
}

QByteArray ArduinoFlash::generateMessage(QByteArray message)
{
    QByteArray retval;
    retval.append(0x1B);
    retval.append(m_sequenceNumber++);
    retval.append((message.length() >> 8) & 0xFF);
    retval.append(message.length() & 0xFF);
    retval.append(0x0E);
    retval.append(message);
    unsigned char checksum=0;
    for (int i=0;i<retval.size();i++)
    {
        checksum ^= ((unsigned char)(retval.at(i)));
    }
    retval.append(checksum);
    //qDebug() << ">>" << retval.toHex();
    return retval;
}
int ArduinoFlash::readBytes(QByteArray &buf,int count)
{
    if (m_privBuffer.size() >= count)
    {
        buf = m_privBuffer.mid(0,count);
        m_privBuffer.remove(0,count);
        return count;
    }
    while (m_port->waitForReadyRead(1000))
    {
        m_privBuffer.append(m_port->readAll());
        if (m_privBuffer.size() >= count)
        {
            buf = m_privBuffer.mid(0,count);
            m_privBuffer.remove(0,count);
            return count;
        }
    }
    return 0;
}
QByteArray ArduinoFlash::generateProgramFlash(QByteArray bytes)
{
    QByteArray retval;
    retval.append(CMD_PROGRAM_FLASH_ISP);
    retval.append((bytes.size() >> 8) & 0xFF);
    retval.append(bytes.size() & 0xFF);
    retval.append((char)0x0); //mode
    retval.append((char)0x0); //delay
    retval.append((char)0x0); //cmd1 AVR_OP_WRITELO
    retval.append((char)0x0); //cmd2 AVR_OP_WRITEPAGE
    retval.append((char)0x0); //cmd3 AVR_OP_READ_LO
    retval.append((char)0x0); //pol1
    retval.append((char)0x0); //pol2
    retval.append(bytes);
    return generateMessage(retval);
}

QByteArray ArduinoFlash::generateLoadAddress(unsigned int address)
{
    QByteArray retval;
    retval.append(CMD_LOAD_ADDRESS);
    retval.append((address >> 24) & 0xFF);
    retval.append((address >> 16) & 0xFF);
    retval.append((address >> 8) & 0xFF);
    retval.append(address & 0xFF);
    return generateMessage(retval);
}
QByteArray ArduinoFlash::generateReadFlash(unsigned short blocksize)
{
    QByteArray retval;
    retval.append(CMD_READ_FLASH_ISP);
    retval.append((blocksize >> 8) & 0xFF);
    retval.append(blocksize & 0xFF);
    return generateMessage(retval);
}
QByteArray ArduinoFlash::readFlash(unsigned short blocksize)
{
    QByteArray retval = sendMessageForResponse(generateReadFlash(blocksize));
    if (retval.size() == 0)
    {
        return QByteArray();
    }
    return retval.mid(2,retval.size()-3);
}
