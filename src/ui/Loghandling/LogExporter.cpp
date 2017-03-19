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
 * @file LogExporter.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 28 Jan 2017
 * @brief File providing implementation for the log exporter classes
 */


#include "LogExporter.h"
#include "logging.h"

#include <QMessageBox>
#include <QApplication>
#include <QProgressDialog>
#include <QScopedPointer>

LogExporterBase::LogExporterBase(QWidget *parent) : mp_parent(parent)
{
    QLOG_DEBUG() << "LogExporterBase::LogExporterBase()";
}

LogExporterBase::~LogExporterBase()
{
    QLOG_DEBUG() << "LogExporterBase::~LogExporterBase()";
}


bool LogExporterBase::exportToFile(const QString &fileName, LogdataStorage::Ptr dataStoragePtr)
{
    typedef QScopedPointer<QProgressDialog, QScopedPointerDeleteLater> scopedDelLaterPtr;

    QLOG_DEBUG() << "LogExporterBase::exportToFile() Filename:" << fileName;

    if(!startExport(fileName))
    {
        return false;
    }

    // create progress dialog
    scopedDelLaterPtr progressDialogPtr(new QProgressDialog("Exporting File", "Cancel", 0, 100, mp_parent));
    progressDialogPtr->setWindowModality(Qt::WindowModal);
    progressDialogPtr->show();
    QApplication::processEvents();

    // Export header data
    QString formatheader = "FMT,128,89,FMT,BBnNZ,Type,Length,Name,Format,Columns";
    writeLine(formatheader);

    QVector<LogdataStorage::dataType> allDataTypesInModel;
    allDataTypesInModel = dataStoragePtr->getAllDataTypes();

    QString outputLine;
    foreach(const LogdataStorage::dataType &type, allDataTypesInModel)
    {
        QTextStream fmtStream(&outputLine);
        fmtStream << "FMT," << type.m_ID << "," << type.m_length << "," << type.m_name << ","
                  << type.m_format << "," << type.m_labels.join(",");
        writeLine(outputLine);
        outputLine.clear();
    }

    // Export measurements
    QVector<QVariant> measurements;
    for(int i = 0; i < dataStoragePtr->rowCount(); ++i)
    {
        dataStoragePtr->getDataRow(i, outputLine, measurements);
        foreach(const QVariant &value, measurements)
        {
            outputLine.append(',');
            outputLine.append(value.toString());
        }
        writeLine(outputLine);
        outputLine.clear();
        measurements.clear();

        if(!(i % 5))
        {
            progressDialogPtr->setValue(static_cast<int>(100.0 * (static_cast<double>(i) / static_cast<double>(dataStoragePtr->rowCount()))));
        }
        QApplication::processEvents();
        if(progressDialogPtr->wasCanceled())
        {
            progressDialogPtr->hide();
            QMessageBox::information(mp_parent,  "Warning", "Export was canceled");
            return false;
        }
    }

    progressDialogPtr->hide();
    endExport();
    return true;
}

//***********************************************************************

AsciiLogExporter::AsciiLogExporter(QWidget *parent) : LogExporterBase (parent)
{
    QLOG_DEBUG() << "AsciiLogExporter::AsciiLogExporter()";
}

AsciiLogExporter::~AsciiLogExporter()
{
    QLOG_DEBUG() << "AsciiLogExporter::~AsciiLogExporter()";
    if(m_outputFile.isOpen())
    {
        m_outputFile.close();
    }
}

bool AsciiLogExporter::startExport(const QString &fileName)
{
    m_outputFile.setFileName(fileName);
    if (!m_outputFile.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QLOG_WARN() << "AsciiLogExporter::startExport() unable to open file.";
        QMessageBox::information(0, "Error","Unable to open output file: " + m_outputFile.errorString());
        return false;
    }
    return true;
}

void AsciiLogExporter::writeLine(QString &line)
{
    if (line.size() > 0)
    {
        line.append("\r\n");
        m_outputFile.write(line.toLatin1());
    }
}

void AsciiLogExporter::endExport()
{
    QString msg = QString("Successfull exported to %1").arg(m_outputFile.fileName());
    QMessageBox::information(0, "Export log", msg);
    QLOG_DEBUG() << msg;
    m_outputFile.close();
}

//***********************************************************************

KmlLogExporter::KmlLogExporter(QWidget *parent) : LogExporterBase (parent)
{
    QLOG_DEBUG() << "KmlLogExporter::KmlLogExporter()";
}

KmlLogExporter::~KmlLogExporter()
{
    QLOG_DEBUG() << "KmlLogExporter::~KmlLogExporter()";
}

bool KmlLogExporter::startExport(const QString &fileName)
{
    QString tempName(fileName);
    m_kmlExporter.start(tempName);
    return true;
}

void KmlLogExporter::writeLine(QString &line)
{
    if (line.size() > 0)
    {
        line.append("\r\n");
        m_kmlExporter.processLine(line);
    }
}

void KmlLogExporter::endExport()
{
    QString generated = m_kmlExporter.finish(true);
    QString msg = QString("Successfull exported to %1.").arg(generated);
    QLOG_DEBUG() << msg;
    QMessageBox::information(0, "Export KML", msg);
}
