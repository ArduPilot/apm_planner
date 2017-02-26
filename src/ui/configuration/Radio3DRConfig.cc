/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013-2017 APM_PLANNER PROJECT <http://www.diydrones.com>

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

#include "logging.h"
#include "MainWindow.h"
#include "SerialSettingsDialog.h"
#include "Radio3DRConfig.h"
#include "Radio3DRSettings.h"
#include "RadioFlashWizard.h"

#include "configuration.h"

#include <QSettings>
#include <QtSerialPort/qserialportinfo.h>
#include <QtSerialPort/qserialport.h>
#include <QTimer>
#include <QMessageBox>

Radio3DRConfig::Radio3DRConfig(QWidget *parent) : QWidget(parent),
    m_radioSettings(NULL),
    m_state(none)
{
    ui.setupUi(this);
    m_settingsDialog = new SettingsDialog;

    ui.settingsButton->setEnabled(true);

    addBaudComboBoxConfig(ui.baudPortComboBox);
    m_settings.name = ui.linkPortComboBox->currentText();

    addRadioBaudComboBoxConfig(*ui.baudComboBox);
    addRadioBaudComboBoxConfig(*ui.baudComboBox_remote);
    addRadioAirBaudComboBoxConfig(*ui.airBaudComboBox);
    addRadioAirBaudComboBoxConfig(*ui.airBaudComboBox_remote);
    addTxPowerComboBoxConfig(*ui.txPowerComboBox);
    addTxPowerComboBoxConfig(*ui.txPowerComboBox_remote);
    addRtsCtsComboBoxConfig(*ui.rtsCtsComboBox);
    addRtsCtsComboBoxConfig(*ui.rtsCtsComboBox_remote);

    addMavLinkComboBoxConfig(*ui.mavLinkComboBox_remote);

    setButtonState(true); // start with buttons disabled

    populateSerialPorts();

    loadSavedSerialSettings();

    initConnections();

    //connect timer for when refreshing the serial port list
    connect(&m_timer,SIGNAL(timeout()),this,SLOT(populateSerialPorts()));
}

Radio3DRConfig::~Radio3DRConfig()
{
    delete m_settingsDialog;
    delete m_radioSettings;
}

void Radio3DRConfig::addBaudComboBoxConfig(QComboBox *comboBox)
{
    comboBox->addItem(QLatin1String("115200"), QSerialPort::Baud115200);
    comboBox->addItem(QLatin1String("57600"), QSerialPort::Baud57600);
    comboBox->addItem(QLatin1String("38400"), QSerialPort::Baud38400);
    comboBox->addItem(QLatin1String("19200"), QSerialPort::Baud19200);
    comboBox->addItem(QLatin1String("9600"), QSerialPort::Baud9600);
    comboBox->addItem(QLatin1String("4800"), QSerialPort::Baud4800);
    comboBox->addItem(QLatin1String("2400"), QSerialPort::Baud2400);
    comboBox->addItem(QLatin1String("1200"), QSerialPort::Baud1200);

    // Set combobox to default baud rate of 57600.
    comboBox->setCurrentIndex(2);
}

