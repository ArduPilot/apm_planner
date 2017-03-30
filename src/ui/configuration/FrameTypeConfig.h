/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013-2017 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file
 *   @brief Airframe type configuration widget header.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Arne Wischmann <wischmann-a@gmx.de>
 *
 */

#ifndef FRAMETYPECONFIG_H
#define FRAMETYPECONFIG_H

#include "UASParameter.h"
#include <QWidget>
#include "ui_FrameTypeConfig.h"
#include "ui_FrameTypeConfigOld.h"
#include "ui_FrameTypeConfigNew.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "QGCUASParamManager.h"
#include "AP2ConfigWidget.h"
#include "APMFirmwareVersion.h"

/**
 * @brief The FrameTypeConfig class provides the outer Frame of the config dialog
 *        and loads a specialized inner dialog depending on the APM FW version.
 */
class FrameTypeConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    static const int FRAME_INIT_VALUE = 100;    /// Used to check if frameClass and FrameType have useful value

    explicit FrameTypeConfig(QWidget *parent = 0);
    ~FrameTypeConfig();

public slots:
    void paramButtonClicked();
    void activateCompareDialog();

signals:
    /**
     * @brief detectedOldFrameType - will be emitted if an ArduPilot FW < 3.5.0 is detected
     *        and the "FRAME" parameter is set.
     * @param frameType - Frame type ID of ArduPilot FW (@see FrameTypeConfigOld::FRAME_PLUS for example)
     */
    void detectedOldFrameType(int frameType);

    /**
     * @brief detectedNewFrameType - will be emitted if an ArduPilot FW >=3.5.0 is detected
     *        and "FRAME_TYPE" and "FRAME_CLASS" are set.
     * @param frameClass - Frame class ID of ArduPilot FW (@see FrameTypeConfigNew::FRAME_CLASS_QUAD for example)
     * @param frameType  - Frame type ID of ArduPilot FW (@see FrameTypeConfigNew::FRAME_TYPE_PLUS for example)
     */
    void detectedNewFrameType(int frameClass, int frameType);


private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void parameterChanged(int uas, int component, int parameterCount, int parameterId,
                           QString parameterName, QVariant value);
private:
    Ui::FrameTypeConfig ui;

    QMap<QString, UASParameter*> m_parameterList;
    QString m_paramFileToCompare;
    APMFirmwareVersion m_currentFirmwareVersion;    /// holds the actual FW version as soon as it has been set.
    int m_frameClass;
    int m_frameType;
};

/**
 * @brief The FrameTypeConfigOld class provides the complete air frame config for
 *        APM versions prior 3.5.0
 *        Those Firmwares support only a parameter named "FRAME"
 */
class FrameTypeConfigOld : public QWidget
{
    Q_OBJECT

public:
    /** @defgroup Frame
     *  Possible values for parameter "FRAME"
     *  @{
     */
    static const int FRAME_PLUS  =  0;
    static const int FRAME_X     =  1;
    static const int FRAME_V     =  2;
    static const int FRAME_H     =  3;
    static const int FRAME_NEWY6 = 10;
    /** @} */ // end of Frame

    explicit FrameTypeConfigOld(UASInterface *uasInterface, QWidget *parent = 0);
    ~FrameTypeConfigOld();

public slots:
    /**
     * @brief setFrameType - to be called as soon as parameter "FRAME" is known.
     *        Sets up gui to reflect the settings
     * @param frameType  - Frame type ID of ArduPilot FW - @see Frame
     */
    void setFrameType(int frameType);

private slots:
    void xFrameSelected();
    void plusFrameSelected();
    void vFrameSelected();
    void hFrameSelected();
    void newY6FrameSelected();

private:
    Ui::FrameTypeConfigOld ui;

    UASInterface *m_uasInterface;
    int           m_frameType;

    void enableButtons(bool enabled);

};

