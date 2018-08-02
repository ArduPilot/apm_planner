#ifndef KMLCREATOR_H
#define KMLCREATOR_H

// Mavlink include is only used for MAV_TYPE constant defined in the protocol
#include <mavlink_types.h>
extern mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS]; // defined in src/main.cc
#include <mavlink.h>

#include "logging.h"
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
    QString hdop()  { return values.value("HDop"); }
    QString lat()   { return values.value("Lat"); }
    QString lng()   { return values.value("Lng"); }
    QString alt()   { return values.value("Alt"); }
    QString speed() { return values.value("Spd"); }
    QString crs()   { return values.value("GCrs"); }
    QString vz()    { return values.value("VZ"); }

    // older logs have TimeMS instead of TimeUS; Also GMS->GPSTimeMS and GWk->Week
    QString msec()  {
        if (values.contains("GMS")) {
            return values.value("GMS");
        }
        else {
            return values.value("GPSTimeMS");
        }
    }
    QString week()  {
        if (values.contains("GWk")) {
            return values.value("GWk");
        }
        else {
            return values.value("Week");
        }
    }
    QString timeUS()  {
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

    QString toStringForKml() {
        QString str = QString("%1,%2,%3").arg(lng(), lat(), alt());
        return str;
    }

    static GPSRecord from(FormatLine& format, QString& line);

    qint64 getUtc_ms() {
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
    QString rollIn()  { return values.value("RollIn").isEmpty() ? values.value("DesRoll") : values.value("RollIn"); }
    QString roll()    { return values.value("Roll"); }
    QString pitchIn() { return values.value("PitchIn").isEmpty() ? values.value("DesPitch") : values.value("PitchIn"); }
    QString pitch()   { return values.value("Pitch"); }
    QString yawIn()   { return values.value("YawIn").isEmpty() ? values.value("DesYaw") : values.value("YawIn"); }
    QString yaw()     { return values.value("Yaw"); }
    QString navYaw()  { return values.value("NavYaw"); }

    virtual bool hasData() {
        return (values.value("Roll").length() > 0);
    }

    static Attitude from(FormatLine& format, QString& line);

    virtual ~Attitude() {}
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
    void writeLogPlacemarkElement(QXmlStreamWriter &, Placemark *);
    void writePlanePlacemarkElement(QXmlStreamWriter &, Placemark *, int &);
    void writePlanePlacemarkElementQ(QXmlStreamWriter &, Placemark *, int &);
    void writeWaypointsPlacemarkElement(QXmlStreamWriter &);
    void endLogPlaceMark(int seq, qint64 startUtc, qint64 endUtc,
            QString& coords, QXmlStreamWriter& writer, Placemark* p);

    QString m_filename;
    QList<Placemark *> m_placemarks;
    QList<CommandedWaypoint> m_waypoints;
    QHash<QString, FormatLine> m_formatLines;
    SummaryData* m_summary;

    // save one Attitude and one quaternion record between GPS records
    XKQ1 m_xkq1;
    NKQ1 m_nkq1;
    AHR2 m_ahr2;
    Attitude m_att;

    bool m_newXKQ1;
    bool m_newNKQ1;
    bool m_newAHR2;
    bool m_newATT;

    MAV_TYPE m_mav_type;
    double m_iconInterval;
};

} // namespace kml

#endif // KMLCREATOR_H