void Radio3DRConfig::fillPortsInfo(QComboBox &comboBox)
{
    QLOG_TRACE() << "3DR Radio fillPortsInfo ";
    QString current = comboBox.itemText(comboBox.currentIndex());
    disconnect(&comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
    comboBox.clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        int found = comboBox.findData(list);
        if ((found == -1)&& (info.manufacturer().contains("FTDI") || info.manufacturer().contains("Silicon Labs"))) {
            QLOG_INFO() << "Found " << list.first();
            comboBox.insertItem(0,list[0], list);
        } else {
            // Do nothing as the port is already listed
        }
    }
    for (int i=0;i<comboBox.count();i++)
    {
        if (comboBox.itemText(i) == current)
        {
            comboBox.setCurrentIndex(i);
            break;
        }
    }
    if(comboBox.count() == 0)
    {
       // no interface found
       comboBox.insertItem(0, "None", QStringList());
       setButtonState(true);
    }
    else
    {
       setLink(comboBox.currentIndex());
       setButtonState(false);
       connect(&comboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
    }
}

void Radio3DRConfig::loadSavedSerialSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();
    settings.beginGroup("3DRRADIO");
    if (settings.contains("COMM_PORT"))
    {
        m_settings.name = settings.value("COMM_PORT").toString();
        m_settings.baudRate = settings.value("COMM_BAUD").toInt();
        m_settings.parity = static_cast<QSerialPort::Parity>
                (settings.value("COMM_PARITY").toInt());
        m_settings.stopBits = static_cast<QSerialPort::StopBits>
                (settings.value("COMM_STOPBITS").toInt());
        m_settings.dataBits = static_cast<QSerialPort::DataBits>
                (settings.value("COMM_DATABITS").toInt());
        m_settings.flowControl = static_cast<QSerialPort::FlowControl>
                (settings.value("COMM_FLOW_CONTROL").toInt());

        ui.linkPortComboBox->setCurrentIndex(ui.linkPortComboBox->findText(m_settings.name));
        ui.baudPortComboBox->setCurrentIndex(ui.baudPortComboBox->findData(m_settings.baudRate));
    } else {
        ui.baudPortComboBox->setCurrentIndex(ui.baudPortComboBox->findData(QSerialPort::Baud57600));
    }
}

void Radio3DRConfig::saveSerialSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("3DRRADIO");
    settings.setValue("COMM_PORT", m_settings.name);
    settings.setValue("COMM_BAUD", m_settings.baudRate);
    settings.setValue("COMM_PARITY", m_settings.parity);
    settings.setValue("COMM_STOPBITS", m_settings.stopBits);
    settings.setValue("COMM_DATABITS", m_settings.dataBits);
    settings.setValue("COMM_FLOW_CONTROL", m_settings.flowControl);
    settings.endGroup();
    settings.sync();
}

void Radio3DRConfig::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    // Start refresh Timer
    QLOG_DEBUG() << "3DR Radio Start Serial Port Scanning";
    m_timer.start(RADIO3DR_UPDATE_PORT_TIME);
    loadSavedSerialSettings();

    MainWindow::instance()->toolBar().disableConnectWidget(true);
}

void Radio3DRConfig::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event);
    // Stop the port list refeshing
    QLOG_DEBUG() << "3DR Radio Stop Serial Port Scanning";
    m_timer.stop();
    saveSerialSettings();
    QLOG_DEBUG() << "3DR Radio Remove Connection to Serial Port";
    delete m_radioSettings;
    resetUI();
    MainWindow::instance()->toolBar().disableConnectWidget(false);
}

void Radio3DRConfig::populateSerialPorts()
{
    QLOG_TRACE() << "populateSerialPorts";
    fillPortsInfo(*ui.linkPortComboBox);
}

void Radio3DRConfig::initConnections()
{
    // Ui Connections
    connect(ui.loadSettingsButton, SIGNAL(clicked()), this, SLOT(readRadioSettings()));
    connect(ui.saveSettingsButton, SIGNAL(clicked()), this, SLOT(writeRemoteRadioSettings()));
    connect(ui.resetDefaultsButton, SIGNAL(clicked()), this, SLOT(resetRemoteRadioSettingsToDefaults()));
    connect(ui.flashPushButton, SIGNAL(clicked()), this, SLOT(flashButtonClicked()));
    connect(ui.copyToRemoteButton, SIGNAL(clicked()), this, SLOT(copyLocalSettingsToRemote()));

    connect(ui.settingsButton, SIGNAL(released()), m_settingsDialog, SLOT(show()));

    connect(ui.baudPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setBaudRate(int)));
    connect(ui.linkPortComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setLink(int)));
}

