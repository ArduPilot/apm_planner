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
    QLOG_DEBUG() << "str=" << str.toUpper();

    int i = 0;
    while(kModesToColors[i][0] != "") {
        if(str.toUpper() == kModesToColors[i][0]) {
            return kModesToColors[i][1];
        }

        ++i;
    }

    return QString("FF00F000");
}

static QString toModeString(QString &line) {

    QStringList parts = line.split(QRegExp(","), QString::KeepEmptyParts);

    // TODO: Need to add Mode: DataLine object to fix
    // MODE, 82081720, 5, 5, 1 // New Message with time
    // MODE, ALT_HOLD, 516     // Old message

    if(parts.length() > 3) {
        QString modeString = parts[2].trimmed();
        bool ok = false;
        int mode = modeString.toInt(&ok);
        if (ok) {
            ModeMessage modeMsg(0, 0, mode, 0, 0);
            return Copter::MessageFormatter::format(modeMsg);
        } else {
            return QString("Mode(%1)").arg(modeString);
        }

    } else if(parts.length() == 3) {
        return parts[1].trimmed();
    }

    return QString();
}

GPSRecord GPSRecord::from(FormatLine& format, QString &line) {
    GPSRecord c;
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

KMLCreator::KMLCreator():
    m_summary(new SummaryData()) {
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

void KMLCreator::processLine(QString &line) {
    if(line.indexOf("FMT,") == 0) {
        FormatLine fl = FormatLine::from(line);
        if(fl.hasData()) {
            m_formatLines[fl.name] = fl;
        }
    }
    else if(line.indexOf("GPS,") == 0) {
        FormatLine fl = m_formatLines.value("GPS");
        if(fl.hasData()) {
            GPSRecord gps = GPSRecord::from(fl, line);

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
                QLOG_WARN() << "Coord has no data";
            }
        }
    }
    else if(line.indexOf("ATT,") == 0) {
        FormatLine fl = m_formatLines.value("ATT");
        if(fl.hasData()) {
            Attitude att = Attitude::from(fl, line);

            if(att.hasData()) {
                Placemark* pm = lastPlacemark();
                if(pm) {
                    pm->add(att);
                }
                else {
                    QLOG_WARN() << "No placemark";
                }
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
                QLOG_WARN() << "Coord has no data";
            }
        }
    }
    else if(line.indexOf("MODE,") == 0) {
        // Time for a new placemark
        QString mode = toModeString(line);
        if(!mode.isEmpty()) {
            QString title = QString("Flight Mode %1").arg(mode.trimmed());
            QString color = getColorFor(mode);
            Placemark *pm = new Placemark(title, mode, color);
            m_placemarks.append(pm);
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
     * Flight log
     */
    foreach(Placemark *pm, m_placemarks) {
        writeLogPlacemarkElement(writer, pm);
    }

    writer.writeEndElement(); // Folder

    /*
     * Planes element
     */
    writer.writeStartElement("Folder");
    writer.writeTextElement("name", "Planes");

    int idx = 0;
    foreach(Placemark *pm, m_placemarks) {
        writePlanePlacemarkElement(writer, pm, idx);
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

    QString s("<![CDATA[\r\n<table>");

    QHashIterator<QString, QString> iter(m);
    while(iter.hasNext()) {
        iter.next();
        QString key = iter.key();
        QString value = iter.value();

        s += QString("<tr><td><b>%1:</b></td><td>%2</td></tr>").arg(key).arg(value);
    }

    s += "</table>\r\n]]>";

    return s;
}

void KMLCreator::writePlanePlacemarkElement(QXmlStreamWriter &writer, Placemark *p, int &idx) {
    if(!p) {
        return;
    }

    foreach(GPSRecord c, p->mPoints) {
        writer.writeStartElement("Placemark");
            writer.writeTextElement("name", QString("Plane %1").arg(idx++));
            writer.writeTextElement("visibility", "0");

            QString desc = descriptionData(p, c);
            if(!desc.isEmpty()) {
                writer.writeTextElement("description", desc);
            }

            writer.writeStartElement("Model");
                writer.writeTextElement("altitudeMode", "absolute");

                writer.writeStartElement("Location");
                    writer.writeTextElement("latitude", c.lat());
                    writer.writeTextElement("longitude", c.lng());
                    writer.writeTextElement("altitude", c.alt());
                writer.writeEndElement(); // Location

                if(p->mAttitudes.size() > 0) {
                    Attitude a = p->mAttitudes.at(0);

                    QString yaw = (p->mode == "AUTO")? a.navYaw(): a.yaw();

                    writer.writeStartElement("Orientation");
                    writer.writeTextElement("heading", yaw);
                        writer.writeTextElement("tilt", a.pitch());
                        writer.writeTextElement("roll", a.roll());
                    writer.writeEndElement(); // Orientation
                }

                writer.writeStartElement("Scale");
                    writer.writeTextElement("x", "1");
                    writer.writeTextElement("y", "1");
                    writer.writeTextElement("x", "1");
                writer.writeEndElement(); // Scale

                writer.writeStartElement("Link");
                    writer.writeTextElement("href", "block_plane_0.dae");
                writer.writeEndElement(); // Link

            writer.writeEndElement(); // Model

        writer.writeEndElement(); // Placemark
    }
}

void KMLCreator::writeLogPlacemarkElement(QXmlStreamWriter &writer, Placemark *p) {
    if(!p) {
        return;
    }

    writer.writeStartElement("Placemark");
    writer.writeTextElement("name", p->title);
    writer.writeTextElement("styleUrl", "#yellowLineGreenPoly");

    writer.writeStartElement("Style");
        writer.writeStartElement("LineStyle");
        writer.writeTextElement("color", p->color);
        writer.writeTextElement("colorMode", "normal");
        writer.writeTextElement("width", "2");
        writer.writeEndElement(); // LineStyle
    writer.writeEndElement();

    writer.writeStartElement("LineString");
    writer.writeTextElement("extrude", "1");
    writer.writeTextElement("altitudeMode", "absolute");

    QString coords;
    foreach(GPSRecord c, p->mPoints) {
        coords += c.toStringForKml();
        coords += " ";
    }

    writer.writeTextElement("coordinates", coords);

    writer.writeEndElement(); // LineString
    writer.writeEndElement(); // Placemark
}

} // namespace kml
