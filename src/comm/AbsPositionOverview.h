#ifndef ABSPOSITIONOVERVIEW_H
#define ABSPOSITIONOVERVIEW_H

#include <QObject>
#include "libs/mavlink/include/mavlink/v1.0/ardupilotmega/mavlink.h"
#include "LinkInterface.h"


class AbsPositionOverview : public QObject
{
    Q_OBJECT
public:
    Q_PROPERTY(quint64 time_usec READ getTimeUsec WRITE setTimeUsec NOTIFY timeUsecChanged)
    Q_PROPERTY(int lat READ getLat WRITE setLat NOTIFY latChanged)
    Q_PROPERTY(int lon READ getLon WRITE setLon NOTIFY lonChanged)
    Q_PROPERTY(int alt READ getAlt WRITE setAlt NOTIFY altChanged)
    Q_PROPERTY(unsigned int eph READ getEph WRITE setEph NOTIFY ephChanged)
    Q_PROPERTY(unsigned int epv READ getEpv WRITE setEpv NOTIFY epvChanged)
    Q_PROPERTY(unsigned int vel READ getVel WRITE setVel NOTIFY velChanged)
    Q_PROPERTY(unsigned int cog READ getCog WRITE setCog NOTIFY cogChanged)
    Q_PROPERTY(unsigned int fix_type READ getFixType WRITE setFixType NOTIFY fixTypeChanged)
    Q_PROPERTY(unsigned int satellites_visible READ getSatellitesVisible WRITE setSatellitesVisible NOTIFY satellitesVisibleChanged)

    Q_PROPERTY(unsigned int time_boot_ms READ getTimeBootMs WRITE setTimeBootMs NOTIFY timeBootMsChanged)
    Q_PROPERTY(int relative_alt READ getRelativeAlt WRITE setRelativeAlt NOTIFY relativeAltChanged)
    Q_PROPERTY(int vx READ getVx WRITE setVx NOTIFY vxChanged)
    Q_PROPERTY(int vy READ getVy WRITE setVy NOTIFY vyChanged)
    Q_PROPERTY(int vz READ getVz WRITE setVz NOTIFY vzChanged)
    Q_PROPERTY(unsigned int hdg READ getHdg WRITE setHdg NOTIFY hdgChanged)


    quint64 getTimeUsec() { return m_timeUsec; }
    int getLat() { return m_lat; }
    int getLon() { return m_lon; }
    int getAlt() { return m_alt; }
    unsigned int getEph() { return m_eph; }
    unsigned int getEpv() { return m_epv; }
    unsigned int getVel() { return m_vel; }
    unsigned int getCog() { return m_cog; }
    unsigned int getFixType() { return m_fixType; }
    unsigned int getSatellitesVisible() { return m_satellitesVisible; }
    void setTimeUsec(quint64 timeUsec) { if (m_timeUsec!=timeUsec){m_timeUsec = timeUsec; emit timeUsecChanged(timeUsec);}}
    void setLat(int lat) { if (m_lat!=lat){m_lat = lat; emit latChanged(lat);}}
    void setLon(int lon) { if (m_lon!=lon){m_lon = lon; emit lonChanged(lon);}}
    void setAlt(int alt) { if (m_alt!=alt){m_alt = alt; emit altChanged(alt);}}
    void setEph(unsigned int eph) { if (m_eph!=eph){m_eph = eph; emit ephChanged(eph);}}
    void setEpv(unsigned int epv) { if (m_epv!=epv){m_epv = epv; emit epvChanged(epv);}}
    void setVel(unsigned int vel) { if (m_vel!=vel){m_vel = vel; emit velChanged(vel);}}
    void setCog(unsigned int cog) { if (m_cog!=cog){m_cog = cog; emit cogChanged(cog);}}
    void setFixType(unsigned int fixType) { if (m_fixType!=fixType){m_fixType = fixType; emit fixTypeChanged(fixType);}}
    void setSatellitesVisible(unsigned int satellitesVisible) { if (m_satellitesVisible!=satellitesVisible){m_satellitesVisible = satellitesVisible; emit satellitesVisibleChanged(satellitesVisible);}}

    unsigned int getTimeBootMs() { return m_timeBootMs; }
    int getRelativeAlt() { return m_relativeAlt; }
    int getVx() { return m_vx; }
    int getVy() { return m_vy; }
    int getVz() { return m_vz; }
    unsigned int getHdg() { return m_hdg; }
    void setTimeBootMs(unsigned int timeBootMs) { if (m_timeBootMs!=timeBootMs){m_timeBootMs = timeBootMs; emit timeBootMsChanged(timeBootMs);}}
    void setRelativeAlt(int relativeAlt) { if (m_relativeAlt!=relativeAlt){m_relativeAlt = relativeAlt; emit relativeAltChanged(relativeAlt);}}
    void setVx(int vx) { if (m_vx!=vx){m_vx = vx; emit vxChanged(vx);}}
    void setVy(int vy) { if (m_vy!=vy){m_vy = vy; emit vyChanged(vy);}}
    void setVz(int vz) { if (m_vz!=vz){m_vz = vz; emit vzChanged(vz);}}
    void setHdg(unsigned int hdg) { if (m_hdg!=hdg){m_hdg = hdg; emit hdgChanged(hdg);}}

private:
    quint64 m_timeUsec;
    int m_lat;
    int m_lon;
    int m_alt;
    unsigned int m_eph;
    unsigned int m_epv;
    unsigned int m_vel;
    unsigned int m_cog;
    unsigned int m_fixType;
    unsigned int m_satellitesVisible;

    unsigned int m_timeBootMs;
    int m_relativeAlt;
    int m_vx;
    int m_vy;
    int m_vz;
    unsigned int m_hdg;

signals:
    void timeUsecChanged(quint64);
    void latChanged(int);
    void lonChanged(int);
    void altChanged(int);
    void ephChanged(unsigned int);
    void epvChanged(unsigned int);
    void velChanged(unsigned int);
    void cogChanged(unsigned int);
    void fixTypeChanged(unsigned int);
    void satellitesVisibleChanged(unsigned int);

    void timeBootMsChanged(unsigned int);
    void relativeAltChanged(int);
    void vxChanged(int);
    void vyChanged(int);
    void vzChanged(int);
    void hdgChanged(unsigned int);

public:
    explicit AbsPositionOverview(QObject *parent = 0);
    ~AbsPositionOverview();

signals:
private:
    void parseGpsRawInt(LinkInterface *link, const mavlink_message_t &message, const mavlink_gps_raw_int_t &state);
    void parseGlobalPositionInt(LinkInterface *link, const mavlink_message_t &message, const mavlink_global_position_int_t &state);
public slots:
    void messageReceived(LinkInterface* link,mavlink_message_t message);
};

#endif // ABSPOSITIONOVERVIEW_H
