/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief APM UAS specilization Object
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 */

#ifndef ARDUPILOTMEGAMAV_H
#define ARDUPILOTMEGAMAV_H

#include "UAS.h"
#include <QString>
#include <QSqlDatabase>

//
// Auto Pilot modes
// ----------------
// Arduplane Flight Mode Defines

class CustomMode {
public:
    CustomMode();
    CustomMode(int aMode);
    int modeAsInt();
    virtual QString operator <<(int mode);
    static QString colorForMode(int aMode);
protected:
    int m_mode;
};

class ApmPlane: public CustomMode {
public:
    static const int modeCount = 16;
    enum planeMode {
    MANUAL        = 0,
    CIRCLE        = 1,
    STABILIZE     = 2,
    TRAINING      = 3,
    ACRO          = 4,
    FLY_BY_WIRE_A = 5,
    FLY_BY_WIRE_B = 6,
    CRUISE        = 7,
    AUTOTUNE      = 8,
    RESERVED_9    = 9,  // RESERVED FOR FUTURE USE
    AUTO          = 10,
    RTL           = 11,
    LOITER        = 12,
    RESERVED_13   = 13, // RESERVED FOR FUTURE USE
    RESERVED_14   = 14, // RESERVED FOR FUTURE USE
    GUIDED        = 15,
    INITIALIZING  = 16
    };

public:
    ApmPlane(planeMode aMode);
    ApmPlane::planeMode mode();
    static QString stringForMode(int aMode);
};
// Arducopter Flight Mode Defines

class ApmCopter: public CustomMode {
public:
    static const int modeCount = 18;
    enum copterMode {
    STABILIZE   = 0,   // hold level position
    ACRO        = 1,   // rate control
    ALT_HOLD    = 2,   // AUTO control
    AUTO        = 3,   // AUTO control
    GUIDED      = 4,   // AUTO control
    LOITER      = 5,   // Hold a single location
    RTL         = 6,   // AUTO control
    CIRCLE      = 7,   // AUTO control
    POSITION    = 8,   // AUTO control
    LAND        = 9,   // AUTO control
    OF_LOITER   = 10,  // Hold a single location using optical flow
                       // sensor
    DRIFT       = 11,  // Drift 'Car Like' mode
    RESERVED_12 = 12,  // RESERVED FOR FUTURE USE
    SPORT       = 13,  // [TODO] Verify this is correct.
    FLIP        = 14,
    AUTOTUNE    = 15,
    POS_HOLD    = 16, // HYBRID LOITER.
    BRAKE       = 17
    };

public:
    ApmCopter(copterMode aMode);
    ApmCopter::copterMode mode();
    static QString stringForMode(int aMode);
};

class ApmRover: public CustomMode {
public:
    static const int modeCount = 16;
    enum roverMode {
    MANUAL        = 0,
    RESERVED_1    = 1, // RESERVED FOR FUTURE USE
    LEARNING      = 2,
    STEERING      = 3,
    HOLD          = 4,
    RESERVED_5    = 5, // RESERVED FOR FUTURE USE
    RESERVED_6    = 6, // RESERVED FOR FUTURE USE
    RESERVED_7    = 7, // RESERVED FOR FUTURE USE
    RESERVED_8    = 8, // RESERVED FOR FUTURE USE
    RESERVED_9    = 9, // RESERVED FOR FUTURE USE
    AUTO          = 10,
    RTL           = 11,
    RESERVED_12   = 12, // RESERVED FOR FUTURE USE
    RESERVED_13   = 13, // RESERVED FOR FUTURE USE
    RESERVED_14   = 14, // RESERVED FOR FUTURE USE
    GUIDED        = 15,
    INITIALIZING  = 16,
    };
public:
    ApmRover(roverMode aMode);
    QString operator <<(roverMode aMode);
    ApmRover::roverMode mode();
    static QString stringForMode(int aMode);
};