void Radio3DRConfig::serialPortOpenFailure(int error, QString errorString)
{
    Q_UNUSED(error);
    QLOG_ERROR() << "Serial Port Open Crtical Error!" << errorString;
    QMessageBox::critical(this, tr("Serial Port"), "Cannot open serial port, please make sure you have your radio connected, and the correct link selected)");
}

void Radio3DRConfig::serialConnectionFailure(QString errorString)
{
   QLOG_ERROR() << "Crtical Error " << errorString;
   // disable buttons
   setButtonState(true);
   // restart serial port scanning
   m_timer.start();
}

void Radio3DRConfig::setBaudRate(int index)
{
    m_settings.baudRate = static_cast<QSerialPort::BaudRate>(
                ui.baudPortComboBox->itemData(index).toInt());
    QLOG_INFO() << "Changed Baud to:" << m_settings.baudRate;

}

void Radio3DRConfig::setLink(int index)
{
    if (index == -1)
    {
        return;
    }
    QString tempName = ui.linkPortComboBox->itemData(index).toStringList()[0];
    if(m_settings.name != tempName)
    {
        m_settings.name = tempName;
        QLOG_INFO() << "Changed Link to:" << m_settings.name;
    }
}

void Radio3DRConfig::readRadioSettings()
{
    QLOG_INFO() << "read 3DR Radio Settings";

    if (m_radioSettings == NULL) {
        m_radioSettings = new Radio3DRSettings(this);
        connect(m_radioSettings, SIGNAL(localReadComplete(Radio3DREeprom&, bool)),
                this, SLOT(localReadComplete(Radio3DREeprom&, bool)));
        connect(m_radioSettings, SIGNAL(remoteReadComplete(Radio3DREeprom&,bool)),
                this, SLOT(remoteReadComplete(Radio3DREeprom&, bool)));
        connect(m_radioSettings, SIGNAL(serialPortOpenFailure(int,QString)),
                this, SLOT(serialPortOpenFailure(int, QString)));
        connect(m_radioSettings, SIGNAL(serialConnectionFailure(QString)),
                this, SLOT(serialConnectionFailure(QString)));
        connect(m_radioSettings, SIGNAL(updateLocalStatus(QString, Radio3DRSettings::stateColor)),
                this, SLOT(updateLocalStatus(QString, Radio3DRSettings::stateColor)));
        connect(m_radioSettings, SIGNAL(updateRemoteStatus(QString, Radio3DRSettings::stateColor)),
                this, SLOT(updateRemoteStatus(QString, Radio3DRSettings::stateColor)));
        connect(m_radioSettings, SIGNAL(updateLocalComplete(int)),
                this, SLOT(updateLocalComplete(int)));
        connect(m_radioSettings, SIGNAL(updateRemoteComplete(int)),
                this, SLOT(updateRemoteComplete(int)));
        connect(m_radioSettings, SIGNAL(updateLocalRssi(QString)),
                this, SLOT(updateLocalRssi(QString)));
        connect(m_radioSettings, SIGNAL(updateRemoteRssi(QString)),
                this, SLOT(updateRemoteRssi(QString)));

    }

    resetUI();

    if(m_radioSettings->openSerialPort(m_settings)){
         m_timer.stop(); // Stop updatuing the ports combobox

        m_radioSettings->writeEscapeSeqeunce(); // Start Sate machine
    }
}

void Radio3DRConfig::updateLocalStatus(QString status, Radio3DRSettings::stateColor color)
{
    QLOG_DEBUG() << "local status:" << status;
    ui.localStatus->setStyleSheet("QLabel { color : " + Radio3DRSettings::stateColorToString(color) + "; }");
    ui.localStatus->setText(tr("<b>STATUS:</b> ") + status);
}

void Radio3DRConfig::updateRemoteStatus(QString status, Radio3DRSettings::stateColor color)
{
    QLOG_DEBUG() << "remote status:" << status;
    ui.remoteStatus->setStyleSheet("QLabel { color : " + Radio3DRSettings::stateColorToString(color) + "; }");
    ui.remoteStatus->setText(tr("<b>STATUS:</b> ") + status);
}

