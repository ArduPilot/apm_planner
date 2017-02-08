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
 *   @brief Definition of Unmanned Aerial Vehicle object
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef _UAS_H_
#define _UAS_H_

#include "UASInterface.h"
#include <MAVLinkProtocol.h>
#include <QVector3D>
#include "QGCMAVLink.h"
#include "QGCHilLink.h"
#include "QGCFlightGearLink.h"
#include "QGCJSBSimLink.h"
//#include "QGCXPlaneLink.h"

/**
 * @brief A generic MAVLINK-connected MAV/UAV
 *
 * This class represents one vehicle. It can be used like the real vehicle, e.g. a call to halt()
 * will automatically send the appropriate messages to the vehicle. The vehicle state will also be
 * automatically updated by the comm architecture, so when writing code to e.g. control the vehicle
 * no knowledge of the communication infrastructure is needed.
 */
class UAS : public UASInterface
{
    Q_OBJECT
public:
    UAS(MAVLinkProtocol* protocol,int id = 0);
    ~UAS();

    static const double lipoFull;  ///< 100% charged voltage
    static const double lipoEmpty; ///< Discharged voltage

    /* MANAGEMENT */

    /** @brief The name of the robot */
    QString getUASName(void) const;
    /** @brief Get short state */
    const QString& getShortState() const;
    /** @brief Get short mode */
    const QString& getShortMode() const;
    /** @brief Translate from mode id to text */
    static QString getShortModeTextFor(int id);
    /** @brief Translate from mode id to audio text */
    static QString getAudioModeTextFor(int id);

    /** @brief Get the human-speakable custom mode string */
    int getCustomMode();

    /** @brief Get the unique system id */
    int getUASID() const;
    /** @brief Get the airframe */
    int getAirframe() const
    {
        return airframe;
    }
    /** @brief Get the components */
    QMap<int, QString> getComponents();

    /** @brief The time interval the robot is switched on */
    quint64 getUptime() const;
    /** @brief Get the status flag for the communication */
    int getCommunicationStatus() const;
    /** @brief Add one measurement and get low-passed voltage */
    double filterVoltage(double value) const;
    /** @brief Get the links associated with this robot */
    QList<LinkInterface*>* getLinks();
    QList<int> getLinkIdList();

