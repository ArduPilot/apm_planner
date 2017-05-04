#include "logging.h"

#include "kmlcreator.h"
#include "ArduPilotMegaMAV.h"

#include <qstringlist.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QWaitCondition>
#include <QMutex>
#include <QQuaternion>
#include <quazip.h>
#include <math.h>

#include <JlCompress.h>

namespace kml {

const float PI = 3.14159265;

static const QString kModesToColors[][2] = {
    // Colors are expressed in aabbggrr.
    {"AUTO", "FFFF00FF"},       // Plane/Copter/Rover
    {"STABILIZE", "FF00FF00"},  // Plane/Copter
    {"LOITER", "FFFF0000"},     // Plane/Copter
    {"OF_LOITER", "FFFF2323"},  // Copter
    {"RTL", "FFFFCE00"},        // Plane/Copter/Rover
    {"ALT_HOLD", "FF00CEFF"},   // Copter
    {"LAND", "FF009900"},       // Plane/Copter
    {"CIRCLE", "FF33FFCC"},     // Plane/Copter
    {"ACRO", "FF0000FF"},       // Plane/Copter
    {"GUIDED", "FFFFAAAA"},     // Plane/Copter/Rover
    {"POSITION", "FFABABAB"},   // Copter
    {"TOY_A", "FF99FF33"},      // Copter (Legacy)
    {"TOY_B", "FF66CC99"},      // Copter (Legacy)
    {"SPORT", "FFCC3300"},      // Copter
    {"DRIFT", "FF0066FF"},      // Copter
    {"AUTOTUNE", "FF99FF33"},   // Plane/Copter
    {"FLIP", "FF66CC99"},       // Copter
    {"MANUAL", "FF00FF00"},     // Plane/Rover
    {"LEARNING", "FFFF0000"},   // Rover
    {"STEERING", "FFFF2323"},   // Rover
    {"HOLD", "FF00CEFF"},       // Rover
    {"INITIALIZING", "FF009900"},  // Plane/Rover
    {"", ""}
};

/** @brief Return the specified degrees converted to radians */
static float toRadians(float deg) {
    return deg * (PI / 180);
}

/** @brief Return the distance between the two specified lat/lng pairs in km */
static float distanceBetween(float hereLat, float hereLng, float thereLat, float thereLng) {
    const float R = 6371; // earth radius in km

    float dLat = toRadians(thereLat - hereLat);
    float dLon = toRadians(thereLng - hereLng);
    float lat1 = toRadians(hereLat);
    float lat2 = toRadians(thereLat);

    float a = sin(dLat/2) * sin(dLat/2) +
            sin(dLon/2) * sin(dLon/2) * cos(lat1) * cos(lat2);

    float c = 2 * atan2(sqrt(a), sqrt(1-a));

    float d = R * c;

    return d;
}

/**
 * @brief Given a mode string, return a color for it.
 * @param str the mode string
 * @return a color value suitable for use in a KML file.
 */
static QString getColorFor(QString &str) {

    int i = 0;
    while(kModesToColors[i][0] != "") {
        if(str.toUpper() == kModesToColors[i][0]) {
            return kModesToColors[i][1];
        }

        ++i;
    }

    return QString("FF00F000");
}

static QString toModeString(const MAV_TYPE mav_type, const QString &modeString) {

    QString string;
    bool ok = false;
    int mode = modeString.toInt(&ok);
    if (ok) {
        ModeMessage modeMsg(0, 0, mode, 0, 0);
        switch (mav_type)
        {
            case MAV_TYPE_QUADROTOR:
            case MAV_TYPE_HEXAROTOR:
            case MAV_TYPE_OCTOROTOR:
            case MAV_TYPE_HELICOPTER:
            case MAV_TYPE_TRICOPTER:
                string = Copter::MessageFormatter::format(modeMsg);
                break;

            case MAV_TYPE_FIXED_WING:
                string = Plane::MessageFormatter::format(modeMsg);
                break;

            case MAV_TYPE_GROUND_ROVER:
                string = Rover::MessageFormatter::format(modeMsg);
                break;

            default:
                string = modeMsg.toString();
                break;
        }
    } else {
        string = QString("Mode(%1)").arg(modeString);
    }
    return string;
}

GPSRecord GPSRecord::from(FormatLine& format, QString &line) {
    GPSRecord c;
    c.readFields(format, line);
    return c;
}

POSRecord POSRecord::from(FormatLine& format, QString &line) {
    POSRecord c;
    c.readFields(format, line);
    return c;
}

CommandedWaypoint CommandedWaypoint::from(FormatLine &format, QString &line) {
    CommandedWaypoint c;
    c.readFields(format, line);
    return c;
}

Attitude Attitude::from(FormatLine &format, QString &line) {
    Attitude a;
    a.readFields(format, line);
    return a;
}

AHR2 AHR2::from(FormatLine &format, QString &line) {
    AHR2 a;
    a.readFields(format, line);
    return a;
}

NKQ1 NKQ1::from(FormatLine &format, QString &line) {
    NKQ1 a;
    a.readFields(format, line);
    return a;
}

XKQ1 XKQ1::from(FormatLine &format, QString &line) {
    XKQ1 a;
    a.readFields(format, line);
    return a;
}

static GPSRecord gpsFromPOS(POSRecord& pos, qint64 gpsOffset) {
    GPSRecord a;
    a.values.insert("Lat", pos.lat());
    a.values.insert("Lng", pos.lng());
    a.values.insert("Alt", pos.alt());
    a.values.insert("TimeUS", pos.timeUS());
    a.setUtCTime(pos.timeUS().toInt() + gpsOffset);
    return a;
}

// get euler roll angle
float get_euler_roll(QQuaternion& q)
{
    return (atan2f(2.0f*(q.scalar()*q.x() + q.y()*q.z()), 1.0f - 2.0f*(q.x()*q.x() + q.y()*q.y())));
}

// get euler pitch angle
float get_euler_pitch(QQuaternion& q)
{
    return asinf(2.0f*(q.scalar()*q.y() - q.z()*q.x()));
}

// get euler yaw angle
float get_euler_yaw(QQuaternion& q)
{
    return atan2f(2.0f*(q.scalar()*q.z() + q.x()*q.y()), 1.0f - 2.0f*(q.y()*q.y() + q.z()*q.z()));
}

// create eulers from a quaternion
void quat_to_euler(QQuaternion& q, float &roll, float &pitch, float &yaw)
{
    float rad2deg = 180 / M_PI;
    roll = rad2deg * get_euler_roll(q);
    pitch = rad2deg * get_euler_pitch(q);
    yaw = rad2deg * get_euler_yaw(q);
}

static Attitude attFromNKQ1(NKQ1& q) {
    QQuaternion quat(q.q1, q.q2, q.q3, q.q4);
    float roll, pitch, yaw;
    quat_to_euler(quat, roll, pitch, yaw);

    // special handling for pitch angles near 90 degrees
    if (abs(pitch) > 80) {

        // rotate quaternion by 90 degrees in pitch
        quat *= QQuaternion::fromAxisAndAngle(0, 1, 0, -90);

        // get rotated euler angles
        // Qt Euler angles are ordered [pitch, yaw, roll] instead of RPY (xyz fixed)
        // so use the logic from AP_Math.cpp instead
        quat_to_euler(quat, roll, pitch, yaw);

        // then rotate back
        if (pitch < 0) {
            pitch += 90;
        }
        else {
            pitch = 90 - pitch;
            roll += 180;
            if (roll > 180) roll -= 360;
            yaw += 180;
            if (yaw > 180) yaw -= 360;
        }
    }

    Attitude a;
    a.values.insert("Roll", QString::number(roll, 'f', 5));
    a.values.insert("Pitch", QString::number(pitch, 'f', 5));
    a.values.insert("Yaw", QString::number(yaw, 'f', 5));
    a.values.insert("DesRoll", "0.0");
    a.values.insert("DesPitch", "0.0");
    a.values.insert("DesYaw", "0.0");
    return a;
}

static Attitude attFromXKQ1(XKQ1& q) {
    return attFromNKQ1((NKQ1&) q);
}

Placemark::Placemark(QString t, QString m, QString clr):
    title(t), mode(m), color(clr) {
}

Placemark::~Placemark() {
}

Placemark& Placemark::add(GPSRecord &p) {
    mPoints.append(p);
    return *this;
}

Placemark& Placemark::add(Attitude &a) {
    mAttitudes.append(a);
    return *this;
}

Placemark& Placemark::addquat(Attitude &a) {
    mAttQuat.append(a);
    return *this;
}

void SummaryData::add(GPSRecord &gps) {
    float speed = gps.speed().toFloat();
    if(speed > topSpeed) {
        topSpeed = speed;
    }

    float alt = gps.alt().toFloat();
    if(alt > highestAltitude) {
        highestAltitude = alt;
    }

    float lat = gps.lat().toFloat();
    float lng = gps.lng().toFloat();

    if(lastLat != 0 && lastLng != 0) {
        float dist = distanceBetween(lastLat, lastLng, lat, lng);
        totalDistance += dist;
    }

    lastLat = lat;
    lastLng = lng;
}

QString SummaryData::summarize() {
    QString s = QString("Total distance: %1 m\r\nTop speed: %2 m/sec\r\nHighest altitude: %3 m")
            .arg(QString::number(totalDistance * 1000))
            .arg(QString::number(topSpeed))
            .arg(QString::number(highestAltitude))
            ;
    return s;
}

KMLCreator::KMLCreator() :
    m_summary(new SummaryData()),
    m_newXKQ1(false),
    m_newNKQ1(false),
    m_newAHR2(false),
    m_newATT(false),
    m_mav_type((MAV_TYPE)0)
{
}

KMLCreator::KMLCreator(MAV_TYPE mav_type) :
    m_summary(new SummaryData()),
    m_newXKQ1(false),
    m_newNKQ1(false),
    m_newAHR2(false),
    m_newATT(false),
    m_mav_type(mav_type)
{
}

KMLCreator::~KMLCreator() {
    if(m_summary) {
        delete m_summary;
    }
}

void KMLCreator::start(QString &fn) {
    m_filename = fn;

    Placemark *pm = new Placemark("Flight Path", "None", "FF0000FF");
    m_placemarks.append(pm);
}

void KMLCreator::processLine(QString &line)
{
    static qint64 gpsOffset = 0;
    if(line.indexOf("FMT,") == 0) {
        FormatLine fl = FormatLine::from(line);
        if(fl.hasData()) {
            m_formatLines[fl.name] = fl;
        }
    }
    else if(line.indexOf("GPS,") == 0) {
        // calculate offset from GPS time to TimeUS timestamp
        // This is used in function gpsFromPOS to construct a "fake" GPS record from an attitude record
        FormatLine fl = m_formatLines.value("GPS");
        if(fl.hasData()) {
            GPSRecord gps = GPSRecord::from(fl, line);
            if(gps.hasData()) {
                qint64 timeUS = gps.timeUS().toInt();
                qint64 utc_ms = gps.getUtc_ms();
                gpsOffset = utc_ms * 1000LL - timeUS;
            }
            else {
                QLOG_WARN() << "GPS message has no data";
            }
        }
    }
    // POS, ATT, AHR2, NKQ1, and XKQ1 messages are all logged at 25Hz (by default).
    else if(line.indexOf("POS,") == 0 && (gpsOffset > 0)) {
        Placemark* pm = lastPlacemark();
        if(!pm) {
            QLOG_WARN() << "No placemark";
        }
        else {
            // use last read attitude with highest priority: EKF3, EKF2, AHR2, ATT
            if (m_newXKQ1) {
                Attitude att = attFromXKQ1(m_xkq1);
                pm->addquat(att);
            }
            else if (m_newNKQ1) {
                Attitude att = attFromNKQ1(m_nkq1);
                pm->addquat(att);
            }
            if (m_newATT) {
                pm->add(m_att);
            }
        }
        m_newXKQ1 = false;
        m_newNKQ1 = false;
        m_newAHR2 = false;
        m_newATT = false;

        FormatLine fl = m_formatLines.value("POS");
        if(fl.hasData()) {
            POSRecord pos = POSRecord::from(fl, line);
            GPSRecord gps = gpsFromPOS(pos, gpsOffset);

            if(gps.hasData()) {
                m_summary->add(gps);

                Placemark* pm = lastPlacemark();
                if(pm) {
                    pm->add(gps);
                }
                else {
                    QLOG_WARN() << "No placemark";
                }
            }
            else {
                QLOG_WARN() << "GPS message has no data";
            }
        }
    }
    // EKF3 quaternion attitude
    else if((line.indexOf("XKQ1,") == 0)) {

        FormatLine fl = m_formatLines.value("XKQ1");
        if(fl.hasData()) {
            m_xkq1 = XKQ1::from(fl, line);

            if(m_xkq1.hasData()) {
                m_newXKQ1 = true;
            }
        }
    }
    // EKF2 quaternion attitude
    else if((line.indexOf("NKQ1,") == 0)) {
        FormatLine fl = m_formatLines.value("NKQ1");
        if(fl.hasData()) {
            m_nkq1 = NKQ1::from(fl, line);

            if(m_nkq1.hasData()) {
                m_newNKQ1 = true;
            }
        }
    }
    else if((line.indexOf("AHR2,") == 0)) {
        FormatLine fl = m_formatLines.value("AHR2");
        if(fl.hasData()) {
            m_ahr2 = AHR2::from(fl, line);

            if(m_ahr2.hasData()) {
                m_newAHR2 = true;
            }
        }
    }
    else if((line.indexOf("ATT,") == 0)) {
        FormatLine fl = m_formatLines.value("ATT");
        if(fl.hasData()) {
            m_att = Attitude::from(fl, line);

            if(m_att.hasData()) {
                m_newATT = true;
            }
        }
    }
    else if(line.indexOf("CMD,") == 0) {
        FormatLine fl = m_formatLines.value("CMD");
        if(fl.hasData()) {
            CommandedWaypoint wp = CommandedWaypoint::from(fl, line);

            if(wp.hasData()) {
                m_waypoints.append(wp);
            }
            else {
                QLOG_WARN() << "CMD message has no data";
            }
        }
    }
    else if(line.indexOf("MODE,") == 0) {
        FormatLine fl = m_formatLines.value("MODE");
        if(fl.hasData()) {
            ModeRecord mrec = ModeRecord::from(fl, line);
            if (mrec.hasData()) {
                // Time for a new placemark
                QString mode = toModeString(m_mav_type, mrec.modeNum());
                QString title = QString("Flight Mode %1").arg(mode.trimmed());
                QLOG_DEBUG() << "MAV_TYPE: " << m_mav_type << ", flight mode: " << mrec.modeNum().toInt() << ": " << mode;
                QString color = getColorFor(mode);
                Placemark *pm = new Placemark(title, mode, color);
                m_placemarks.append(pm);
            }
        }
    }
}

Placemark* KMLCreator::lastPlacemark() {
    return (m_placemarks.size() > 0)? m_placemarks[m_placemarks.size()-1]: 0;
}

QString KMLCreator::finish(bool kmz) {
    if(m_filename.isEmpty()) {
        QLOG_DEBUG() << "No filename specified. Call start() first.";
        return "";
    }

    QString result(m_filename);

    QLOG_DEBUG() << "write kml to " << m_filename;

    QFile file(m_filename);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QLOG_ERROR() << "Unable to write to " << m_filename;
        return "";
    }

