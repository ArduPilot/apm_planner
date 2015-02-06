#include "QsLog.h"
#include "Radio3DRSettings.h"
#include <QtSerialPort/qserialport.h>
#include <QtSerialPort/qserialportinfo.h>

#include <QMessageBox>
#include <QTimer>

Radio3DREeprom::Radio3DREeprom():
    m_radioFreqCode(0),
    m_version(0),
    m_eepromVersion(0),
    m_serialSpeed(0),
    m_airSpeed(0),
    m_netID(0),
    m_txPower(0),
    m_ecc(0),
    m_mavlink(0),
    m_oppResend(0),
    m_minFreq(0),
    m_maxFreq(0),
    m_numChannels(0),
    m_dutyCyle(0),
    m_lbtRssi(0),
    m_manchester(0),
    m_rtsCts(0),
    m_maxWindow(0)
{

}

bool Radio3DREeprom::setParameter(QString &parameterString)
{
     QLOG_DEBUG() << "Radio3DREeprom Param:" << parameterString;
     QStringList values = parameterString.split("=", QString::SkipEmptyParts);
     bool success = false;

     if(values.length() == 0){
         QLOG_ERROR() << "Parameter String has Zero Params";
         return false;
     } else {
        success = true;
     }

     if(values[0].contains("S0:")){
         m_eepromVersion = values[1].toInt();

     } else if(values[0].contains("S1:")){
         m_serialSpeed = values[1].toInt();

     } else if(values[0].contains("S2:")){
         m_airSpeed = values[1].toInt();

     } else if(values[0].contains("S3:")){
         m_netID = values[1].toInt();

     } else if(values[0].contains("S4:")){
         m_txPower = values[1].toInt();

     } else if(values[0].contains("S5:")){
         m_ecc = values[1].toInt();

     } else if(values[0].contains("S6:")){
         m_mavlink = values[1].toInt();

     } else if(values[0].contains("S7:")){
         m_oppResend = values[1].toInt();

     } else if(values[0].contains("S8:")){
         m_minFreq = values[1].toInt();

     } else if(values[0].contains("S9:")){
         m_maxFreq = values[1].toInt();

     } else if(values[0].contains("S10:")){
         m_numChannels = values[1].toInt();

     } else if(values[0].contains("S11:")){
         m_dutyCyle = values[1].toInt();

     } else if(values[0].contains("S12:")){
         m_lbtRssi = values[1].toInt();

     } else if(values[0].contains("S13:")){
         m_manchester = values[1].toInt();

     } else if(values[0].contains("S14:")){
         m_rtsCts = values[1].toInt();

     } else if(values[0].contains("S15:")){
         m_maxWindow = values[1].toInt();
     }
     return success;
}

bool Radio3DREeprom::setVersion(const QString &versionString)
{
    QLOG_DEBUG() << "Radio3DREeprom Ver:" << versionString;
    QStringList values = versionString.split(" ", QString::SkipEmptyParts);

    if(values.count()>0 && values[0].contains("SiK")){
//        QRegExp versionEx("\\d\\*.\\d+");
//        int pos = versionEx.indexIn(versionString);
//        if (pos > -1) {
//            m_version = versionEx.cap(1).toFloat();
//        }
        QString majorMinorVersion(values[1]);
        majorMinorVersion.truncate(3); // [TODO] retrun x.n as the version for now
                                       // but this doesn't work for 1.10 and greater
                                       // Need to fix the above RegExp code to match
                                       // xx.nn (it should but doesn't
        m_version = majorMinorVersion.toFloat();

        // The number of params should be worked out from the returned ATI5 message
        // Or by using a lookup table of version to num of params.
        if (m_version < SIK_LOWLATENCY_MIN_VERSION){
            m_numberOfParams = 14;
            m_endParam = "S14";
        } else {
            m_numberOfParams = 15;
            m_endParam = "S15";
        }

    } else {
        return false;
    }
    m_versionString = versionString;
    return true;
}