    Q_PROPERTY(double localX READ getLocalX WRITE setLocalX NOTIFY localXChanged)
    Q_PROPERTY(double localY READ getLocalY WRITE setLocalY NOTIFY localYChanged)
    Q_PROPERTY(double localZ READ getLocalZ WRITE setLocalZ NOTIFY localZChanged)
    Q_PROPERTY(double latitude READ getLatitude WRITE setLatitude NOTIFY latitudeChanged)
    Q_PROPERTY(double longitude READ getLongitude WRITE setLongitude NOTIFY longitudeChanged)
    Q_PROPERTY(double satelliteCount READ getSatelliteCount WRITE setSatelliteCount NOTIFY satelliteCountChanged)
    Q_PROPERTY(bool isLocalPositionKnown READ localPositionKnown)
    Q_PROPERTY(bool isGlobalPositionKnown READ globalPositionKnown)
    Q_PROPERTY(double roll READ getRoll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(double pitch READ getPitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(double yaw READ getYaw WRITE setYaw NOTIFY yawChanged)
    Q_PROPERTY(double distToWaypoint READ getDistToWaypoint WRITE setDistToWaypoint NOTIFY distToWaypointChanged)
    Q_PROPERTY(double airSpeed READ getGroundSpeed WRITE setGroundSpeed NOTIFY airSpeedChanged)
    Q_PROPERTY(double groundSpeed READ getGroundSpeed WRITE setGroundSpeed NOTIFY groundSpeedChanged)
    Q_PROPERTY(double bearingToWaypoint READ getBearingToWaypoint WRITE setBearingToWaypoint NOTIFY bearingToWaypointChanged)
    Q_PROPERTY(double altitudeAMSL READ getAltitudeAMSL WRITE setAltitudeAMSL NOTIFY altitudeAMSLChanged)
    Q_PROPERTY(double altitudeRelative READ getAltitudeRelative WRITE setAltitudeRelative NOTIFY altitudeRelativeChanged)

    void setGroundSpeed(double val)
    {
        groundSpeed = val;
        emit groundSpeedChanged(val,"groundSpeed");
        QString unitName = "Ground Speed";
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"m/s",QVariant(val),getUnixTime());
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"km/h",QVariant(val*3.6),getUnixTime());
        emit valueChanged(this->uasId,statusImperial().arg(unitName),"mi/h",QVariant(val*2.2369362921),getUnixTime());
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"kn",QVariant(val*1.9438444924),getUnixTime());
    }
    double getGroundSpeed() const
    {
        return groundSpeed;
    }

    void setAirSpeed(double val)
    {
        airSpeed = val;
        emit airSpeedChanged(val,"airSpeed");
        QString unitName = "Air Speed";
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"m/s",QVariant(val),getUnixTime());
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"km/h",QVariant(val*3.6),getUnixTime());
        emit valueChanged(this->uasId,statusImperial().arg(unitName),"mi/h",QVariant(val*2.2369362921),getUnixTime());
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"kn",QVariant(val*1.9438444924),getUnixTime());
    }

    double getAirSpeed() const
    {
        return airSpeed;
    }

    void setLocalX(double val)
    {
        localX = val;
        emit localXChanged(val,"localX");
        emit valueChanged(this->uasId,statusName().arg("localX"),"m",QVariant(val),getUnixTime());
    }

    double getLocalX() const
    {
        return localX;
    }

    void setLocalY(double val)
    {
        localY = val;
        emit localYChanged(val,"localY");
        emit valueChanged(this->uasId,statusName().arg("localY"),"m",QVariant(val),getUnixTime());
    }
    double getLocalY() const
    {
        return localY;
    }

    void setLocalZ(double val)
    {
        localZ = val;
        emit localZChanged(val,"localZ");
        emit valueChanged(this->uasId,statusName().arg("localZ"),"m",QVariant(val),getUnixTime());
    }
    double getLocalZ() const
    {
        return localZ;
    }

    void setLatitude(double val)
    {
        latitude = val;
        emit latitudeChanged(val,"latitude");
        emit valueChanged(this->uasId,statusName().arg("Latitude"),"deg",QVariant(val),getUnixTime());
    }

    double getLatitude() const
    {
        return latitude;
    }

    void setLongitude(double val)
    {
        longitude = val;
        emit longitudeChanged(val,"longitude");
        emit valueChanged(this->uasId,statusName().arg("Longitude"),"deg",QVariant(val),getUnixTime());
    }

    double getLongitude() const
    {
        return longitude;
    }


    void setAltitudeAMSL(double val)
    {
        altitudeAMSL = val;
        emit altitudeAMSLChanged(val, "altitudeAMSL");
        QString unitName = "Alt MSL";
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"m",QVariant(val),getUnixTime());
        emit valueChanged(this->uasId,statusImperial().arg(unitName),"ft",QVariant(val*3.280839895),getUnixTime());
    }

    double getAltitudeAMSL() const
    {
        return altitudeAMSL;
    }

    void setAltitudeRelative(double val)
    {
        altitudeRelative = val;
        emit altitudeRelativeChanged(val, "altitudeRelative");
        QString unitName = "Alt REL";
        emit valueChanged(this->uasId,statusMetric().arg(unitName),"m",QVariant(val),getUnixTime());
        emit valueChanged(this->uasId,statusImperial().arg(unitName),"ft",QVariant(val*3.280839895),getUnixTime());
    }

    double getAltitudeRelative() const
    {
        return altitudeRelative;
    }

    // GPS RAW values
    void setGPSLatitude(double val)
    {
        latitude_gps = val;
        emit valueChanged(this->uasId,statusGPS().arg("GPS Lat"),"deg",QVariant(val),getUnixTime());
    }

    double getGPSLatitude() const
    {
        return longitude_gps;
    }

    void setGPSLongitude(double val)
    {
        longitude_gps = val;
        emit valueChanged(this->uasId,statusGPS().arg("GPS Lng"),"deg",QVariant(val),getUnixTime());
    }

    double getGPSLongitude() const
    {
        return longitude_gps;
    }

    void setGPSAltitude(double val)
    {
        altitude_gps = val;
        QString unitName = "GPS Alt MSL";
        emit valueChanged(this->uasId,statusGPS().arg(unitName),"m",QVariant(val),getUnixTime());
        emit valueChanged(this->uasId,statusGPS().arg(unitName),"ft",QVariant(val*3.280839895),getUnixTime());
    }

    double getGPSAltitude() const
    {
        return altitude_gps;
    }

    void setGPSVelocity(double val)
    {
        velocity_gps = val;
        QString unitName = "GPS Velocity";
        emit valueChanged(this->uasId,statusGPS().arg(unitName),"m/s",QVariant(val),getUnixTime());
        emit valueChanged(this->uasId,statusGPS().arg(unitName),"km/h",QVariant(val*3.6),getUnixTime());
        emit valueChanged(this->uasId,statusGPS().arg(unitName),"mi/h",QVariant(val*2.2369362921),getUnixTime());
        emit valueChanged(this->uasId,statusGPS().arg(unitName),"kn",QVariant(val*1.9438444924),getUnixTime());
    }

    double getGPSVelocity() const
    {
        return velocity_gps;
    }

    void setSatelliteCount(double val)
    {
        m_satelliteCount = val;
        emit satelliteCountChanged(val,"satelliteCount");
        emit valueChanged(this->uasId,statusGPS().arg("GPS Sats"),"n",QVariant(val),getUnixTime());
    }

    int getSatelliteCount() const
    {
        return m_satelliteCount;
    }

    void setGpsHdop(double val)
    {
        m_gps_hdop = val;
        emit gpsHdopChanged(val,"GPS HDOP");
        emit valueChanged(this->uasId,statusGPS().arg("GPS HDOP"),"",QVariant(val),getUnixTime());
    }

    double getGpsHdop() const
    {
        return m_gps_hdop;
    }

    void setGpsFix(int val)
    {
        m_gps_fix = val;
        emit gpsFixChanged(val,"GPS Fix");
        emit valueChanged(this->uasId,statusGPS().arg("GPS Fix"),"",QVariant(val),getUnixTime());
    }

    double getGpsFix() const
    {
        return m_gps_fix;
    }

    QString getGpsFixString()
    {
        switch(m_gps_fix){
        case 5:
            return "3D+RTK";
        case 4:
            return "3D+DGPS";
        case 3:
            return "3D";
        case 2:
            return "2D";
        case 1:
            return "NO FIX";
        default:
            return ".";
        }
    }

    virtual bool localPositionKnown() const
    {
        return isLocalPositionKnown;
    }

    virtual bool globalPositionKnown() const
    {
        return isGlobalPositionKnown;
    }

    void setDistToWaypoint(double val)
    {
        distToWaypoint = val;
        emit distToWaypointChanged(val,"distToWaypoint");
        emit valueChanged(this->uasId,statusName().arg("distToWaypoint"),"m",QVariant(val),getUnixTime());
    }

    double getDistToWaypoint() const
    {
        return distToWaypoint;
    }

    void setBearingToWaypoint(double val)
    {
        bearingToWaypoint = val;
        emit bearingToWaypointChanged(val,"bearingToWaypoint");
        emit valueChanged(this->uasId,statusName().arg("bearingToWaypoint"),"deg",QVariant(val),getUnixTime());
    }

    double getBearingToWaypoint() const
    {
        return bearingToWaypoint;
    }


    void setRoll(double val)
    {
        roll = val;
        emit rollChanged(val,"roll");
    }

    double getRoll() const
    {
        return roll;
    }

    void setPitch(double val)
    {
        pitch = val;
        emit pitchChanged(val,"pitch");
    }

    double getPitch() const
    {
        return pitch;
    }

    void setYaw(double val)
    {
        yaw = val;
        emit yawChanged(val,"yaw");
    }

    double getYaw() const
    {
        return yaw;
    }

    bool getSelected() const;
    QVector3D getNedPosGlobalOffset() const
    {
        return nedPosGlobalOffset;
    }

    QVector3D getNedAttGlobalOffset() const
    {
        return nedAttGlobalOffset;
    }

    friend class UASWaypointManager;

