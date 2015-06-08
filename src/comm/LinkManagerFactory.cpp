#include "LinkManagerFactory.h"
#include "serialconnection.h"
#include "UDPLink.h"
#include "UDPClientLink.h"
#include "TCPLink.h"


void LinkManagerFactory::connectLinkSignals(LinkInterface *link, LinkManager *lmgr)
{
    connect(link,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),lmgr->getProtocol(),SLOT(receiveBytes(LinkInterface*,QByteArray)));
    connect(link,SIGNAL(connected(LinkInterface*)),lmgr,SLOT(linkConnected(LinkInterface*)));
    connect(link,SIGNAL(disconnected(LinkInterface*)),lmgr,SLOT(linkDisonnected(LinkInterface*)));
    connect(link,SIGNAL(error(LinkInterface*,QString)),lmgr,SLOT(linkErrorRec(LinkInterface*,QString)));
    connect(link, SIGNAL(linkChanged(LinkInterface*)),lmgr,SLOT(linkUpdated(LinkInterface*)));
}

int LinkManagerFactory::addSerialConnection()
{
    LinkManager *lmgr = LinkManager::instance();
    SerialConnection *link = new SerialConnection();
    connectLinkSignals(link, lmgr);

    connect(link,SIGNAL(timeoutTriggered(LinkInterface*)),lmgr,SLOT(linkTimeoutTriggered(LinkInterface*)));

    lmgr->addLink(link);
    return link->getId();
}

int LinkManagerFactory::addSerialConnection(QString port,int baud)
{
    LinkManager *lmgr = LinkManager::instance();
    SerialConnection *link = new SerialConnection();
    connectLinkSignals(link, lmgr);

    connect(link,SIGNAL(timeoutTriggered(LinkInterface*)),lmgr,SLOT(linkTimeoutTriggered(LinkInterface*)));

    link->setPortName(port);
    link->setBaudRate(baud);

    lmgr->addLink(link);
    return link->getId();

}
int LinkManagerFactory::addUdpConnection(QHostAddress addr,int port)
{
    LinkManager *lmgr = LinkManager::instance();
    UDPLink* link = new UDPLink(addr,port);
    connectLinkSignals(link, lmgr);

    lmgr->addLink(link);
    link->connect();
    return link->getId();

}

int LinkManagerFactory::addUdpClientConnection(QHostAddress addr,int port)
{
    LinkManager *lmgr = LinkManager::instance();
    UDPClientLink* link = new UDPClientLink(addr,port);
    connectLinkSignals(link, lmgr);

    lmgr->addLink(link);
    return link->getId();
}

int LinkManagerFactory::addTcpConnection(QHostAddress addr,int port,bool asServer)
{
    LinkManager *lmgr = LinkManager::instance();

    TCPLink *link = new TCPLink(addr,port,asServer);

    connectLinkSignals(link, lmgr);

    lmgr->addLink(link);
    if (asServer)
    {
        link->connect();
    }
    return link->getId();
}