    QXmlStreamWriter writer;

    writer.setDevice(&file);
    writer.setAutoFormatting(true);
    writer.setAutoFormattingIndent(4);
    writer.writeStartDocument();
    writer.writeStartElement("kml");
    writer.writeAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
    writer.writeAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
    writer.writeStartElement("Document");

    writer.writeStartElement("Style");
        writer.writeAttribute(QString("id"), QString("yellowLineGreenPoly"));
        writer.writeStartElement("LineStyle");
            writer.writeTextElement("color", "7F00FFFF");
            writer.writeTextElement("colorMode", "normal");
            writer.writeTextElement("width", "2");
        writer.writeEndElement(); // LineStyle
        writer.writeStartElement("PolyStyle");
            writer.writeTextElement("color", "7F00FF00");
            writer.writeTextElement("colorMode", "normal");
        writer.writeEndElement(); // PolyStyle
    writer.writeEndElement(); // Style

    writer.writeStartElement("Folder");
    writer.writeTextElement("name", "Flight Path");
    writer.writeTextElement("description", m_summary->summarize());

    /*
     * Flight path (complete)
     */
    foreach(Placemark *pm, m_placemarks) {
        writePathElement(writer, pm);
    }

    writer.writeEndElement(); // Folder

    writer.writeStartElement("Folder");
    writer.writeTextElement("name", "Flight Path (segmented)");
    writer.writeTextElement("description", m_summary->summarize());