protected: //COMMENTS FOR TEST UNIT
    bool m_heartbeatsEnabled;
    /// LINK ID AND STATUS
    int uasId;                    ///< Unique system ID
    QMap<int, QString> components;///< IDs and names of all detected onboard components
    QList<LinkInterface*>* links; ///< List of links this UAS can be reached by
    QList<int> unknownPackets;    ///< Packet IDs which are unknown and have been received
    //MAVLinkProtocol* mavlink;     ///< Reference to the MAVLink instance
    CommStatus commStatus;        ///< Communication status
    double receiveDropRate;        ///< Percentage of packets that were dropped on the MAV's receiving link (from GCS and other MAVs)
    double sendDropRate;           ///< Percentage of packets that were not received from the MAV by the GCS
    quint64 lastHeartbeat;        ///< Time of the last heartbeat message
    QTimer* statusTimeout;        ///< Timer for various status timeouts

    /// BASIC UAS TYPE, NAME AND STATE
    QString name;                 ///< Human-friendly name of the vehicle, e.g. bravo
    unsigned char type;           ///< UAS type (from type enum)
    int airframe;                 ///< The airframe type
    int autopilot;                ///< Type of the Autopilot: -1: None, 0: Generic, 1: PIXHAWK, 2: SLUGS, 3: Ardupilot (up to 15 types), defined in MAV_AUTOPILOT_TYPE ENUM
    bool systemIsArmed;           ///< If the system is armed
    uint8_t base_mode;                 ///< The current mode of the MAV
    uint32_t custom_mode;         ///< The current mode of the MAV
    int status;                   ///< The current status of the MAV
    QString shortModeText;        ///< Short textual mode description
    QString shortStateText;       ///< Short textual state description
    int systemId;                 ///< Currently connected mavlink system id
    int componentId;              ///< Currently connected mavlink component id
    int getSystemId() { return systemId; }
    int getComponentId() { return componentId; }

    /// OUTPUT
    QList<double> actuatorValues;
    QList<QString> actuatorNames;
    QList<double> motorValues;
    QList<QString> motorNames;
    double thrustSum;           ///< Sum of forward/up thrust of all thrust actuators, in Newtons
    double thrustMax;           ///< Maximum forward/up thrust of this vehicle, in Newtons

    // dongfang: This looks like a candidate for being moved off to a separate class.
    /// BATTERY / ENERGY
    BatteryType batteryType;    ///< The battery type
    int cells;                  ///< Number of cells
    double fullVoltage;          ///< Voltage of the fully charged battery (100%)
    double emptyVoltage;         ///< Voltage of the empty battery (0%)
    double startVoltage;         ///< Voltage at system start
    double tickVoltage;          ///< Voltage where 0.1 V ticks are told
    double lastTickVoltageValue; ///< The last voltage where a tick was announced
    double tickLowpassVoltage;   ///< Lowpass-filtered voltage for the tick announcement
    double warnVoltage;          ///< Voltage where QGC will start to warn about low battery
    double warnLevelPercent;     ///< Warning level, in percent
    double currentVoltage;      ///< Voltage currently measured
    double lpVoltage;            ///< Low-pass filtered voltage
    double currentCurrent;      ///< Battery current currently measured
    bool batteryRemainingEstimateEnabled; ///< If the estimate is enabled, QGC will try to estimate the remaining battery life
    double chargeLevel;          ///< Charge level of battery, in percent
    int timeRemaining;          ///< Remaining time calculated based on previous and current
    bool lowBattAlarm;          ///< Switch if battery is low


    /// TIMEKEEPING
    quint64 startTime;            ///< The time the UAS was switched on
    quint64 onboardTimeOffset;

    /// MANUAL CONTROL
    bool manualControl;             ///< status flag, true if roll/pitch/yaw/thrust are controlled manually
    bool overrideRC;                ///< status flag, true if RC overrides are in effect

    /// POSITION
    bool positionLock;          ///< Status if position information is available or not
    bool isLocalPositionKnown;      ///< If the local position has been received for this MAV
    bool isGlobalPositionKnown;     ///< If the global position has been received for this MAV

    double localX;
    double localY;
    double localZ;

    double latitude;            ///< Global latitude as estimated by position estimator
    double longitude;           ///< Global longitude as estimated by position estimator
    double altitudeAMSL;        ///< Global altitude as estimated by position estimator
    double altitudeRelative;    ///< Altitude above home as estimated by position estimator

    int m_satelliteCount;       ///< Number of satellites visible to raw GPS
    double m_gps_hdop;          ///< GPS HDOP
    int m_gps_fix;              ///< GPS FIX type 1, 2 = 2D, 3 = 3D
    bool globalEstimatorActive; ///< Global position estimator present, do not fall back to GPS raw for position
    double latitude_gps;        ///< Global latitude as estimated by raw GPS
    double longitude_gps;       ///< Global longitude as estimated by raw GPS
    double altitude_gps;        ///< Global altitude as estimated by raw GPS
    double velocity_gps;        ///< Global velocity as estimated by raw GPS
    double speedX;              ///< True speed in X axis
    double speedY;              ///< True speed in Y axis
    double speedZ;              ///< True speed in Z axis

    QVector3D nedPosGlobalOffset;   ///< Offset between the system's NED position measurements and the swarm / global 0/0/0 origin
    QVector3D nedAttGlobalOffset;   ///< Offset between the system's NED position measurements and the swarm / global 0/0/0 origin

    /// WAYPOINT NAVIGATION
    double distToWaypoint;       ///< Distance to next waypoint
    double airSpeed;             ///< Airspeed
    double groundSpeed;          ///< Groundspeed
    double bearingToWaypoint;    ///< Bearing to next waypoint
    UASWaypointManager waypointManager;

    /// ATTITUDE
    bool attitudeKnown;             ///< True if attitude was received, false else
    bool attitudeStamped;           ///< Should arriving data be timestamped with the last attitude? This helps with broken system time clocks on the MAV
    quint64 lastAttitude;           ///< Timestamp of last attitude measurement
    double roll;
    double pitch;
    double yaw;

    // dongfang: This looks like a candidate for being moved off to a separate class.
    /// IMAGING
    int imageSize;              ///< Image size being transmitted (bytes)
    int imagePackets;           ///< Number of data packets being sent for this image
    int imagePacketsArrived;    ///< Number of data packets recieved
    int imagePayload;           ///< Payload size per transmitted packet (bytes). Standard is 254, and decreases when image resolution increases.
    int imageQuality;           ///< Quality of the transmitted image (percentage)
    int imageType;              ///< Type of the transmitted image (BMP, PNG, JPEG, RAW 8 bit, RAW 32 bit)
    int imageWidth;             ///< Width of the image stream
    int imageHeight;            ///< Width of the image stream
    QByteArray imageRecBuffer;  ///< Buffer for the incoming bytestream
    QImage image;               ///< Image data of last completely transmitted image
    quint64 imageStart;
    bool blockHomePositionChanges;   ///< Block changes to the home position
    bool receivedMode;          ///< True if mode was retrieved from current conenction to UAS

    /// PARAMETERS
    QMap<int, QMap<QString, QVariant>* > parameters; ///< All parameters
    bool paramsOnceRequested;       ///< If the parameter list has been read at least once
    QGCUASParamManager* paramManager; ///< Parameter manager class

    /// SIMULATION
    QGCHilLink* simulation;         ///< Hardware in the loop simulation link

