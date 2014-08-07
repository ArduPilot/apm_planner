/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 Michael Carpenter <malcom2073@gmail.com>

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
 *   @brief ArduinoFlash implementation While I didn't directly use any code from him for this class, it was heavily influenced by
 *   smurfy's FlashTool for MegaPirateNG (https://github.com/MegaPirateNG/FlashTool)
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *
 */


#ifndef ARDUINOFLASH_H
#define ARDUINOFLASH_H

#include <QThread>
#include <QtSerialPort/qserialport.h>
#include <QMutex>
#include <QByteArray>
class ArduinoFlash : public QThread
{
    Q_OBJECT
public:
    explicit ArduinoFlash(QObject *parent = 0);
    void loadFirmware(QString comport,QString filename);
    void abortLoading();
private:
    bool m_running;
    QMutex m_runningMutex;
    void start(Priority = InheritPriority);
    void run();
    unsigned char m_sequenceNumber;
    int readBytes(QByteArray &buf,int count);
    QByteArray generateMessage(QByteArray message);
    QByteArray m_privBuffer;
    QSerialPort *m_port;
    QByteArray readMessage();
    QString m_firmwareFile;
    QString m_comPort;
    QByteArray generateProgramFlash(QByteArray bytes);
    QByteArray generateLoadAddress(unsigned int address);
    QByteArray readFlash(unsigned short blocksize);
    QByteArray sendMessageForResponse(QByteArray message);
    QByteArray generateReadFlash(unsigned short blocksize);
signals:
    void flashProgress(qint64 pos,qint64 total);
    void flashComplete();
    void flashFailed();
    void verifyProgress(qint64 pos,qint64 total);
    void verifyComplete();
    void verifyFailed();
    void firmwareUploadError(QString error);
    void firmwareUploadStarted();
    void firmwareUploadComplete();
    void statusUpdate(QString status);
    void debugUpdate(QString debug);
public slots:

};

#endif // ARDUINOFLASH_H