void Radio3DRConfig::updateLocalComplete(int result)
{
    QString status;
    if (result != 0){
            status = "FAILED";
            ui.localStatus->setStyleSheet("QLabel { color : red; }");
    } else {
            status = "SUCCESS";
            ui.localStatus->setStyleSheet("QLabel { color : green; }");
    }

    QLOG_DEBUG() << "local status:" << status;
    ui.localStatus->setText(tr("<b>STATUS:</b> ") + status);

    if (result == 0){ // reboot for both reset and write states
        m_radioSettings->rebootRemoteRadio();
        thread()->usleep(300000);   // wait a little until reset is sent
        m_radioSettings->rebootLocalRadio();
    }

}

void Radio3DRConfig::updateRemoteComplete(int result)
{
    QString status;
    if (result != 0){
        status = "FAILED";
        ui.remoteStatus->setStyleSheet("QLabel { color : red; }");
    } else {
        status = "SUCCESS";
        ui.remoteStatus->setStyleSheet("QLabel { color : green; }");
    }

    QLOG_DEBUG() << "remote status:" << status;
    ui.remoteStatus->setText(tr("<b>STATUS:</b> ") + status);

    if (result == 0){
        switch(m_state){
        case writeRadioSettings:
            writeLocalRadioSettings();
            break;
        case resetRadioSettings:
            resetLocalRadioSettingsToDefaults();
            break;
        default:
            break;
        }
    }
}

void Radio3DRConfig::localReadComplete(Radio3DREeprom& eeprom, bool success)
{
    QLOG_INFO() << "local Radio Read Complete success:" << success;
    if (success){

        if (eeprom.version() < SIK_LOWLATENCY_MIN_VERSION){
            ui.maxWindowSpinBox->setEnabled(false);
            addMavLinkComboBoxConfig(*ui.mavLinkComboBox);
        } else {
            ui.maxWindowSpinBox->setEnabled(true);
            addMavLinkLowLatencyComboBoxConfig(*ui.mavLinkComboBox);
        }

        ui.versionLabel->setText(eeprom.versionString());

        ui.formatLineEdit->setText(QString::number(eeprom.eepromVersion()));
        ui.baudComboBox->setCurrentIndex(ui.baudComboBox->findData(eeprom.serialSpeed()));
        ui.airBaudComboBox->setCurrentIndex(ui.airBaudComboBox->findData(eeprom.airSpeed()));
        ui.netIdSpinBox->setValue(eeprom.netID());
        ui.txPowerComboBox->setCurrentIndex(ui.txPowerComboBox->findData(eeprom.txPower()));
        ui.eccCheckBox->setChecked(eeprom.ecc());
        ui.mavLinkComboBox->setCurrentIndex(eeprom.mavlink());
        ui.opResendCheckBox->setChecked(eeprom.oppResend());

        ui.dutyCycleSpinBox->setValue(eeprom.dutyCyle());
        ui.lbtRssiSpinBox->setValue(eeprom.lbtRssi());
        ui.numChannelsSpinBox->setValue(eeprom.numChannels());

        setupFrequencyComboBox(*ui.minFreqComboBox, eeprom.frequencyCode());
        setupFrequencyComboBox(*ui.maxFreqComboBox, eeprom.frequencyCode());

        int lowFreqIndex = ui.maxFreqComboBox->findData(eeprom.minFreq());
        if (lowFreqIndex == -1){
            ui.maxFreqComboBox->addItem(QString::number(eeprom.minFreq()), eeprom.minFreq());
        }
        ui.minFreqComboBox->setCurrentIndex(ui.minFreqComboBox->findData(eeprom.minFreq()));

        int highFreqIndex = ui.maxFreqComboBox->findData(eeprom.maxFreq());
        if (highFreqIndex == -1){
            ui.maxFreqComboBox->addItem(QString::number(eeprom.maxFreq()), eeprom.maxFreq());
        }
        ui.maxFreqComboBox->setCurrentIndex(ui.maxFreqComboBox->findData(eeprom.maxFreq()));

        ui.maxWindowSpinBox->setValue(eeprom.maxWindow());

        ui.rtsCtsComboBox->setCurrentIndex(ui.rtsCtsComboBox->findData(eeprom.rtsCts()));
    }
}

