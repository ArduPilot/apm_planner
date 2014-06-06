#ifndef NEW_MAVLINKDECODER_H
#define NEW_MAVLINKDECODER_H
#include <QObject>
#include "LinkInterface.h"
#include <QThread>
#include <QFile>
#include <QMap>
#include "QsLog.h"
#include "MAVLinkDecoder.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

class ConnectionManager;
class New_MAVLinkDecoder : public QObject
{
    Q_OBJECT
public:
    New_MAVLinkDecoder(QObject *parent=0);
    void passManager(ConnectionManager *manager) { m_connectionManager = manager; }
private:
    int getSystemId() { return 252; }
    int getComponentId() { return 1; }
    bool m_loggingEnabled;
    QFile *m_logfile;
    ConnectionManager *m_connectionManager;
    void stopLogging() { }
    bool m_throwAwayGCSPackets;
    bool m_enable_version_check;
    bool versionMismatchIgnore;
    QMap<int,qint64> totalReceiveCounter;
    QMap<int,qint64> currReceiveCounter;
    QMap<int,QMap<int,uint8_t> > lastIndex;
    QMap<int,qint64> totalLossCounter;
    QMap<int,qint64> currLossCounter;
    bool m_multiplexingEnabled;
    quint64 getUnixTimeFromMs(int systemID, quint64 time);
    QMap<int,int> componentID;
    QMap<int,bool> componentMulti;
    QMap<uint16_t, bool> messageFilter;               ///< Message/field names not to emit
    QMap<uint16_t, bool> textMessageFilter;           ///< Message/field names not to emit in text mode
    mavlink_message_t receivedMessages[256]; ///< Available / known messages
    mavlink_message_info_t messageInfo[256]; ///< Message information
    QMap<int,quint64> onboardTimeOffset;
    QMap<int,quint64> firstOnboardTime;
    QMap<int,quint64> onboardToGCSUnixTimeOffsetAndDelay;

signals:


    void protocolStatusMessage(const QString& title, const QString& message);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
    void textMessageReceived(int uasid, int componentid, int severity, const QString& text);
    void receiveLossChanged(int id,float value);
public slots:
    void messageReceived(LinkInterface* link, mavlink_message_t message);
    void sendMessage(mavlink_message_t msg);
    void emitFieldValue(mavlink_message_t* msg, int fieldid, quint64 time);
};

#endif // NEW_MAVLINKDECODER_H
