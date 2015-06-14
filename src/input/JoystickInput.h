/*=====================================================================

PIXHAWK Micro Air Vehicle Flying Robotics Toolkit

(c) 2009, 2010 PIXHAWK PROJECT  <http://pixhawk.ethz.ch>

This file is part of the PIXHAWK project

    PIXHAWK is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PIXHAWK is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PIXHAWK. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of joystick interface
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *   @author Andreas Romer <mavteam@student.ethz.ch>
 *
 */

#ifndef _JOYSTICKINPUT_H_
#define _JOYSTICKINPUT_H_

#include <QThread>
#include <QList>
#include <QAtomicInt>
#ifdef Q_OS_MAC
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif

#include "UASInterface.h"

/**
 * @brief Joystick input
 */
class JoystickInput : public QThread
{
    Q_OBJECT

public:
    JoystickInput();
    ~JoystickInput();
    void run();
    void shutdown();

    const QString& getName() const;

    const QString getActiveJoystickId();

    const int getNumberOfButtons() const;

    /**
     * @brief Load joystick settings
     */
    void loadSettings();

    /**
     * @brief Store joystick settings
     */
    void storeSettings();

    int getMappingThrustAxis() const
    {
        return thrustAxis;
    }

    bool getThrustReversed() const
    {
      return thrustReversed;
    }

    int getMappingXAxis() const
    {
        return xAxis;
    }

    bool getXReversed() const
    {
        return xReversed;
    }

    int getMappingYAxis() const
    {
        return yAxis;
    }

    bool getYReversed() const
    {
        return yReversed;
    }

    int getMappingYawAxis() const
    {
        return yawAxis;
    }

    bool getYawReversed() const
    {
        return yawReversed;
    }

    int getMappingAutoButton() const
    {
        return autoButtonMapping;
    }

    int getMappingStabilizeButton() const
    {
        return stabilizeButtonMapping;
    }

protected:
    SDL_Joystick* joystick;
    QString joystickName;
    UASInterface* uas;
    QAtomicInt done;

    // Axis 3 is thrust (CALIBRATION!)
    volatile int thrustAxis;
    volatile int xAxis;
    volatile int yAxis;
    volatile int yawAxis;
    volatile bool thrustReversed;
    volatile bool xReversed;
    volatile bool yReversed;
    volatile bool yawReversed;
    volatile int autoButtonMapping;
    int stabilizeButtonMapping;
    int thrustValue;
    int xValue;
    int yValue;
    int yawValue;
    int hatValue;
    Uint32 valuesTicks;

    static const double sdlJoystickMin;
    static const double sdlJoystickMax;

    void init();
    bool sdlJoystickUpdate(unsigned);
    double scaleJoystickAxisValue(int v, bool reversed);
    void scaleJoystickHatValue(int v, int& xHat, int& yHat);

signals:

    void joystickSelected(const QString&);

    /**
     * @brief Signal containing all joystick raw positions
     *
     * @param roll left / roll / y axis, left: -1, middle: 0, right: 1
     * @param pitch forward / pitch / x axis, front: 1, center: 0, back: -1
     * @param yaw turn axis, left-turn: -1, centered: 0, right-turn: 1
     * @param thrust Thrust, 0%: 0, 100%: 1
     * @param xHat hat vector in forward-backward direction, +1 forward, 0 center, -1 backward
     * @param yHat hat vector in left-right direction, -1 left, 0 center, +1 right
     * @param buttons bitmask of pressed buttons
     */
    void joystickChanged(double roll, double pitch, double yaw, double thrust, int xHat, int yHat, int buttons);

    /**
     * @brief Thrust lever of the joystick has changed
     *
     * @param thrust Thrust, 0%: 0, 100%: 1.0
     */
    void thrustChanged(double thrust);

    /**
      * @brief X-Axis / forward-backward axis has changed
      *
      * @param x forward / pitch / x axis, back: -1.0, center: 0.0, front: +1.0
      */
    void xChanged(double x);

    /**
      * @brief Y-Axis / left-right axis has changed
      *
      * @param y left / roll / y axis, left: -1.0, middle: 0.0, right: +1.0
      */
    void yChanged(double y);

    /**
      * @brief Yaw / left-right turn has changed
      *
      * @param yaw turn axis, left-turn: -1.0, middle: 0.0, right-turn: +1.0
      */
    void yawChanged(double yaw);

    /**
      * @brief Joystick button has been pressed
      *
      * @param key index of the pressed key
      */
    void buttonPressed(int key);

    /**
      * @brief Hat (8-way switch on the top) has changed position
      *
      * Coordinate frame for joystick hat:
      *
      *    y
      *    ^
      *    |
      *    |
      *    0 ----> x
      *
      *
      * @param x vector in left-right direction
      * @param y vector in forward-backward direction
      */
    void hatDirectionChanged(int x, int y);

public slots:
    void setActiveUAS(UASInterface* uas);
    void setMappingThrustAxis(int mapping)
    {
        thrustAxis = mapping;
    }

    void setMappingXAxis(int mapping)
    {
        xAxis = mapping;
    }

    void setMappingYAxis(int mapping)
    {
        yAxis = mapping;
    }

    void setMappingYawAxis(int mapping)
    {
        yawAxis = mapping;
    }

    void setThrustReversed(bool reversed)
    {
        thrustReversed = reversed;
    }

    void setXReversed(bool reversed)
    {
        xReversed = reversed;
    }

    void setYReversed(bool reversed)
    {
        yReversed = reversed;
    }

    void setYawReversed(bool reversed)
    {
        yawReversed = reversed;
    }

    void setMappingAutoButton(int mapping)
    {
        autoButtonMapping = mapping;
    }

    void setMappingStabilizeButton(int mapping)
    {
        stabilizeButtonMapping = mapping;
    }
};

#endif // _JOYSTICKINPUT_H_
