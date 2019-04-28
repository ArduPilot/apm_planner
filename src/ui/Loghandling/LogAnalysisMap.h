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
 * @file LogAnalysisMap.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 05 Mrz 2019
 * @brief File providing header for the log analysing map class
 */

#ifndef LOGANALYSISMAP_HPP
#define LOGANALYSISMAP_HPP

#include <QObject>
#include <QWidget>

#include "LogdataStorage.h"
#include "gpsitem.h"

namespace Ui {
    class LogAnalysisMap;
}

/**
 * @brief The LogAnalysisMap class is used to show the gps trail of an UAV for log analysis
 */
class LogAnalysisMap : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief LogAnalysis - CTOR
     * @param parent - parent object
     */
    explicit LogAnalysisMap(QWidget *parent = nullptr);

    /**
     * @brief LogAnalysis - DTOR
     *
     */
    ~LogAnalysisMap();

    /**
     * @brief setDataStorage is used to give the pointer of the datastorage to the map
     *        class. The call also evaluates the name of the gps data in the model to fetch
     *        the data later.
     * @param dataPtr - shared pointer to the datamodel
     */
    void setDataStorage(LogdataStorage::Ptr dataPtr);

    /**
     * @brief paintUAVTrail fetches the gps data from the datamodel and paints the trail to the map view.
     *        it also adds the small UAV icon to show its current position.
     */
    void paintUAVTrail();

    /**
     * @brief setUavCursor sets the small UAV icon to the position associated with the index given. The index
     *        is the value of the x-axis in the graph.
     * @param index - valid index of the log analysis graph @see LogAnalysis class
     */
    void setUavCursor(int index);


public slots:

private:

    static constexpr int  s_MapScaling = 10;     ///< Scaling factor for map and zoomslider

    Ui::LogAnalysisMap *mp_Ui;                   ///< Pointer to UI

    LogdataStorage::Ptr          m_dataStoragePtr;     ///< Shared pointer to data storage
    LogdataStorage::dataType     m_gpsType;            ///< Datatype of the GPS message used by the datamodel
    QString                      m_latName;            ///< Datamodel name to fetch latitude from data storage
    QString                      m_lonName;            ///< Datamodel name to fetch longitude from data storage

    LogdataStorage::dataType     m_attType;            ///< Datatype of the ATT message used by the datamodel
    QString                      m_headingName;        ///< Datamodel name to fetch heading from data storage

    mapcontrol::GPSItem         *mp_trailCursor {nullptr};  ///< Pointer to the UAV icon (mapwidget is owner)

    QVector<double>              m_xValues;            ///< X-axis values of the gps data
    QVector<double>              m_latValues;          ///< latitude data
    QVector<double>              m_lonValues;          ///< longitude data
    int                          m_validIndex {0};     ///< first index where lat and lon values are valid (sometimes values start with 0)

    QVector<double>              m_xValuesHeading;     ///< X-axis values of the ATT data
    QVector<double>              m_headingValues;      ///< The heading data of the UAV

    /**
     * @brief loadSettings loads the default / last used settings.
     */
    void loadSettings();

    /**
     * @brief saveSettings saves the currently used seetings
     */
    void saveSettings();

    /**
     * @brief findDataNames tries to find the names of the GPS and the ATT data to fetch them from the datamodel.
     *        This must be done as the logs use different names for the same data.
     */
    void findDataNames();

    /**
     * @brief findBestIndexMatch finds the nearest index for the selected X-data (by the cursor).
     *        All data types can have a different amount of values for the flight (sampling rate). Therefore we have
     *        find the nearest match for the selected x-Value. As the X-Data and the Y-Data are coupled by the
     *        vector index the method returns the index which can be used to fetch the best Y-Data for the given
     *        X-Data.
     *
     * @param index - The selected X-Axis value we want to find the match for.
     * @param data - The data vector to search for the X-Axis value.
     * @return The index which represents the best match for the x-Axis value
     */
    int findBestIndexMatch(int index, const QVector<double> &data);

    /**
     * @brief scaleData scales the GPS data into to 180° / -180° interval as some data comes alread scaled
     *        while other data is not.
     */
    void scaleData();

private slots:

    /**
     * @brief setMapZoom sets the zoom factor of the map to value.
     * @param value - zoom factor to set the map to.
     */
    void setMapZoom(int value);

    /**
     * @brief setZoom sets the position of the zoom slider to value.
     * @param value - zoom factor to set the slider to.
     */
    void setZoom(int value);


};

#endif // LOGANALYSIS_HPP

