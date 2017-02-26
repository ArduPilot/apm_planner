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

#ifndef RADIO3DRSETTINGS_H
#define RADIO3DRSETTINGS_H

#include "SerialSettingsDialog.h"
#include <QObject>
#include <QPointer>
#include <QTimer>
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>

//  Command Set for 3DR SiK Radios
//
//        +++ - Escape to command mode (need 1s silence before it will work)
//        ATI - show radio version
//        ATI2 - show board type
//        ATI3 - show board frequency
//        ATI4 - show board version
//        ATI5 - show all user settable EEPROM parameters
//        ATI6 - display TDM timing report
//        ATI7 - display RSSI signal report
//        ATO - exit AT command mode
//        ATSn? - display radio parameter number 'n'
//        ATSn=X - set radio parameter number 'n' to 'X'
//        ATZ - reboot the radio
//        AT&W - write current parameters to EEPROM
//        AT&F - reset all parameters to factory default
//        AT&T=RSSI - enable RSSI debug reporting
//        AT&T=TDM - enable TDM debug reporting
//        AT&T - disable debug reporting
//
//        e.g.    ATI0 Version = SiK 1.5 on HM-TRP
//                ATI5
//                S0: FORMAT=25
//                S1: SERIAL_SPEED=57
//                S2: AIR_SPEED=64
//                S3: NETID=41
//                S4: TXPOWER=20
//                S5: ECC=1
//                S6: MAVLINK=1
//                S7: OPPRESEND=1
//                S8: MIN_FREQ=915000
//                S9: MAX_FREQ=928000
//                S10: NUM_CHANNELS=50
//                S11: DUTY_CYCLE=100
//                S12: LBT_RSSI=0
//                S13: MANCHESTER=0
//                S14: RTSCTS=0
//                S15: MAX_WINDOW=131 // new for SiK 1.8 and greater

#define SIK_LOWLATENCY_MIN_VERSION 1.8f
#define LAST_PARAM_SIK17_AND_LESS "S14"
#define LAST_PARAM_SIK18_AND_GREATER "S15"

class Radio3DREeprom
{
    // Helper Object that stores Radio Settings
public:

    enum Mode {local, remote};

    enum Frequency {
        FREQ_NONE   = 0xf0,
        FREQ_433    = 0x43,
        FREQ_470    = 0x47,
        FREQ_868    = 0x86,
        FREQ_915    = 0x91,
    };

    enum Board {
        DEVICE_ID_RF50      = 0x4d,
        DEVICE_ID_HM_TRP    = 0x4e,
        DEVICE_ID_RFD900    = 0X42,
        DEVICE_ID_RFD900A   = 0X43,

        DEVICE_ID_RFD900U   = 0X80 | 0x01,
        DEVICE_ID_RFD900P   = 0x80 | 0x02,

        FAILED = 0x11,
    };

public:

    Radio3DREeprom();
    // Take the repsonse from an A(R)TI5 repsonse.
    bool setVersion(const QString &versionString);
    void numberOfParams(int numberOfParams) { m_numberOfParams = numberOfParams;}
    const QString endParam() { return m_endParam;}
    bool setParameter(QString &parameterString);
    bool setRadioFreqCode(int freqCode);

    // returns a valid AT string to set the eeprom
    const QString formattedParameter(Mode mode, int index);

    // accessors
    float version() {return m_version;}
    const QString& versionString();
    QString deviceIdString();
    int deviceId() {return m_deviceId;}
    int numberOfParams() {return m_numberOfParams;}
    int eepromVersion() { return m_eepromVersion;}
    int serialSpeed(){ return m_serialSpeed;}
    int airSpeed(){ return m_airSpeed;}
    int netID(){ return m_netID;}
    int txPower(){ return m_txPower;}
    int ecc(){ return m_ecc;}
    int mavlink(){ return m_mavlink;}
    int oppResend(){ return m_oppResend;}
    int minFreq(){ return m_minFreq;}
    int maxFreq(){ return m_maxFreq;}
    int numChannels(){ return m_numChannels;}
    int dutyCyle(){ return m_dutyCyle;}
    int lbtRssi(){ return m_lbtRssi;}
    int manchester(){ return m_manchester;}
    int rtsCts(){ return m_rtsCts;}
    int radioFrequency();
    int frequencyCode() {return m_radioFreqCode;}
    QString frequencyCodeString();
    int maxWindow() {return m_maxWindow;}