public:
    void setHeartbeatEnabled(bool enabled) { m_heartbeatsEnabled = enabled; }
    /** @brief Set the current battery type */
    void setBattery(BatteryType type, int cells);
    /** @brief Estimate how much flight time is remaining */
    int calculateTimeRemaining();
    /** @brief Get the current charge level */
    double getChargeLevel();
    /** @brief Get the human-readable status message for this code */
    void getStatusForCode(int statusCode, QString& uasState, QString& stateDescription);

    /** @brief Get the human-readable custom mode string*/
    virtual QString getCustomModeText();

    /** @brief Get the human-speakable custom mode string */
    virtual QString getCustomModeAudioText();

    /** @brief Ask to play autopilots audio annoucement for mode changes */
    virtual void playCustomModeChangedAudioMessage();
    /** @brief Ask to play autopilots audio annoucement for armed changes */
    virtual void playArmStateChangedAudioMessage(bool armedState);

    /** @brief Check if vehicle is in autonomous mode */
    bool isAuto();
    /** @brief Check if vehicle is armed */
    bool isArmed() const { return systemIsArmed; }
    /** @brief Check if vehicle is in HIL mode */
    bool isHilEnabled() const { return hilEnabled; }

    /** @brief Get reference to the waypoint manager **/
    UASWaypointManager* getWaypointManager() {
        return &waypointManager;
    }
    /** @brief Get reference to the param manager **/
    QGCUASParamManager* getParamManager() const {
        return paramManager;
    }

    /** @brief Get the HIL simulation */
    QGCHilLink* getHILSimulation() const {
        return simulation;
    }
    // TODO Will be removed
    /** @brief Set reference to the param manager **/
    void setParamManager(QGCUASParamManager* manager) {
        paramManager = manager;
    }
    int getSystemType();

    /**
     * @brief Returns true for systems that can reverse. If the system has no control over position, it returns false as
     * @return If the specified vehicle type can
     */
    bool systemCanReverse() const
    {
        switch(type)
        {
        case MAV_TYPE_GENERIC:
        case MAV_TYPE_FIXED_WING:
        case MAV_TYPE_ROCKET:
        case MAV_TYPE_FLAPPING_WING:

        // System types that don't have movement
        case MAV_TYPE_ANTENNA_TRACKER:
        case MAV_TYPE_GCS:
        case MAV_TYPE_FREE_BALLOON:
        default:
            return false;
        case MAV_TYPE_QUADROTOR:
        case MAV_TYPE_COAXIAL:
        case MAV_TYPE_HELICOPTER:
        case MAV_TYPE_AIRSHIP:
        case MAV_TYPE_GROUND_ROVER:
        case MAV_TYPE_SURFACE_BOAT:
        case MAV_TYPE_SUBMARINE:
        case MAV_TYPE_HEXAROTOR:
        case MAV_TYPE_OCTOROTOR:
        case MAV_TYPE_TRICOPTER:
            return true;
        }
    }

    QString getSystemTypeName()
    {
        switch(type)
        {
        case MAV_TYPE_GENERIC:
            return "GENERIC";
            break;
        case MAV_TYPE_FIXED_WING:
            return "FIXED_WING";
            break;
        case MAV_TYPE_QUADROTOR:
            return "QUADROTOR";
            break;
        case MAV_TYPE_COAXIAL:
            return "COAXIAL";
            break;
        case MAV_TYPE_HELICOPTER:
            return "HELICOPTER";
            break;
        case MAV_TYPE_ANTENNA_TRACKER:
            return "ANTENNA_TRACKER";
            break;
        case MAV_TYPE_GCS:
            return "GCS";
            break;
        case MAV_TYPE_AIRSHIP:
            return "AIRSHIP";
            break;
        case MAV_TYPE_FREE_BALLOON:
            return "FREE_BALLOON";
            break;
        case MAV_TYPE_ROCKET:
            return "ROCKET";
            break;
        case MAV_TYPE_GROUND_ROVER:
            return "GROUND_ROVER";
            break;
        case MAV_TYPE_SURFACE_BOAT:
            return "BOAT";
            break;
        case MAV_TYPE_SUBMARINE:
            return "SUBMARINE";
            break;
        case MAV_TYPE_HEXAROTOR:
            return "HEXAROTOR";
            break;
        case MAV_TYPE_OCTOROTOR:
            return "OCTOROTOR";
            break;
        case MAV_TYPE_TRICOPTER:
            return "TRICOPTER";
            break;
        case MAV_TYPE_FLAPPING_WING:
            return "FLAPPING_WING";
            break;
        default:
            return "";
            break;
        }
    }

    QImage getImage();
    void requestImage();
    int getAutopilotType(){
        return autopilot;
    }
    QString getAutopilotTypeName()
    {
        switch (autopilot)
        {
        case MAV_AUTOPILOT_GENERIC:
            return "GENERIC";
            break;
//        case MAV_AUTOPILOT_PIXHAWK:
//            return "PIXHAWK";
//            break;
        case MAV_AUTOPILOT_SLUGS:
            return "SLUGS";
            break;
        case MAV_AUTOPILOT_ARDUPILOTMEGA:
            return "ARDUPILOTMEGA";
            break;
        case MAV_AUTOPILOT_OPENPILOT:
            return "OPENPILOT";
            break;
        case MAV_AUTOPILOT_GENERIC_WAYPOINTS_ONLY:
            return "GENERIC_WAYPOINTS_ONLY";
            break;
        case MAV_AUTOPILOT_GENERIC_WAYPOINTS_AND_SIMPLE_NAVIGATION_ONLY:
            return "GENERIC_MISSION_NAVIGATION_ONLY";
            break;
        case MAV_AUTOPILOT_GENERIC_MISSION_FULL:
            return "GENERIC_MISSION_FULL";
            break;
        case MAV_AUTOPILOT_INVALID:
            return "NO AP";
            break;
        case MAV_AUTOPILOT_PPZ:
            return "PPZ";
            break;
        case MAV_AUTOPILOT_UDB:
            return "UDB";
            break;
        case MAV_AUTOPILOT_FP:
            return "FP";
            break;
        case MAV_AUTOPILOT_PX4:
            return "PX4";
            break;
        default:
            return "";
            break;
        }
    }
	
  	/** From UASInterface */
    bool isMultirotor();
	bool isRotaryWing();
    bool isFixedWing();
    bool isGroundRover();
    bool isHelicopter();

