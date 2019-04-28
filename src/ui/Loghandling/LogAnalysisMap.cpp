/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2019 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file LogAnalysisMap.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 17 Mrz 2019
 * @brief File providing implementation for the log analysing map class
 */

#include "LogAnalysisMap.h"
#include "logging.h"
#include "pointlatlng.h"

#include "ui_LogAnalysisMap.h"

#include <utility>

//************************************************************************************

LogAnalysisMap::LogAnalysisMap(QWidget *parent) :
    QWidget(parent),
    mp_Ui(new Ui::LogAnalysisMap)
 {
    QLOG_DEBUG() << "LogAnalysisMap::LogAnalysisMap - CTOR";
    mp_Ui->setupUi(this);

    // setup zoom slider
    mp_Ui->zoomSlider->setMinimum(mp_Ui->map->MinZoom() * s_MapScaling);
    mp_Ui->zoomSlider->setMaximum(mp_Ui->map->MaxZoom() * s_MapScaling);
    setZoom(static_cast<int>(mp_Ui->map->ZoomReal()));

    connect(mp_Ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setMapZoom(int)));
    connect(mp_Ui->map, SIGNAL(zoomChanged(int)), this, SLOT(setZoom(int)));

    loadSettings();

    mp_Ui->map->SetUseOpenGL(true);

}

LogAnalysisMap::~LogAnalysisMap()
{
    QLOG_DEBUG() << "LogAnalysisMap::~LogAnalysisMap - DTOR";
    saveSettings();
}

void LogAnalysisMap::setDataStorage(LogdataStorage::Ptr dataPtr)
{
    m_dataStoragePtr = std::move(dataPtr);
    findDataNames();
}

void LogAnalysisMap::paintUAVTrail()
{
    // fetch data
    m_dataStoragePtr->getValues(m_latName, false, m_xValues, m_latValues);
    m_dataStoragePtr->getValues(m_lonName, false, m_xValues, m_lonValues);

    m_dataStoragePtr->getValues(m_headingName, false, m_xValuesHeading, m_headingValues);

    if (!m_latValues.empty() && !m_lonValues.empty())
    {
        scaleData();

        // setup trail
        mapcontrol::GPSItem *p_uav = mp_Ui->map->AddTrail();
        p_uav->SetTrailDistance(0);
        p_uav->ShowUavPic(false);

        // set position of map
        internals::PointLatLng pos(m_latValues.at(m_validIndex), m_lonValues.at(m_validIndex));
        mp_Ui->map->SetCurrentPosition(pos);

        // add all GPS positions to the trail
        for (auto i = m_validIndex; i < m_latValues.size(); ++i)
        {
            internals::PointLatLng pos(m_latValues.at(i), m_lonValues.at(i));
            p_uav->SetUAVPos(pos, 10);
        }

        p_uav->SetShowTrail(false);
        p_uav->RefreshPos();

        // create UAV icon as cursor
        if (mp_trailCursor == nullptr)
        {
            mp_trailCursor = mp_Ui->map->AddTrailCursor();
        }
        mp_trailCursor->SetTrailType(mapcontrol::UAVTrailType::NoTrail); // Cursor does not add a trailline
        mp_trailCursor->ShowUavPic(true);
        mp_trailCursor->SetUAVPos(pos, 10);
    }
    else
    {
        QLOG_INFO() << "LogAnalysisMap: No GPS data - no trail";
    }
}

void LogAnalysisMap::setUavCursor(int index)
{
    auto bestGpsIndex = findBestIndexMatch(index, m_xValues);
    auto bestHeadingIndex = findBestIndexMatch(index, m_xValuesHeading);

    internals::PointLatLng pos(m_latValues.at(bestGpsIndex), m_lonValues.at(bestGpsIndex));
    mp_trailCursor->SetUAVPos(pos, 10);
    mp_trailCursor->SetUAVHeading(m_headingValues.at(bestHeadingIndex));
    mp_trailCursor->RefreshPos();
}

void LogAnalysisMap::loadSettings()
{
    QSettings settings;
    settings.beginGroup("LOGANALYSIS_MAP_SETTINGS");

    restoreGeometry(settings.value("GEOMETRY").toByteArray());

    int zoomVal = settings.value("ZOOM", 10).toInt();
    setMapZoom(zoomVal);    // set zoom on map
    setZoom(zoomVal);       // adjust zoom slider

    internals::PointLatLng pos;
    pos.SetLat(settings.value("MAP_POSITION_LAT").toDouble());
    pos.SetLng(settings.value("MAP_POSITION_LON").toDouble());
    mp_Ui->map->SetCurrentPosition(pos);

}

void LogAnalysisMap::saveSettings()
{
    QSettings settings;
    settings.beginGroup("LOGANALYSIS_MAP_SETTINGS");

    settings.setValue("GEOMETRY", saveGeometry());

    settings.setValue("ZOOM", mp_Ui->zoomSlider->value() / s_MapScaling);

    internals::PointLatLng pos = mp_Ui->map->CurrentPosition();
    settings.setValue("MAP_POSITION_LAT", pos.Lat());
    settings.setValue("MAP_POSITION_LON", pos.Lng());
}

