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


#include "LinkManager.h"
#include "PxQuadMAV.h"
#include "SlugsMAV.h"
#include "ArduPilotMegaMAV.h"
#include "UASManager.h"
#include "UDPLink.h"
#include "TCPLink.h"
#include <QSettings>
#include "qserialportinfo.h"
LinkManager::LinkManager(QObject *parent) :
    QObject(parent)
{
    m_mavlinkLoggingEnabled = true;
    m_mavlinkDecoder = new MAVLinkDecoder(this);
    m_mavlinkParser = new MAVLinkProtocol(this);
    m_mavlinkParser->setConnectionManager(this);
    connect(m_mavlinkParser,SIGNAL(messageReceived(LinkInterface*,mavlink_message_t)),m_mavlinkDecoder,SLOT(receiveMessage(LinkInterface*,mavlink_message_t)));
    connect(m_mavlinkParser,SIGNAL(protocolStatusMessage(QString,QString)),this,SLOT(protocolStatusMessageRec(QString,QString)));
    loadSettings();
    //Check to see if we have a single serial and single UDP connection, since they are the defaults

    /*//LinkManager::instance()->addSerialConnection();
    //LinkManager::instance()->addUdpConnection(QHostAddress::Any,14550);*/
    bool foundserial = false;
    bool foundudp = false;
    for (QMap<int,LinkInterface*>::const_iterator i= m_connectionMap.constBegin();i!=m_connectionMap.constEnd();i++)
    {
        if (i.value()->getLinkType() == LinkInterface::SERIAL_LINK)
        {
            foundserial = true;
        }
        if (i.value()->getLinkType() == LinkInterface::UDP_LINK)
        {
            foundudp = true;
        }
    }
    if (!foundserial)
    {
        addSerialConnection();
    }
    if (!foundudp)
    {
        addUdpConnection(QHostAddress::Any,14550);
    }
}
void LinkManager::stopLogging()
{
    if (!m_mavlinkLoggingEnabled)
    {
        return;
    }
    m_mavlinkParser->stopLogging();
}
LinkManager::~LinkManager()
{
    saveSettings();
}

void LinkManager::loadSettings()
{
    QSettings settings;
    settings.beginGroup("LINKMANAGER");
    m_mavlinkLoggingEnabled = settings.value("LOGGING",true).toBool();
    int linkssize = settings.beginReadArray("LINKS");
    for (int i=0;i<linkssize;i++)
    {
        settings.setArrayIndex(i);
        int linkid = settings.value("linkid").toInt();
        QString type = settings.value("type").toString();
        if (type == "SERIAL_LINK")
        {
            QString port = settings.value("port").toString();
            int baud = settings.value("baud").toInt();
            if (baud < 0 || baud > 12500000)
            {
                //Bad baud rate.
                baud = 115200;
            }

            addSerialConnection(port,baud);
        }
        else if (type == "UDP_LINK")
        {
            int hostcount = settings.beginReadArray("HOSTS");
            if (hostcount == 0)
            {
                //Use defaults
                addUdpConnection(QHostAddress::Any,14550);
            }
            else
            {
                settings.setArrayIndex(0);
                QString host = settings.value("host").toString();
                int port = settings.value("port").toInt();
                addUdpConnection(QHostAddress(host),port);
            }
            settings.endArray();
        }
        else if (type == "TCP_LINK")
        {
            QString host = settings.value("host").toString();
            int port = settings.value("port").toInt();
            addTcpConnection(QHostAddress(host),port);
        }
    }

    int portsize = settings.beginReadArray("PORTBAUDPAIRS");
    for (int i=0;i<portsize;i++)
    {
        settings.setArrayIndex(i);
        m_portToBaudMap[settings.value("port").toString()] = settings.value("baud").toInt();
    }
    settings.endArray();
    settings.endArray();
}