    /*
     * Flight path (segmented)
     */
    foreach(Placemark *pm, m_placemarks) {
        writeLogPlacemarkElement(writer, pm);
    }

    writer.writeEndElement(); // Folder

    /*
     * Planes element
     */
    writer.writeStartElement("Folder");
    writer.writeTextElement("name", "Attitudes");

    int idx = 0;
    foreach(Placemark *pm, m_placemarks) {
        writePlanePlacemarkElement(writer, pm, idx);
    }

    writer.writeEndElement(); // Folder

    /*
     * Planes element (quaternion)
     */
    writer.writeStartElement("Folder");
    writer.writeTextElement("name", "EKFattitudes");

    idx = 0;
    foreach(Placemark *pm, m_placemarks) {
        writePlanePlacemarkElementQ(writer, pm, idx);
    }

    writer.writeEndElement(); // Folder

    /*
     * Waypoints element
     */
    writer.writeStartElement("Folder");
    writer.writeTextElement("name", "Waypoints");

    writeWaypointsPlacemarkElement(writer);

    writer.writeEndElement(); // Folder

    writer.writeEndElement(); // Document
    writer.writeEndDocument(); // kml


    /*
     * Cleanup
     */
    foreach(Placemark *pm, m_placemarks) {
        delete pm;
    }

