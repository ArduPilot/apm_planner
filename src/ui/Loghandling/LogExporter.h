/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2017 APM_PLANNER PROJECT <http://www.ardupilot.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file LogExporter.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 28 Jan 2017
 * @brief File providing header for the log exporter classes
 */


#ifndef LOGEXPORTER_H
#define LOGEXPORTER_H

#include <QString>

#include "LogdataStorage.h"
#include "src/output/kmlcreator.h"

/**
 * @brief The LogExporterBase class - for different log exporters. It handeles
 *        the exporting workflow for every line oriented export.
 *        It provides a progress dialog with a cancel button.
 */
class LogExporterBase
{
public:

    /**
     * @brief Shared pointer for LogExporterBase objects
     */
    typedef QSharedPointer<LogExporterBase> Ptr;

    /**
     * @brief LogExporterBase - CTOR
     * @param parent - Parent widget needed for progress and info windows.
     */
    explicit LogExporterBase(QWidget *parent);

    /**
     * @brief ~LogExporterBase - DTOR
     */
    virtual ~LogExporterBase();

    /**
     * @brief exportToFile - exports the content of the LogdataStorage pointed by
     *        dataStoragePtr to a file with name fileName.
     * @param fileName - filename for the export
     * @param dataStoragePtr - shared pointer to a filled LogdataStorage
     * @return true on success, false otherwise
     */
    bool exportToFile(const QString &fileName, LogdataStorage::Ptr dataStoragePtr);

private:

    QWidget *mp_parent; /// pointer to parent widget - do not delete

    /**
     * @brief startExport - must be implemented by derived classes. It has to setup
     *        all preconditions needed to call writeline afterwards.
     * @param fileName - filename for the export
     * @return true on success, false otherwise
     */
    virtual bool startExport(const QString &fileName) = 0;

    /**
     * @brief writeLine - must be implemented by derived classes. Will be called by the
     *        export function for every logline stored in datamodel.
     * @param line - All data as a string
     */
    virtual void writeLine(QString &line) = 0;

    /**
     * @brief endExport - must be implemented by derived classes. Will be called by the
     *        export function at the end of the export process. It should close all used
     *        resources.
     */
    virtual void endExport() = 0;
};

//***********************************************************************

/**
 * @brief The AsciiLogExporter class is used to export a *.log file.
 */
class AsciiLogExporter : public LogExporterBase
{
public:

    /**
     * @brief Shared pointer for AsciiLogExporter objects
     */
    typedef QSharedPointer<AsciiLogExporter> Ptr;

    /**
     * @brief AsciiLogExporter - CTOR
     * @param parent - Parent widget needed for progress and info windows.
     */
    AsciiLogExporter(QWidget *parent);

    /**
     * @brief ~AsciiLogExporter - DTOR
     */
    virtual ~AsciiLogExporter();

private:

    QFile m_outputFile;     /// file object for exporting

    /**
     * @brief startExport - Creates and opens the output file
     * @param fileName - file name
     * @return - true on success, false otherwise
     */
    virtual bool startExport(const QString &fileName);

    /**
     * @brief writeLine - writes the line directly into the output file.
     * @param line - string to be written to the file
     */
    virtual void writeLine(QString &line);

    /**
     * @brief endExport - closes the output file
     */
    virtual void endExport();
};

//***********************************************************************

/**
 * @brief The KmlLogExporter class is used to export kml files which can be used
 *        with google earth.
 */
class KmlLogExporter : public LogExporterBase
{
public:

    /**
     * @brief Shared pointer for KmlLogExporter objects
     */
    typedef QSharedPointer<KmlLogExporter> Ptr;

    /**
     * @brief KmlLogExporter - CTOR
     * @param parent - Parent widget needed for progress and info windows.
     */
    KmlLogExporter(QWidget *parent);

    /**
     * @brief ~KmlLogExporter - DTOR
     */
    virtual ~KmlLogExporter();

private:

    kml::KMLCreator m_kmlExporter;      /// KML export object

    /**
     * @brief startExport - sets up the kmlExporter.
     * @param fileName - file name to be used by the kmlExporter
     * @return
     */
    virtual bool startExport(const QString &fileName);

    /**
     * @brief writeLine - gives the line to kmlExporter which extracts the
     *        neede data.
     * @param line - data to be analyzed
     */
    virtual void writeLine(QString &line);

    /**
     * @brief endExport - exports the data collected by the kmlExporter to
     *        the file.
     */
    virtual void endExport();
};


#endif // LOGEXPORTER_H
