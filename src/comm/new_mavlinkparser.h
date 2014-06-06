#ifndef NEW_MAVLINKPARSER_H
#define NEW_MAVLINKPARSER_H

#include <QThread>
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include <QByteArray>
#include "LinkInterface.h"
#include <QFile>
#include "QGC.h"
#include <QDataStream>
#include "UASInterface.h"
#include "MAVLinkDecoder.h"
class LinkManager;
class New_MAVLinkParser : public QObject
{
    Q_OBJECT
public:
    explicit New_MAVLinkParser(QObject *parent = 0);
    void setConnectionManager(LinkManager *manager) { m_connectionManager = manager; }
private:
    int getSystemId() { return 252; }
    int getComponentId() { return 1; }
    bool m_loggingEnabled;
    QFile *m_logfile;
    void sendMessage(mavlink_message_t msg) { }
    void stopLogging() { }
    bool m_throwAwayGCSPackets;
    LinkManager *m_connectionManager;
    bool versionMismatchIgnore;
    QMap<int,qint64> totalReceiveCounter;
    QMap<int,qint64> currReceiveCounter;
    QMap<int,QMap<int,uint8_t> > lastIndex;
    QMap<int,qint64> totalLossCounter;
    QMap<int,qint64> currLossCounter;
    bool m_enable_version_check;

signals:
    void protocolStatusMessage(const QString& title, const QString& message);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
    void textMessageReceived(int uasid, int componentid, int severity, const QString& text);
    void receiveLossChanged(int id,float value);
    void messageReceived(LinkInterface *link,mavlink_message_t message);

public slots:
    void receiveBytes(LinkInterface* link, QByteArray b);
};

#endif // NEW_MAVLINKPARSER_H