bool Radio3DREeprom::setRadioFreqCode(int freqCode)
{
    m_radioFreqCode = freqCode;
    return true;
}

const QString Radio3DREeprom::formattedParameter(Mode mode, int index)
{
    QString modeString;

    if(mode == local) {
        modeString = "A";
    } else {
        modeString = "R";
    }
    QString command;

    switch(index){
    case 1:
        command = QString("%1TS1=%2\r\n").arg(modeString).arg(m_serialSpeed);
        break;
    case 2:
        command = QString("%1TS2=%2\r\n").arg(modeString).arg(m_airSpeed);
        break;
    case 3:
        command = QString("%1TS3=%2\r\n").arg(modeString).arg(m_netID);
        break;
    case 4:
        command = QString("%1TS4=%2\r\n").arg(modeString).arg(m_txPower);
        break;
    case 5:
        command = QString("%1TS5=%2\r\n").arg(modeString).arg(m_ecc);
        break;
    case 6:
        command = QString("%1TS6=%2\r\n").arg(modeString).arg(m_mavlink);
        break;
    case 7:
        command = QString("%1TS7=%2\r\n").arg(modeString).arg(m_oppResend);
        break;
    case 8:
        command = QString("%1TS8=%2\r\n").arg(modeString).arg(m_minFreq);
        break;
    case 9:
        command = QString("%1TS9=%2\r\n").arg(modeString).arg(m_maxFreq);
        break;
    case 10:
        command = QString("%1TS10=%2\r\n").arg(modeString).arg(m_numChannels);
        break;
    case 11:
        command = QString("%1TS11=%2\r\n").arg(modeString).arg(m_dutyCyle);
        break;
    case 12:
        command = QString("%1TS12=%2\r\n").arg(modeString).arg(m_lbtRssi);
        break;
    case 13:
        command = QString("%1TS13=%2\r\n").arg(modeString).arg(m_manchester);
        break;
    case 14:
        command = QString("%1TS14=%2\r\n").arg(modeString).arg(m_rtsCts);
        break;
    case 15:
        command = QString("%1TS15=%2\r\n").arg(modeString).arg(m_maxWindow);
        break;
    }

    QLOG_DEBUG() << "Setting Radio Parameter: " << command;
    return command;
}

const QString& Radio3DREeprom::versionString()
{
    return m_versionString;
}

Radio3DRSettings::Radio3DRSettings(QObject *parent) :
    QObject(parent),
    m_state(none),
    m_serialPort(NULL),
    m_retryCount(0)
{
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
    //                S15: MAX_WINDOW=131 // or 33 for SiK 1.8 and greater
    m_timer.setSingleShot(true);
    m_timerReadWrite.setSingleShot(true);
}

Radio3DRSettings::~Radio3DRSettings()
{
    if (m_serialPort) m_serialPort->close();
    delete m_serialPort;
}