/**
 * @brief The FrameTypeConfigNew class provides the complete air frame config for
 *        APM versions since 3.5.0
 *        Those Firmwares support two parameters named "FRAME_CLASS" and "FRAME_TYPE"
 */
class FrameTypeConfigNew : public QWidget
{
    Q_OBJECT

public:
    /** @defgroup Frame class
     *  Possible values for parameter "FRAME_CLASS"
     *  @{
     */
    static const int FRAME_CLASS_UNDEFINED = 0;
    static const int FRAME_CLASS_QUAD      = 1;
    static const int FRAME_CLASS_HEXA      = 2;
    static const int FRAME_CLASS_OCTA      = 3;
    static const int FRAME_CLASS_OCTAQUAD  = 4;
    static const int FRAME_CLASS_Y6        = 5;
    static const int FRAME_CLASS_HELI      = 6;
    static const int FRAME_CLASS_TRI       = 7;
    static const int FRAME_CLASS_SINGLE    = 8;
    static const int FRAME_CLASS_COAX      = 9;
    /** @} */ // end of Frame class

    /** @defgroup Frame type
     *  Possible values for parameter "FRAME_TYPE"
     *  @{
     */
    static const int FRAME_TYPE_PLUS   =  0;
    static const int FRAME_TYPE_X      =  1;
    static const int FRAME_TYPE_V      =  2;
    static const int FRAME_TYPE_H      =  3;
    static const int FRAME_TYPE_V_TAIL =  4;
    static const int FRAME_TYPE_A_TAIL =  5;
    static const int FRAME_TYPE_Y6B    = 10;
    /** @} */ // end of Frame types


    explicit FrameTypeConfigNew(UASInterface *uasInterface, QWidget *parent = 0);
    ~FrameTypeConfigNew();

public slots:
    /**
     * @brief setFrameType - to be called as soon as "FRAME_CLASS" and "FRAME_TYPE" are known.
     *        Sets up gui to reflect the settings
     * @param frameClass - Frame class ID of ArduPilot FW - @see Frame class
     * @param frameType  - Frame type ID of ArduPilot FW - @see Frame type
     */
    void setFrameType(int frameClass, int frameType);

private slots:
    /**
     * @brief FrameClass button handlers
     */
    void FrameClassQuadSelected();
    void FrameClassHexaSelected();
    void FrameClassOctaSelected();
    void FrameClassOctaQuadSelected();
    void FrameClassY6Selected();
    void FrameClassHeliSelected();
    void FrameClassTriSelected();
    void FrameClassSingleSelected();
    void FrameClassCoaxSelected();

    /**
     * @brief FrameType button handlers
     */
    void FrameTypePlusSelected();
    void FrameTypeXSelected();
    void FrameTypeHSelected();
    void FrameTypeVSelected();
    void FrameTypeVTailSelected();
    void FrameTypeATailSelected();
    void FrameTypeY6BSelected();

private:
    Ui::FrameTypeConfigNew ui;

    UASInterface *m_uasInterface;
    int m_frameClass;
    int m_frameType;

    /**
     * @brief enableClassButtons - En / Disables all frame class buttons.
     * @param enabled - true enable, false disable.
     */
    void enableClassButtons(bool enabled);

    /**
     * @brief enableTypeWidgets - Makes the frame type buttons visible / invisible
     * @param plus: true - Plus frame type visible, false - not visible
     * @param X   : true - X frame type visible, false - not visible
     * @param HV  : true - H and V frame type visible, false - not visible
     * @param AVTail : true - A/V-Tail frame type visible, false - not visible
     * @param Y6B : true - Y6B frame type visible, false - not visible
     */
    void enableTypeWidgets(bool plus, bool X, bool HV, bool AVTail, bool Y6B);

    /**
     * @brief writeFrameParams - Writes the content of the members m_frameClass and m_frameType
     *        to the MAV.
     */
    void writeFrameParams();

};

#endif // FRAMETYPECONFIG_H