void Radio3DRConfig::resetUI()
{
    ui.versionLabel->setText("");
    ui.rssiTextEdit->setText("");
    ui.localStatus->setStyleSheet("QLabel { color : black; }");
    ui.localStatus->setText(tr("STATUS:"));

    ui.versionLabel_remote->setText("");
    ui.rssiTextEdit_remote->setText("");
    ui.remoteStatus->setStyleSheet("QLabel { color : black; }");
    ui.remoteStatus->setText(tr("STATUS:"));
}

void Radio3DRConfig::remoteReadComplete(Radio3DREeprom& eeprom, bool success)
{
    QLOG_INFO() << "remote Radio Read Complete success:" << success;
    if (success){

        if (eeprom.version() < SIK_LOWLATENCY_MIN_VERSION){
            ui.maxWindowSpinBox_remote->setEnabled(false);
            addMavLinkComboBoxConfig(*ui.mavLinkComboBox_remote);
        } else {
            ui.maxWindowSpinBox_remote->setEnabled(true);
             addMavLinkLowLatencyComboBoxConfig(*ui.mavLinkComboBox_remote);
        }

        ui.versionLabel_remote->setText(eeprom.versionString());

        ui.formatLineEdit_remote->setText(QString::number(eeprom.eepromVersion()));
        ui.baudComboBox_remote->setCurrentIndex(ui.baudComboBox_remote->findData(eeprom.serialSpeed()));
        ui.airBaudComboBox_remote->setCurrentIndex(ui.airBaudComboBox_remote->findData(eeprom.airSpeed()));
        ui.netIdSpinBox_remote->setValue(eeprom.netID());
        ui.txPowerComboBox_remote->setCurrentIndex(ui.txPowerComboBox_remote->findData(eeprom.txPower()));
        ui.eccCheckBox_remote->setChecked(eeprom.ecc());
        ui.mavLinkComboBox_remote->setCurrentIndex(eeprom.mavlink());
        ui.opResendCheckBox_remote->setChecked(eeprom.oppResend());

        ui.dutyCycleSpinBox_remote->setValue(eeprom.dutyCyle());
        ui.lbtRssiSpinBox_remote->setValue(eeprom.lbtRssi());
        ui.numChannelsSpinBox_remote->setValue(eeprom.numChannels());

        setupFrequencyComboBox(*ui.minFreqComboBox_remote, eeprom.frequencyCode());
        setupFrequencyComboBox(*ui.maxFreqComboBox_remote, eeprom.frequencyCode());

        int lowFreqIndex = ui.minFreqComboBox_remote->findData(eeprom.minFreq());
        if (lowFreqIndex == -1){
            ui.minFreqComboBox_remote->addItem(QString::number(eeprom.minFreq()), eeprom.minFreq());
        }
        ui.minFreqComboBox_remote->setCurrentIndex(ui.minFreqComboBox_remote->findData(eeprom.minFreq()));

        int highFreqIndex = ui.maxFreqComboBox_remote->findData(eeprom.maxFreq());
        if (highFreqIndex == -1){
            ui.maxFreqComboBox_remote->addItem(QString::number(eeprom.maxFreq()), eeprom.maxFreq());
        }
        ui.maxFreqComboBox_remote->setCurrentIndex(ui.maxFreqComboBox_remote->findData(eeprom.maxFreq()));

        ui.maxWindowSpinBox_remote->setValue(eeprom.maxWindow());

        ui.rtsCtsComboBox_remote->setCurrentIndex(ui.rtsCtsComboBox_remote->findData(eeprom.rtsCts()));
    }
}