bool Radio3DRSettings::openSerialPort(SerialSettings settings)
{
    if (m_serialPort == NULL){
        m_serialPort = new QSerialPort(settings.name, this);

        if (!m_serialPort){
            emit localReadComplete(m_localRadio, false);
            emit updateLocalStatus(tr("FAILED to create serial port"));
            return false;
        }
    }

    m_serialPort->close();

    m_serialPort->setPortName(settings.name);

    if(m_serialPort->open(QIODevice::ReadWrite)){
        // Start Reading the settings
        QLOG_DEBUG() << "Radio3DRSettings Serial Port Open SUCCESS" << settings.name;
        // Serial Port Connections

        QLOG_DEBUG() << "Port" << settings.name << settings.baudRate << settings.dataBits
                     << "FC" << settings.flowControl << "P" << settings.parity
                     << "SB" << settings.stopBits;
        if (m_serialPort->setBaudRate(settings.baudRate)
                && m_serialPort->setDataBits(settings.dataBits)
                && m_serialPort->setFlowControl(settings.flowControl)
                && m_serialPort->setParity(settings.parity)
                && m_serialPort->setStopBits(settings.stopBits)) {
            QLOG_INFO() << "Port Configured with success";
            connect( m_serialPort, SIGNAL(destoyed()), this, SLOT(deleteSerialPort()));
        } else {
            QLOG_ERROR() << "Port Configured with FAIL";
            emit localReadComplete(m_localRadio, false);
            emit updateRemoteStatus(tr("FAILED to configure serial port"));
            emit serialPortOpenFailure(m_serialPort->error(), m_serialPort->errorString());
            closeSerialPort();
            return false;
        }

        connect(m_serialPort, SIGNAL(error(QSerialPort::SerialPortError)), this,
                SLOT(handleError(QSerialPort::SerialPortError)));

        connect(m_serialPort, SIGNAL(readyRead()), this, SLOT(readData()));
        m_state = enterCommandMode;
        return true;
    } else {
        // Abort
        QLOG_DEBUG() << "Radio3DRSettings Serial Port Open FAILURE!" << m_serialPort->errorString();
        emit serialPortOpenFailure(m_serialPort->error(), m_serialPort->errorString());
        closeSerialPort();
        return false;
    }

    return false;
}

void Radio3DRSettings::closeSerialPort()
{
    QLOG_DEBUG() << "Close Serial Port:" << m_serialPort->portName() << m_serialPort;
    if (m_serialPort){
        m_serialPort->flush();
        m_serialPort->close();
    }
}

void Radio3DRSettings::deleteSerialPort()
{
    QLOG_INFO() << "Delete Serial Port:" << m_serialPort->portName() << m_serialPort;
    if(m_serialPort) delete m_serialPort;
}

