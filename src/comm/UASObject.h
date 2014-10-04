#ifndef UASOBJECT_H
#define UASOBJECT_H

#include <QObject>
#include "LinkInterface.h"
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include "RelPositionOverview.h"
#include "AbsPositionOverview.h"

/*#include "libs/mavlink/include/mavlink/v1.0-qt/ardupilotmega/mavlink_message_ahrs.h"
#include "libs/mavlink/include/mavlink/v1.0-qt/common/mavlink_message_global_position_int.h"
#include "libs/mavlink/include/mavlink/v1.0-qt/common/mavlink_message_local_position_ned.h"
#include "libs/mavlink/include/mavlink/v1.0-qt/common/mavlink_message_vfr_hud.h"
#include "libs/mavlink/include/mavlink/v1.0-qt/common/mavlink_message_hil_controls.h"
#include "libs/mavlink/include/mavlink/v1.0-qt/common/mavlink_message_attitude.h"*/
#include "VehicleOverview.h"
class UASObject : public QObject
{
    Q_OBJECT
public:

    class MissionOverview
    {
        //MISSION_ITEM
        //MISSION_CURRENT
        //MISSION_COUNT
        //MISSION_ITEM_REACHED
    };
    class ServosRcOverview
    {
        //rc_channels_scaled
        //rc_channels_raw
        //servo_output_raw
        //RC_CHANNELS
    };

    explicit UASObject(QObject *parent = 0);
    VehicleOverview *getVehicleOverview() { return m_vehicleOverview; }
    RelPositionOverview *getRelPositionOverview() { return m_relPositionOverview; }
    AbsPositionOverview *getAbsPositionOverview() { return m_absPositionOverview; }
private slots:
private:
    //mavlink_message_heartbeat_t lastHeartbeat;
    VehicleOverview *m_vehicleOverview;
    RelPositionOverview *m_relPositionOverview;
    AbsPositionOverview *m_absPositionOverview;
signals:

   /*void ahrsReceived(QSharedPointer<mavlink_message_ahrs_t>);
    void globalPositionReceived(QSharedPointer<mavlink_message_global_position_int_t>);
    void localPositionNedReceived(QSharedPointer<mavlink_message_local_position_ned_t>);
    void vfrHudReceived(QSharedPointer<mavlink_message_vfr_hud_t>);
    void hilControlsReceived(QSharedPointer<mavlink_message_hil_controls_t>);
    void attitudeReceived(QSharedPointer<mavlink_message_attitude_t>);*/
public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);
};

#endif // UASOBJECT_H