void Radio3DRConfig::writeLocalRadioSettings()
{
    QLOG_INFO() << "save 3DR Local Radio Settings";

    if(m_radioSettings == NULL){
        QMessageBox::critical(this, tr("Radio Config"), tr("Please Load Settings before attempting to change them"));
        return;
    }

    m_newRadioSettings.setVersion(ui.versionLabel->text()); // This sets the correct number of PARAMS
    m_newRadioSettings.airSpeed(ui.airBaudComboBox->itemData(ui.airBaudComboBox->currentIndex()).toInt());
    m_newRadioSettings.serialSpeed(ui.baudComboBox->itemData(ui.baudComboBox->currentIndex()).toInt());
    m_newRadioSettings.netID(ui.netIdSpinBox->value());
    m_newRadioSettings.txPower(ui.txPowerComboBox->itemData(ui.txPowerComboBox->currentIndex()).toInt());
    m_newRadioSettings.ecc(ui.eccCheckBox->checkState() == Qt::Checked?1:0);
    m_newRadioSettings.mavlink(ui.mavLinkComboBox->currentIndex());
    m_newRadioSettings.oppResend(ui.opResendCheckBox->checkState() == Qt::Checked?1:0);
    m_newRadioSettings.minFreq(ui.minFreqComboBox->itemData(ui.minFreqComboBox->currentIndex()).toInt());
    m_newRadioSettings.maxFreq(ui.maxFreqComboBox->itemData(ui.maxFreqComboBox->currentIndex()).toInt());
    m_newRadioSettings.numChannels(ui.numChannelsSpinBox->value());
    m_newRadioSettings.dutyCyle(ui.dutyCycleSpinBox->value());
    m_newRadioSettings.lbtRssi(ui.lbtRssiSpinBox->value());
    m_newRadioSettings.manchester(0);
    m_newRadioSettings.rtsCts(ui.rtsCtsComboBox->itemData(ui.rtsCtsComboBox->currentIndex()).toInt());
    m_newRadioSettings.maxWindow(ui.maxWindowSpinBox->value());

    m_radioSettings->writeLocalSettings(m_newRadioSettings);
    m_state = complete;

}

void Radio3DRConfig::writeRemoteRadioSettings()
{
    QLOG_INFO() << "save 3DR Remote Radio Settings";

    if(m_radioSettings == NULL){
        QMessageBox::critical(this, tr("Radio Config"), tr("Please Load Settings before attempting to change them"));
        return;
    }

    if (ui.versionLabel_remote->text().length() == 0){
        // No remote radio connected so write the local version.
        writeLocalRadioSettings();
        return;
    }

    m_newRadioSettings.setVersion(ui.versionLabel_remote->text()); // This sets the correct number of PARAMS
    m_newRadioSettings.airSpeed(ui.airBaudComboBox_remote->itemData(ui.airBaudComboBox_remote->currentIndex()).toInt());
    m_newRadioSettings.serialSpeed(ui.baudComboBox_remote->itemData(ui.baudComboBox_remote->currentIndex()).toInt());
    m_newRadioSettings.netID(ui.netIdSpinBox_remote->value());
    m_newRadioSettings.txPower(ui.txPowerComboBox_remote->itemData(ui.txPowerComboBox_remote->currentIndex()).toInt());
    m_newRadioSettings.ecc(ui.eccCheckBox_remote->checkState() == Qt::Checked?1:0);
    m_newRadioSettings.mavlink(ui.mavLinkComboBox_remote->currentIndex());
    m_newRadioSettings.oppResend(ui.opResendCheckBox_remote->checkState() == Qt::Checked?1:0);
    m_newRadioSettings.minFreq(ui.minFreqComboBox_remote->itemData(ui.minFreqComboBox_remote->currentIndex()).toInt());
    m_newRadioSettings.maxFreq(ui.maxFreqComboBox_remote->itemData(ui.maxFreqComboBox_remote->currentIndex()).toInt());
    m_newRadioSettings.numChannels(ui.numChannelsSpinBox_remote->value());
    m_newRadioSettings.dutyCyle(ui.dutyCycleSpinBox_remote->value());
    m_newRadioSettings.lbtRssi(ui.lbtRssiSpinBox_remote->value());
    m_newRadioSettings.manchester(0);
    m_newRadioSettings.rtsCts(ui.rtsCtsComboBox_remote->itemData(ui.rtsCtsComboBox_remote->currentIndex()).toInt());
    m_newRadioSettings.maxWindow(ui.maxWindowSpinBox_remote->value());

    m_radioSettings->writeRemoteSettings(m_newRadioSettings);
    m_state = writeRadioSettings;

}