void Radio3DRSettings::readData()
{
    if(!m_serialPort->canReadLine()){
        QLOG_TRACE() << "read data: cannot read line";
        return;
    }

    QString currentLine(m_serialPort->readLine());
    QLOG_DEBUG() << "Radio readData currentline:" << currentLine;

    if(currentLine.contains("+++")){
        // We are already in command mode and it just reflecrted the escape sequence
        QLOG_WARN() << "3DR Radio already in command mode";
        // Reset Radios to known state and start again
        m_serialPort->flush();
//        m_serialPort->write("\r\n");
//        m_serialPort->flush();
        m_timer.stop();
        emit updateLocalStatus(tr("Already in Command Mode"));
        readLocalVersionString();
        return;
    }

    if(currentLine.contains("RTZ")){
        // Rebooting remote, now reboot local radio.
        m_serialPort->write("ATZ\r\n");
        return;
    }

    if(currentLine.startsWith("AT")||currentLine.startsWith("RT")){
        //throw away the command echo
        if(m_serialPort->canReadLine()){
           currentLine = m_serialPort->readLine();
           QLOG_DEBUG() << "Radio readData nextline:" << currentLine;

        } else {
            // no data to read so wait for more
            currentLine = "";
            return;
        }
    }

    switch(m_state){
    case enterCommandMode: {
        if (currentLine.contains("OK")){
            m_timer.stop();
            QLOG_INFO() << "3DR Radio: Entered Command Mode";
            emit updateLocalStatus(tr("Entered Command mode"));
//            m_serialPort->write("AT&T\r\n");
            readLocalVersionString();
        }
    } break;

    case writeLocalParams: {
        if (currentLine.contains("OK")){
            emit updateLocalStatus(tr("param %1 written ok").arg(m_paramIndexSend));
            m_paramIndexSend++;

            if (m_paramIndexSend > m_newLocalRadio.numberOfParams()){
                emit updateLocalStatus(tr("param write complete"));
                m_state = writeLocalEepromValues;
                m_serialPort->write("AT&W\r\n");
                return;
            }

            QLOG_INFO() << "Writing Local Param:"
                        << m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend);
            m_serialPort->write(m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend).toLatin1());

        } else {
            emit updateLocalStatus(tr("param %1 failed write").arg(m_paramIndexSend));
            QLOG_ERROR() << "FAILED Writing Local Param:"
                         << m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend);
            // [ToDo] Can retry at this point
        }
    } break;

    case writeRemoteParams: {
        if (currentLine.contains("OK")){
            emit updateRemoteStatus(tr("param %1 written ok").arg(m_paramIndexSend));
            m_paramIndexSend++;

            if (m_paramIndexSend > m_newRemoteRadio.numberOfParams()){
                emit updateRemoteStatus(tr("param write complete"));
                // Now write to eeprom and reset
                m_state = writeRemoteEepromValues;
                m_serialPort->write("RT&W\r\n");
                return;
            }

            QLOG_INFO() << "Writing Remote Param:"
                        << m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend);
            m_serialPort->write(m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend).toLatin1());

        } else {
            emit updateRemoteStatus(tr("param %1 failed write").arg(m_paramIndexSend));
            QLOG_ERROR() << "FAILED Writing Remote Param:"
                         << m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote,m_paramIndexSend);
            // [ToDo] Can retry at this point
        }
    } break;

    case writeRemoteFactorySettings: {
        if (currentLine.contains("OK")){
            emit updateRemoteStatus(tr("Reset to factory Settings"));
            m_serialPort->write("RT&F\r\n"); // Write eeprom settings
            m_state = writeRemoteEepromValues;
        } else {
            emit updateRemoteStatus(tr("Failed to reset to factory settings"));
            m_state = error;
        }
    } break;

    case writeRemoteEepromValues: {
        if (currentLine.contains("OK")){
            emit updateRemoteStatus(tr("Saved to EEPROM"));
            emit updateRemoteComplete(0); // 0 == SUCCESS
        } else {
            emit updateRemoteStatus(tr("Failed to save settings to eeprom"));
            emit updateRemoteComplete(-1); // -1 == FAILED
            m_state = error;
        }
    } break;

    case writeLocalFactorySettings: {
        if (currentLine.contains("OK")){
            emit updateLocalStatus(tr("Reset to factory Settings"));
            m_serialPort->write("AT&F\r\n"); // Write eeprom settings
            m_state = writeLocalEepromValues;
        } else {
            emit updateLocalStatus(tr("Failed to reset to factory settings"));
            m_state = error;
        }
    } break;

    case writeLocalEepromValues: {
        if (currentLine.contains("OK")){
            emit updateLocalStatus(tr("Saved to EEPROM"));
            emit updateLocalComplete(0); // 0 == SUCCESS
            m_state = none;
        } else {
            emit updateLocalStatus(tr("Failed to save settings to EEPROM"));
            emit updateLocalComplete(-1); // -1 == FAILED
            m_state = error;
        }
    } break;

    case readLocalVersion:{
        QRegExp rx("^SiK\\s+(.*)\\s+on\\s+(.*)");
        if(currentLine.contains(rx)) {
            QLOG_INFO() << "3DR Local Radio: Version" << currentLine;
            if (!m_localRadio.setVersion(currentLine)) {
                // Failed to read Version
                QLOG_DEBUG() << " Failed to Read Version";
                emit localReadComplete(m_localRadio, false);
                emit updateLocalStatus(tr("FAILED"));
                closeSerialPort();
                m_state = error;
            } else {
                emit updateLocalStatus(tr("Read local radio version"));
                readLocalRadioFrequency();
            }
        }
    } break;

    case readLocalFrequency:{
        if(currentLine.toInt() > 0){
            int freqCode = currentLine.toInt();
            QLOG_DEBUG() << "Read Local Freq:" << QString().sprintf("0x%x",freqCode);
            emit updateLocalStatus(tr("read local radio frequency"));
            m_localRadio.setRadioFreqCode(freqCode);
            readLocalSettingsStrings();
        } else {
            emit localReadComplete(m_localRadio, false);
            emit updateLocalStatus(tr("FAILED"));
            closeSerialPort();
            m_state = error;
        }
    } break;

    case readLocalParams:{
        //[TODO] add more error checking by checking return value of setParam
        m_localRadio.setParameter(currentLine);
        while(m_serialPort->canReadLine()){
            currentLine = m_serialPort->readLine();
            m_localRadio.setParameter(currentLine);
        };
        if (currentLine.contains(m_localRadio.endParam())) {
            // All data received
            emit updateLocalStatus(tr("SUCCESS"));
            emit localReadComplete(m_localRadio, true);
            readLocalRssi();
        }; // else wait for more
    } break;

    case readRemoteVersion:{
        m_timerReadWrite.stop();
        QRegExp rx("^SiK\\s+(.*)\\s+on\\s+(.*)");
        if(currentLine.contains(rx)) {
            QLOG_INFO() << "3DR Remote Radio: Version" << currentLine;
            if (!m_remoteRadio.setVersion(currentLine)) {
                // Failed to read Version
                QLOG_DEBUG() << " Failed to Read Version";
                emit remoteReadComplete(m_remoteRadio, false);
                emit updateRemoteStatus(tr("FAILED"));
                closeSerialPort();
                m_state = error;
            } else {
                emit updateRemoteStatus(tr("Remote Version read"));
                readRemoteRadioFrequency();
            }
        }
    } break;

    case readRemoteFrequency:{
        if(currentLine.toInt() > 0){
            int freqCode = currentLine.toInt();
            QLOG_DEBUG() << "Read Remote Freq:" << QString().sprintf("0x%x",freqCode);
            emit updateRemoteStatus(tr("Read remote radio frequency"));
            m_remoteRadio.setRadioFreqCode(freqCode);
            readRemoteSettingsStrings();
        } else {
            emit remoteReadComplete(m_localRadio, false);
            emit updateRemoteStatus(tr("FAILED"));
            closeSerialPort();
            m_state = error;
        }


    } break;

    case readRemoteParams:{
        //[TODO] add more error checking by checking return value of setParam
        m_remoteRadio.setParameter(currentLine);
        while(m_serialPort->canReadLine()){
            currentLine = m_serialPort->readLine();
            m_remoteRadio.setParameter(currentLine);
        };
        if (currentLine.contains(m_remoteRadio.endParam())) {
            // All data received
            emit updateRemoteStatus(tr("SUCCESS"));
            emit remoteReadComplete(m_remoteRadio, true);
//            closeSerialPort();
            // Start the RSSI reading
            readRemoteRssi();
        };
    } break;

    case readRssiLocal:{
        if (currentLine.startsWith("L/R RSSI:")){
            emit updateLocalRssi(currentLine);
        }
        readRemoteVersionString();
    }break;

    case readRssiRemote:{
        if (currentLine.startsWith("L/R RSSI:")){
            emit updateRemoteRssi(currentLine);
        }
        m_state = portOpen; // finshed
    }break;

    case rebootLocal:
    case rebootRemote:
        // May need to consume echo'd bytes here[?]
        break;
    case error:
        QLOG_DEBUG() << "Error: " << m_serialPort->errorString();
    case none:
    case complete:
    default:
