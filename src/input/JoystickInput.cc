/*=====================================================================
======================================================================*/

/**
 * @file
 *   @brief Joystick interface
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *   @author Andreas Romer <mavteam@student.ethz.ch>
 *
 */

#include "QsLog.h"
#include "JoystickInput.h"
#include "UAS.h"
#include "QGC.h"

#include <mavlink.h>
#ifdef Q_OS_MAC
#include <SDL_revision.h>
#else
#include <SDL2/SDL_revision.h>
#endif
#include <limits.h>
#include <QMutexLocker>
#include <QSettings>

/*static*/ const double JoystickInput::sdlJoystickMin = -32768.0;
/*static*/ const double JoystickInput::sdlJoystickMax = 32767.0;

/**
 * The coordinate frame of the joystick axis is the aeronautical frame like shown on this image:
 * @image html http://pixhawk.ethz.ch/wiki/_media/standards/body-frame.png Aeronautical frame
 */
JoystickInput::JoystickInput() :
        joystick(NULL),
        joystickName(""),
        uas(NULL),
        done(0),
        thrustAxis(2),
        xAxis(0),
        yAxis(1),
        yawAxis(3),
        thrustReversed(false),
        xReversed(false),
        yReversed(false),
        yawReversed(false),
        autoButtonMapping(-1),
        stabilizeButtonMapping(-1),
        thrustValue(sdlJoystickMax*2),
        xValue(sdlJoystickMax*2),
        yValue(sdlJoystickMax*2),
        yawValue(sdlJoystickMax*2),
        hatValue(sdlJoystickMax*2),
        valuesTicks(0)
{
//    init();
    loadSettings();
}

JoystickInput::~JoystickInput()
{
    storeSettings();
    done.store(1);
}

const QString JoystickInput::getActiveJoystickId()
{
    char joystickId[64];

    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), joystickId, sizeof(joystickId));
    return QString::fromStdString(joystickId);
}

int JoystickInput::getNumberOfButtons() const
{
    return SDL_JoystickNumButtons(joystick);
}

void JoystickInput::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();
    settings.beginGroup("QGC_JOYSTICK_INPUT_" + getActiveJoystickId());
    xAxis = (settings.value("X_AXIS_MAPPING", xAxis).toInt());
    xReversed = (settings.value("X_REVERSED", xReversed).toBool());
    yAxis = (settings.value("Y_AXIS_MAPPING", yAxis).toInt());
    yReversed = (settings.value("Y_REVERSED", yReversed).toBool());
    thrustAxis = (settings.value("THRUST_AXIS_MAPPING", thrustAxis).toInt());
    thrustReversed = (settings.value("THRUST_REVERSED", thrustReversed).toBool());
    yawAxis = (settings.value("YAW_AXIS_MAPPING", yawAxis).toInt());
    yawReversed = (settings.value("YAW_REVERSED", yawReversed).toBool());
    autoButtonMapping = (settings.value("AUTO_BUTTON_MAPPING", autoButtonMapping).toInt());
    stabilizeButtonMapping = (settings.value("STABILIZE_BUTTON_MAPPING", stabilizeButtonMapping).toInt());
    settings.endGroup();
}

void JoystickInput::storeSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("QGC_JOYSTICK_INPUT_" + getActiveJoystickId());
    settings.setValue("X_AXIS_MAPPING", xAxis);
    settings.setValue("X_REVERSED", xReversed);
    settings.setValue("Y_AXIS_MAPPING", yAxis);
    settings.setValue("Y_REVERSED", yReversed);
    settings.setValue("THRUST_AXIS_MAPPING", thrustAxis);
    settings.setValue("THRUST_REVERSED", thrustReversed);
    settings.setValue("YAW_AXIS_MAPPING", yawAxis);
    settings.setValue("YAW_REVERSED", yawReversed);
    settings.setValue("AUTO_BUTTON_MAPPING", autoButtonMapping);
    settings.setValue("STABILIZE_BUTTON_MAPPING", stabilizeButtonMapping);
    settings.endGroup();
    settings.sync();
}