public slots:

    void protocolStatusMessageRec(const QString& title, const QString& message);
    void valueChangedRec(const int uasId, const QString& name, const QString& unit, const QVariant& value, const quint64 msec);
    void textMessageReceivedRec(int uasid, int componentid, int severity, const QString& text);
    void receiveLossChangedRec(int id,float value);

    /** @brief Set the autopilot type */
    void setAutopilotType(int apType)
    {
        autopilot = apType;
        emit systemSpecsChanged(uasId);
    }
    /** @brief Set the type of airframe */
    void setSystemType(int systemType);
    /** @brief Set the specific airframe type */
    void setAirframe(int airframe)
    {
        if((airframe >= QGC_AIRFRAME_GENERIC) && (airframe < QGC_AIRFRAME_END_OF_ENUM))
        {
          this->airframe = airframe;
          emit systemSpecsChanged(uasId);
        }
        
    }
    /** @brief Set a new name **/
    void setUASName(const QString& name);
    /** @brief Executes a command **/
    void executeCommand(MAV_CMD command);
    /** @brief Executes a command with 7 params */
    void executeCommand(MAV_CMD command, int confirmation, float param1, float param2, float param3, float param4, float param5, float param6, float param7, int component);
    /** @brief Executes a command ack, with success boolean **/
    void executeCommandAck(int num, bool success);
    /** @brief Set the current battery type and voltages */
    void setBatterySpecs(const QString& specs);
    /** @brief Get the current battery type and specs */
    QString getBatterySpecs();

    /** @brief Launches the system **/
    void launch();
    /** @brief Write this waypoint to the list of waypoints */
    //void setWaypoint(Waypoint* wp); FIXME tbd
    /** @brief Set currently active waypoint */
    //void setWaypointActive(int id); FIXME tbd
    /** @brief Order the robot to return home **/
    void home();
    /** @brief Order the robot to land **/
    void land();
    /** @brief Order the robot to pair its receiver **/
    void pairRX(int rxType, int rxSubType);

    void halt();
    void go();

    /** @brief Enable / disable HIL */
    void enableHilFlightGear(bool enable, QString options, bool sensorHil, QObject * configuration);
    void enableHilJSBSim(bool enable, QString options);

    /** @brief Send the full HIL state to the MAV */
    void sendHilState(quint64 time_us, float roll, float pitch, float yaw, float rollRotationRate,
                        float pitchRotationRate, float yawRotationRate, double lat, double lon, double alt,
                        float vx, float vy, float vz, float ind_airspeed, float true_airspeed, float xacc, float yacc, float zacc);

    void sendHilGroundTruth(quint64 time_us, float roll, float pitch, float yaw, float rollRotationRate,
                        float pitchRotationRate, float yawRotationRate, double lat, double lon, double alt,
                        float vx, float vy, float vz, float ind_airspeed, float true_airspeed, float xacc, float yacc, float zacc);

    /** @brief RAW sensors for sensor HIL */
    void sendHilSensors(quint64 time_us, float xacc, float yacc, float zacc, float rollspeed, float pitchspeed, float yawspeed,
                        float xmag, float ymag, float zmag, float abs_pressure, float diff_pressure, float pressure_alt, float temperature, quint32 fields_changed);

    /**
     * @param time_us
     * @param lat
     * @param lon
     * @param alt
     * @param fix_type
     * @param eph
     * @param epv
     * @param vel
     * @param cog course over ground, in radians, -pi..pi
     * @param satellites
     */
    void sendHilGps(quint64 time_us, double lat, double lon, double alt, int fix_type, float eph, float epv, float vel, float vn, float ve, float vd,  float cog, int satellites);


    /** @brief Places the UAV in Hardware-in-the-Loop simulation status **/
    void startHil();

    /** @brief Stops the UAV's Hardware-in-the-Loop simulation status **/
    void stopHil();


    /** @brief Stops the robot system. If it is an MAV, the robot starts the emergency landing procedure **/
    void emergencySTOP();

    /** @brief Kills the robot. All systems are immediately shut down (e.g. the main power line is cut). This might lead to a crash **/
    bool emergencyKILL();

    /** @brief Shut the system cleanly down. Will shut down any onboard computers **/
    void shutdown();

    /** @brief Reboot the system**/
    void reboot();

    /** @brief Set the target position for the robot to navigate to. */
    void setTargetPosition(float x, float y, float z, float yaw);

    void startLowBattAlarm();
    void stopLowBattAlarm();

    /** @brief Arm system */
    virtual void armSystem();
    /** @brief Disable the motors */
    virtual void disarmSystem();
    /** @brief Toggle the armed state of the system. */
    void toggleArmedState();
    /**
     * @brief Tell the UAS to switch into a completely-autonomous mode, so disable manual input.
     */
    void goAutonomous();
    /**
     * @brief Tell the UAS to switch to manual control. Stabilized attitude may simultaneously be engaged.
     */
    void goManual();
    /**
     * @brief Tell the UAS to switch between manual and autonomous control.
     */
    void toggleAutonomy();

    /** @brief Set the values for the manual control of the vehicle */
    void setManualControlCommands(double roll, double pitch, double yaw, double thrust, int xHat, int yHat, int buttons);
    /** @brief Receive a button pressed event from an input device, e.g. joystick */
    void receiveButton(int buttonIndex);

    /** @brief Add a link associated with this robot */
    void addLink(LinkInterface* link);
    /** @brief Remove a link associated with this robot */
    void removeLink(QObject* object);

    /** @brief Receive a message from one of the communication links. */
    void receiveMessage(LinkInterface* link, mavlink_message_t message);

