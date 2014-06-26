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
#include "MAVLinkDecoder.h"
#include "MAVLinkProtocol.h"
//#include "MAVLinkProtocol.h"
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
    ~LinkManager();
    void loadSettings();
    void saveSettings();
    int addSerialConnection(QString port,int baud);
    int addSerialConnection();
    int addUdpConnection(QHostAddress addr,int port);
    int addTcpConnection(QHostAddress addr,int port);
    void modifySerialConnection(int index,QString port,int baud);
    void modifyTcpConnection(int index,QHostAddress addr,int port);
    void removeSerialConnection(int index);
    void connectLink(int index);
    void disconnectLink(int index);
    UASInterface* getUas(int id);
    UASInterface* createUAS(MAVLinkProtocol* mavlink, LinkInterface* link, int sysid, mavlink_heartbeat_t* heartbeat, QObject* parent=NULL);
    void addLink(LinkInterface *link);
    QList<int> getLinks();
    void removeLink(LinkInterface *link);
    LinkInterface::LinkType getLinkType(int linkid);
    bool getLinkConnected(int linkid);
    QString getSerialLinkPort(int linkid);
    QString getLinkName(int linkid);
    int getSerialLinkBaud(int linkid);
    int getUdpLinkPort(int linkid);
    int getTcpLinkPort(int linkid);
    QHostAddress getTcpLinkHost(int linkid);
    void setUdpLinkPort(int linkid, int port);
    void addUdpHost(int linkid,QString hostname);
    QList<QString> getCurrentPorts();
    void stopLogging();
    void startLogging();
private:
    QMap<int,LinkInterface*> m_connectionMap;
    QMap<int,UASInterface*> m_uasMap;
    MAVLinkDecoder *m_mavlinkDecoder;
    MAVLinkProtocol *m_mavlinkParser;
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
