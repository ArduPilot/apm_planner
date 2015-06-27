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

#include "ArduPilotMegaMAV.h"
#include "ApmFirmwareConfig.h"
#include "QsLog.h"
#include "LinkManager.h"
#include "LinkInterface.h"
#include <QtSerialPort/qserialportinfo.h>
#include "MainWindow.h"
#include "PX4FirmwareUploader.h"
#include <QTimer>
#include <QSettings>
#include "arduino_intelhex.h"

#define ATMEGA2560CHIPID QByteArray().append(0x1E).append(0x98).append(0x01)

static const QString DEFAULT_FIRMWARE_TYPE = "stable";
static const QString DEFAULT_AUTOPILOT_HW_TYPE = "";


ApmFirmwareConfig::ApmFirmwareConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_throwPropSpinWarning(false),
    m_replugRequestMessageBox(0),
    m_px4UnplugTimer(0),
    m_px4uploader(NULL),
    m_arduinoUploader(NULL),
    m_firmwareType(DEFAULT_FIRMWARE_TYPE),
    m_autopilotType(DEFAULT_AUTOPILOT_HW_TYPE),
    m_timeoutCounter(0),
    m_hasError(0),
    m_betaFirmwareChecked(false),
    m_trunkFirmwareChecked(false),
    m_enableUpdateCheck(false),
    m_notificationOfUpdate(false),
    m_updateCheckInitiated(false),
    m_isAdvancedMode(false)
{
    ui.setupUi(this);
    ui.progressBar->setVisible(false);
    ui.cancelPushButton->setVisible(false);
    ui.rebootButton->setVisible(false);
    ui.warningLabel->setVisible(false);
    ui.warningLabelAC33->setVisible(false);
    ui.textBrowser->setVisible(false);

    loadSettings();
    //QNetworkRequest req(QUrl("https://raw.github.com/diydrones/binary/master/Firmware/firmware2.xml"));

    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    if (settings.contains("ADVANCED_MODE"))
    {
        m_isAdvancedMode = settings.value("ADVANCED_MODE").toBool();
    }
    settings.endGroup();

    m_networkManager = new QNetworkAccessManager(this);

    connect(ui.roverPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.planePushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.copterPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.hexaPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.octaQuadPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.octaPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.quadPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.triPushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));
    connect(ui.y6PushButton,SIGNAL(clicked()),this,SLOT(flashButtonClicked()));

    connect(ui.flashCustomFWButton,SIGNAL(clicked()),this,SLOT(flashCustomFirmware()));

    connect(ui.betaFirmwareButton,SIGNAL(clicked()),this,SLOT(betaFirmwareButtonClicked()));
    connect(ui.stableFirmwareButton,SIGNAL(clicked()),this,SLOT(stableFirmwareButtonClicked()));

    ui.betaFirmwareButton->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *action = new QAction(QString("Load Trunk Firmware"),ui.betaFirmwareButton);
    connect(action,SIGNAL(triggered()),this,SLOT(trunkFirmwareButtonClicked()));
    ui.betaFirmwareButton->addAction(action);

    connect(ui.cancelPushButton,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));

    ui.progressBar->setMaximum(100);
    ui.progressBar->setValue(0);

    ui.textBrowser->setVisible(ui.showOutputCheckBox->isChecked());
    connect(ui.showOutputCheckBox,SIGNAL(clicked(bool)),ui.textBrowser,SLOT(setVisible(bool)));

    connect(ui.linkComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));

    addButtonStyleSheet(ui.roverPushButton);
    addButtonStyleSheet(ui.planePushButton);
    addButtonStyleSheet(ui.copterPushButton);
    addButtonStyleSheet(ui.hexaPushButton);
    addButtonStyleSheet(ui.octaQuadPushButton);
    addButtonStyleSheet(ui.octaPushButton);
    addButtonStyleSheet(ui.quadPushButton);
    addButtonStyleSheet(ui.triPushButton);
    addButtonStyleSheet(ui.y6PushButton);

    initConnections();

    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(populateSerialPorts()));

    QTimer::singleShot(1000, this, SLOT(populateSerialPorts()));

    updateFirmwareButtons();
}

void ApmFirmwareConfig::advancedModeChanged(bool state)
{
    m_isAdvancedMode = state;
    updateFirmwareButtons();
}

void ApmFirmwareConfig::updateFirmwareButtons()
{
    ui.betaFirmwareButton->setVisible(m_isAdvancedMode);
    ui.flashCustomFWButton->setVisible(m_isAdvancedMode);
    ui.stableFirmwareButton->setVisible(m_isAdvancedMode);
}

void ApmFirmwareConfig::loadSettings()
{
    // Load defaults from settings
    QSettings settings;
    settings.sync();
    settings.beginGroup("APM_FIRMWARE_CONFIG");
    m_enableUpdateCheck = settings.value("ENABLE_UPDATE_CHECK",true).toBool();
    m_lastVersionSkipped = settings.value("VERSION_LAST_SKIPPED", "0.0.0" ).toString();
    ui.showOutputCheckBox->setChecked(settings.value("SHOW_OUTPUT", Qt::Unchecked).toBool());

    settings.endGroup();
}

void ApmFirmwareConfig::storeSettings()
{
    // Store settings
    QSettings settings;
    settings.beginGroup("APM_FIRMWARE_CONFIG");
    settings.setValue("ENABLE_UPDATE_CHECK", m_enableUpdateCheck);
    settings.setValue("VERSION_LAST_SKIPPED", m_lastVersionSkipped);
    settings.setValue("SHOW_OUTPUT", ui.showOutputCheckBox->isChecked());
    settings.endGroup();
    settings.sync();
    QLOG_DEBUG() << "Storing settings!";
}