void LogAnalysisMap::findDataNames()
{
    if (m_dataStoragePtr.isNull())
    {
        QLOG_WARN() << "LogAnalysisMap: m_dataStoragePtr is not set. Can not create UAV trail!";
        return;
    }

    // fetch all datatypes and try to find GPS and ATT data
    QVector<LogdataStorage::dataType> types = m_dataStoragePtr->getAllDataTypes();
    bool foundGps = false;
    bool foundAtt = false;
    for (const LogdataStorage::dataType &val : types)
    {
        if(!foundGps &&  (val.m_name == "GPS" || val.m_name == "GPS_RAW_INT")) // TODO MAgic Constant
        {
            m_gpsType = val;
            foundGps = true;
        }
        if(!foundAtt && (val.m_name == "ATT" || val.m_name == "ATTITUDE")) // TODO MAgic Constant
        {
            m_attType = val;
            foundAtt = true;
        }
        if (foundAtt && foundGps)
        {
            break;
        }
    }
    // check if type is valid
    if (m_gpsType.m_length == 0)
    {
        QLOG_INFO() << "LogAnalysisMap: No GPS data found. Can't create trail.";
        return;
    }
    if (m_attType.m_length == 0)
    {
        QLOG_INFO() << "LogAnalysisMap: No heading data found.";
    }

    m_latName = m_gpsType.m_name;
    m_latName.append('.');
    m_lonName = m_latName;

    for (const QString &label : m_gpsType.m_labels)
    {
        if (label.compare("lat", Qt::CaseInsensitive) == 0)// TODO MAgic Constant
        {
            m_latName.append(label);
        }
        if (label.compare("lng", Qt::CaseInsensitive) == 0)// TODO MAgic Constant
        {
            m_lonName.append(label);
        }
        if (label.compare("lon", Qt::CaseInsensitive) == 0)// TODO MAgic Constant
        {
            m_lonName.append(label);
        }
    }

    m_headingName = m_attType.m_name;
    m_headingName.append('.');
    for(const QString &label : m_attType.m_labels)
    {
        if (label.compare("yaw", Qt::CaseInsensitive) == 0)   // TODO Magic constant
        {
            m_headingName.append(label);
        }
    }

    QLOG_DEBUG() << "LogAnalysisMap: datamodel name for Gps latitude:" << m_latName;
    QLOG_DEBUG() << "LogAnalysisMap: datamodel name for Gps longitudes:" << m_lonName;
    QLOG_DEBUG() << "LogAnalysisMap: datamodel name for heading:" << m_headingName;
}

int LogAnalysisMap::findBestIndexMatch(int index, const QVector<double> &data)
{
    int intervalSize = data.size();
    int intervalStart = 0;
    int middle = 0;

    if (index <= 0.0)
    {
        middle = 0;
    }
    else if (index >= data.back())
    {
        middle = data.size() - 1;
    }
    else
    {
        while (intervalSize > 1)
        {
            middle = intervalStart + intervalSize / 2;
            auto temp = static_cast<int>(data.at(middle) + 0.5);
            if (index > temp)
            {
                intervalStart = middle;
            }
            else if (index == temp)
            {
                break;
            }
            intervalSize = intervalSize / 2;
        }
    }

    return middle;
}

void LogAnalysisMap::scaleData()
{
    double latVal = 0.0;
    double lonVal = 0.0;
    double scaling = 1.0;

    // first find value which is not 0 for lat and lon and store its index
    for (m_validIndex = 0; m_validIndex < m_latValues.size(); ++m_validIndex)
    {
        if ((m_latValues.at(m_validIndex) != 0.0) && (m_lonValues.at(m_validIndex) != 0.0))
        {
            latVal = m_latValues.at(m_validIndex);
            lonVal = m_lonValues.at(m_validIndex);
            break;
        }
    }

    // ... then find the value which scales lat and lon into the 180, -180 interval...
    while (((latVal / scaling) > 180.0) || ((latVal / scaling) < -180.0))
    {
        scaling *= 10.0;
    }
    while (((lonVal / scaling) > 180.0) || ((lonVal / scaling) < -180.0))
    {
        scaling *= 10.0;
    }

    // ...then scale the whole data arrays
    if (scaling > 1.0)
    {
        QLOG_INFO() << "LogAnalysisMap: Scale the GPS-Values by " << scaling;
        for(double &val: m_latValues)
        {
            val /= scaling;
        }

        for(double &val: m_lonValues)
        {
            val /= scaling;
        }
    }
}

void LogAnalysisMap::setMapZoom(int value)
{
    mp_Ui->map->SetZoom(value / s_MapScaling );
}

void LogAnalysisMap::setZoom(int value)
{
    mp_Ui->zoomSlider->setValue(value * s_MapScaling);
}