void Radio3DRConfig::copyLocalSettingsToRemote()
{
    QLOG_INFO() << "Copy 3DR Local Radio Settings to remote";
    if(m_radioSettings == NULL){
        QMessageBox::critical(this, tr("Radio Config"), tr("Please Load Settings before attempting to change them"));
        return;
    }

    ui.baudComboBox_remote->setCurrentIndex(ui.baudComboBox->currentIndex());
    ui.airBaudComboBox_remote->setCurrentIndex(ui.airBaudComboBox->currentIndex());
    ui.netIdSpinBox_remote->setValue(ui.netIdSpinBox->value());
    ui.txPowerComboBox_remote->setCurrentIndex(ui.txPowerComboBox->currentIndex());
    ui.eccCheckBox_remote->setChecked(ui.eccCheckBox->checkState());
    ui.mavLinkComboBox_remote->setCurrentIndex(ui.mavLinkComboBox->currentIndex());
    ui.opResendCheckBox_remote->setChecked(ui.opResendCheckBox->checkState());

    ui.dutyCycleSpinBox_remote->setValue(ui.dutyCycleSpinBox->value());
    ui.lbtRssiSpinBox_remote->setValue(ui.lbtRssiSpinBox->value());
    ui.numChannelsSpinBox_remote->setValue(ui.numChannelsSpinBox->value());

    ui.minFreqComboBox_remote->setCurrentIndex(ui.minFreqComboBox->currentIndex());
    ui.maxFreqComboBox_remote->setCurrentIndex(ui.maxFreqComboBox->currentIndex());

    ui.maxWindowSpinBox_remote->setValue(ui.maxWindowSpinBox->value());
    ui.rtsCtsComboBox_remote->setCurrentIndex(ui.rtsCtsComboBox->currentIndex());
}

void Radio3DRConfig::updateLocalRssi(QString status)
{
    ui.rssiTextEdit->setText(status);
}

void Radio3DRConfig::updateRemoteRssi(QString status)
{
    ui.rssiTextEdit_remote->setText(status);
}

void Radio3DRConfig::addRadioBaudComboBoxConfig(QComboBox &comboBox)
{
    comboBox.addItem(QLatin1String("115200"), 115 );
    comboBox.addItem(QLatin1String("57600"), 57 );
    comboBox.addItem(QLatin1String("38400"), 38 );
    comboBox.addItem(QLatin1String("19200"), 19 );
    comboBox.addItem(QLatin1String("9600"), 9 );
    comboBox.addItem(QLatin1String("4800"), 4 );
    comboBox.addItem(QLatin1String("2400"), 2 );
    comboBox.addItem(QLatin1String("1200"), 1 );

}

void Radio3DRConfig::addRadioAirBaudComboBoxConfig(QComboBox &comboBox)
{
    comboBox.addItem(QLatin1String("250000"), 250);
    comboBox.addItem(QLatin1String("192000"), 192 );
    comboBox.addItem(QLatin1String("128000"), 128 );
    comboBox.addItem(QLatin1String("96000"), 96 );
    comboBox.addItem(QLatin1String("64000"), 64 );
    comboBox.addItem(QLatin1String("32000"), 32 );
    comboBox.addItem(QLatin1String("16000"), 16 );
    comboBox.addItem(QLatin1String("8000"), 8 );
    comboBox.addItem(QLatin1String("4000"), 4 );
    comboBox.addItem(QLatin1String("2000"), 2 );
}

