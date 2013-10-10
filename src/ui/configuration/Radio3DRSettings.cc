#include "QsLog.h"
#include "Radio3DRSettings.h"
#include "qserialport.h"
#include "qserialportinfo.h"

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
    m_rtsCts(0)
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

     }
     return success;
}

bool Radio3DREeprom::setVersion(QString &versionString)
{
    QLOG_DEBUG() << "Radio3DREeprom Ver:" << versionString;
    QStringList values = versionString.split(" ", QString::SkipEmptyParts);
    if(values[0].contains("SiK")){
        m_version = values[1].toFloat();
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
    QString result;

    if(mode == local) {
        result = "A";
    } else {
        result = "R";
    }
    switch(index){
    case 1:
        result += "TS1=" + QString::number(m_serialSpeed) + "\r\n";
        break;
    case 2:
        result += "TS2=" + QString::number(m_airSpeed) + "\r\n";
        break;
    case 3:
        result += "TS3=" + QString::number(m_netID) + "\r\n";
        break;
    case 4:
        result += "TS4=" + QString::number(m_txPower) + "\r\n";
        break;
    case 5:
        result += "TS5=" + QString::number(m_ecc) + "\r\n";
        break;
    case 6:
        result += "TS6=" + QString::number(m_mavlink) + "\r\n";
        break;
    case 7:
        result += "TS7=" + QString::number(m_oppResend) + "\r\n";
        break;
    case 8:
        result += "TS8=" + QString::number(m_minFreq) + "\r\n";
        break;
    case 9:
        result += "TS9=" + QString::number(m_maxFreq) + "\r\n";
        break;
    case 10:
        result += "TS10=" + QString::number(m_numChannels) + "\r\n";
        break;
    case 11:
        result += "TS11=" + QString::number(m_dutyCyle) + "\r\n";
        break;
    case 12:
        result += "TS12=" + QString::number(m_lbtRssi) + "\r\n";
        break;
    case 13:
        result += "TS13=" + QString::number(m_manchester) + "\r\n";
        break;
    case 14:
        result += "TS14=" + QString::number(m_rtsCts) + "\r\n";
        break;
    }
    QLOG_DEBUG() << "Setting Radio Parameter: " << result;
    return result;
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
    m_timer.setSingleShot(true);
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
            connect( m_serialPort, SIGNAL(destoyed), this, SLOT(deleteSerialPort()));
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
        // Disengage Command Mode
//        m_serialPort->write("RTZ\r\n");
//        m_serialPort->write("ATZ\r\n");
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

            if (m_paramIndexSend > Radio3DREeprom::numberofParams){
                emit updateLocalStatus(tr("param write complete"));
                return;
            }

            QLOG_INFO() << "Writing Local Param:"
                        << m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend);
            m_serialPort->write(m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend).toAscii());

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

            if (m_paramIndexSend > Radio3DREeprom::numberofParams){
                emit updateRemoteStatus(tr("param write complete"));
                return;
            }

            QLOG_INFO() << "Writing Remote Param:"
                        << m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend);
            m_serialPort->write(m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend).toAscii());

        } else {
            emit updateRemoteStatus(tr("param %1 failed write").arg(m_paramIndexSend));
            QLOG_ERROR() << "FAILED Writing Remote Param:"
                         << m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote,m_paramIndexSend);
            // [ToDo] Can retry at this point
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
            QLOG_DEBUG() << "Read Local Freq:" << currentLine.toInt();
            emit updateLocalStatus(tr("read local radio frequency"));
            m_localRadio.setRadioFreqCode(currentLine.toInt());
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
        if (currentLine.contains("S14:")) {
            // All data received
            emit updateLocalStatus(tr("SUCCESS"));
            emit localReadComplete(m_localRadio, true);
            readRemoteVersionString();
        }; // else wait for more
    } break;

    case readRemoteVersion:{
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
            QLOG_DEBUG() << "Read Remote Freq:" << currentLine.toInt();
            emit updateRemoteStatus(tr("Read remote radio frequency"));
            m_remoteRadio.setRadioFreqCode(currentLine.toInt());
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
        if (currentLine.contains("S14:")) {
            // All data received
            emit updateRemoteStatus(tr("SUCCESS"));
            emit remoteReadComplete(m_remoteRadio, true);
//            closeSerialPort();
            // Start the RSSI reading
            readLocalRssi();
        };
    } break;

    case readRssiLocal:{
        if (currentLine.startsWith("L/R RSSI:")){
            emit updateRemoteRssi(currentLine);
        }
        readRemoteRssi();
    }break;

    case readRssiRemote:{
        if (currentLine.startsWith("L/R RSSI:")){
            emit updateLocalRssi(currentLine);
        }
        m_state = portOpen; // finshed
    }break;

    case error:
        QLOG_DEBUG() << "Error: " << m_serialPort->errorString();
    case none:
    case complete:
    default:
        closeSerialPort();
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
    } else {
        m_state = error;
        emit remoteReadComplete(m_remoteRadio, false);
        emit updateRemoteStatus(tr("FAILED"));
    }

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
    QLOG_DEBUG() << " Sending" << m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend).toAscii();
    m_serialPort->write(m_newLocalRadio.formattedParameter(Radio3DREeprom::local, m_paramIndexSend).toAscii());
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
    QLOG_DEBUG() << " Sending" << m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend).toAscii();
    m_serialPort->write(m_newRemoteRadio.formattedParameter(Radio3DREeprom::remote, m_paramIndexSend).toAscii());
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

void Radio3DRSettings::resetToDefaults()
{
    if (m_serialPort && m_serialPort->isOpen()){
        QLOG_INFO() << "Reseting local and remote radios back to defaults";
        m_serialPort->write("RT&F\r\nRTZ\rnAT&F\r\nATZ\r\n");
    }

}