class ArduPilotMegaMAV : public UAS
{
    Q_OBJECT
public:
    static QString getNameFromEventId(int ecode);
    static QPair<QString,QString> getErrText(int subsys,int ecode);
    ArduPilotMegaMAV(MAVLinkProtocol* mavlink, int id = 0);
    /** @brief Set camera mount stabilization modes */
    void setMountConfigure(unsigned char mode, bool stabilize_roll,bool stabilize_pitch,bool stabilize_yaw);
    /** @brief Set camera mount control */
    void setMountControl(double pa,double pb,double pc,bool islatlong);

    QString getCustomModeText();
    QString getCustomModeAudioText();
    void playCustomModeChangedAudioMessage();
    void playArmStateChangedAudioMessage(bool armedState) ;

signals:
    void versionDetected(QString versionString);

public slots:
    /** @brief Receive a MAVLink message from this MAV */
    void receiveMessage(LinkInterface* link, mavlink_message_t message);
    void RequestAllDataStreams();

    // Overides from UAS virtual interface
    virtual void armSystem();
    virtual void disarmSystem();

    // UAS Interface
    void textMessageReceived(int uasid, int componentid, int severity, QString text);
    void heartbeatTimeout(bool timeout, unsigned int ms);

private slots:
    void uasConnected();
    void uasDisconnected();

private:
    void createNewMAVLinkLog(uint8_t type);

private:
    QTimer *txReqTimer;
    int severityCompatibilityMode;
};


/**
 * @brief Helper class for making it easier to handle the
 *        errorcodes.
 *        This virtual base class implements everything need
 *        to handle MAV Errors exept the to string method which
 *        must be specialized and implemented in derived classes
 */
class ErrorBase
{
public:

    ErrorBase();

    virtual ~ErrorBase();

    /**
     * @brief This != operator does include the timestamp "Timeus"
     *        field of this class
     */
    bool operator != (const ErrorBase &rhs);

    /**
     * @brief Getter for the Subsystem ID which emitted the error
     * @return Subsystem ID
     */
    quint8 getSubsystemCode();

    /**
     * @brief Getter for the Errorcode emitted by the subsystem
     * @return Errorcode
     */
    quint8 getErrorCode();

    /**
     * @brief Reads an QSqlRecord and sets the internal data.
     *        The record should contain the colums "TimeUS",
     *        "Subsys" and "ECode" in order to get an apropriate
     *        returnvalue.
     * @param record[in] - Filled QSqlRecord
     * @return true - all Fields could be read
     *         false - not all data could be read
     */
    bool setFromSqlRecord(const QSqlRecord &record);

    /**
     * @brief Converts the ErrorCode into an interpreted string
     *        Must be implemented by derived classes
     * @return The interpreted Qstring
     */
    virtual QString toString() const = 0;

protected:

    quint8 SubSys;        /// Subsystem signaling the error
    quint8 ErrorCode;     /// Errorcode of the Subsystem
};


/**
 * @brief Basic ErrorType class wich should be used for all generic
 *        handling of error messages
 */
class ErrorType : public ErrorBase
{
public:

    ErrorType();

    virtual ~ErrorType();

    /**
     * @brief Basic to String method giving just Subsystem ID
     *        and Errorcode
     * @return Non interpreted string
     */
    virtual QString toString() const;
};


/**
 * @brief Specialized ErrorType for all copters. Should only be
 *        used if an full interpreted error string is needed.
 *        Just use the copy constructor to construct it from an
 *        ErrorType.
 */
class CopterErrorType : public ErrorBase
{
public:

    CopterErrorType();

    /**
     * @brief Copy constructor to create a CopterErrorType from
     *        ErrorType
     * @param ErrorType code to construct from.
     */
    CopterErrorType(ErrorType &code);

    virtual ~CopterErrorType();

    /**
     * @brief Specialized to String method giving a full interpreted
     *        string for copters
     * @return Full interpreted string
     */
    virtual QString toString() const;
};



#endif // ARDUPILOTMAV_H
