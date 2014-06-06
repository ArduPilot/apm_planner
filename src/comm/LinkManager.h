#ifndef LINKMANAGER_H
#define LINKMANAGER_H

#include <QObject>
/**
 * @brief The ConnectionManager class
 * This class handles all connections between the GCS and the actual hardware.
 * It will create (on request) serial or UDP links, connect the links to the associated mavlink parsers,
 * and emit signals upwards when mavlink messages come in.
 * This class lives in the UI thread
 * The Serial Link lives in the UI Thread
 * The mavlink decoder lives in its own thread
 * the UAS Class lives in the UI thread
 */
#include "serialconnection.h"
#include "new_mavlinkdecoder.h"
#include "new_mavlinkparser.h"
#include "MAVLinkProtocol.h"
#include <QMap>
#include "UASInterface.h"
#include "UAS.h"
class LinkManager : public QObject
{
    Q_OBJECT
public:
    explicit LinkManager(QObject *parent = 0);
    static LinkManager* instance()
    {
        static LinkManager* _instance = 0;
        if(_instance == 0)
        {
            _instance = new LinkManager();
        }
        return _instance;
    }
    int addSerialConnection(QString port,int baud);
    int addSerialConnection();
    int addUdpConnection(QHostAddress addr,int port);
    void modifySerialConnection(int index,QString port,int baud);
    void removeSerialConnection(int index);
    void connectLink(int index);
    void disconnectLink(int index);
    UASInterface* getUas(int id);
    UASInterface* createUAS(New_MAVLinkParser* mavlink, LinkInterface* link, int sysid, mavlink_heartbeat_t* heartbeat, QObject* parent=NULL);
    void addLink(LinkInterface *link);
    QList<LinkInterface*> getLinks();
    void removeLink(LinkInterface *link);
    LinkInterface::LinkType getLinkType(int linkid);
    bool getLinkConnected(int linkid);
    QString getSerialLinkPort(int linkid);
    QString getLinkName(int linkid);
    int getSerialLinkBaud(int linkid);
    int getUdpLinkPort(int linkid);
    void setUdpLinkPort(int linkid, int port);
    void addUdpHost(int linkid,QString hostname);
    QList<QString> getCurrentPorts();
private:
    QMap<int,LinkInterface*> m_connectionMap;
    QMap<int,UASInterface*> m_uasMap;
    New_MAVLinkDecoder *m_mavlinkDecoder;
    New_MAVLinkParser *m_mavlinkParser;
signals:
    //void newLink(LinkInterface* link);
    void newLink(int linkid);
    void protocolStatusMessage(QString title,QString text);
    void linkChanged(int linkid);
private slots:
    void linkConnected(LinkInterface* link);
    void linkDisonnected(LinkInterface* link);
    
public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);
    void protocolStatusMessageRec(QString title,QString text);
};

#endif // LINKMANAGER_H