    m_placemarks.clear();

    file.close();

    QFileInfo fileInfo(file);
    QDir outDir = fileInfo.absoluteDir();

    // Make sure the model file is in place.
    QFile model(":/files/vehicles/block_plane/block_plane_0.dae");
    QFileInfo modelInfo(model);
    QString baseModelFile = modelInfo.fileName();

    QString modelOutput = QString("%1/%2").arg(outDir.absolutePath()).arg(baseModelFile);
    model.copy(modelOutput);

    if(kmz) {
        QString fn = file.fileName();
        QString kmzFile;

        if(fn.endsWith(".kml")) {
            kmzFile = fn.replace(fn.indexOf(".kml"), 4, ".kmz");
        }
        else if(fn.endsWith(".kmz")) {
            kmzFile = fn;
        }
        else {
            kmzFile = fn + ".kmz";
        }

        result = kmzFile;

        QStringList params;

        params << file.fileName();

        params << modelOutput;

        JlCompress::compressFiles(kmzFile, params);

        foreach(QString fn, params) {
            QFile(fn).remove();
        }

        QLOG_DEBUG() << "Done";
    }

    return result;
}

void KMLCreator::writeWaypointsPlacemarkElement(QXmlStreamWriter &writer) {
    QString coordString;
    foreach(CommandedWaypoint c, m_waypoints) {
        if ( c.isNavigationCommand() ) {
            // Add waypoints that are NAV points.
            coordString += c.toStringForKml();
            coordString += " ";
        }
    }

    writer.writeStartElement("Placemark");
        writer.writeTextElement("name", "Waypoints");

        writer.writeStartElement("Style");
            writer.writeStartElement("LineStyle");
                writer.writeTextElement("color", "FFFFFFFF");
                writer.writeTextElement("colorMode", "normal");
                writer.writeTextElement("width", "2");
            writer.writeEndElement(); // LineStyle

            writer.writeStartElement("PolyStyle");
                writer.writeTextElement("color", "7F000000");
                writer.writeTextElement("colorMode", "normal");
            writer.writeEndElement(); // PolyStyle
        writer.writeEndElement(); // Style

        writer.writeStartElement("LineString");
            writer.writeTextElement("extrude", "1");
            writer.writeTextElement("altitudeMode", "relativeToGround");
            writer.writeTextElement("coordinates", coordString);
        writer.writeEndElement(); // LineString

    writer.writeEndElement(); // Placemark
}