void LinkManager::saveSettings()
{
    QSettings settings;
    settings.beginGroup("LINKMANAGER");
    settings.setValue("LOGGING",m_mavlinkLoggingEnabled);
    settings.beginWriteArray("LINKS");
    int index = 0;
    for (QMap<int,LinkInterface*>::const_iterator i= m_connectionMap.constBegin();i!=m_connectionMap.constEnd();i++)
    {
        settings.setArrayIndex(index++);
        settings.setValue("linkid",i.value()->getId());
        if (i.value()->getLinkType() == LinkInterface::SERIAL_LINK)
        {
            SerialConnection *link = qobject_cast<SerialConnection*>(i.value());
            settings.setValue("type","SERIAL_LINK");
            settings.setValue("port",link->getPortName());
            settings.setValue("baud",link->getBaudRate());
        }
        else if (i.value()->getLinkType() == LinkInterface::UDP_LINK)
        {
            UDPLink *link = qobject_cast<UDPLink*>(i.value());
            settings.setValue("type","UDP_LINK");
            settings.beginWriteArray("HOSTS");
            for (int j=0;j<link->getHosts().size();j++)
            {
                settings.setArrayIndex(j);
                settings.setValue("host",link->getHosts().at(j).toString());
                settings.setValue("port",link->getPort());
            }
            settings.endArray();
            settings.setValue("port",link->getPort());
        }
        else if (i.value()->getLinkType() == LinkInterface::TCP_LINK)
        {
            TCPLink *link = qobject_cast<TCPLink*>(i.value());
            settings.setValue("type","TCP_LINK");
            settings.setValue("host",link->getHostAddress().toString());
            settings.setValue("port",link->getPort());
        }
    }
    settings.endArray();
    settings.beginWriteArray("PORTBAUDPAIRS");
    index = 0;
    for (QMap<QString,int>::const_iterator i=m_portToBaudMap.constBegin();i!=m_portToBaudMap.constEnd();i++)
    {
        settings.setArrayIndex(index++);
        settings.setValue("port",i.key());
        settings.setValue("baud",i.value());
    }
    settings.endArray();
    settings.endGroup();
    settings.sync();
}
void LinkManager::setLogSubDirectory(QString dir)
{
    if (!dir.startsWith("/"))
    {
        m_logSubDir = "/" + dir;
    }
    else
    {
        m_logSubDir = dir;
    }
    if (!m_logSubDir.endsWith("/"))
    {
        m_logSubDir += "/";
    }
    QDir logdir(QGC::MAVLinkLogDirectory());
    if (!logdir.cd(m_logSubDir.mid(1)))
    {
        logdir.mkdir(m_logSubDir.mid(1));
    }
}
void LinkManager::enableLogging(bool enabled)
{

    if (enabled)
    {
        m_mavlinkLoggingEnabled = enabled;
        startLogging();
    }
    else
    {
        stopLogging();
        m_mavlinkLoggingEnabled = enabled;
    }
}

bool LinkManager::loggingEnabled()
{
    return m_mavlinkLoggingEnabled;
}

void LinkManager::startLogging()
{
    if (!m_mavlinkLoggingEnabled)
    {
        return;
    }
    QString logFileName = QGC::MAVLinkLogDirectory() + m_logSubDir + QGC::fileNameAsTime();
    QLOG_DEBUG() << "LinkManger::startLogging()" << logFileName;
    m_mavlinkParser->startLogging(logFileName);
}

int LinkManager::addSerialConnection()
{
    //Add with defaults
    SerialConnection *connection = new SerialConnection();
    connect(connection,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlinkParser,SLOT(receiveBytes(LinkInterface*,QByteArray)));
    connect(connection,SIGNAL(connected(LinkInterface*)),this,SLOT(linkConnected(LinkInterface*)));
    connect(connection,SIGNAL(disconnected(LinkInterface*)),this,SLOT(linkDisonnected(LinkInterface*)));
    connect(connection,SIGNAL(error(LinkInterface*,QString)),this,SLOT(linkErrorRec(LinkInterface*,QString)));
    m_connectionMap.insert(connection->getId(),connection);
    emit newLink(connection->getId());
    saveSettings();
    return connection->getId();
}
LinkInterface::LinkType LinkManager::getLinkType(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return LinkInterface::UNKNOWN_LINK;
    }
    return m_connectionMap.value(linkid)->getLinkType();
}

