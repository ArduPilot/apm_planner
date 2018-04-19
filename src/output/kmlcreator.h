#ifndef KMLCREATOR_H
#define KMLCREATOR_H

#include "logging.h"
#include "mavlink.h"
#include <qstring.h>
#include <qlist.h>
#include <QHash>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QPointer>

#include "logdata.h"

// these definitions should match those in ardupilot/libraries/AP_GPS/AP_GPS.h
// the number of GPS leap seconds
#define GPS_LEAPSECONDS 18ULL
#define UNIX_OFFSET_SEC (315964800ULL - GPS_LEAPSECONDS)
#define SEC_PER_WEEK    (7ULL * 86400ULL)

class QFile;

namespace kml {

/**
 * @brief A GPS record from a log file.
 */
struct GPSRecord: DataLine {
    QString hdop()  const { return values.value("HDop"); }
    QString lat()   const { return values.value("Lat"); }
    QString lng()   const { return values.value("Lng"); }
    QString alt()   const { return values.value("Alt"); }
    QString speed() const { return values.value("Spd"); }
    QString crs()   const { return values.value("GCrs"); }
    QString vz()    const { return values.value("VZ"); }

    // older logs have TimeMS instead of TimeUS; Also GMS->GPSTimeMS and GWk->Week
    QString msec() const {
        if (values.contains("GMS")) {
            return values.value("GMS");
        }
        else {
            return values.value("GPSTimeMS");
        }
    }
    QString week() const {
        if (values.contains("GWk")) {
            return values.value("GWk");
        }
        else {
            return values.value("Week");
        }
    }
    QString timeUS() const {
        if (values.contains("TimeUS")) {
            return values.value("TimeUS");
        }
        else {
            return values.value("TimeMS") + "000";
        }
    }

    virtual bool hasData() {
        bool status;
        int week = this->week().toInt(&status);
        return status && (week > 0);
    }

    QString toStringForKml() const {
        QString str = QString("%1,%2,%3").arg(lng(), lat(), alt());
        return str;
    }

    static GPSRecord from(FormatLine& format, QString& line);

    qint64 getUtc_ms() const {
        // msec since start of week (max value is 2^29.17, so it just fits in a signed 32 bit int)
        int32_t week_ms = this->msec().toInt();
        // weeks since 6 Jan 1980
        int32_t week = this->week().toInt();

        // this is the offset as defined in ardupilot/libraries/AP_GPS/AP_GPS.h
        qint64 utc_msec = UNIX_OFFSET_SEC * 1000LL + week * SEC_PER_WEEK * 1000LL + week_ms;
        return utc_msec;
    }

    void setUtCTime(qint64 timeUS) {
        // convert to msec
        qint64 utc = timeUS / 1000LL;
        // offset to GPS epoch
        utc = utc - (UNIX_OFFSET_SEC * 1000LL);

        int32_t week = utc / (SEC_PER_WEEK * 1000LL);
        int32_t week_ms = utc - week * SEC_PER_WEEK * 1000LL;

        values.remove("GWk");
        values.remove("GMS");

        values.insert("GWk", QString::number(week));
        values.insert("GMS", QString::number(week_ms));
    }

    virtual ~GPSRecord() {}
};

/**
 * @brief A MODE record from a log file.
 */
struct ModeRecord: DataLine {
    QString modeNum()   {
        if (values.contains("ModeNum")) {
            return values.value("ModeNum");
        }
        else {
            return values.value("Mode");
        }
    }

    QString timeUS()    { return values.value("TimeUS"); }

    virtual bool hasData() {
        return (modeNum().length() > 0);
    }

    static ModeRecord from(FormatLine& format, QString& line) {
        ModeRecord rec;
        rec.readFields(format, line);
        return rec;
    }

    virtual ~ModeRecord() {}
};


/**
 * @brief A POS record from a log file.
 */
struct POSRecord: DataLine {
    QString lat()  { return values.value("Lat"); }
    QString lng()    { return values.value("Lng"); }
    QString alt() { return values.value("Alt"); }
    QString relHomeAlt()   { return values.value("RelHomeAlt"); }
    QString relOriginAlt()   { return values.value("RelOriginAlt"); }
    QString timeUS()   { return values.value("TimeUS"); }

    virtual bool hasData() {
        return (values.value("Lat").length() > 0);
    }

    static POSRecord from(FormatLine& format, QString& line);