static QString descriptionData(Placemark *p, GPSRecord &c) {
    QHash<QString, QString> m;

    m["Speed"] = c.speed();
    m["Alt"] = c.alt();
    m["HDOP"] = c.hdop();

    if(p->mAttitudes.length() > 0) {
        Attitude a = p->mAttitudes.at(0);

        m["Roll in"] = a.rollIn();
        m["Roll"] = a.roll();
        m["Pitch in"] = a.pitchIn();
        m["Pitch"] = a.pitch();
        m["Yaw in"] = a.yawIn();
        m["Yaw"] = a.yaw();
    }

    QString s;
    QHashIterator<QString, QString> iter(m);
    while(iter.hasNext()) {
        iter.next();
        QString key = iter.key();
        QString value = iter.value();

        s += QString("<b>%1:</b>%2<br>").arg(key).arg(value);
    }
    return s;
}

static QString RPYdescription(Attitude &att) {
    QString s;
    s.append("Roll: " + att.roll());
    s.append("\nPitch: " + att.pitch());
    s.append("\nYaw: " + att.yaw());
    return s;
}

QString utc2KmlTimeStamp(qint64 utc_msec) {
    QDateTime time = QDateTime::fromMSecsSinceEpoch(utc_msec);
    // Qt::ISODateWithMs is not defined?
    int msec = utc_msec - (utc_msec / 1000) * 1000;
    return time.toString(Qt::ISODate) + "." + QString::number(msec);
}