#ifdef QGC_PROTOBUF_ENABLED
    /** @brief Receive a message from one of the communication links. */
    virtual void receiveExtendedMessage(LinkInterface* link, std::tr1::shared_ptr<google::protobuf::Message> message);
#endif

    /** @brief Send a message over this link (to this or to all UAS on this link) */
    void sendMessage(LinkInterface* link, mavlink_message_t message);
    /** @brief Send a message over all links this UAS can be reached with (!= all links) */
    void sendMessage(mavlink_message_t message);

    /** @brief Temporary Hack for sending packets to patch Antenna. Send a message over all serial links except for this UAS's */
    void forwardMessage(mavlink_message_t message);

    /** @brief Set this UAS as the system currently in focus, e.g. in the main display widgets */
    void setSelected();

    /** @brief Set current mode of operation, e.g. auto or manual */
    void setMode(int mode);

    /** @brief Set current mode of operation, e.g. auto or manual, always uses the current arming status for safety reason */
    void setMode(uint8_t newBaseMode, uint32_t newCustomMode);

    /** @brief Set current mode of operation, e.g. auto or manual, does not check the arming status, for anything else than arming/disarming operations use setMode instead */
    void setModeArm(uint8_t newBaseMode, uint32_t newCustomMode);

    /** @brief Request all parameters */
    void requestParameters();

    /** @brief Request a single parameter by name */
    void requestParameter(int component, const QString& parameter);
    /** @brief Request a single parameter by index */
    void requestParameter(int component, int id);

    /** @brief Set a system parameter */
    void setParameter(const int compId, const QString& paramId, const QVariant& value);

    /** @brief Write parameters to permanent storage */
    void writeParametersToStorage();
    /** @brief Read parameters from permanent storage */
    void readParametersFromStorage();

    /** @brief Get the names of all parameters */
    QList<QString> getParameterNames(int component);

    /** @brief Get the ids of all components */
    QList<int> getComponentIds();

    void enableAllDataTransmission(int rate);
    void enableRawSensorDataTransmission(int rate);
    void enableExtendedSystemStatusTransmission(int rate);
    void enableRCChannelDataTransmission(int rate);
    void enableRawControllerDataTransmission(int rate);
    //void enableRawSensorFusionTransmission(int rate);
    void enablePositionTransmission(int rate);
    void enableExtra1Transmission(int rate);
    void enableExtra2Transmission(int rate);
    void enableExtra3Transmission(int rate);

    /** @brief Update the system state */
    virtual void updateState();

    /** @brief Set world frame origin at current GPS position */
    void setLocalOriginAtCurrentGPSPosition();
    /** @brief Set world frame origin / home position at this GPS position */
    void setHomePosition(double lat, double lon, double alt);
    /** @brief Set local position setpoint */
    void setLocalPositionSetpoint(float x, float y, float z, float yaw);
    /** @brief Add an offset in body frame to the setpoint */
    void setLocalPositionOffset(float x, float y, float z, float yaw);

    void startRadioControlCalibration(int param=1);
    void endRadioControlCalibration();
    void startMagnetometerCalibration();
    void startGyroscopeCalibration();
    void startPressureCalibration();
    void startCompassMotCalibration();

    void startDataRecording();
    void stopDataRecording();
    void deleteSettings();
	
    // Log Download
    void logRequestList(uint16_t start, uint16_t end);
    void logRequestData(uint16_t id, uint32_t ofs, uint32_t count);
    void logEraseAll();
    void logRequestEnd();

    void sendHeartbeat();

