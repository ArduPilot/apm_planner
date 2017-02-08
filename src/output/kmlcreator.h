#ifndef KMLCREATOR_H
#define KMLCREATOR_H

#include "mavlink.h"
#include <qstring.h>
#include <qlist.h>
#include <QHash>
#include <QStringList>
#include <QXmlStreamWriter>
#include <QPointer>

#include "logdata.h"

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

    virtual bool hasData() {
        return (values.value("Lat").length() > 0);
    }

    QString toStringForKml() {
        QString str = QString("%1,%2,%3").arg(lng(), lat(), alt());
        return str;
    }

    static GPSRecord from(FormatLine& format, QString& line);

    virtual ~GPSRecord() {}
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
    QList<Attitude> mAttitudes;

    Placemark(QString t, QString m, QString clr);
    ~Placemark();

    Placemark& add(GPSRecord &p);
    Placemark& add(Attitude &a);
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
    KMLCreator();
    ~KMLCreator();

    void start(QString &fn);

    void processLine(QString &line);

    QString finish(bool kmz = false);

private:
    Placemark *lastPlacemark();

    void writeLogPlacemarkElement(QXmlStreamWriter &, Placemark *);
    void writePlanePlacemarkElement(QXmlStreamWriter &, Placemark *, int &);
    void writeWaypointsPlacemarkElement(QXmlStreamWriter &);

    QString m_filename;
    QList<Placemark *> m_placemarks;
    QList<CommandedWaypoint> m_waypoints;
    QHash<QString, FormatLine> m_formatLines;
    SummaryData* m_summary;
};

} // namespace kml

#endif // KMLCREATOR_H
