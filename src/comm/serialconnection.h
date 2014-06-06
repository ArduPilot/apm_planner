#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H
#include "LinkInterface.h"
#include "qserialport.h"
#include "SerialLinkInterface.h"
#include <QMap>
class SerialConnection : public SerialLinkInterface
{
    Q_OBJECT
public:
    explicit SerialConnection(QObject *parent = 0);
    bool connect();
    void disableTimeouts();
    void enableTimeouts();
    int getId() const;
    QString getName() const;
    bool isConnected() const;
    qint64 getConnectionSpeed() const;
    bool disconnect();
    qint64 bytesAvailable();
    void writeBytes(const char* buf,qint64 size);
    void readBytes();

public:
    QList<QString> getCurrentPorts();
    QString getPortName() const;
    int getBaudRate() const;
    int getDataBits() const;
    int getStopBits() const;
    void requestReset();
    int getBaudRateType() const;
    int getFlowType() const;
    int getParityType() const;
    int getDataBitsType() const;
    int getStopBitsType() const;
    LinkType getLinkType() { return SERIAL_LINK; }

public slots:
    bool setPortName(QString portName);
    bool setBaudRate(int rate);
    bool setBaudRateType(int rateIndex);
    bool setFlowType(int flow);
    bool setParityType(int parity);
    bool setDataBitsType(int dataBits);
    bool setStopBitsType(int stopBits);
    void loadSettings();
    void writeSettings();

private:
    QSerialPort *m_port;
    QString m_portName;
    int m_baud;
    int m_linkId;
    bool m_isConnected;
    QMap<QString,int> m_portBaudMap;
    QList<QString> m_portList;

signals:
    void updateLink(LinkInterface *link);
    
public slots:
    void readyRead();
    bool setBaudRateString(QString baud);
    
};

#endif // SERIALCONNECTION_H