signals:
    /** @brief The main/battery voltage has changed/was updated */
    //void voltageChanged(int uasId, double voltage); // Defined in UASInterface already
    /** @brief An actuator value has changed */
    //void actuatorChanged(UASInterface*, int actId, double value); // Defined in UASInterface already
    /** @brief An actuator value has changed */
    void actuatorChanged(UASInterface* uas, QString actuatorName, double min, double max, double value);
    void motorChanged(UASInterface* uas, QString motorName, double min, double max, double value);
    /** @brief The system load (MCU/CPU usage) changed */
    void loadChanged(UASInterface* uas, double load);
    /** @brief Propagate a heartbeat received from the system */
    //void heartbeat(UASInterface* uas); // Defined in UASInterface already
    void imageStarted(quint64 timestamp);
    /** @brief A new camera image has arrived */
    void imageReady(UASInterface* uas);
    /** @brief HIL controls have changed */
    void hilControlsChanged(uint64_t time, float rollAilerons, float pitchElevator, float yawRudder, float throttle, uint8_t systemMode, uint8_t navMode);
    /** @brief HIL actuator outputs have changed */
    void hilActuatorsChanged(uint64_t time, float act1, float act2, float act3, float act4, float act5, float act6, float act7, float act8);

    /** @brief Raw servo output values */
    void servoRawOutputChanged(uint64_t time, float act1, float act2, float act3, float act4, float act5, float act6, float act7, float act8);


    void localXChanged(double val,QString name);
    void localYChanged(double val,QString name);
    void localZChanged(double val,QString name);
    void longitudeChanged(double val,QString name);
    void latitudeChanged(double val,QString name);
    void altitudeAMSLChanged(double val,QString name);
    void altitudeRelativeChanged(double val,QString name);
    void rollChanged(double val,QString name);
    void pitchChanged(double val,QString name);
    void yawChanged(double val,QString name);
    void satelliteCountChanged(int val,QString name);
    void gpsHdopChanged(double val, QString name);
    void gpsFixChanged(int val, QString name);
    void distToWaypointChanged(double val,QString name);
    void groundSpeedChanged(double val, QString name);
    void airSpeedChanged(double val, QString name);
    void bearingToWaypointChanged(double val,QString name);