    virtual ~POSRecord() {}
};

/**
 * @brief An ATT record from a log file.
 */
struct Attitude: DataLine {
    QString rollIn()  const { return values.value("RollIn").isEmpty() ? values.value("DesRoll") : values.value("RollIn"); }
    QString roll()    const { return values.value("Roll"); }
    QString pitchIn() const { return values.value("PitchIn").isEmpty() ? values.value("DesPitch") : values.value("PitchIn"); }
    QString pitch()   const { return values.value("Pitch"); }
    QString yawIn()   const { return values.value("YawIn").isEmpty() ? values.value("DesYaw") : values.value("YawIn"); }
    QString yaw()     const { return values.value("Yaw"); }
    QString navYaw()  const { return values.value("NavYaw"); }

    virtual bool hasData() {
        return (values.value("Roll").length() > 0);
    }

    static Attitude from(FormatLine& format, QString& line);

    virtual ~Attitude() {}
};

/**
 * @brief An AOA record from a log file.
 */
struct AoaSsa: DataLine {
    QString AOA() const { return values.value("AOA"); }
    QString SSA() const { return values.value("SSA"); }

    virtual bool hasData() {
        return (values.value("AOA").length() > 0);
    }

    static AoaSsa from(FormatLine& format, QString& line);

    virtual ~AoaSsa() {}
};

/**
 * @brief An AHR2 record from a log file.
 */
struct AHR2: DataLine {
    QString roll()    { return values.value("Roll"); }
    QString pitch()   { return values.value("Pitch"); }
    QString yaw()     { return values.value("Yaw"); }
    QString alt()  { return values.value("Alt"); }
    QString lat()   { return values.value("Lat"); }
    QString lng()   { return values.value("Lng"); }

    virtual bool hasData() {
        return (values.value("Roll").length() > 0);
    }

    static AHR2 from(FormatLine& format, QString& line);

    virtual ~AHR2() {}
};

/**
 * @brief An XKQ1 record from a log file.
 */
struct XKQ1: DataLine {
public:
    float q1, q2, q3, q4;

    virtual bool hasData() {
        bool ok;
        q1 = values.value("Q1").toFloat(&ok);
        if (!ok) return false;
        q2 = values.value("Q2").toFloat(&ok);
        if (!ok) return false;
        q3 = values.value("Q3").toFloat(&ok);
        if (!ok) return false;
        q4 = values.value("Q4").toFloat(&ok);
        return ok;
    }

    static XKQ1 from(FormatLine& format, QString& line);

    virtual ~XKQ1() {}
};

/**
 * @brief An NKQ1 record from a log file.
 */
struct NKQ1: DataLine {
public:
    float q1, q2, q3, q4;

    virtual bool hasData() {
        bool ok;
        q1 = values.value("Q1").toFloat(&ok);
        if (!ok) return false;
        q2 = values.value("Q2").toFloat(&ok);
        if (!ok) return false;
        q3 = values.value("Q3").toFloat(&ok);
        if (!ok) return false;
        q4 = values.value("Q4").toFloat(&ok);
        return ok;
    }

    static NKQ1 from(FormatLine& format, QString& line);

    virtual ~NKQ1() {}
};

/**
 * @brief A CMD line from a log file. Only used in this context to provide lat/lng/altitude
 * for generating waypoint data in the KML file.
 */
struct CommandedWaypoint: DataLine {

    int index() { return values.value("CNum").toInt(); }
    MAV_CMD commandId() { return (MAV_CMD)values.value("CId").toInt(); }
    QString lat() { return values.value("Lat"); }
    QString lng() { return values.value("Lng"); }
    QString alt() { return values.value("Alt"); }

    virtual bool hasData() {
        return (values.value("Lat").length() > 0);
    }

    QString toStringForKml() {
        QString str = QString("%1,%2,%3").arg(lng(), lat(), alt());
        return str;
    }

    bool isNavigationCommand() {

        if (commandId() < MAV_CMD_NAV_LAST) {

            if ((lat() == "0") || (lng() == "0")) {
                return false; // Lat/Lng 0.0,0.0 is invalid
            }
            return true;

        } else {
            return false;
        }
    }

    static CommandedWaypoint from(FormatLine& format, QString& line);