int LinkManager::addSerialConnection(QString port,int baud)
{
    SerialConnection *connection = new SerialConnection();
    connect(connection,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlinkParser,SLOT(receiveBytes(LinkInterface*,QByteArray)));
    connect(connection,SIGNAL(connected(LinkInterface*)),this,SLOT(linkConnected(LinkInterface*)));
    connect(connection,SIGNAL(disconnected(LinkInterface*)),this,SLOT(linkDisonnected(LinkInterface*)));
    connect(connection,SIGNAL(error(LinkInterface*,QString)),this,SLOT(linkErrorRec(LinkInterface*,QString)));
    connection->setPortName(port);
    connection->setBaudRate(baud);
    m_connectionMap.insert(connection->getId(),connection);
    //emit newLink(connection);
    emit newLink(connection->getId());
    saveSettings();
    return connection->getId();

}
int LinkManager::addUdpConnection(QHostAddress addr,int port)
{
    UDPLink* udpLink = new UDPLink(addr,port);
    connect(udpLink,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlinkParser,SLOT(receiveBytes(LinkInterface*,QByteArray)));
    connect(udpLink,SIGNAL(connected(LinkInterface*)),this,SLOT(linkConnected(LinkInterface*)));
    connect(udpLink,SIGNAL(disconnected(LinkInterface*)),this,SLOT(linkDisonnected(LinkInterface*)));
    connect(udpLink,SIGNAL(error(LinkInterface*,QString)),this,SLOT(linkErrorRec(LinkInterface*,QString)));
    udpLink->connect();
    m_connectionMap.insert(udpLink->getId(),udpLink);
    emit newLink(udpLink->getId());
    saveSettings();
    return udpLink->getId();

}
int LinkManager::addTcpConnection(QHostAddress addr,int port)
{
    TCPLink *tcplink = new TCPLink(addr,port);
    connect(tcplink,SIGNAL(bytesReceived(LinkInterface*,QByteArray)),m_mavlinkParser,SLOT(receiveBytes(LinkInterface*,QByteArray)));
    connect(tcplink,SIGNAL(connected(LinkInterface*)),this,SLOT(linkConnected(LinkInterface*)));
    connect(tcplink,SIGNAL(disconnected(LinkInterface*)),this,SLOT(linkDisonnected(LinkInterface*)));
    m_connectionMap.insert(tcplink->getId(),tcplink);
    emit newLink(tcplink->getId());
    saveSettings();
    return tcplink->getId();
}

void LinkManager::addLink(LinkInterface *link)
{
    m_connectionMap.insert(link->getId(),link);
}
void LinkManager::removeLink(LinkInterface *link)
{
   // This is called with a LINK_ID not an interface. needs mor rework
    //This function is not yet supported, it will be once we support multiple MAVs
}

void LinkManager::removeLink(int linkId)
{
    //This function is not yet supported, it will be once we support multiple MAVs
}

void LinkManager::connectLink(int index)
{
    if (m_connectionMap.contains(index))
    {
        if (!m_connectionMap.value(index)->connect())
        {
            //Can't connect, there will be a signal emitted.
        }
    }
}
void LinkManager::disconnectLink(int index)
{
    if (m_connectionMap.contains(index))
    {
        m_connectionMap.value(index)->disconnect();
    }
}
void LinkManager::modifyTcpConnection(int index,QHostAddress addr,int port)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    TCPLink *iface = qobject_cast<TCPLink*>(m_connectionMap.value(index));
    if (!iface)
    {
        return;
    }
    iface->setHostAddress(addr);
    iface->setPort(port);
    emit linkChanged(index);
    saveSettings();
}

void LinkManager::modifySerialConnection(int index,QString port,int baud)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(index));
    if (!iface)
    {
        return;
    }
    iface->setPortName(port);
    if (baud != -1)
    {
        iface->setBaudRate(baud);
        m_portToBaudMap[port] = baud;
    }
    else
    {
        //Check the map, if we've had a baud rate on that port before, use it
        int newbaud = iface->getBaudRate();
        if (m_portToBaudMap.contains(port))
        {
            newbaud = m_portToBaudMap.value(port);
        }
        iface->setBaudRate(newbaud);
        m_portToBaudMap[port] = newbaud;
    }

    emit linkChanged(index);
    saveSettings();
}
QString LinkManager::getLinkName(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return "";
    }
    return m_connectionMap.value(linkid)->getName();
}

