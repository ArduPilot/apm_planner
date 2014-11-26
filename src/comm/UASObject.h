#ifndef UASOBJECT_H
#define UASOBJECT_H

#include "LinkInterface.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"

#include "VehicleOverview.h"
#include "RelPositionOverview.h"
#include "AbsPositionOverview.h"
#include "MissionOverview.h"

#include <QObject>

class UASObject : public QObject
{
    Q_OBJECT
public:
    explicit UASObject(QObject *parent = 0);
    ~UASObject();

    VehicleOverview *getVehicleOverview();
    RelPositionOverview *getRelPositionOverview();
    AbsPositionOverview *getAbsPositionOverview();
    MissionOverview *getMissionOverview();

signals:
    // Define signals here

public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);

private:
    //mavlink_message_heartbeat_t lastHeartbeat;
    VehicleOverview* m_vehicleOverview;
    RelPositionOverview* m_relPositionOverview;
    AbsPositionOverview* m_absPositionOverview;
    MissionOverview* m_missionOverview;
};

#endif // UASOBJECT_H
