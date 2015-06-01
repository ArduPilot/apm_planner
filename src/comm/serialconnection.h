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
 *   @brief LinkManager
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author QGROUNDCONTROL PROJECT - This code has GPLv3+ snippets from QGROUNDCONTROL, (c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 */

#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H
#include "LinkInterface.h"
#include <QtSerialPort/qserialport.h>
#include "SerialLinkInterface.h"
#include <QMap>
#include <QTimer>

#define SERIAL_TIMEOUT_MILLISECONDS 5000
class SerialConnection : public SerialLinkInterface
{
    Q_OBJECT
public:
    explicit SerialConnection();
    virtual ~SerialConnection();

    // From LinkInterface
    void disableTimeouts();
    void enableTimeouts();
    int getId() const;
    QString getName() const;
    QString getShortName() const;
    QString getDetail() const;
    void requestReset();
    bool isConnected() const;
    qint64 getConnectionSpeed() const;
    qint64 bytesAvailable();
    LinkType getLinkType() { return SERIAL_LINK; }

    // From SerialLinkInterface
    QList<QString> getCurrentPorts();
    QString getPortName() const;
    int getBaudRate() const;
    int getDataBits() const;
    int getStopBits() const;
    int getBaudRateType() const;
    int getFlowType() const;
    int getParityType() const;
    int getDataBitsType() const;
    int getStopBitsType() const;

public slots:
    // from LinkInterface
    bool connect();
    bool disconnect();
    void writeBytes(const char* buf,qint64 size);
    void readBytes();

    // From SerialLinkInterface
    bool setPortName(QString portName);
    bool setBaudRate(int rate);
    bool setBaudRateType(int rateIndex);
    bool setFlowType(int flow);
    bool setParityType(int parity);
    bool setDataBitsType(int dataBits);
    bool setStopBitsType(int stopBits);
    void loadSettings();
    void writeSettings();

    void readyRead();
    bool setBaudRateString(QString baud);

signals:
    void timeoutTriggered(LinkInterface*);

private slots:
    void timeoutTimerTick();
    void connectionDestroyed(QObject* object);
    void portError(QSerialPort::SerialPortError serialPortError);

private:
    QSerialPort *m_port;
    QString m_portName;
    int m_baud;
    int m_linkId;
    bool m_isConnected;
    QMap<QString,int> m_portBaudMap;
    QList<QString> m_portList;
    int m_retryCount;
    QTimer *m_timeoutTimer;
    qint64 m_lastTimeoutMessage;
    bool m_timeoutsEnabled;
    bool m_timeoutMessageSent;
    
};

#endif // SERIALCONNECTION_H