QString LinkManager::getSerialLinkPort(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return "";
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return "";
    }
    return iface->getPortName();
}
bool LinkManager::getLinkConnected(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return "";
    }
    return m_connectionMap.value(linkid)->isConnected();
}

int LinkManager::getUdpLinkPort(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return 0;
    }
    UDPLink *iface = qobject_cast<UDPLink*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return 0;
    }
    return iface->getPort();
}
int LinkManager::getTcpLinkPort(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return 0;
    }
    TCPLink *iface = qobject_cast<TCPLink*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return 0;
    }
    return iface->getPort();
}

QHostAddress LinkManager::getTcpLinkHost(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return QHostAddress::Null;
    }
    TCPLink *iface = qobject_cast<TCPLink*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return QHostAddress::Null;
    }
    return iface->getHostAddress();
}

void LinkManager::setUdpLinkPort(int linkid, int port)
{
    if (!m_connectionMap.contains(linkid))
    {
        return;
    }
    UDPLink *iface = qobject_cast<UDPLink*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return;
    }
    iface->setPort(port);
    emit linkChanged(linkid);
    saveSettings();
}
void LinkManager::addUdpHost(int linkid,QString hostname)
{
    if (!m_connectionMap.contains(linkid))
    {
        return;
    }
    UDPLink *iface = qobject_cast<UDPLink*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return;
    }
    iface->addHost(hostname);
    saveSettings();
}

int LinkManager::getSerialLinkBaud(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return 0;
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(linkid));
    if (!iface)
    {
        return 0;
    }
    return iface->getBaudRate();
}
QList<QString> LinkManager::getCurrentPorts()
{
    QList<QString> m_portList;
    QList<QSerialPortInfo> portList =  QSerialPortInfo::availablePorts();

    if( portList.count() == 0){
        QLOG_INFO() << "No Ports Found" << m_portList;
    }

    foreach (const QSerialPortInfo &info, portList)
    {
        QLOG_TRACE() << "PortName    : " << info.portName()
                     << "Description : " << info.description();
        QLOG_TRACE() << "Manufacturer: " << info.manufacturer();

        m_portList.append(info.portName());
    }
    return m_portList;
}

void LinkManager::removeSerialConnection(int index)
{

}
void LinkManager::messageReceived(LinkInterface* link,mavlink_message_t message)
{

}
UASInterface* LinkManager::getUas(int id)
{
    if (m_uasMap.contains(id))
    {
        return m_uasMap.value(id);
    }
    return 0;
}
QList<int> LinkManager::getLinks()
{
    QList<int> links;
    for (int i=0;i<m_connectionMap.keys().size();i++)
    {
        links.append(m_connectionMap.value(m_connectionMap.keys().at(i))->getId());
    }
    return links;
}