void Radio3DRConfig::addTxPowerComboBoxConfig(QComboBox &comboBox)
{
    comboBox.addItem(QLatin1String("1dBm"), 1); // (1.3mW)
    comboBox.addItem(QLatin1String("2dBm"), 2); //  (1.6mW)
    comboBox.addItem(QLatin1String("5dBm"), 5); // (3.2mW)
    comboBox.addItem(QLatin1String("8dBm"), 8); // (6.3mW)
    comboBox.addItem(QLatin1String("11dBm"), 11); // (12.5mW)
    comboBox.addItem(QLatin1String("14dBm"), 14); //  (25mW)
    comboBox.addItem(QLatin1String("17dBm"), 17); // (50mW)
    comboBox.addItem(QLatin1String("20dBm"), 20); //  (100mW)
}

void Radio3DRConfig::addRtsCtsComboBoxConfig(QComboBox &comboBox)
{
    comboBox.addItem(QLatin1String("Off"), 0);
    comboBox.addItem(QLatin1String("On"), 1);
}

void Radio3DRConfig::addMavLinkComboBoxConfig(QComboBox &comboBox)
{
    comboBox.clear();
    comboBox.addItem(QLatin1String("None"), 0);
    comboBox.addItem(QLatin1String("MAVLink"), 1);
}

void Radio3DRConfig::addMavLinkLowLatencyComboBoxConfig(QComboBox &comboBox)
{
    comboBox.clear();
    comboBox.addItem(QLatin1String("None"), 0);
    comboBox.addItem(QLatin1String("High Bandwidth"), 1);
    comboBox.addItem(QLatin1String("Low Latency"), 2);
}

void Radio3DRConfig::setupFrequencyComboBox(QComboBox &comboBox, int freqCode )
{
    int minFreq;
    int maxFreq;
    int freqStepSize;

    switch(freqCode){
    case Radio3DREeprom::FREQ_915:
        minFreq = 895000;
        maxFreq = 935000;
        freqStepSize = 1000;
        break;
    case Radio3DREeprom::FREQ_433:
        minFreq = 414000;
        maxFreq = 460000;
        freqStepSize = 50;
        break;
    case Radio3DREeprom::FREQ_868:
        minFreq = 849000;
        maxFreq = 889000;
        freqStepSize = 1000;
        break;
    default:
        minFreq = 1;    // this supports RFD900, RFD900A, RFD900U, RFD900P
        maxFreq = 30;
        freqStepSize = 1;
    }
    for (int freq = minFreq; freq <= maxFreq; freq = freq + freqStepSize) {
        comboBox.addItem(QString::number(freq), freq);
    }
}

void Radio3DRConfig::resetRemoteRadioSettingsToDefaults()
{
    if(m_radioSettings) {

        if (QMessageBox::warning(this, tr("Reset Radios"), tr("You are about to reset your radios to their factory settings!"),
                             QMessageBox::Ok, QMessageBox::Cancel) == QMessageBox::Ok){
            m_radioSettings->resetRemoteRadioToDefaults();
            m_state = resetRadioSettings;
        }
    }
}

void Radio3DRConfig::resetLocalRadioSettingsToDefaults()
{
    if(m_radioSettings) {
        m_radioSettings->resetLocalRadioToDefaults();
        m_state = complete;
    }
}

void Radio3DRConfig::flashButtonClicked()
{
    QLOG_DEBUG() << "Radio Flash Wizard Started";
    RadioFlashWizard* flashRadioWizard = new RadioFlashWizard(this);
    flashRadioWizard->exec();
}

void Radio3DRConfig::setButtonState(bool disabled)
{
   ui.loadSettingsButton->setDisabled(disabled);
   ui.saveSettingsButton->setDisabled(disabled);
   ui.resetDefaultsButton->setDisabled(disabled);
   ui.flashPushButton->setDisabled(disabled);
   ui.copyToRemoteButton->setDisabled(disabled);
}