    virtual ~CommandedWaypoint() {}
};

/**
 * @brief A container of data for aerobatic maneuver logs in a KML file.
 * First pass through logfile calls processLine which creates a list of GPSRecord
 * and associated POS attitude records in this struct.
 * and marks
 * the beginning and end of all "straight and level" flight segments.
 * Each maneuver consists of all records from beginning of S&L segN to
 * the end of S&L segN+1. Takeoff and landing are special; from start of
 * log to beginning of S&L seg0 and from last S&L seg to end of log, respectively.
 *
 */
class ManeuverData {
public:
    ManeuverData(){};
    ~ManeuverData(){};

    void add(GPSRecord &p, Attitude &a, AoaSsa &as) {
        this->mGPS.append(p);
        this->mAttitudes.append(a);
        this->mAS.append(as);
    }

    QList<GPSRecord> mGPS;
    QList<Attitude> mAttitudes;
    QList<AoaSsa> mAS;
};

struct SegSpec {
    SegSpec(int b, int e, qint64 sutc, qint64 eutc, float d): begin(b), end(e), beginUtc(sutc), endUtc(eutc), duration(d) {}
    int begin;
    int end;
    qint64 beginUtc;
    qint64 endUtc;
    qint64 duration;
};

/**
 * @brief A container of data for creating Placemarks in a KML file.
 */
struct Placemark {
    QString title;
    QString mode;
    QString color;
    QList<GPSRecord> mPoints;
    QList<GPSRecord> mGPS;
    QList<Attitude> mAttitudes;
    QList<Attitude> mAttQuat;

    Placemark(QString t, QString m, QString clr);
    ~Placemark();

    Placemark& add(GPSRecord &p);
    Placemark& addgps(GPSRecord &p);
    Placemark& add(Attitude &a);
    Placemark& addquat(Attitude &a);
};

struct SummaryData {
    float topSpeed;
    float highestAltitude;
    float totalDistance;

    float lastLat;
    float lastLng;

    SummaryData()
    : topSpeed(0),
      highestAltitude(0),
      totalDistance(0),
      lastLat(0),
      lastLng(0)
    {}

    void add(GPSRecord& gps);
    QString summarize();
};

/**
 * @brief An interface for creating KML files.
 *
 * To use it, call start() with a filename you want to create.
 *
 * While reading line-by-line through a dataflash log (either from the serial port or a file), call processLine() for
 * each one of the lines. The KMLCreator will collect placemarks and other items. When done, call finish() and
 * optionally specify whether you want to create a .kmz file (instead of kml). If you pass true to generate a .kmz
 * file, it will create a compressed .kmz file with the generated KML file and a model file in it. The block_plane_0.dae
 * will be included in the .kmz file in that case. If you specify false for creating the .kmz file, the
 * block_plane_0.dae file will be left in the same directory as the .kml file.
 */
class KMLCreator {
public:
    void start(QString &fn);

    void processLine(QString &line);

    QString finish(bool kmz = false);

    KMLCreator();
    KMLCreator(MAV_TYPE mav_type, double iconInterval);

    virtual ~KMLCreator();

private:
    Placemark *lastPlacemark();

    void writePathElement(QXmlStreamWriter &writer, Placemark *p);
    void writeManeuversElement(QXmlStreamWriter &, ManeuverData &, float p_lp=0.95f, float sl_dur=3.0f);
    void writeManeuverSegments(QXmlStreamWriter &, ManeuverData &, float p_lp=0.95f, float sl_dur=3.0f);
    void writePlanePlacemarkElement(QXmlStreamWriter &, Placemark *, int &);
    void writePlanePlacemarkElementQ(QXmlStreamWriter &, Placemark *, int &);
    void writeWaypointsPlacemarkElement(QXmlStreamWriter &);
    void endLogPlaceMark(int, qint64, qint64, QString, QString&, QXmlStreamWriter&, QString&, QString&);

    QString m_filename;
    QList<Placemark *> m_placemarks;
    ManeuverData m_maneuverData;
    QList<CommandedWaypoint> m_waypoints;
    QHash<QString, FormatLine> m_formatLines;
    SummaryData* m_summary;

    // save one Attitude and one quaternion record between GPS records
    XKQ1 m_xkq1;
    NKQ1 m_nkq1;
    AHR2 m_ahr2;
    Attitude m_att;
    AoaSsa m_aoa;

    bool m_newXKQ1;
    bool m_newNKQ1;
    bool m_newAHR2;
    bool m_newATT;

    MAV_TYPE m_mav_type;
    double m_iconInterval;

    QList<SegSpec *> seg_maneuvers(float sl_dur, float p_lp, ManeuverData &md);
};

} // namespace kml

#endif // KMLCREATOR_H