void JoystickInput::setActiveUAS(UASInterface* uas)
{
    // Only connect / disconnect is the UAS is of a controllable UAS class
    if (this->uas)
    {
        UAS* tmp = dynamic_cast<UAS*>(this->uas);
        if (tmp)
        {
            emit joystickChanged(-1.0, -1.0, -1.0, 0.0, 0, 0, 0);

            disconnect(this, SIGNAL(joystickChanged(double,double,double,double,int,int,int)), tmp, SLOT(setManualControlCommands(double,double,double,double,int,int,int)));
            disconnect(this, SIGNAL(buttonPressed(int)), tmp, SLOT(receiveButton(int)));
        }
    }

    this->uas = uas;

    UAS* tmp = dynamic_cast<UAS*>(this->uas);
    if (tmp)
    {
        connect(this, SIGNAL(joystickChanged(double,double,double,double,int,int,int)), tmp, SLOT(setManualControlCommands(double,double,double,double,int,int,int)));
        connect(this, SIGNAL(buttonPressed(int)), tmp, SLOT(receiveButton(int)));
    }

    if (!isRunning())
    {
        start();
    }
}

void JoystickInput::init()
{
    // INITIALIZE SDL Joystick support
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        QLOG_ERROR() << "Couldn't initialize SimpleDirectMediaLayer:" << SDL_GetError();
        return;
    }

    SDL_version sdlCompiledVersion, sdlLinkedVersion;
    SDL_VERSION(&sdlCompiledVersion);
    SDL_GetVersion(&sdlLinkedVersion);
    QLOG_DEBUG() << "Compiled against SDL" << QString("%1.%2.%3").arg(sdlCompiledVersion.major).arg(sdlCompiledVersion.minor).arg(sdlCompiledVersion.patch)
                 << ", Linked against SDL" << QString("%1.%2.%3").arg(sdlLinkedVersion.major).arg(sdlLinkedVersion.minor).arg(sdlLinkedVersion.patch);

    // Wait for joystick if none is connected
    while (done.load() == 0)
    {
        int numJoysticks = SDL_NumJoysticks();
        if (numJoysticks == 0)
        {
            // no joystick detected. reset SDL.

            SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
            QGC::SLEEP::msleep(1000);
            if (done.load() != 0)
                break;
            if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
            {
                QLOG_ERROR() << "Couldn't initialize SimpleDirectMediaLayer:" << SDL_GetError();
                return;
            }
            continue;
        }

        QLOG_INFO() << numJoysticks << "input device(s) found:";
        int joystickId = -1;
        for(int i = 0; i < SDL_NumJoysticks(); ++i)
        {
            SDL_Joystick* temp = SDL_JoystickOpen(i);
            if (temp)
            {
                if (joystickId == -1)
                    joystickId = i;

                SDL_JoystickUpdate();
                QLOG_INFO() << "\t-" << SDL_JoystickName(temp) << "("
                            << SDL_JoystickNumAxes(temp) << "Axis,"
                            << SDL_JoystickNumButtons(temp) << "Buttons,"
                            << SDL_JoystickNumHats(temp) << "Hats"
                            << ")";

                SDL_JoystickClose(temp);
            }
            else
            {
                QLOG_INFO() << "\t" << i << SDL_GetError();
            }
        }
        if (joystickId == -1)
            continue;

        joystick = SDL_JoystickOpen(joystickId);
        if (! joystick)
        {
            QLOG_INFO() << SDL_GetError();
            continue;
        }

        joystickName = QString(SDL_JoystickName(joystick));
        QLOG_INFO() << "Opened" << joystickName;
        loadSettings();
        emit joystickSelected(joystickName);

#if 0
        SDL_JoystickEventState(SDL_ENABLE);
#endif

        break;
    }
}

void JoystickInput::shutdown()
{
    done.store(1);
}

/** @brief Return the result of SDL_JoystickGetAxis (v) scaled to [-1.0,1.0]
 */
double JoystickInput::scaleJoystickAxisValue(int v, bool reversed)
{
    double d = ((double)v) / sdlJoystickMax;
    if (reversed)
        d = -d;
    // Bound rounding errors
    if (d > 1.0f) d = 1.0f;
    if (d < -1.0f) d = -1.0f;
    return d;
}

void JoystickInput::scaleJoystickHatValue(int v, int& xHat, int& yHat)
{
    xHat = 0;
    yHat = 0;

    // Build up vectors describing the hat position
    //
    // Coordinate frame for joystick hat:
    //
    //    y
    //    ^
    //    |
    //    |
    //    0 ----> x
    //

    if ((SDL_HAT_UP & v) != 0) yHat = 1;
    if ((SDL_HAT_DOWN & v) != 0) yHat = -1;

    if ((SDL_HAT_LEFT & v) != 0) xHat = -1;
    if ((SDL_HAT_RIGHT & v) != 0) xHat = 1;
}

