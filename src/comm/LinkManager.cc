/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Brief Description
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */
#include "QsLog.h"
#include "LinkManager.h"
#include <QList>
#include <QApplication>
#include <iostream>



LinkManager* LinkManager::instance()
{
    static LinkManager* _instance = 0;
    if(_instance == 0) {
        _instance = new LinkManager();

        /* Set the application as parent to ensure that this object
         * will be destroyed when the main application exits */
        _instance->setParent(qApp);
    }
    return _instance;
}

/**
 * @brief Private singleton constructor
 *
 * This class implements the singleton design pattern and has therefore only a private constructor.
 **/
LinkManager::LinkManager()
{
    links = QList<LinkInterface*>();
    protocolLinks = QMap<ProtocolInterface*, LinkInterface*>();
}

LinkManager::~LinkManager()
{
    disconnectAll();
    foreach (LinkInterface* link, links)
    {
        if(link) link->deleteLater();
    }
}

void LinkManager::add(LinkInterface* link)
{
    QThread* thread = QThread::currentThread();
    QLOG_DEBUG() << "LinkManager::Add " << link
                 << "Thread " << thread;
    if (link && !links.contains(link))
    {
        QLOG_DEBUG() << "Sucess: Added " << link;
        connect(link, SIGNAL(destroyed(QObject*)), this, SLOT(removeObj(QObject*)));
        links.append(link);
        emit newLink(link);
    } else {
        QLOG_DEBUG() << "duplicate (not added)" << link;
    }
}

void LinkManager::addProtocol(LinkInterface* link, ProtocolInterface* protocol)
{
    // Connect link to protocol
    // the protocol will receive new bytes from the link
    QLOG_DEBUG() << "LinkManager::addProtocol link:" << link << "protocol" << protocol;
    if(!link || !protocol) return;

    QList<LinkInterface*> linkList = protocolLinks.values(protocol);

    // If protocol has not been added before (list length == 0)
    // OR if link has not been added to protocol, add
    if (linkList.length() == 0 || !linkList.contains(link))
    {
        // Protocol is new, add
        connect(link, SIGNAL(bytesReceived(LinkInterface*, QByteArray)),
                protocol, SLOT(receiveBytes(LinkInterface*, QByteArray)), Qt::QueuedConnection);
        // Store the connection information in the protocol links map
        protocolLinks.insertMulti(protocol, link);
    }
    QLOG_INFO() << "ADDED LINK TO PROTOCOL" << link->getName() << protocol->getName() << "protocolLinks.size:"
                << protocolLinks.size() << "Links.size:" << links.size();
}

QList<LinkInterface*> LinkManager::getLinksForProtocol(ProtocolInterface* protocol)
{
    return protocolLinks.values(protocol);
}


bool LinkManager::connectAll()
{
    QLOG_DEBUG() << "LinkManager::connectAll()";
    bool allConnected = true;

    foreach (LinkInterface* link, links)
    {
        if(!link) {}
        else if(!link->connect()) allConnected = false;
    }

    return allConnected;
}

bool LinkManager::disconnectAll()
{
    QLOG_DEBUG() << "LinkManager::disconnectAll()";
    bool allDisconnected = true;

    foreach (LinkInterface* link, links)
    {
        //static int i=0;
        if(!link) {}
        else if(!link->disconnect()) allDisconnected = false;
    }

    return allDisconnected;
}

bool LinkManager::connectLink(LinkInterface* link)
{
    QLOG_DEBUG() << "LinkManager::connectLink " << link;
    if(!link) return false;
    return link->connect();
}

bool LinkManager::disconnectLink(LinkInterface* link)
{
    QLOG_DEBUG() << "LinkManager::disconnectLink " << link;
    if(!link) return false;
    return link->disconnect();
}

void LinkManager::removeObj(QObject* link)
{
    QLOG_DEBUG() << "LinkManager::removeObj " << link;
    LinkInterface* linkInterface = dynamic_cast<LinkInterface*>(link);
    if (linkInterface)
    {
        removeLink(linkInterface);
    }
}

bool LinkManager::removeLink(LinkInterface* link)
{

    if(link)
    {
        for (int i=0; i < QList<LinkInterface*>(links).size(); i++)
        {
            if(link==links.at(i))
            {
                links.removeAt(i); //remove from link list
            }
        }
        // Remove link from protocol map
        QList<ProtocolInterface* > protocols = protocolLinks.keys(link);
        foreach (ProtocolInterface* proto, protocols)
        {
            protocolLinks.remove(proto, link);
        }

        // Emit removal of link
        emit linkRemoved(link);

        return true;
    }
    return false;
}

/**
 * The access time is linear in the number of links.
 *
 * @param id link identifier to search for
 * @return A pointer to the link or NULL if not found
 */
LinkInterface* LinkManager::getLinkForId(int id)
{
    QLOG_DEBUG() << "LinkManager::getLinkForId " << id;
    foreach (LinkInterface* link, links)
    {
        if (link->getId() == id) return link;
    }
    return NULL;
}

/**
 *
 */
const QList<LinkInterface*> LinkManager::getLinks()
{
    return QList<LinkInterface*>(links);
}