void ApmFirmwareConfig::populateSerialPorts()
{
    QString current = ui.linkComboBox->itemText(ui.linkComboBox->currentIndex());
    disconnect(ui.linkComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
    ui.linkComboBox->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QStringList list;
        list << info.portName()
             << info.description()
             << info.manufacturer()
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : QString())
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : QString());

        if (!(info.portName().contains("Bluetooth")))
        {
            // Don't add bluetooth ports to be less confusing to the user
            // on windows, the friendly name is annoyingly identical between devices. On OSX it's different
            // We also want to only display COM ports for PX4/Pixhawk, or arduino mega 2560's.
            // We also want to show FTDI based 232/TTL devices, for APM 1.0/2.0 devices which use FTDI for usb comms.
            if (info.description().toLower().contains("px4") || info.description().toLower().contains("mega") ||
                    info.productIdentifier() == 0x0001 || info.productIdentifier() == 0x0003 ||
                    info.productIdentifier() == 0x0010 || info.productIdentifier() == 0x0011 ||
                    info.productIdentifier() == 0x0012 || info.productIdentifier() == 0x0013 ||
                    info.productIdentifier() == 0x0014)
            {
                ui.linkComboBox->insertItem(0,list[0], list);
            }
            //QLOG_DEBUG() << "Inserting " << list.first();
        }
    }
    for (int i=0;i<ui.linkComboBox->count();i++)
    {
        if (ui.linkComboBox->itemText(i) == current)
        {
            ui.linkComboBox->setCurrentIndex(i);
            setLink(ui.linkComboBox->currentIndex());
            connect(ui.linkComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
            return;
        }
    }
    connect(ui.linkComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));
    if (current == "")
    {
        setLink(ui.linkComboBox->currentIndex());
    }
    if (ui.linkComboBox->count() == 0)
    {
        //no ports found
        ui.linkComboBox->setEnabled(false);
        ui.comPortNameLabel->setText("No valid device found. \nCheck to be sure your APM2.5+ \n or Pixhawk/PX4 device is plugged in, and \ndrivers are installed.");
    }
    else
    {
        ui.linkComboBox->setEnabled(true);
    }
}

void ApmFirmwareConfig::showEvent(QShowEvent *)
{
    // Start Port scanning
    m_timer->start(2000);
    if(ui.stackedWidget->currentIndex() == 0)
    {
        MainWindow::instance()->toolBar().disableConnectWidget(true);
    }
    QSettings settings;
    if (settings.contains("ADVANCED_MODE"))
    {
        m_isAdvancedMode = settings.value("ADVANCED_MODE").toBool();
    }
    updateFirmwareButtons();
}

void ApmFirmwareConfig::hideEvent(QHideEvent *)
{
    // Stop Port scanning
    m_timer->stop();
    if(ui.stackedWidget->currentIndex() == 0)
    {
        MainWindow::instance()->toolBar().disableConnectWidget(false);
    }
    //MainWindow::instance()->toolBar().disableConnectWidget(false);
}

void ApmFirmwareConfig::uasConnected()
{
   // MainWindow::instance()->toolBar().disableConnectWidget(false);
    ui.stackedWidget->setCurrentIndex(1);
}
void ApmFirmwareConfig::cancelButtonClicked()
{
    if(!m_arduinoUploader && !m_px4uploader)
    {
        return;
    }
    if (QMessageBox::question(this,tr("Warning"),tr("You are about to cancel the firmware upload process. ONLY do this if the process has not started properly. Are you sure you want to continue?"),QMessageBox::Yes,QMessageBox::No) != QMessageBox::Yes)
    {
        return;
    }
    cleanUp();
    ui.statusLabel->setText("Flashing Canceled");
}

void ApmFirmwareConfig::cleanUp()
{
    // stop and cleanup an operation
    if (m_px4uploader)
    {
        QLOG_DEBUG() << "PX4 Flashing Cleanup";
        px4Cleanup();
        return;
    }
    else if (m_arduinoUploader)
    {
        m_arduinoUploader->abortLoading();
        m_arduinoUploader->wait(500);
        m_arduinoUploader->deleteLater();
        m_arduinoUploader = NULL;
    }
}

void ApmFirmwareConfig::px4StatusUpdate(QString update)
{
    ui.statusLabel->setText(update);
    ui.textBrowser->append(update);
}
void ApmFirmwareConfig::px4DebugUpdate(QString update)
{
    ui.textBrowser->append(update);
}
void ApmFirmwareConfig::arduinoStatusUpdate(QString update)
{
    ui.statusLabel->setText(update);
    ui.textBrowser->append(update);
}

void ApmFirmwareConfig::arduinoDebugUpdate(QString update)
{
    ui.textBrowser->append(update);
}

void ApmFirmwareConfig::uasDisconnected()
{
    ui.stackedWidget->setCurrentIndex(0);
}

void ApmFirmwareConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
        m_uas = 0;
    }
    if (!uas)
    {
        //ui.stackedWidget->setCurrentIndex(0);
    }
    else
    {
        m_uas = uas;
        connect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));
        connect(uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        connect(uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
        uasConnected();

        ArduPilotMegaMAV* apm = dynamic_cast<ArduPilotMegaMAV*>(uas);
        if (apm)
            connect(apm,SIGNAL(versionDetected(QString)), this, SLOT(checkForUpdates(QString)));

    }
}

void ApmFirmwareConfig::hideBetaLabels()
{
    for (int i=0;i<m_betaButtonLabelList.size();i++)
    {
        m_betaButtonLabelList[i]->hide();
    }
    ui.warningLabel->hide();
    ui.warningLabelAC33->hide();
}

void ApmFirmwareConfig::showBetaLabels()
{
    for (int i=0;i<m_betaButtonLabelList.size();i++)
    {
        m_betaButtonLabelList[i]->show();
    }
    ui.warningLabel->show();
}

void ApmFirmwareConfig::addBetaLabel(QWidget *parent)
{
    QLabel *label = new QLabel(parent);
    QVBoxLayout *layout = new QVBoxLayout();
    parent->setLayout(layout);
    label->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    label->setText("<h1><font color=#FFAA00>BETA</font></h1>");
    layout->addWidget(label);
    m_betaButtonLabelList.append(label);
}