void KMLCreator::writePlanePlacemarkElement(QXmlStreamWriter &writer, Placemark *p, int &idx) {
    if(!p) {
        return;
    }

    int index = 0;
    foreach(GPSRecord c, p->mPoints) {

        // decimate by 5 to reduce the default logging rate to 5Hz
        if ((index %5) == 0) {
            writer.writeStartElement("Placemark");
            QString dateTime = utc2KmlTimeStamp(c.getUtc_ms());
                writer.writeStartElement("TimeStamp");
                    writer.writeTextElement("when", dateTime);
                writer.writeEndElement(); // TimeStamp

                qint64 timeUS = c.timeUS().toInt();
                double ts_sec = (double)timeUS / 1e6;
                QString timeLabel = dateTime.mid(dateTime.indexOf('T')+1, 12);
                writer.writeTextElement("name", QString("Plane %1").arg(idx++) + ": " + QString::number(ts_sec,'f',3) + ": " + timeLabel);
                writer.writeTextElement("visibility", "0");

                QString desc = descriptionData(p, c);
                if(!desc.isEmpty()) {
                    writer.writeStartElement("description");
                    writer.writeCDATA(desc);
                    writer.writeEndElement(); // description
                }

                writer.writeStartElement("Model");
                    writer.writeTextElement("altitudeMode", "absolute");

                    writer.writeStartElement("Location");
                        writer.writeTextElement("latitude", c.lat());
                        writer.writeTextElement("longitude", c.lng());
                        writer.writeTextElement("altitude", c.alt());
                    writer.writeEndElement(); // Location

                    int attitudeSize = p->mAttitudes.size();
                    if(attitudeSize > 0)
                    {
                        int attitudeIndex = index < attitudeSize ? index : attitudeSize - 1;

                        Attitude a = p->mAttitudes.at(attitudeIndex);
                        QString yaw = (p->mode == "AUTO")? a.navYaw(): a.yaw();

                        writer.writeStartElement("Orientation");
                        writer.writeTextElement("heading", yaw);
                            // the sign of tilt and roll has to be changed
                            QString signChangedPitch = QString::number(a.pitch().toDouble() * -1);
                            QString signChangedRoll = QString::number(a.roll().toDouble() * -1);
                            writer.writeTextElement("tilt", signChangedPitch);
                            writer.writeTextElement("roll", signChangedRoll);
                        writer.writeEndElement(); // Orientation
                    }

                    writer.writeStartElement("Scale");
                        writer.writeTextElement("x", ".5");
                        writer.writeTextElement("y", ".5");
                        writer.writeTextElement("z", ".5");
                    writer.writeEndElement(); // Scale

                    writer.writeStartElement("Link");
                        writer.writeTextElement("href", "block_plane_0.dae");
                    writer.writeEndElement(); // Link

                writer.writeEndElement(); // Model

            writer.writeEndElement(); // Placemark
        }
        ++index;
    }
}