/**
 * @brief Calls SDL_JoystickUpdate()
 * @return true if the joystick state changed "significantly" -- the axis
 *         changed by more than 1%, or a button is pressed,
 *         or more than timeout milliseconds elapsed
 */
bool JoystickInput::sdlJoystickUpdate(unsigned timeout)
{
    SDL_JoystickUpdate();

    const Uint32 ticks = SDL_GetTicks();

    const int currentThrustValue = SDL_JoystickGetAxis(joystick, thrustAxis);
    const int currentXValue = SDL_JoystickGetAxis(joystick, xAxis);
    const int currentYValue = SDL_JoystickGetAxis(joystick, yAxis);
    const int currentYawValue = SDL_JoystickGetAxis(joystick, yawAxis);
    const int currentHatValue = SDL_JoystickGetHat(joystick, 0);

    QLOG_TRACE() << "THROTTLE AXIS is:" << currentThrustValue;
    QLOG_TRACE() << "X AXIS is:" << currentXValue;
    QLOG_TRACE() << "Y AXIS is:" << currentYValue;
    QLOG_TRACE() << "Z AXIS is:" << currentYawValue;
    QLOG_TRACE() << "HAT is:" << currentHatValue;

    if (SDL_TICKS_PASSED(ticks, valuesTicks + timeout))
        goto yes;

    if (abs(currentThrustValue - thrustValue) > std::abs(thrustValue * 0.01)
        || abs(currentXValue - xValue) > std::abs(xValue * 0.01)
        || abs(currentYValue - yValue) > std::abs(yValue * 0.01)
        || abs(currentYawValue - yawValue) > std::abs(yawValue * 0.01)
        || abs(currentHatValue - hatValue) > std::abs(hatValue * 0.01))
    {
        goto yes;
    }

    for (int i = 0; i < SDL_JoystickNumButtons(joystick); i++)
    {
        if (SDL_JoystickGetButton(joystick, i))
        {
            QLOG_TRACE() << "BUTTON" << i << "pressed";
            goto yes;
        }
    }

    return false;

yes:

    valuesTicks = ticks;
    thrustValue = currentThrustValue;
    xValue = currentXValue;
    yValue = currentYValue;
    yawValue = currentYawValue;
    hatValue = currentHatValue;

    return true;
}

/**
 * @brief Execute the Joystick process
 */
