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
#include "MAVLinkDecoder.h"
#include "MAVLinkProtocol.h"
//#include "MAVLinkProtocol.h"
#include <QMap>
#include "UASInterface.h"
#include "UAS.h"
#include "UASObject.h"
class LinkManager : public QObject
{
    Q_OBJECT
public:
    explicit LinkManager(QObject *parent = 0);
    static LinkManager* instance();
    ~LinkManager();

    void disableTimeouts(int index);
    void enableTimeouts(int index);
    void disableAllTimeouts();
    void enableAllTimeouts();

    MAVLinkProtocol* getProtocol() const;
    bool connectLink(int index);
    void disconnectLink(int index);

    UASInterface* getUas(int id);
    UASInterface* createUAS(MAVLinkProtocol* mavlink, LinkInterface* link, int sysid, mavlink_heartbeat_t* heartbeat, QObject* parent=NULL);

    void addLink(LinkInterface *link);
    QList<int> getLinks();

    LinkInterface* getLink(int linkId);
    // Remove a link based on instance
    void removeLink(LinkInterface *link);
    // Remove a link based on unique id
    void removeLink(int linkId);

    LinkInterface::LinkType getLinkType(int linkid);
    bool getLinkConnected(int linkid);

    QString getSerialLinkPort(int linkid); // [TODO] remove
    QString getLinkName(int linkid); // [TODO] remove
    QString getLinkShortName(int linkid); // [TODO] remove
    QString getLinkDetail(int linkid); // [TODO] remove
    int getSerialLinkBaud(int linkid); // [TODO] remove

    QList<QString> getCurrentPorts();
    void stopLogging();
    void startLogging();
    void setLogSubDirectory(QString dir);
    bool loggingEnabled();
    UASObject *getUasObject(int uasid);
    QMap<int,UASObject*> m_uasObjectMap; // [TODO] make private

    void addSimObject(uint8_t sysid,UASObject *obj); // [TODO] remove
    void removeSimObject(uint8_t sysid); // [TODO] remove

signals:
    //void newLink(LinkInterface* link);
    void newLink(int linkid);
    void protocolStatusMessage(QString title,QString text);
    void linkChanged(int linkid);

    /** @brief aggregated signal for when link status changes */
    void linkChanged(LinkInterface *link);

    void linkError(int linkid, QString message);
    void messageReceived(LinkInterface* link,mavlink_message_t message);

public slots:
    void receiveMessage(LinkInterface* link,mavlink_message_t message);
    void protocolStatusMessageRec(QString title,QString text);
    void enableLogging(bool enabled);
    void reloadSettings();
    void linkUpdated(LinkInterface* link);

private slots:
    void linkConnected(LinkInterface* link);
    void linkDisonnected(LinkInterface* link);
    void linkErrorRec(LinkInterface* link,QString error);
    void linkTimeoutTriggered(LinkInterface*);

private:
    void loadSettings();
    void saveSettings();

private:
    QMap<int,LinkInterface*> m_connectionMap;
    QMap<int,UASInterface*> m_uasMap;
    QMap<QString,int> m_portToBaudMap;
    MAVLinkDecoder *m_mavlinkDecoder;
    MAVLinkProtocol *m_mavlinkProtocol;
    QString m_logSubDir;
    bool m_mavlinkLoggingEnabled;
};

#endif // LINKMANAGER_H
