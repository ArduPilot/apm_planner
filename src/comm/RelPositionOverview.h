#ifndef RELPOSITIONOVERVIEW_H
#define RELPOSITIONOVERVIEW_H

#include <QObject>
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include "LinkInterface.h"

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
    Q_PROPERTY(double yawspeed READ getYawspeed WRITE setYawspeed NOTIFY yawspeedChanged)
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
signals:

public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message,QByteArray payload);
};

#endif // RELPOSITIONOVERVIEW_H
