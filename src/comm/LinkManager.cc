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

#include "LinkManagerFactory.h"
#include "LinkManager.h"
#include "PxQuadMAV.h"
#include "SlugsMAV.h"
#include "ArduPilotMegaMAV.h"
#include "UASManager.h"
#include "serialconnection.h"
#include "UDPLink.h"
#include "UDPClientLink.h"
#include "TCPLink.h"
#include "UASObject.h"
#include <QApplication>
#include <QSettings>
#include <QtSerialPort/qserialportinfo.h>
#include <QTimer>


LinkManager* LinkManager::instance()
{
    static LinkManager* _instance = 0;
    if(_instance == 0)
    {
        _instance = new LinkManager();
    }
    return _instance;
}

LinkManager::LinkManager(QObject *parent) :
    QObject(parent)
{
    m_mavlinkLoggingEnabled = true;
    m_mavlinkDecoder = new MAVLinkDecoder(this);
    m_mavlinkProtocol = new MAVLinkProtocol();
    m_mavlinkProtocol->setConnectionManager(this);
    connect(m_mavlinkProtocol,SIGNAL(messageReceived(LinkInterface*,mavlink_message_t)),m_mavlinkDecoder,SLOT(receiveMessage(LinkInterface*,mavlink_message_t)));
    connect(m_mavlinkProtocol,SIGNAL(messageReceived(LinkInterface*,mavlink_message_t)),this,SLOT(receiveMessage(LinkInterface*,mavlink_message_t)));
    connect(m_mavlinkProtocol,SIGNAL(protocolStatusMessage(QString,QString)),this,SLOT(protocolStatusMessageRec(QString,QString)));

    QTimer::singleShot(500, this, SLOT(reloadSettings()));
}

void LinkManager::reloadSettings()
{
    loadSettings();
    //Check to see if we have a single serial and single UDP connection, since they are the defaults

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
        LinkManagerFactory::addSerialConnection();
    }
    if (!foundudp)
    {
        LinkManagerFactory::addUdpConnection(QHostAddress::Any,14550);
    }
}

void LinkManager::stopLogging()
{
    if (!m_mavlinkLoggingEnabled)
    {
        return;
    }
    m_mavlinkProtocol->stopLogging();
}

LinkManager::~LinkManager()
{
    m_mavlinkProtocol->setConnectionManager(NULL);
    delete m_mavlinkProtocol;
    m_mavlinkProtocol = NULL;
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

            LinkManagerFactory::addSerialConnection(port,baud);
        }
        else if (type == "UDP_LINK")
        {
            int port = settings.value("port").toInt();
            int linkid = LinkManagerFactory::addUdpConnection(QHostAddress::Any,port);
            UDPLink *iface = qobject_cast<UDPLink*>(getLink(linkid));

            int hostcount = settings.beginReadArray("HOSTS");
            for (int j=0;j<hostcount;++j)
            {
                settings.setArrayIndex(j);
                QString host = settings.value("host").toString();
                int port = settings.value("port").toInt();
                iface->addHost(tr("%1:%2").arg(host, port));
            }
            settings.endArray(); // HOSTS
        }
        else if (type == "TCP_LINK")
        {
            QHostAddress hostAddress = QHostAddress(settings.value("host").toString());
            QString hostName = settings.value("hostname").toString();
            int port = settings.value("port").toInt();
            bool asServer = settings.value("asServer").toBool();
            LinkManagerFactory::addTcpConnection(hostAddress, hostName, port, asServer);
        }
        else if (type == "UDP_CLIENT_LINK")
        {
            QString host = settings.value("host").toString();
            int port = settings.value("port").toInt();
            LinkManagerFactory::addUdpClientConnection(QHostAddress(host),port);
        }
    }
    settings.endArray(); // HOSTS
    int portsize = settings.beginReadArray("PORTBAUDPAIRS");
    for (int i=0;i<portsize;i++)
    {
        settings.setArrayIndex(i);
        m_portToBaudMap[settings.value("port").toString()] = settings.value("baud").toInt();
    }
    settings.endArray(); // PORTBAUDPAIRS
    settings.endGroup();
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
                settings.setValue("port",link->getPorts().at(j));
            }
            settings.endArray();
            settings.setValue("port",link->getPort());
        }
        else if (i.value()->getLinkType() == LinkInterface::UDP_CLIENT_LINK)
        {
            UDPClientLink *link = qobject_cast<UDPClientLink*>(i.value());
            settings.setValue("type","UDP_CLIENT_LINK");
            settings.setValue("host",link->getHostAddress().toString());
            settings.setValue("port",link->getPort());
        }
        else if (i.value()->getLinkType() == LinkInterface::TCP_LINK)
        {
            TCPLink *link = qobject_cast<TCPLink*>(i.value());
            settings.setValue("type","TCP_LINK");
            settings.setValue("host",link->getHostAddress().toString());
            settings.setValue("hostname",link->getName());
            settings.setValue("port",link->getPort());
            settings.setValue("asServer",link->isServer());
        }
    }
    settings.endArray(); // LINKS
    settings.beginWriteArray("PORTBAUDPAIRS");
    index = 0;
    for (QMap<QString,int>::const_iterator i=m_portToBaudMap.constBegin();i!=m_portToBaudMap.constEnd();i++)
    {
        settings.setArrayIndex(index++);
        settings.setValue("port",i.key());
        settings.setValue("baud",i.value());
    }
    settings.endArray(); // PORTBAUDPAIRS
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
    m_mavlinkProtocol->startLogging(logFileName);
}


MAVLinkProtocol* LinkManager::getProtocol() const
{
    return m_mavlinkProtocol;
}

