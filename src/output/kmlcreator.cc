#include "QsLog.h"

#include "kmlcreator.h"

#include <qstringlist.h>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QWaitCondition>
#include <QMutex>
#include <quazip.h>

#include <JlCompress.h>

namespace kml {

/**
 * @brief Given a mode string, return a color for it.
 * @param str the mode string
 * @return a color value suitable for use in a KML file.
 */
static QString getColorFor(QString &str) {
    QLOG_DEBUG() << "str=" << str;

    // Colors are expressed in aabbggrr.
    if("AUTO" == str) return QString("FFFF00FF");
    if("STABILIZE" == str) return QString("FF00FF00");
    if("LOITER" == str) return QString("FFFF0000");
    if("OF_LOITER" == str) return QString("FFFF2323");
    if("RTL" == str) return QString("FFFFCE00");
    if("ALT_HOLD" == str) return QString("FF00CEFF");
    if("LAND" == str) return QString("FF009900");
    if("CIRCLE" == str) return QString("FF33FFCC");
    if("ACRO" == str) return QString("FF0000FF");
    if("GUIDED" == str) return QString("FFFFAAAA");
    if("POSITION" == str) return QString("FFABABAB");
    if("TOY_A" == str) return QString("FF99FF33");
    if("TOY_B" == str) return QString("FF66CC99");
    if("SPORT" == str) return QString("FFCC3300");

    return QString("FF000000");
}

static QString toModeString(QString &line) {
    QStringList parts = line.split(QRegExp(","), QString::KeepEmptyParts);

    if(parts.length() > 2) {
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

KMLCreator::KMLCreator() {
}

KMLCreator::~KMLCreator() {
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
            GPSRecord point = GPSRecord::from(fl, line);

            if(point.hasData()) {
                Placemark* pm = lastPlacemark();
                if(pm) {
                    pm->add(point);
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
        coordString += c.toStringForKml();
        coordString += " ";
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
    m["Alt"] = c.relAlt();
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
                writer.writeTextElement("altitudeMode", "relativeToGround");

                writer.writeStartElement("Location");
                    writer.writeTextElement("latitude", c.lat());
                    writer.writeTextElement("longitude", c.lng());
                    writer.writeTextElement("altitude", c.relAlt());
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
    writer.writeTextElement("altitudeMode", "relativeToGround");

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
