/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 *   @brief Radio Calibration Configuration widget header.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Arne Wischmann <wischmann-a@gmx.de>
 *
 */

#ifndef RADIOCALIBRATIONCONFIG_H
#define RADIOCALIBRATIONCONFIG_H

//#include <QWidget>
#include <QTimer>
//#include <QShowEvent>
//#include <QHideEvent>
#include <QCheckBox>

#include "UASManager.h"
#include "UASInterface.h"
#include "AP2ConfigWidget.h"
#include "QGCRadioChannelDisplay.h"

// UI forward declaration allow faster compiling on UI changes
namespace Ui
{
    class RadioCalibrationConfig;
}


class RadioCalibrationConfig : public AP2ConfigWidget
{
    Q_OBJECT

    static const int RC_CHANNEL_PWM_MIN = 850; // Spektrum DX6i reports 898 on ch7 even though its 6 channels
    static const int RC_CHANNEL_PWM_MAX = 2100;
    static const int RC_CHANNEL_NUM_MAX = 16;
    static const int RC_CHANNEL_LOWER_CONTROL_CH_MAX = 4;
    
public:
    explicit RadioCalibrationConfig(QWidget *parent = nullptr);
    ~RadioCalibrationConfig();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots:
    void activeUASSet(UASInterface *uas);
    void remoteControlChannelRawChanged(int chan,float val);
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void guiUpdateTimerTick();
    void calibrateButtonClicked();

    void pitchClicked(bool state);
    void rollClicked(bool state);
    void yawClicked(bool state);
    void throttleClicked(bool state);

    void elevonsChecked(bool state);
    void elevonsReversed(bool state);
    void elevonsCh1Rev(bool state);
    void elevonsCh2Rev(bool state);
    void elevonOutput();

    void modeIndexChanged(int index);

    void pitchDZChanged(int value);
    void rollDZChanged(int value);
    void throtDZChanged(int value);
    void yawDZChanged(int value);
    void writeDZButtonPressed();

private:
    void updateChannelReversalStates();
    void setParamChannelRev(const QString& param, bool state);
    void updateChannelRevState(QCheckBox *checkbox, int channelId);
    void readSettings();
    void writeSettings();
    bool isRadioControlActive();
    bool isInRange(double value, double min, double max);
    bool validRadioSettings();

private:
    Ui::RadioCalibrationConfig *ui; //!< Pointer to UI

    QVector<qint32> rcMin;          //!< holds minimum RC-value for each channel after calibration
    QVector<qint32> rcMax;          //!< holds maximum RC-value for each channel after calibration
    QVector<qint32> rcValue;        //!< current RC-value
    QVector<qint32> rcDeadzone;     //!< RC deadzone for the lower 4 channels (roll, pitch, throttle, yaw)

    QTimer *guiUpdateTimer;         //!< timer for gui update
    bool m_calibrationEnabled;      //!< set to true during calibration

    int m_pitchChannel;             //!< Channel number used for pitch
    int m_rollChannel;              //!< Channel number used for roll
    int m_yawChannel;               //!< Channel number used for yaw
    int m_throttleChannel;          //!< Channel number used for throttle

    QGCRadioChannelDisplay* m_pitchWidget;      //!< pointer to widget showing pitch value
    QGCRadioChannelDisplay* m_throttleWidget;   //!< pointer to widget showing throttle value
    QCheckBox* m_pitchCheckBox;                 //!< pointer to checkbox showing pitch reverse
    QCheckBox* m_throttleCheckBox;              //!< pointer to checkbox showing throttle reverse
    int m_rcMode;                               //!< Mode of RC-Transmitter
};

#endif // RADIOCALIBRATIONCONFIG_H