void KMLCreator::writePlanePlacemarkElementQ(QXmlStreamWriter &writer, Placemark *p, int &idx) {
    if(!p) {
        return;
    }

    // generate placemarks for quaternion attitudes
    int index = 0;
    Attitude att;
    foreach(GPSRecord c, p->mPoints) {

        // decimate by 5 to reduce the default logging rate to 5Hz
        if ((index % 5) == 0) {
            QString dateTime = utc2KmlTimeStamp(c.getUtc_ms());
            writer.writeStartElement("Placemark");
                writer.writeStartElement("TimeStamp");
                    writer.writeTextElement("when", utc2KmlTimeStamp(c.getUtc_ms()));
                writer.writeEndElement(); // TimeStamp

                qint64 timeUS = c.timeUS().toInt();
                double ts_sec = (double)timeUS / 1e6;
                QString timeLabel = dateTime.mid(dateTime.indexOf('T')+1, 12);
                writer.writeTextElement("name", QString("Plane %1").arg(idx++) + ": " + QString::number(ts_sec,'f',3) + ": " + timeLabel);
                writer.writeTextElement("visibility", "0");

                writer.writeStartElement("Model");
                    writer.writeTextElement("altitudeMode", "absolute");

                    writer.writeStartElement("Location");
                        writer.writeTextElement("latitude", c.lat());
                        writer.writeTextElement("longitude", c.lng());
                        writer.writeTextElement("altitude", c.alt());
                    writer.writeEndElement(); // Location

                    if (index >= p->mAttQuat.size()) break;
                    att = p->mAttQuat.at(index);
                    QString yaw = att.yaw();

                    writer.writeStartElement("Orientation");
                    writer.writeTextElement("heading", yaw);
                        // the sign of tilt and roll has to be changed
                        QString signChangedPitch = QString::number(att.pitch().toDouble() * -1);
                        QString signChangedRoll = QString::number(att.roll().toDouble() * -1);
                        writer.writeTextElement("tilt", signChangedPitch);
                        writer.writeTextElement("roll", signChangedRoll);
                    writer.writeEndElement(); // Orientation

                    writer.writeStartElement("Scale");
                        writer.writeTextElement("x", ".5");
                        writer.writeTextElement("y", ".5");
                        writer.writeTextElement("z", ".5");
                    writer.writeEndElement(); // Scale

                    writer.writeStartElement("Link");
                        writer.writeTextElement("href", "block_plane_0.dae");
                    writer.writeEndElement(); // Link

                writer.writeEndElement(); // Model

                QString desc = RPYdescription(att);
                if(!desc.isEmpty()) {
                    writer.writeStartElement("description");
                    writer.writeCDATA(desc);
                    writer.writeEndElement(); // description
                }


            writer.writeEndElement(); // Placemark
        }
        ++index;
    }
}