void ApmFirmwareConfig::addButtonStyleSheet(QWidget *parent)
{
    parent->setStyleSheet("QPushButton{\
                          background-color: rgb(57, 57, 57);\
                          }\
                          QPushButton:pressed{\
                          background-color: rgb(100, 100, 100);\
                          }");
}

void ApmFirmwareConfig::requestFirmwares(QString type,QString autopilot, bool notification = false)
{
    //type can be "stable" "beta" or "latest"
    //autopilot can be "apm" "px4" or "pixhawk"
    QLOG_DEBUG() << "Requesting firmware:" << type << autopilot;
    m_betaFirmwareChecked = false;
    m_trunkFirmwareChecked = false;
    m_notificationOfUpdate = notification;
    hideBetaLabels();
    QString prestring = "apm2";
    if (autopilot == "apm")
    {
        prestring = "apm2";
    }
    else if (autopilot == "px4")
    {
        prestring = "PX4";
    }
    else if (autopilot == "pixhawk")
    {
        prestring = "PX4";
    }
    if (type != "stable")
    {
        ui.warningLabel->show();
    }
    else
    {
        ui.warningLabel->hide();
    }

    ui.warningLabelAC33->hide();



    m_autopilotType = autopilot;
    m_firmwareType = type;
    QNetworkReply *reply1 = NULL;
    QNetworkReply *reply2 = NULL;
    QNetworkReply *reply3 = NULL;
    QNetworkReply *reply4 = NULL;
    QNetworkReply *reply5 = NULL;
    QNetworkReply *reply6 = NULL;
    QNetworkReply *reply7 = NULL;
    QNetworkReply *reply8 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Plane/" + type + "/" + prestring + "/git-version.txt")));
    QNetworkReply *reply9 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Rover/" + type + "/" + prestring + "/git-version.txt")));

    if (autopilot == "apm")
    {
        m_buttonToUrlMap[ui.roverPushButton] = "http://firmware.diydrones.com/Rover/" + type + "/" + prestring + "/APMrover2.hex";
        m_buttonToUrlMap[ui.planePushButton] = "http://firmware.diydrones.com/Plane/" + type + "/" + prestring + "/ArduPlane.hex";

        if (type == "latest")
        {
            ui.warningLabelAC33->show();
            /*
             * AC3.3 only supports Pixhawk, APM1/APM2 is discontinued.
             * Last known 'latest': http://firmware.diydrones.com/Copter/2015-03/2015-03-13-00:03/
             * stable and beta both still support, as they are not 3.3 yet
             */
            QString prepath = "http://firmware.diydrones.com/Copter/2015-03/2015-03-13-00:03/" + prestring;
            m_buttonToUrlMap[ui.copterPushButton] = prepath + "-heli/ArduCopter.hex";
            m_buttonToUrlMap[ui.hexaPushButton] = prepath + "-hexa/ArduCopter.hex";
            m_buttonToUrlMap[ui.octaQuadPushButton] = prepath + "-octa-quad/ArduCopter.hex";
            m_buttonToUrlMap[ui.octaPushButton] = prepath + "-octa/ArduCopter.hex";
            m_buttonToUrlMap[ui.quadPushButton] = prepath + "-quad/ArduCopter.hex";
            m_buttonToUrlMap[ui.triPushButton] = prepath + "-tri/ArduCopter.hex";
            m_buttonToUrlMap[ui.y6PushButton] = prepath + "-y6/ArduCopter.hex";

            reply1 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-heli/git-version.txt")));
            reply2 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-quad/git-version.txt")));
            reply3 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-hexa/git-version.txt")));
            reply4 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-octa/git-version.txt")));
            reply5 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-octa-quad/git-version.txt")));
            reply6 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-tri/git-version.txt")));
            reply7 = m_networkManager->get(QNetworkRequest(QUrl(prepath + "-y6/git-version.txt")));


        }
        else
        {
            //TODO: Need to add beta and stable as they push APM1/APM2 support off

            m_buttonToUrlMap[ui.copterPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-heli/ArduCopter.hex";
            m_buttonToUrlMap[ui.hexaPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-hexa/ArduCopter.hex";
            m_buttonToUrlMap[ui.octaQuadPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa-quad/ArduCopter.hex";
            m_buttonToUrlMap[ui.octaPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa/ArduCopter.hex";
            m_buttonToUrlMap[ui.quadPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-quad/ArduCopter.hex";
            m_buttonToUrlMap[ui.triPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-tri/ArduCopter.hex";
            m_buttonToUrlMap[ui.y6PushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-y6/ArduCopter.hex";

            reply1 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-heli/git-version.txt")));
            reply2 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-quad/git-version.txt")));
            reply3 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-hexa/git-version.txt")));
            reply4 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa/git-version.txt")));
            reply5 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa-quad/git-version.txt")));
            reply6 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-tri/git-version.txt")));
            reply7 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-y6/git-version.txt")));


        }
    }
    else if (autopilot == "px4")
    {
        m_buttonToUrlMap[ui.roverPushButton] = "http://firmware.diydrones.com/Rover/" + type + "/" + prestring + "/APMrover2-v1.px4";
        m_buttonToUrlMap[ui.planePushButton] = "http://firmware.diydrones.com/Plane/" + type + "/" + prestring + "/ArduPlane-v1.px4";
        m_buttonToUrlMap[ui.copterPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-heli/ArduCopter-v1.px4";
        m_buttonToUrlMap[ui.hexaPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-hexa/ArduCopter-v1.px4";
        m_buttonToUrlMap[ui.octaQuadPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa-quad/ArduCopter-v1.px4";
        m_buttonToUrlMap[ui.octaPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa/ArduCopter-v1.px4";
        m_buttonToUrlMap[ui.quadPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-quad/ArduCopter-v1.px4";
        m_buttonToUrlMap[ui.triPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-tri/ArduCopter-v1.px4";
        m_buttonToUrlMap[ui.y6PushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-y6/ArduCopter-v1.px4";

        reply1 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-heli/git-version.txt")));
        reply2 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-quad/git-version.txt")));
        reply3 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-hexa/git-version.txt")));
        reply4 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa/git-version.txt")));
        reply5 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa-quad/git-version.txt")));
        reply6 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-tri/git-version.txt")));
        reply7 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-y6/git-version.txt")));


    }
    else if (autopilot == "pixhawk")
    {
        m_buttonToUrlMap[ui.roverPushButton] = "http://firmware.diydrones.com/Rover/" + type + "/" + prestring + "/APMrover2-v2.px4";
        m_buttonToUrlMap[ui.planePushButton] = "http://firmware.diydrones.com/Plane/" + type + "/" + prestring + "/ArduPlane-v2.px4";
        m_buttonToUrlMap[ui.copterPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-heli/ArduCopter-v2.px4";
        m_buttonToUrlMap[ui.hexaPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-hexa/ArduCopter-v2.px4";
        m_buttonToUrlMap[ui.octaQuadPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa-quad/ArduCopter-v2.px4";
        m_buttonToUrlMap[ui.octaPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa/ArduCopter-v2.px4";
        m_buttonToUrlMap[ui.quadPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-quad/ArduCopter-v2.px4";
        m_buttonToUrlMap[ui.triPushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-tri/ArduCopter-v2.px4";
        m_buttonToUrlMap[ui.y6PushButton] = "http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-y6/ArduCopter-v2.px4";

        reply1 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-heli/git-version.txt")));
        reply2 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-quad/git-version.txt")));
        reply3 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-hexa/git-version.txt")));
        reply4 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa/git-version.txt")));
        reply5 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-octa-quad/git-version.txt")));
        reply6 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-tri/git-version.txt")));
        reply7 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/" + type + "/" + prestring + "-y6/git-version.txt")));


    }
    else
    {
        QLOG_ERROR() << "Unknown autopilot in ApmFirmwareConfig::requestFirmwares()" << autopilot;
        ui.statusLabel->setText("Unable to request firmware versions, autopilot version unrecognized: " + autopilot);
        return;
    }

    //http://firmware.diydrones.com/Plane/stable/apm2/ArduPlane.hex
    connect(reply1,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply1,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply2,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply2,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply3,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply3,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply4,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply4,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply5,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply5,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply6,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply6,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply7,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply7,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply8,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply8,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
    connect(reply9,SIGNAL(finished()),this,SLOT(firmwareListFinished()));
    connect(reply9,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
}

void ApmFirmwareConfig::betaFirmwareButtonClicked()
{
    QLOG_DEBUG() << "Beta FW Button clicked";

    QMessageBox::information(this,tr("Warning"),tr("These are beta firmware downloads. Use at your own risk!!!"));
    ui.label->setText(tr("<h2>Beta Firmware</h2>"));
    //equestBetaFirmwares();
    showBetaLabels();
    requestFirmwares("beta",m_autopilotType);
}

void ApmFirmwareConfig::stableFirmwareButtonClicked()
{
    ui.label->setText(tr("<h2>Firmware</h2>"));
    requestFirmwares("stable",m_autopilotType);
}

void ApmFirmwareConfig::trunkFirmwareButtonClicked()
{
    QMessageBox::information(this,tr("Warning"),tr("These are trunk firmware downloads. These should ONLY BE USED if you know what you're doing!!!"));
    ui.label->setText(tr("<h2>Trunk Firmware</h2>"));
    ui.betaFirmwareButton->setChecked(true);
    requestFirmwares("latest",m_autopilotType);
}


void ApmFirmwareConfig::px4Warning(QString message)
{
    QMessageBox::information(this,tr("Warning"),tr("Warning: ") + message,"Continue");
    ui.statusLabel->setText(tr("Error during upload"));
}
void ApmFirmwareConfig::px4Error(QString error)
{
    QMessageBox::information(0,tr("Error"),tr("Error during upload:") + error);
    ui.statusLabel->setText(tr("Error during upload"));
}
void ApmFirmwareConfig::px4Cleanup()
{
    QLOG_DEBUG() << "px4cleanup resources";
    if (m_px4uploader){
        m_px4uploader->stop();
        m_px4uploader->wait(250);
        m_px4uploader->deleteLater();
        m_px4uploader = NULL;
    }

    if (m_px4UnplugTimer){
        m_px4UnplugTimer->stop();
        m_px4UnplugTimer->deleteLater();
        m_px4UnplugTimer = 0;
    }

    if (m_replugRequestMessageBox) {
        m_replugRequestMessageBox->hide();
        m_replugRequestMessageBox->deleteLater();
        m_replugRequestMessageBox = 0;
    }
}

void ApmFirmwareConfig::px4Finished()
{
    ui.progressBar->setValue(100);
    if (!m_hasError)
    {
        ui.statusLabel->setText(tr("Flashing complete"));
        QMessageBox::information(this,"Complete","PX4 Flashing is complete!");
    } else {
        ui.statusLabel->setText(tr("Flashing FAILED!"));
        QMessageBox::critical(this,"FAILED","PX4 Flashing failed!");
    }

    cleanUp();

    emit showBlankingScreen();
    ui.progressBar->setVisible(false);
    ui.cancelPushButton->setVisible(false);
}

void ApmFirmwareConfig::downloadFinished()
{
    QLOG_DEBUG() << "Download finished, flashing firmware";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        return;
    }
    if (reply->error() != QNetworkReply::NoError)
    {
        //Something went wrong when downloading the firmware.
        QMessageBox::information(this,tr("Error downloading firmware"),tr("There was an error while downloading the firmware.\nError number: ") + QString::number(reply->error()) + "\nError text: " + reply->errorString());
        ui.textBrowser->append("Error downloading firmware.");
        ui.textBrowser->append("Error Number: " + QString::number(reply->error()));
        ui.textBrowser->append("Error Text: " + reply->errorString());
        return;
    }
    QByteArray hex = reply->readAll();
    m_tempFirmwareFile = new QTemporaryFile();
    m_tempFirmwareFile->open();
    m_tempFirmwareFile->write(hex);
    m_tempFirmwareFile->flush();
    m_tempFirmwareFile->close();

    QLOG_DEBUG() << "Temp file to flash is: " << m_tempFirmwareFile->fileName();
    ui.textBrowser->append("Finished downloading " + m_tempFirmwareFile->fileName());
    flashFirmware(m_tempFirmwareFile->fileName());
}


 void ApmFirmwareConfig::flashFirmware(QString filename)
 {
    ui.cancelPushButton->setVisible(true);
    ui.progressBar->setVisible(true);
    QList<QSerialPortInfo> portList =  QSerialPortInfo::availablePorts();

    foreach (const QSerialPortInfo &info, portList)
    {
        QLOG_DEBUG() << "PortName    : " << info.portName()
               << "Description : " << info.description();
        QLOG_DEBUG() << "Manufacturer: " << info.manufacturer();
    }

    if (m_autopilotType == "apm")
    {
        QLOG_DEBUG() << "Attempting to Open port";

        m_arduinoUploader = new ArduinoFlash();
        connect(m_arduinoUploader,SIGNAL(flashProgress(qint64,qint64)),this,SLOT(arduinoFlashProgress(qint64,qint64)));
        connect(m_arduinoUploader,SIGNAL(verifyProgress(qint64,qint64)),this,SLOT(arduinoVerifyProgress(qint64,qint64)));
        connect(m_arduinoUploader,SIGNAL(firmwareUploadStarted()),this,SLOT(arduinoUploadStarted()));
        connect(m_arduinoUploader,SIGNAL(firmwareUploadError(QString)),this,SLOT(arduinoError(QString)));
        connect(m_arduinoUploader,SIGNAL(statusUpdate(QString)),this,SLOT(arduinoStatusUpdate(QString)));
        connect(m_arduinoUploader,SIGNAL(debugUpdate(QString)),this,SLOT(arduinoDebugUpdate(QString)));
        connect(m_arduinoUploader,SIGNAL(firmwareUploadComplete()),this,SLOT(arduinoUploadComplete()));

        m_arduinoUploader->loadFirmware(m_settings.name,filename);

    }
    else if (m_autopilotType == "pixhawk" || m_autopilotType == "px4")
    {
        if (m_px4uploader)
        {
            QLOG_FATAL() << "Tried to load PX4 Firmware when it was already started!";
            return;
        }
        m_px4uploader = new PX4FirmwareUploader();
        connect(m_px4uploader,SIGNAL(statusUpdate(QString)),this,SLOT(px4StatusUpdate(QString)));
        connect(m_px4uploader,SIGNAL(debugUpdate(QString)),this,SLOT(px4DebugUpdate(QString)));
        connect(m_px4uploader,SIGNAL(finished()),this,SLOT(px4Cleanup()));
        connect(m_px4uploader,SIGNAL(flashProgress(qint64,qint64)),this,SLOT(firmwareDownloadProgress(qint64,qint64)));
        connect(m_px4uploader,SIGNAL(error(QString)),this,SLOT(px4Error(QString)));
        connect(m_px4uploader,SIGNAL(warning(QString)),this,SLOT(px4Warning(QString)));
        connect(m_px4uploader,SIGNAL(complete()),this,SLOT(px4Finished()));
        connect(m_px4uploader,SIGNAL(requestDevicePlug()),this,SLOT(requestDeviceReplug()));
        connect(m_px4uploader,SIGNAL(devicePlugDetected()),this,SLOT(devicePlugDetected()));
        m_px4uploader->loadFile(filename);
        m_timeoutCounter=0;
        m_hasError=false;
        ui.progressBar->setValue(0);
    }
}
void ApmFirmwareConfig::requestDeviceReplug()
{
    if (m_replugRequestMessageBox)
    {
        m_replugRequestMessageBox->hide();
        delete m_replugRequestMessageBox;
        m_replugRequestMessageBox = 0;
    }
    m_replugRequestMessageBox = new QProgressDialog("Please unplug, and plug back in the PX4/Pixhawk","Cancel",0,30,this);
    QProgressBar *bar = new QProgressBar(m_replugRequestMessageBox);
    m_replugRequestMessageBox->setBar(bar);
    bar->hide();
    connect(m_replugRequestMessageBox,SIGNAL(canceled()),this,SLOT(cancelButtonClicked()));
    m_replugRequestMessageBox->show();
    m_px4UnplugTimer = new QTimer(this);
    //connect(m_px4UnplugTimer,SIGNAL(timeout()),this,SLOT(px4UnplugTimerTick()));
    //m_px4UnplugTimer->start(1000);
    //QMessageBox::information(this,"Warning","Please click ok, then unplug, and plug back in the PX4/Pixhawk");
}
void ApmFirmwareConfig::px4UnplugTimerTick()
{
    m_replugRequestMessageBox->setValue(m_replugRequestMessageBox->value()+1);
    if (m_replugRequestMessageBox->value() >= 30)
    {
        m_px4UnplugTimer->stop();
        m_px4UnplugTimer->deleteLater();
        m_px4UnplugTimer = 0;
        m_replugRequestMessageBox->hide();
        m_replugRequestMessageBox->deleteLater();
        m_replugRequestMessageBox = 0;
        cancelButtonClicked();

    }
}

void ApmFirmwareConfig::devicePlugDetected()
{
    if (m_px4UnplugTimer)
    {
        m_px4UnplugTimer->stop();
        m_px4UnplugTimer->deleteLater();
        m_px4UnplugTimer = 0;
    }
    if (m_replugRequestMessageBox)
    {
        m_replugRequestMessageBox->hide();
        delete m_replugRequestMessageBox;
        m_replugRequestMessageBox = 0;
    }
}

void ApmFirmwareConfig::firmwareDownloadProgress(qint64 received,qint64 total)
{
    ui.progressBar->setValue( 100.0 * ((double)received/(double)total));
}

void ApmFirmwareConfig::flashButtonClicked()
{
    QLOG_DEBUG() << "flashButtonClicked";
    QPushButton *senderbtn = qobject_cast<QPushButton*>(sender());
    if (m_buttonToUrlMap.contains(senderbtn))
    {
        QStringList pathsplit = m_buttonToUrlMap[senderbtn].split("/");
        QString confirmmsg = "";
        if (pathsplit.size() > 2)
        {
            confirmmsg = "You are about to install " + pathsplit[pathsplit.size()-1] + " for " + pathsplit[pathsplit.size()-2];
        }
        else
        {
            confirmmsg = "You are about to install " + m_buttonToUrlMap[senderbtn].mid(m_buttonToUrlMap[senderbtn].lastIndexOf("/")+1);
        }
        if (QMessageBox::question(0,"Confirm",confirmmsg,QMessageBox::Ok,QMessageBox::Abort) == QMessageBox::Abort)
        {
            //aborted.
            return;
        }

        QLOG_DEBUG() << "Go download:" << m_buttonToUrlMap[senderbtn];
        QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(m_buttonToUrlMap[senderbtn])));
        //http://firmware.diydrones.com/Plane/stable/apm2/ArduPlane.hex
        ui.textBrowser->append("Started downloading " + m_buttonToUrlMap[senderbtn]);
        connect(reply,SIGNAL(finished()),this,SLOT(downloadFinished()));

        connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
        connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(firmwareDownloadProgress(qint64,qint64)));
        ui.statusLabel->setText("Downloading");
        ui.progressBar->setVisible(true);
        ui.progressBar->setMaximum(100);
        if (m_buttonToWarning.contains(senderbtn))
        {
            if (m_buttonToWarning[senderbtn])
            {
                m_throwPropSpinWarning = true;

            }
        }
    }
    else
    {
        QLOG_DEBUG() << "Flash button clicked without any HTTP links!";
        QLOG_DEBUG() << "Autopilot:" << m_autopilotType;
        QLOG_DEBUG() << "Firmware:" << m_firmwareType;
        QLOG_DEBUG() << "Set COM port:" << m_settings.name;
    }
}
void ApmFirmwareConfig::setLink(int index)
{
    if (ui.linkComboBox->itemData(index).toStringList().size() > 0)
    {
        bool blank = false;
        if (m_settings.name == "")
        {
            blank = true;
        }
        m_settings.name = ui.linkComboBox->itemData(index).toStringList()[0];
#ifdef Q_OS_WIN
        ui.comPortNameLabel->setText(ui.linkComboBox->itemData(index).toStringList()[1] + "\n" + ui.linkComboBox->itemData(index).toStringList()[2]);
#else
        ui.comPortNameLabel->setText(ui.linkComboBox->itemData(index).toStringList()[1] + "\n" + ui.linkComboBox->itemData(index).toStringList()[2]);
#endif
        foreach(QSerialPortInfo info,QSerialPortInfo::availablePorts())
        {
            if (info.portName() == m_settings.name)
            {
                QString platform = processPortInfo(info);
                if (platform != "Unknown" && (m_autopilotType != platform || blank)){
                    requestFirmwares(m_firmwareType, platform);
                    QLOG_DEBUG() << platform << " Detected";
                }
            }
        }
    }
}

QString ApmFirmwareConfig::processPortInfo(const QSerialPortInfo &info)
{
    // Include FTDI based 232/TTL devices, for APM 1.0/2.0 devices which use FTDI for usb comms.
    if ((info.description().toLower().contains("mega") && info.description().contains("2560")) \
            || info.productIdentifier() == 0x6001 || info.productIdentifier() == 0x6010 || info.productIdentifier() == 0x6014 )
    {
        //APM
        return "apm";
    }
    else if (info.description().toLower().contains("px4"))
    {
        //PX4
        if (info.productIdentifier() == 0x0010) //Both PX4 and PX4 bootloader are 0x0010.
        {
            return "px4";
        }
        else if (info.productIdentifier() == 0x0011 || info.productIdentifier() == 0x0001
                 || info.productIdentifier() == 0x0016) //0x0011 is the Pixhawk, 0x0001 is the bootloader.
        {
            return "pixhawk";
        }
        else
        {
            return "Unknown";
        }
    }
    else
    {
        //Unknown
        QLOG_DEBUG() << "Unknown detected:" << info.productIdentifier() << info.description();
        return "Unknown";
    }
}

void ApmFirmwareConfig::firmwareListError(QNetworkReply::NetworkError error)
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QLOG_ERROR() << "Error!" << reply->errorString();
}

bool ApmFirmwareConfig::stripVersionFromGitReply(QString url, QString reply,QString type,QString stable,QString *out)
{
    if (url.contains(type) && url.contains("git-version.txt") && url.contains(stable))
    {
        QString version = reply.mid(reply.indexOf("APMVERSION:")+12).replace("\n","").replace("\r","").trimmed();
        *out = version;
        return true;
    }
    return false;
}

void ApmFirmwareConfig::firmwareListFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    QString replystr = reply->readAll();
    QString outstr = "";

    if (reply->error() != QNetworkReply::NoError)
    {
        QLOG_DEBUG() << "firmwareListFinished error: " << reply->error() << reply->errorString();
    }
    QString cmpstr = "";
    QString labelstr = "";
    QString apmver = "";
    if (m_autopilotType == "apm")
    {
        apmver = "apm2";
    }
    else if (m_autopilotType == "px4" || m_autopilotType == "pixhawk")
    {
        apmver = "PX4";
    }
    if (m_firmwareType == "beta")
    {
        cmpstr = "beta";
        labelstr = "BETA\n";
    }
    else if (m_firmwareType == "latest")
    {
        cmpstr = "latest";
        labelstr = "TRUNK\n";
    }
    else
    {
        cmpstr = "stable";
        labelstr = "";
    }

    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-heli",cmpstr,&outstr))
    {
        //Version checking
        m_buttonToWarning[ui.copterPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.copterLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-quad",cmpstr,&outstr))
    {
        //Update version checkin
        compareVersionsForNotification("ArduCopter", outstr); // We only check one of the copter frame types
        m_buttonToWarning[ui.quadPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.quadLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-hexa",cmpstr,&outstr))
    {
        //Version checking
        m_buttonToWarning[ui.hexaPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.hexaLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-octa-quad",cmpstr,&outstr))
    {
        //Version checking
        m_buttonToWarning[ui.octaQuadPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.octaQuadLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-octa",cmpstr,&outstr))
    {
        //Version checking
        m_buttonToWarning[ui.octaPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.octaLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-tri",cmpstr,&outstr))
    {
        //Version checking
        m_buttonToWarning[ui.triPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.triLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,apmver + "-y6",cmpstr,&outstr))
    {
        //Version checking
        m_buttonToWarning[ui.y6PushButton] = versionIsGreaterThan(outstr,3.1);
        ui.y6Label->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"Plane",cmpstr,&outstr))
    {
        //Update version checkin
        compareVersionsForNotification("ArduPlane", outstr);
        m_buttonToWarning[ui.planePushButton] = versionIsGreaterThan(outstr,3.1);
        ui.planeLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"Rover",cmpstr,&outstr))
    {
        //Update version checkin
        compareVersionsForNotification("ArduRover", outstr);
        m_buttonToWarning[ui.roverPushButton] = versionIsGreaterThan(outstr,3.1);
        ui.roverLabel->setText(labelstr + outstr);
        return;
    }

    //QLOG_DEBUG() << "Match not found for:" << reply->url();
    //QLOG_DEBUG() << "Git version line:" <<  replystr;
}
//Takes the format: "AnythingHere VX.Y.Z, where .Z is optional, and X and Y can be any number of digits.
bool ApmFirmwareConfig::versionIsGreaterThan(QString verstr,double version)
{
    QRegExp versionEx("\\d*\\.\\d+");
    QString versionstr = "";
    int pos = versionEx.indexIn(verstr);
    if (pos > -1) {
        versionstr = versionEx.cap(0);
    }
    float versionfloat = versionstr.toFloat();
    return ((versionfloat+0.005) > (version));
}

bool ApmFirmwareConfig::compareVersionStrings(const QString& newVersion, const QString& currentVersion)
{
    int newMajor,newMinor,newBuild = 0;
    int currentMajor, currentMinor,currentBuild = 0;

    QString newBuildSubMoniker, oldBuildSubMoniker; // holds if the build is a rc or dev build

    QRegExp versionEx("(\\d*\\.\\d+\\.?\\d?)-?(rc\\d)?");
    QString versionstr = "";
    int pos = versionEx.indexIn(newVersion);
    if (pos > -1) {
        // Split first sub-element to get numercal major.minor.build version
        QLOG_DEBUG() << "Detected newVersion:" << versionEx.capturedTexts()<< " count:"
                     << versionEx.captureCount();
        versionstr = versionEx.cap(1);
        QStringList versionList = versionstr.split(".");
        newMajor = versionList[0].toInt();
        newMinor = versionList[1].toInt();
        if (versionList.size() > 2){
            newBuild = versionList[2].toInt();
        }
        // second subelement is either rcX candidate or developement build
        if (versionEx.captureCount() == 2)
            newBuildSubMoniker = versionEx.cap(2);
    }

    QRegExp versionEx2("(\\d*\\.\\d+\\.?\\d?)-?(rc\\d)?");
    versionstr = "";
    pos = versionEx2.indexIn(currentVersion);
    if (pos > -1) {
        QLOG_DEBUG() << "Detected currentVersion:" << versionEx2.capturedTexts() << " count:"
                     << versionEx2.captureCount();
        versionstr = versionEx2.cap(1);
        QStringList versionList = versionstr.split(".");
        currentMajor = versionList[0].toInt();
         currentMinor = versionList[1].toInt();
        if (versionList.size() > 2){
            currentBuild = versionList[2].toInt();
        }
        // second subelement is either rcX candidate or developement build
        if (versionEx2.captureCount() == 2)
            oldBuildSubMoniker = versionEx2.cap(2);
    }

    QLOG_DEBUG() << "Verison Compare:" <<QString().sprintf(" New Version %d.%d.%d > Old Version %d.%d.%d",
                                                 newMajor,newMinor,newBuild,currentMajor, currentMinor,currentBuild);
    if (newMajor>currentMajor){
        // A Major release
        return true;
    } else if (newMajor == currentMajor){
        if (newMinor >  currentMinor){
            // A minor release
            return true;
        } else if (newMinor ==  currentMinor){
            if (newBuild > currentBuild)
                // new build (or tiny release)
                return true;
            else if (newBuild == currentBuild) {
                // Check if RC is newer
                // If the version isn't newer, it might be a new release candidate
                int newRc = 0, oldRc = 0;

                if (newBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)
                        && oldBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)) {
                    QRegExp releaseNumber("\\d+");
                    pos = releaseNumber.indexIn(newBuildSubMoniker);
                    if (pos > -1) {
                        QLOG_DEBUG() << "Detected newRc:" << versionEx.capturedTexts();
                        newRc = releaseNumber.cap(0).toInt();
                    }

                    QRegExp releaseNumber2("\\d+");
                    pos = releaseNumber2.indexIn(oldBuildSubMoniker);
                    if (pos > -1) {
                        QLOG_DEBUG() << "Detected oldRc:" << versionEx.capturedTexts();
                        oldRc = releaseNumber.cap(0).toInt();
                    }

                    if (newRc > oldRc)
                        return true;
                }

                if (newBuildSubMoniker.length() == 0
                        && oldBuildSubMoniker.startsWith("RC", Qt::CaseInsensitive)) {
                    QLOG_DEBUG() << "Stable build newer that last unstable release candidate ";
                    return true; // this means a new stable build of the unstable rc is available
                }
            }
        }
    }



    return false;
}

void ApmFirmwareConfig::flashCustomFirmware()
{
    // Show File SelectionDialog

    QString filename = QFileDialog::getOpenFileName(this, tr("Open File"), QGC::appDataDirectory(),
                                                     tr("bin (*.hex *.px4)"));
    QApplication::processEvents(); // Helps clear dialog from screen

    if (filename.length() > 0){
        QLOG_DEBUG() << "Selected File to flash: " << filename;
        ui.progressBar->setEnabled(true);
        ui.progressBar->setTextVisible(false);
        ui.statusLabel->setText("Flashing");
        flashFirmware(filename);

    } else {
        QLOG_DEBUG() << "custom firmware flash cancelled: ";
        return;
    }

}

void ApmFirmwareConfig::checkForUpdates(const QString &versionString)
{
    if (m_enableUpdateCheck){
        QLOG_DEBUG() << "APM Version Check For Updates";

        // Wait for signal about the INS_PRODUCT_ID then tigger FW version fetch.
        m_currentVersionString = versionString;
        m_updateCheckInitiated = true;
    }
}

void ApmFirmwareConfig::compareVersionsForNotification(const QString &apmPlatform, const QString& newFwVersion)
{
    if (m_currentVersionString.contains(apmPlatform)) {
        QStringList list = m_currentVersionString.split(" ");
        if (compareVersionStrings(newFwVersion, list[1])){
            // Show Update fwVersion
            if(m_lastVersionSkipped.contains(newFwVersion)){
                    QLOG_DEBUG() << " Version Skipped: " << newFwVersion;
                    return; // need to skip this one
            }
            QLOG_INFO() << "Update Avaiable for " << apmPlatform << " Ver: " << newFwVersion;
            if (QMessageBox::Ignore == QMessageBox::information(this, tr("Update Status"),
                                     tr("New %1 firmware %2 is available for %3")
                                     .arg(m_firmwareType).arg(newFwVersion).arg(apmPlatform)
                                     ,QMessageBox::Ignore, QMessageBox::Ok)){
                    m_lastVersionSkipped = newFwVersion;
                    storeSettings();
            }
        }
    }
}


void ApmFirmwareConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if(m_updateCheckInitiated){
        if (parameterName.contains("INS_PRODUCT_ID")){
            m_updateCheckInitiated = false;
            // determine board type for firmware update
            switch(value.toInt()){
            // @Values: 0:Unknown,1:APM1-1280,2:APM1-2560,88:APM2,3:SITL,4:PX4v1,5:PX4v2,Flymaple:256,Linux:257

            case 88:{ //APM2
                requestFirmwares(m_firmwareType,"apm",true);
            }break;
            case 4:{ //PX4v1
                requestFirmwares(m_firmwareType,"px4",true);
            }break;
            case 5:{ //PX4v2
                requestFirmwares(m_firmwareType,"pixhawk",true);
            }break;

            default:
                ;// do nothing.
            }
        }
    }
}

ApmFirmwareConfig::~ApmFirmwareConfig()
{
    storeSettings();
}

void ApmFirmwareConfig::arduinoUploadStarted()
{
    ui.progressBar->setValue(0);
    ui.progressBar->setMaximum(200);
}

void ApmFirmwareConfig::arduinoError(QString error)
{
    ui.statusLabel->setText(error);
    ui.textBrowser->append(error);
    QMessageBox::information(0,"Error",error);
    cleanUp();
    if (m_tempFirmwareFile) m_tempFirmwareFile->deleteLater(); //This will remove the temporary file.
    m_tempFirmwareFile = NULL;
    ui.progressBar->setVisible(false);
    ui.cancelPushButton->setVisible(false);
}

void ApmFirmwareConfig::arduinoFlashProgress(qint64 pos,qint64 total)
{
    ui.progressBar->setValue(((double)pos / (double)total) * 100.0);
}

void ApmFirmwareConfig::arduinoVerifyProgress(qint64 pos,qint64 total)
{
    ui.progressBar->setValue((((double)pos / (double)total) * 100.0) + 100);
}

void ApmFirmwareConfig::arduinoVerifyComplete()
{

}

void ApmFirmwareConfig::arduinoVerifyFailed()
{

}

void ApmFirmwareConfig::arduinoFlashComplete()
{

}

void ApmFirmwareConfig::arduinoFlashFailed()
{

}

void ApmFirmwareConfig::arduinoUploadComplete()
{
    cleanUp();
    //Ensure we're reading 100%
    ui.progressBar->setMaximum(100);
    ui.progressBar->setValue(100);
    QMessageBox::information(this,"Complete","APM Flashing is complete!");
    ui.statusLabel->setText(tr("Flashing complete"));
    emit showBlankingScreen();
    if (m_throwPropSpinWarning)
    {
        QMessageBox::information(parentWidget(),"Warning","As of AC 3.1, motors will spin when armed. This is configurable through the MOT_SPIN_ARMED parameter");
        m_throwPropSpinWarning = false;
    }
    //QLOG_DEBUG() << "Upload finished!" << QString::number(status);
    if (m_tempFirmwareFile) m_tempFirmwareFile->deleteLater(); //This will remove the temporary file.
    m_tempFirmwareFile = NULL;
    ui.progressBar->setVisible(false);
    ui.cancelPushButton->setVisible(false);
}