//        closeSerialPort();
        m_state = complete;
    }
}

void Radio3DRSettings::writeEscapeSeqeunce()
{
    QLOG_DEBUG() << "Radio3DRSettings::writeEscapeSeqeunce()";
    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(writeEscapeSeqeunce()));
    emit updateLocalStatus(tr("entering command mode"));
    if (m_state == enterCommandMode){
        if (m_retryCount++ < 3) {
            QLOG_INFO() << "Retry " << m_retryCount;
            m_serialPort->write("\r\n");
            m_serialPort->flush();
            connect(&m_timer, SIGNAL(timeout()), this, SLOT(writeEscapeSeqeunceNow()), Qt::UniqueConnection);
            m_timer.start(1100);
        } else {
            m_retryCount = 0;
            closeSerialPort();
            m_state = error; // Quit any more retries
            emit localReadComplete(m_localRadio, false);
            emit remoteReadComplete(m_remoteRadio, false);
            emit updateLocalStatus(tr("FAILED"));
            emit updateRemoteStatus(tr("FAILED"));
        }
    }
}

void Radio3DRSettings::writeEscapeSeqeunceNow()
{
    disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(writeEscapeSeqeunceNow()));
    if (m_serialPort == NULL) return;

    QLOG_DEBUG() << "Radio3DRSettings::writeEscapeSeqeunceNow()";
    m_serialPort->write("\x2B\x2B\x2B"); // +++
    m_serialPort->flush();

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(writeEscapeSeqeunce()), Qt::UniqueConnection);
    m_timer.start(1100);
}


