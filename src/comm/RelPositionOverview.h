#ifndef RELPOSITIONOVERVIEW_H
#define RELPOSITIONOVERVIEW_H

#include <QObject>
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include "LinkInterface.h"

#define ToRad(x) (x*0.01745329252)      // *pi/180
#define ToDeg(x) (x*57.2957795131)      // *180/pi

class RelPositionOverview : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(unsigned int time_boot_ms READ getTimeBootMs WRITE setTimeBootMs NOTIFY timeBootMsChanged)
    Q_PROPERTY(double roll READ getRoll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(double pitch READ getPitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(double yaw READ getYaw WRITE setYaw NOTIFY yawChanged)
    Q_PROPERTY(double rollspeed READ getRollspeed WRITE setRollspeed NOTIFY rollspeedChanged)
    Q_PROPERTY(double pitchspeed READ getPitchspeed WRITE setPitchspeed NOTIFY pitchspeedChanged)
    //vfr_hud
    Q_PROPERTY(double yawspeed READ getYawspeed WRITE setYawspeed NOTIFY yawspeedChanged)
    Q_PROPERTY(double airspeed READ getAirspeed WRITE setAirspeed NOTIFY airspeedChanged)
    Q_PROPERTY(double groundspeed READ getGroundspeed WRITE setGroundspeed NOTIFY groundspeedChanged)
    Q_PROPERTY(double alt READ getAlt WRITE setAlt NOTIFY altChanged)
    Q_PROPERTY(double climb READ getClimb WRITE setClimb NOTIFY climbChanged)
    Q_PROPERTY(int heading READ getHeading WRITE setHeading NOTIFY headingChanged)
    Q_PROPERTY(unsigned int throttle READ getThrottle WRITE setThrottle NOTIFY throttleChanged)

    //vfr_hud
    double getAirspeed() { return m_airspeed; }
    double getGroundspeed() { return m_groundspeed; }
    double getAlt() { return m_alt; }
    double getClimb() { return m_climb; }
    int getHeading() { return m_heading; }
    unsigned int getThrottle() { return m_throttle; }
    void setAirspeed(double airspeed) { if (m_airspeed!=airspeed){m_airspeed = airspeed; emit airspeedChanged(airspeed);}}
    void setGroundspeed(double groundspeed) { if (m_groundspeed!=groundspeed){m_groundspeed = groundspeed; emit groundspeedChanged(groundspeed);}}
    void setAlt(double alt) { if (m_alt!=alt){m_alt = alt; emit altChanged(alt);}}
    void setClimb(double climb) { if (m_climb!=climb){m_climb = climb; emit climbChanged(climb);}}
    void setHeading(int heading) { if (m_heading!=heading){m_heading = heading; emit headingChanged(heading);}}
    void setThrottle(unsigned int throttle) { if (m_throttle!=throttle){m_throttle = throttle; emit throttleChanged(throttle);}}
private:
    double m_airspeed;
    double m_groundspeed;
    double m_alt;
    double m_climb;
    int m_heading;
    unsigned int m_throttle;
signals:
    void airspeedChanged(double);
    void groundspeedChanged(double);
    void altChanged(double);
    void climbChanged(double);
    void headingChanged(int);
    void throttleChanged(unsigned int);
public:
    unsigned int getTimeBootMs() { return m_timeBootMs; }
    double getRoll() { return m_roll; }
    double getPitch() { return m_pitch; }
    double getYaw() { return m_yaw; }
    double getRollspeed() { return m_rollspeed; }
    double getPitchspeed() { return m_pitchspeed; }
    double getYawspeed() { return m_yawspeed; }
    void setTimeBootMs(unsigned int timeBootMs) { if (m_timeBootMs!=timeBootMs){m_timeBootMs = timeBootMs; emit timeBootMsChanged(timeBootMs);}}
    void setRoll(double roll) { if (m_roll!=roll){m_roll = roll; emit rollChanged(roll);}}
    void setPitch(double pitch) { if (m_pitch!=pitch){m_pitch = pitch; emit pitchChanged(pitch);}}
    void setYaw(double yaw) { if (m_yaw!=yaw){m_yaw = yaw; emit yawChanged(yaw);}}
    void setRollspeed(double rollspeed) { if (m_rollspeed!=rollspeed){m_rollspeed = rollspeed; emit rollspeedChanged(rollspeed);}}
    void setPitchspeed(double pitchspeed) { if (m_pitchspeed!=pitchspeed){m_pitchspeed = pitchspeed; emit pitchspeedChanged(pitchspeed);}}
    void setYawspeed(double yawspeed) { if (m_yawspeed!=yawspeed){m_yawspeed = yawspeed; emit yawspeedChanged(yawspeed);}}
private:
    unsigned int m_timeBootMs;
    double m_roll;
    double m_pitch;
    double m_yaw;
    double m_rollspeed;
    double m_pitchspeed;
    double m_yawspeed;
signals:
    void timeBootMsChanged(unsigned int);
    void rollChanged(double);
    void pitchChanged(double);
    void yawChanged(double);
    void rollspeedChanged(double);
    void pitchspeedChanged(double);
    void yawspeedChanged(double);
public:
    explicit RelPositionOverview(QObject *parent = 0);
    ~RelPositionOverview();
    //scaled_imu
    //SCALED_IMU2
    //raw_imu
    //attitude
    //attitude_quaternion
    //STATE_CORRECTION
    //VFR_HUD
    //HIGHRES_IMU
private:
    void parseAttitude(LinkInterface *link, const mavlink_message_t &message, const mavlink_attitude_t &state);
    void parseVfrHud(LinkInterface *link, const mavlink_message_t &message, const mavlink_vfr_hud_t &state);
signals:

public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);
};

#endif // RELPOSITIONOVERVIEW_H