void JoystickInput::run()
{
    init();
    if (joystick == NULL)
    {
        exit();
        return;
    }

    while (done.load() == 0){
#if 0
        // SDL_WaitEventTimeout() seems to lock up when axis 2 is
        // all the way up (at least on my joystick -- a cheap chinese PSP knock off)
        // Plus it is really not that useful to pump SDL events
        // since the most relevant signal includes all axis/hats/buttons
        // together.

        SDL_Event event;

        // Sleep, update rate of joystick is approx. 50 Hz (1000 ms / 50 = 20 ms)
        while (SDL_WaitEventTimeout(&event, 20))
        {
            // Todo check if it would be more beneficial to use the event structure
            switch(event.type) {
            case SDL_KEYDOWN:
                QLOG_TRACE() << "SDL_KEYDOWN";
                break;

            case SDL_QUIT:
                QLOG_TRACE() << "SDL_QUIT";
                done.store(1);
                break;

            case SDL_JOYBUTTONDOWN:
                QLOG_TRACE() << "SDL_JOYBUTTONDOWN button" << event.jbutton.button;
                if (event.jbutton.state == SDL_PRESSED)
                    emit buttonPressed(event.jbutton.button);
                break;

            case SDL_JOYBUTTONUP:
                QLOG_TRACE() << "SDL_JOYBUTTONDOWN button" << event.jbutton.button;
                break;

            case SDL_JOYAXISMOTION:

                // if (event.jaxis.axis == 2
                //     && -2200 <= event.jaxis.value && event.jaxis.value <= -1100)
                // {
                //     break;
                // }

                QLOG_TRACE() << "SDL_JOYAXISMOTION axis" << event.jaxis.axis << "value=" << event.jaxis.value;

                if (event.jaxis.axis == thrustAxis)
                {
                    // THRUST
                    double thrust = sdlJoystickAxisValue(event.jaxis.value, thrustReversed);
                    emit thrustChanged((float)thrust);
                }
                else if (event.jaxis.axis == xAxis)
                {
                    // X Axis
                    double x = sdlJoystickAxisValue(event.jaxis.value, xReversed);
                    emit xChanged((float)x);
                }
                else if (event.jaxis.axis == yAxis)
                {
                    // Y Axis
                    double y = sdlJoystickAxisValue(event.jaxis.value, yReversed);
                    emit yChanged((float)y);
                }
                else if (event.jaxis.axis == yawAxis)
                {
                    // Yaw Axis
                    double yaw = sdlJoystickAxisValue(event.jaxis.value, yawReversed);
                    emit yawChanged((float)yaw);
                }
                else
                {
                    // Display all axes
                    for(int i = 0; i < SDL_JoystickNumAxes(joystick); i++)
                    {
                        QLOG_TRACE() << "AXIS" << i << "is: " << SDL_JoystickGetAxis(joystick, i);
                    }
                }

                break;

            case SDL_JOYHATMOTION:
            {
                const char* status;
                switch (event.jhat.value)
                {
                case SDL_HAT_CENTERED:  status = "centered";    break;
                case SDL_HAT_UP:        status = "up";          break;
                case SDL_HAT_RIGHT:     status = "right";       break;
                case SDL_HAT_DOWN:      status = "down";        break;
                case SDL_HAT_LEFT:      status = "left";        break;
                case SDL_HAT_RIGHTUP:   status = "right-up";    break;
                case SDL_HAT_RIGHTDOWN: status = "right-down";  break;
                case SDL_HAT_LEFTUP:    status = "left-up";     break;
                case SDL_HAT_LEFTDOWN:  status = "left-down";   break;
                default:                status = "???";         break;
                }
                QLOG_TRACE() << "SDL_JOYHATMOTION hat" << event.jhat.hat << status;

                if (event.jhat.hat == 0)
                {
                    int xHat,yHat;
                    sdlJoystickHatValue(event.jhat.value, xHat, yHat);

                    QLOG_TRACE() << "xHat" << xHat << "," << "yHat" << yHat;

                    // Send new values to rest of groundstation
                    emit hatDirectionChanged(xHat, yHat);
                }

                break;
            }

            case SDL_JOYBALLMOTION:
                QLOG_TRACE() << "SDL_JOYBALLMOTION";
                break;

            default:
                QLOG_TRACE() << "SDL event" << event.type << "occured";
                break;
            }
        }
#else
        // Sleep, update rate of joystick is approx. 50 Hz (1000 ms / 50 = 20 ms)
        QGC::SLEEP::msleep(20);
#endif

        if (done.load() != 0)
            break;

        // signal rate set to 10Hz (100ms)
        if (! sdlJoystickUpdate(100))
            continue;

        // Display all axis
        // for (int i = 0; i < SDL_JoystickNumAxes(joystick); i++)
        // {
        //     QLOG_TRACE() << "AXIS" << i << "is: " << SDL_JoystickGetAxis(joystick, i);
        // }

        double thrust = 0.5 + scaleJoystickAxisValue(thrustValue, thrustReversed)/2;
        double x = scaleJoystickAxisValue(xValue, xReversed);
        double y = scaleJoystickAxisValue(yValue, yReversed);
        double yaw = scaleJoystickAxisValue(yawValue, yawReversed);

        // Display all hats
        // for (int i = 0; i < SDL_JoystickNumHats(joystick); i++)
        // {
        //     QLOG_TRACE() << "HAT" << i << "is: " << SDL_JoystickGetHat(joystick, i);
        // }

        int xHat,yHat;
        scaleJoystickHatValue(hatValue, xHat, yHat);

        // Buttons
        int buttons = 0;
        for (int i = 0; i < SDL_JoystickNumButtons(joystick); i++)
        {
            // QLOG_TRACE() << "BUTTON" << i << "is: " << SDL_JoystickGetButton(joystick, i);

            if (SDL_JoystickGetButton(joystick, i))
            {
                QLOG_TRACE() << "BUTTON" << i << "PRESSED";

                buttons |= 1 << i;

                if (uas)
                {
                    if (i == autoButtonMapping)
                    {
                        uas->setMode(MAV_MODE_FLAG_AUTO_ENABLED);
                    }
                    else if (i == stabilizeButtonMapping)
                    {
                        uas->setMode(MAV_MODE_FLAG_STABILIZE_ENABLED);
                    }
                }
            }
        }

        emit joystickChanged(y, x, yaw, thrust, xHat, yHat, buttons);
    }
    done.store(0);

    exit();
}

const QString& JoystickInput::getName() const
{
    return joystickName;
}
