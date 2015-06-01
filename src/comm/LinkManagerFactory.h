#ifndef LINKMANAGERFACTORY_H
#define LINKMANAGERFACTORY_H

#include "LinkManager.h"
#include <QObject>

class LinkManagerFactory : public QObject
{
//    Q_OBJECT
public:
//    explicit LinkManagerFactory(QObject *parent = 0);
//    ~LinkManagerFactory();

    // Serial Links
    static int addSerialConnection(QString port,int baud);
    static int addSerialConnection();

    // IP Links
    static int addUdpConnection(QHostAddress addr,int port);
    static int addUdpClientConnection(QHostAddress addr,int port);
    static int addTcpConnection(QHostAddress addr,int port,bool asServer);

private:
    static void connectLinkSignals(LinkInterface *link, LinkManager *lmgr);
};

#endif // LINKMANAGERFACTORY_H