void Radio3DRSettings::readLocalVersionString()
{
    QLOG_DEBUG() << "Radio3DRSettings::readLocalVersionString()";
    emit updateLocalStatus(tr("Read local radio version"));
    if (m_serialPort && m_serialPort->isOpen()){
        // start state machine
        m_serialPort->write("ATI0\r\n");
        m_serialPort->flush();
        m_state = readLocalVersion;
    } else {
        m_state = error;
        emit localReadComplete(m_localRadio, false);
        emit updateRemoteStatus(tr("FAILED"));
    }
}

void Radio3DRSettings::readRemoteVersionString()
{
    QLOG_DEBUG() << "Radio3DRSettings::readRemoteVersionString()";
    emit updateRemoteStatus(tr("Reading remote version"));
    if (m_serialPort && m_serialPort->isOpen()){
        // start state machine
        m_serialPort->write("RTI0\r\n");
        m_serialPort->flush();
        m_state = readRemoteVersion;
        connect(&m_timerReadWrite, SIGNAL(timeout()), this , SLOT(readRemoteTimeout()));
        m_timerReadWrite.start(500);
    } else {
        m_state = error;
        emit remoteReadComplete(m_remoteRadio, false);
        emit updateRemoteStatus(tr("FAILED"));
    }
}

void Radio3DRSettings::readRemoteTimeout()
{
    // No remote so read local radio.
    QLOG_DEBUG() << " Failed to Read Remote Version";
    emit remoteReadComplete(m_remoteRadio, false);
    emit updateRemoteStatus(tr("FAILED"));
}

void Radio3DRSettings::readLocalRadioFrequency()
{
    QLOG_DEBUG() << "Radio3DRSettings::readLocalRadioFrequency()";
    emit updateLocalStatus(tr("Read local radio frequency"));
    if (m_serialPort && m_serialPort->isOpen()){
        // start state machine
        m_serialPort->write("ATI3\r\n");
        m_serialPort->flush();
        m_state = readLocalFrequency;
    } else {
        m_state = error;
        emit localReadComplete(m_localRadio, false);
        emit updateLocalStatus(tr("FAILED"));
    }
}

void Radio3DRSettings::readRemoteRadioFrequency()
{
    QLOG_DEBUG() << "Radio3DRSettings::readRemoteRadioFrequency()";
    emit updateRemoteStatus(tr("Read remote radio frequency"));
    if (m_serialPort && m_serialPort->isOpen()){
        // start state machine
        m_serialPort->write("RTI3\r\n");
        m_serialPort->flush();
        m_state = readRemoteFrequency;
    } else {
        m_state = error;
        emit remoteReadComplete(m_remoteRadio, false);
        emit updateRemoteStatus(tr("FAILED"));
    }
}

void Radio3DRSettings::readLocalSettingsStrings()
{
    QLOG_DEBUG() << "Radio3DRSettings::readLocalSettingsStrings()";
    emit updateLocalStatus(tr("Read local radio eeprom values"));
    if (m_serialPort && m_serialPort->isOpen()){
        m_serialPort->write("ATI5\r\n"); // Request all EEPROM params
        m_serialPort->flush();
        m_state = readLocalParams;
    } else {
        m_state = error;
        emit localReadComplete(m_localRadio, false);
        emit updateLocalStatus(tr("FAILED"));
    }
}