    // settors
    void deviceId(int deviceId){m_deviceId = deviceId;}
    void serialSpeed(int serialSpeed){ m_serialSpeed = serialSpeed;}
    void airSpeed(int airSpeed){ m_airSpeed = airSpeed;}
    void netID(int netID){ m_netID = netID;}
    void txPower(int txPower){ m_txPower = txPower;}
    void ecc(int ecc){ m_ecc = ecc;}
    void mavlink(int mavlink){ m_mavlink = mavlink;}
    void oppResend(int oppResend){ m_oppResend = oppResend;}
    void minFreq(int minFreq){ m_minFreq = minFreq;}
    void maxFreq(int maxFreq){ m_maxFreq = maxFreq;}
    void numChannels(int numChannels){ m_numChannels = numChannels;}
    void dutyCyle(int dutyCycle){ m_dutyCyle = dutyCycle;}
    void lbtRssi(int lbtRssi){ m_lbtRssi = lbtRssi;}
    void manchester(int manchester){ m_manchester = manchester;}
    void rtsCts(int rtsCts){ m_rtsCts = rtsCts;}
    void maxWindow(int maxWindow){ m_maxWindow = maxWindow;}

private:
    // EEPROM settings
    QString m_versionString;
    int m_deviceId;
    int m_numberOfParams;
    int m_radioFreqCode;
    float m_version;
    int m_eepromVersion;
    int m_serialSpeed;
    int m_airSpeed;
    int m_netID;
    int m_txPower;
    int m_ecc;
    int m_mavlink;
    int m_oppResend;
    int m_minFreq;
    int m_maxFreq;
    int m_numChannels;
    int m_dutyCyle;
    int m_lbtRssi;
    int m_manchester;
    int m_rtsCts;
    int m_maxWindow;
    QString m_endParam;
};

class Radio3DRSettings : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief The stateColor enum will be emitted by the update status
     *        methods to allow the receiver an approbiate coloring of the
     *        output
     */
    enum stateColor {
       black,
       green,
       red
    };

    /**
     * @brief stateColorToString - converts a stateColor enum to a string
     * @param color - Color enum wich shall be converted
     * @return - the sting representation of the color
     */
    static QString stateColorToString(stateColor color);

    explicit Radio3DRSettings(QObject *parent = 0);
    virtual ~Radio3DRSettings();
    
signals:
    void serialPortOpenFailure(int error, QString errorString);
    void serialConnectionFailure(QString errorString);
    void startReadLocalParams();
    void startReadRemoteParams();
    void localReadComplete(Radio3DREeprom& eeprom, bool success);
    void remoteReadComplete(Radio3DREeprom& eeprom, bool success);
    void updateLocalComplete(int error);
    void updateRemoteComplete(int error);
    void updateLocalStatus(QString status, Radio3DRSettings::stateColor color);
    void updateRemoteStatus(QString status, Radio3DRSettings::stateColor color);
    void updateLocalRssi(QString status);
    void updateRemoteRssi(QString status);

public slots:
    bool openSerialPort(SerialSettings settings);

    void writeEscapeSeqeunce();
    void writeEscapeSeqeunceNow();
    void readLocalVersionString();
    void readRemoteVersionString();
    void readLocalRadioFrequency();
    void readRemoteRadioFrequency();
    void readLocalSettingsStrings();
    void readRemoteSettingsStrings();

    void writeLocalSettings(Radio3DREeprom eepromSettings);
    void writeRemoteSettings(Radio3DREeprom eepromSettings);

    void readLocalRssi();
    void readRemoteRssi();

    void resetLocalRadioToDefaults();
    void resetRemoteRadioToDefaults();

    void rebootLocalRadio();
    void rebootRemoteRadio();

    void readData();

    void readRemoteTimeout();
    void handleError(QSerialPort::SerialPortError error);

private:
    void closeSerialPort();

    enum State { none, sendEscapeSequence, enterCommandMode,
                 readLocalVersion, readLocalFrequency, readLocalParams,
                 readRemoteVersion, readRemoteFrequency,
                 readRemoteParams,
                 writeLocalParams,
                 writeRemoteParams,
                 readRssiLocal, readRssiRemote,
                 writeRemoteFactorySettings,
                 writeRemoteEepromValues,
                 writeLocalFactorySettings,
                 writeLocalEepromValues,
                 rebootLocal,
                 rebootRemote,
                 complete, portOpen, error };

private:
    State m_state;
    Radio3DREeprom m_localRadio;
    Radio3DREeprom m_newLocalRadio;
    Radio3DREeprom m_remoteRadio;
    Radio3DREeprom m_newRemoteRadio;

    // Helper Variables
    int m_freqStepSize; // 100 for 433Mhz, 1000 for 915/868Mhz & 1 for RFD900,

    QScopedPointer<QSerialPort> m_serialPort;
    int m_retryCount;
    QString m_rxBuffer;
    QTimer m_timer;
    QTimer m_timerReadWrite;
    int m_paramIndexSend;

};

#endif // RADIO3DRSETTINGS_H