UASInterface* LinkManager::createUAS(MAVLinkProtocol* mavlink, LinkInterface* link, int sysid, mavlink_heartbeat_t* heartbeat, QObject* parent)
{
    QPointer<QObject> p;

    if (parent != NULL)
    {
        p = parent;
    }
    else
    {
        p = mavlink;
    }

    UASInterface* uas;

    switch (heartbeat->autopilot)
    {
    case MAV_AUTOPILOT_GENERIC:
    {
        UAS* mav = new UAS(0, sysid);
        // Set the system type
        mav->setSystemType((int)heartbeat->type);
        // Connect this robot to the UAS object
        connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
#ifdef QGC_PROTOBUF_ENABLED
        connect(mavlink, SIGNAL(extendedMessageReceived(LinkInterface*, std::tr1::shared_ptr<google::protobuf::Message>)), mav, SLOT(receiveExtendedMessage(LinkInterface*, std::tr1::shared_ptr<google::protobuf::Message>)));
#endif
        uas = mav;
    }
    break;
    case MAV_AUTOPILOT_PIXHAWK:
    {
        PxQuadMAV* mav = new PxQuadMAV(0, sysid);
        // Set the system type
        mav->setSystemType((int)heartbeat->type);
        // Connect this robot to the UAS object
        // it is IMPORTANT here to use the right object type,
        // else the slot of the parent object is called (and thus the special
        // packets never reach their goal)
        connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
#ifdef QGC_PROTOBUF_ENABLED
        connect(mavlink, SIGNAL(extendedMessageReceived(LinkInterface*, std::tr1::shared_ptr<google::protobuf::Message>)), mav, SLOT(receiveExtendedMessage(LinkInterface*, std::tr1::shared_ptr<google::protobuf::Message>)));
#endif
        uas = mav;
    }
    break;
    case MAV_AUTOPILOT_SLUGS:
    {
        SlugsMAV* mav = new SlugsMAV(0, sysid);
        // Set the system type
        mav->setSystemType((int)heartbeat->type);
        // Connect this robot to the UAS object
        // it is IMPORTANT here to use the right object type,
        // else the slot of the parent object is called (and thus the special
        // packets never reach their goal)
        connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
        uas = mav;
    }
    break;
    case MAV_AUTOPILOT_ARDUPILOTMEGA:
    {
        ArduPilotMegaMAV* mav = new ArduPilotMegaMAV(0, sysid);
        // Set the system type
        mav->setSystemType((int)heartbeat->type);
        // Connect this robot to the UAS object
        // it is IMPORTANT here to use the right object type,
        // else the slot of the parent object is called (and thus the special
        // packets never reach their goal)
        connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
        uas = mav;
    }
    break;
#ifdef QGC_USE_SENSESOAR_MESSAGES
    case MAV_AUTOPILOT_SENSESOAR:
        {
            senseSoarMAV* mav = new senseSoarMAV(0,sysid);
            mav->setSystemType((int)heartbeat->type);
            connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
            uas = mav;
            break;
        }
#endif
    default:
    {
        UAS* mav = new UAS(0, sysid);
        mav->setSystemType((int)heartbeat->type);
        // Connect this robot to the UAS object
        // it is IMPORTANT here to use the right object type,
        // else the slot of the parent object is called (and thus the special
        // packets never reach their goal)
        connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
        uas = mav;
    }
    break;
    }

    m_uasMap.insert(sysid,uas);

    // Set the autopilot type
    uas->setAutopilotType((int)heartbeat->autopilot);

    // Make UAS aware that this link can be used to communicate with the actual robot
    uas->addLink(link);

    // Now add UAS to "official" list, which makes the whole application aware of it
    UASManager::instance()->addUAS(uas);

    return uas;

}
void LinkManager::setSerialParityType(int index,int parity)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(index));
    if (!iface)
    {
        return;
    }
    iface->setParityType(parity);
}

void LinkManager::setSerialFlowType(int index,int flow)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(index));
    if (!iface)
    {
        return;
    }
    iface->setFlowType(flow);
}
void LinkManager::setSerialDataBits(int index,int bits)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(index));
    if (!iface)
    {
        return;
    }
    iface->setDataBitsType(bits);
}

void LinkManager::setSerialStopBits(int index,int bits)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    SerialLinkInterface *iface = qobject_cast<SerialLinkInterface*>(m_connectionMap.value(index));
    if (!iface)
    {
        return;
    }
    iface->setStopBitsType(bits);
}

void LinkManager::protocolStatusMessageRec(QString title,QString text)
{
    emit protocolStatusMessage(title,text);
    QLOG_DEBUG() << "Protocol Status Message:" << title << text;
}
void LinkManager::linkConnected(LinkInterface* link)
{
    emit linkChanged(link->getId());
}

void LinkManager::linkDisonnected(LinkInterface* link)
{
    emit linkChanged(link->getId());
}
void LinkManager::linkErrorRec(LinkInterface *link,QString errorstring)
{
    emit linkError(link->getId(),errorstring);
}