protected:
    /** @brief Get the UNIX timestamp in milliseconds, enter microseconds */
    quint64 getUnixTime(quint64 time=0);
    /** @brief Get the UNIX timestamp in milliseconds, enter milliseconds */
    quint64 getUnixTimeFromMs(quint64 time);
    /** @brief Get the UNIX timestamp in milliseconds, ignore attitudeStamped mode */
    quint64 getUnixReferenceTime(quint64 time);

    /** @brief convert Joystick input ([-1.0, +1.0]) to RC PPM value ([1000, 2000]) for channel */
    uint16_t scaleJoystickToRC(double pct, int channel) const;

    virtual void processParamValueMsg(mavlink_message_t& msg, const QString& paramName,const mavlink_param_value_t& rawValue, mavlink_param_union_t& paramValue);

    int componentID[256];
    bool componentMulti[256];
    bool connectionLost; ///< Flag indicates a timed out connection
    quint64 connectionLossTime; ///< Time the connection was interrupted
    quint64 lastVoltageWarning; ///< Time at which the last voltage warning occured
    quint64 lastNonNullTime;    ///< The last timestamp from the MAV that was not null
    unsigned int onboardTimeOffsetInvalidCount;     ///< Count when the offboard time offset estimation seemed wrong
    bool hilEnabled;            ///< Set to true if HIL mode is enabled from GCS (UAS might be in HIL even if this flag is not set, this defines the GCS HIL setting)
    bool sensorHil;             ///< True if sensor HIL is enabled
    quint64 lastSendTimeGPS;     ///< Last HIL GPS message sent
    quint64 lastSendTimeSensors;

    QList< QPair<int, QString> >  paramRequestQueue;

    QTimer m_parameterSendTimer;


protected slots:
    void requestNextParamFromQueue();

    /** @brief Write settings to disk */
    void writeSettings();
    /** @brief Read settings from disk */
    void readSettings();

private:
    QString statusName() const;
    QString statusMetric() const;
    QString statusImperial() const;
    QString statusGPS() const;
};


#endif // _UAS_H_