void Radio3DRSettings::readRemoteSettingsStrings()
{
    QLOG_DEBUG() << "Radio3DRSettings::readRemoteSettingsStrings()";
    emit updateRemoteStatus(tr("Read remote radio eeprom values"));
    if (m_serialPort && m_serialPort->isOpen()){
        m_serialPort->write("RTI5\r\n"); // Request all EEPROM params
        m_serialPort->flush();
        m_state = readRemoteParams;
    } else {
        m_state = error;
        emit remoteReadComplete(m_remoteRadio, false);
        emit updateRemoteStatus(tr("FAILED"));
    }
}


void Radio3DRSettings::writeLocalSettings(Radio3DREeprom eepromSettings)
{
    QLOG_DEBUG() << "Radio3DRSettings::writeLocalSettings()";
    if (!m_serialPort->isOpen()) {
        QLOG_DEBUG() << "Serial Port not Open";
        return;
    }
    m_newLocalRadio = eepromSettings;
    m_paramIndexSend = 1; // start of the first parameter
    QLOG_DEBUG() << " Sending" << m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend).toLatin1();
    m_serialPort->write(m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend).toLatin1());
    m_state = writeLocalParams;
}

void Radio3DRSettings::writeRemoteSettings(Radio3DREeprom eepromSettings)
{
    QLOG_DEBUG() << "Radio3DRSettings::writeRemoteSettings()";

    if (!m_serialPort->isOpen()) {
        QLOG_DEBUG() << "Serial Port not Open";
        return;
    }
    m_newRemoteRadio = eepromSettings;
    m_paramIndexSend = 1; // start of the first parameter
    QLOG_DEBUG() << " Sending" << m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend).toLatin1();
    m_serialPort->write(m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend).toLatin1());
    m_state = writeRemoteParams;
}

void Radio3DRSettings::readLocalRssi()
{
    QLOG_DEBUG() << "readLocalRssi";
    m_serialPort->write("ATI7\r\n");
    m_state = readRssiLocal;

}

void Radio3DRSettings::readRemoteRssi()
{
    QLOG_DEBUG() << "readRemoteRssi";
    m_serialPort->write("RTI7\r\n");
    m_state = readRssiRemote;
}

void Radio3DRSettings::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QLOG_ERROR() << "Crtical Error!" << m_serialPort->errorString();
        closeSerialPort();
    }
}

void Radio3DRSettings::resetLocalRadioToDefaults()
{
    if (!m_serialPort->isOpen()) {
        QLOG_DEBUG() << "Serial Port not Open";
        return;
    }
    QLOG_INFO() <<  "Reseting local radio back to factory settings";;

    m_serialPort->write("AT&F\r\n");
    m_state = writeLocalFactorySettings;

}

void Radio3DRSettings::resetRemoteRadioToDefaults()
{
    if (!m_serialPort->isOpen()) {
        QLOG_DEBUG() << "Serial Port not Open";
        return;
    }
    QLOG_INFO() << "Reseting remote radio back to factory settings";

    m_serialPort->write("RT&F\r\n");
    m_state = writeRemoteFactorySettings;

}

void Radio3DRSettings::rebootLocalRadio()
{
    if (!m_serialPort->isOpen()) {
        QLOG_DEBUG() << "Serial Port not Open";
        return;
    }
    QLOG_INFO() << "Reboot local radio";

    m_serialPort->flush();
    m_serialPort->write("ATZ\r\n");
    m_serialPort->flush();
    m_state = rebootLocal;

}

void Radio3DRSettings::rebootRemoteRadio()
{
    if (!m_serialPort->isOpen()) {
        QLOG_DEBUG() << "Serial Port not Open";
        return;
    }
    QLOG_INFO() << "Reboot remote radio";

    m_serialPort->flush();
    m_serialPort->write("RTZ\r\n");
    m_serialPort->flush();
    m_state = rebootRemote;

}