// create a Placemark element containing the entire trajectory
void KMLCreator::writePathElement(QXmlStreamWriter &writer, Placemark *p) {
    if(!p || p->mPoints.size()==0) {
        return;
    }

    // construct list of lat/lng/alt coordinates
    QString coords("\n");
    qint64 endUtc = p->mPoints.last().getUtc_ms();
    qint64 startUtc = p->mPoints.first().getUtc_ms();

    foreach(GPSRecord c, p->mPoints) {
        coords += c.toStringForKml() + "\n";
    }

    // create Placemark element
    writer.writeStartElement("Placemark");

    writer.writeStartElement("TimeSpan");
    writer.writeTextElement("begin", utc2KmlTimeStamp(startUtc));
    writer.writeTextElement("end", utc2KmlTimeStamp(endUtc));
    writer.writeEndElement(); // TimeSpan

    writer.writeTextElement("name", "flight path");
    writer.writeTextElement("description", utc2KmlTimeStamp(startUtc) + ", " + utc2KmlTimeStamp(endUtc));
    writer.writeTextElement("styleUrl", "#yellowLineGreenPoly");

    writer.writeStartElement("Style");
        writer.writeStartElement("LineStyle");
        writer.writeTextElement("color", "3F7F7F7F");
        writer.writeTextElement("colorMode", "normal");
        writer.writeTextElement("width", "2");
        writer.writeEndElement(); // LineStyle
    writer.writeEndElement(); // Style

    writer.writeStartElement("LineString");
    writer.writeTextElement("altitudeMode", "absolute");
    writer.writeTextElement("coordinates", coords);
    writer.writeEndElement(); // LineString

    writer.writeEndElement(); // Placemark
}

// end current Placemark
void KMLCreator::endLogPlaceMark(int seq, qint64 startUtc, qint64 endUtc,
        QString& coords, QXmlStreamWriter& writer, Placemark* p) {

    writer.writeStartElement("TimeSpan");

    writer.writeTextElement("begin", utc2KmlTimeStamp(startUtc));
    writer.writeTextElement("end", utc2KmlTimeStamp(endUtc));
    writer.writeEndElement(); // TimeSpan

    writer.writeTextElement("name", p->title + ": " + QString::number(seq));
    writer.writeTextElement("description", utc2KmlTimeStamp(startUtc));
    writer.writeTextElement("styleUrl", "#yellowLineGreenPoly");

    writer.writeStartElement("Style");
        writer.writeStartElement("LineStyle");
        writer.writeTextElement("color", p->color);
        writer.writeTextElement("colorMode", "normal");
        writer.writeTextElement("width", "2");
        writer.writeEndElement(); // LineStyle
    writer.writeEndElement(); // Style

    writer.writeStartElement("LineString");
    writer.writeTextElement("altitudeMode", "absolute");
    writer.writeTextElement("coordinates", coords);
    writer.writeEndElement(); // LineString

    writer.writeEndElement(); // Placemark
}

void KMLCreator::writeLogPlacemarkElement(QXmlStreamWriter &writer, Placemark *p) {
    if(!p || p->mPoints.size()==0) {
        return;
    }

    // for each 1000 milliseconds of data, create a Placemark representing that segment of the trajectory
    QString coords("\n");
    QString lastCoords;
    qint64 startUtc=0, endUtc=0;
    int seq=0;
    foreach(GPSRecord c, p->mPoints) {
        if (startUtc == 0) {
            startUtc = c.getUtc_ms();
            // create first Placemark
            writer.writeStartElement("Placemark");
        }
        if (endUtc >= startUtc+1000) {
            // end current Placemark
            endLogPlaceMark(seq++, startUtc, endUtc, coords, writer, p);
            // leave the last set of coordinates in the buffer so that segments are contiguous
            coords.clear();
            coords += lastCoords + "\n";

            // start a new Placemark
            startUtc = endUtc;
            writer.writeStartElement("Placemark");
        }
        lastCoords = c.toStringForKml();
        coords += lastCoords + "\n";
        endUtc = c.getUtc_ms();
    }
    // end current Placemark
    endLogPlaceMark(seq, startUtc, endUtc, coords, writer, p);
}

} // namespace kml