LinkInterface::LinkType LinkManager::getLinkType(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return LinkInterface::UNKNOWN_LINK;
    }
    return m_connectionMap.value(linkid)->getLinkType();
}


void LinkManager::addLink(LinkInterface *link)
{
    m_connectionMap.insert(link->getId(),link);
    emit newLink(link->getId());
//    saveSettings();
}

LinkInterface* LinkManager::getLink(int linkId)
{
    return m_connectionMap.value(linkId, 0);
}

void LinkManager::removeLink(LinkInterface *link)
{
   // This is called with a LINK_ID not an interface. needs mor rework
    //This function is not yet supported, it will be once we support multiple MAVs
    Q_ASSERT(link == NULL); // This shoud not be called, assert if it anything but NULL
}

void LinkManager::removeLink(int linkId)
{
    if (m_connectionMap.contains(linkId))
    {
        if (m_connectionMap.value(linkId)->isConnected())
        {
            m_connectionMap.value(linkId)->disconnect();
        }
        delete m_connectionMap.value(linkId);
        m_connectionMap.remove(linkId);
        saveSettings();
    }
}

bool LinkManager::connectLink(int index)
{
    if (m_connectionMap.contains(index))
    {
        return m_connectionMap.value(index)->connect();
    }
    return false;
}

void LinkManager::disconnectLink(int index)
{
    if (m_connectionMap.contains(index))
    {
        m_connectionMap.value(index)->disconnect();
    }
}

void LinkManager::linkUpdated(LinkInterface *link)
{
    emit linkChanged(link);
    emit linkChanged(link->getId());
    saveSettings(); // [todo] may need to verify if this is needed always (refactor to link objects)
}

QString LinkManager::getLinkName(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return "";
    }
    return m_connectionMap.value(linkid)->getName();
}

QString LinkManager::getLinkShortName(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return "";
    }
    return m_connectionMap.value(linkid)->getShortName();
}

QString LinkManager::getLinkDetail(int linkid)
{
    if (!m_connectionMap.contains(linkid))
    {
        return "";
    }

    return m_connectionMap.value(linkid)->getDetail();
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

void LinkManager::receiveMessage(LinkInterface* link,mavlink_message_t message)
{
    emit messageReceived(link,message);
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
void LinkManager::addSimObject(uint8_t sysid,UASObject *obj)
{
    m_uasObjectMap[sysid] = obj;
    obj->moveToThread(QApplication::instance()->thread());
}
void LinkManager::removeSimObject(uint8_t sysid)
{
    m_uasObjectMap.remove(sysid);
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
//    case MAV_AUTOPILOT_PX4:
//    {
//        PxQuadMAV* mav = new PxQuadMAV(0, sysid);
//        // Set the system type
//        mav->setSystemType((int)heartbeat->type);
//        // Connect this robot to the UAS object
//        // it is IMPORTANT here to use the right object type,
//        // else the slot of the parent object is called (and thus the special
//        // packets never reach their goal)
//        connect(mavlink, SIGNAL(messageReceived(LinkInterface*, mavlink_message_t)), mav, SLOT(receiveMessage(LinkInterface*, mavlink_message_t)));
//#ifdef QGC_PROTOBUF_ENABLED
//        connect(mavlink, SIGNAL(extendedMessageReceived(LinkInterface*, std::tr1::shared_ptr<google::protobuf::Message>)), mav, SLOT(receiveExtendedMessage(LinkInterface*, std::tr1::shared_ptr<google::protobuf::Message>)));
//#endif
//        uas = mav;
//    }
//    break;
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

    UASObject *obj = new UASObject();
    connect(mavlink,SIGNAL(messageReceived(LinkInterface*,mavlink_message_t)),obj,SLOT(messageReceived(LinkInterface*,mavlink_message_t)));
    m_uasObjectMap[sysid] = obj;

    m_uasMap.insert(sysid,uas);

    // Set the autopilot type
    uas->setAutopilotType((int)heartbeat->autopilot);

    // Make UAS aware that this link can be used to communicate with the actual robot
    uas->addLink(link);

    // Now add UAS to "official" list, which makes the whole application aware of it
    UASManager::instance()->addUAS(uas);

    return uas;
}

UASObject *LinkManager::getUasObject(int uasid)
{
    if (m_uasObjectMap.contains(uasid))
    {
        return m_uasObjectMap.value(uasid);
    }
    return 0;
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
    QLOG_DEBUG() << "LinkManager::linkDisonnected: " << link->getName() << link->getId();
    emit linkChanged(link->getId());
}

void LinkManager::linkErrorRec(LinkInterface *link,QString errorstring)
{
    emit linkError(link->getId(),errorstring);
}

void LinkManager::linkTimeoutTriggered(LinkInterface *link)
{
    Q_UNUSED(link)
    //Link has had a timeout
    //Disabled until it is fixed and more more robust - MLC
    //emit linkError(link->getId(),"Connected to link, but unable to receive any mavlink packets, (link is silent). Disconnecting");
    //link->disconnect();
}

void LinkManager::disableTimeouts(int index)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    m_connectionMap.value(index)->disableTimeouts();
}

void LinkManager::enableTimeouts(int index)
{
    if (!m_connectionMap.contains(index))
    {
        return;
    }
    m_connectionMap.value(index)->enableTimeouts();
}

void LinkManager::disableAllTimeouts()
{
    for (QMap<int,LinkInterface*>::const_iterator i = m_connectionMap.constBegin(); i != m_connectionMap.constEnd();i++)
    {
        i.value()->disableTimeouts();
    }
}

void LinkManager::enableAllTimeouts()
{
    for (QMap<int,LinkInterface*>::const_iterator i = m_connectionMap.constBegin(); i != m_connectionMap.constEnd();i++)
    {
        i.value()->disableTimeouts();
    }
}
