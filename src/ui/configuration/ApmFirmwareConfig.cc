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

#include "ApmFirmwareConfig.h"
#include "QsLog.h"
#include "LinkManager.h"
#include "LinkInterface.h"
#include "qserialport.h"
#include "qserialportinfo.h"
#include "SerialLink.h"

#include <QTimer>

ApmFirmwareConfig::ApmFirmwareConfig(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    m_timeoutCounter=0;
    m_port=0;
    m_hasError=0;
    //firmwareStatus = 0;
    m_betaFirmwareChecked = false;
    m_trunkFirmwareChecked = false;
    m_tempFirmwareFile=0;
    ui.progressBar->setVisible(false);
    ui.warningLabel->setVisible(false);
    //

    //QNetworkRequest req(QUrl("https://raw.github.com/diydrones/binary/master/Firmware/firmware2.xml"));



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
    QTimer::singleShot(10000,this,SLOT(requestFirmwares()));
    connect(ui.betaFirmwareButton,SIGNAL(clicked(bool)),this,SLOT(betaFirmwareButtonClicked(bool)));
    ui.betaFirmwareButton->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *action = new QAction(QString("Load Trunk Firmware"),ui.betaFirmwareButton);
    connect(action,SIGNAL(triggered()),this,SLOT(trunkFirmwareButtonClicked()));
    ui.betaFirmwareButton->addAction(action);
    connect(ui.cancelPushButton,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));
    ui.cancelPushButton->setEnabled(false);
    ui.cancelPushButton->setVisible(false);

    ui.progressBar->setMaximum(100);
    ui.progressBar->setValue(0);

    ui.textBrowser->setVisible(false);
    connect(ui.showOutputCheckBox,SIGNAL(clicked(bool)),ui.textBrowser,SLOT(setShown(bool)));

    connect(ui.linkComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setLink(int)));

    /*addBetaLabel(ui.roverPushButton);
    addBetaLabel(ui.planePushButton);
    addBetaLabel(ui.copterPushButton);
    addBetaLabel(ui.quadPushButton);
    addBetaLabel(ui.hexaPushButton);
    addBetaLabel(ui.octaQuadPushButton);
    addBetaLabel(ui.octaPushButton);
    addBetaLabel(ui.triPushButton);
    addBetaLabel(ui.y6PushButton);*/
    populateSerialPorts();
    if (ui.linkComboBox->count() > 0)
    {
        setLink(0);
    }

    m_uas = 0;
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
    m_timer = new QTimer(this);
    connect(m_timer,SIGNAL(timeout()),this,SLOT(populateSerialPorts()));
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

        if (!(info.portName().contains("Bluetooth"))){
            // Don't add bluetooth ports to be less confusing to the user
            // on windows, the friendly name is annoyingly identical between devices. On OSX it's different
            ui.linkComboBox->insertItem(0,list[0], list);
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
}

void ApmFirmwareConfig::showEvent(QShowEvent *)
{
    // Start Port scanning
    m_timer->start(2000);
}

void ApmFirmwareConfig::hideEvent(QHideEvent *)
{
    // Stop Port scanning
    m_timer->stop();
}

void ApmFirmwareConfig::uasConnected()
{
    ui.stackedWidget->setCurrentIndex(1);
}
void ApmFirmwareConfig::cancelButtonClicked()
{
    if (QMessageBox::question(this,tr("Warning"),tr("You are about to cancel the firmware upload process. ONLY do this if the process has not started properly. Are you sure you want to continue?"),QMessageBox::Yes,QMessageBox::No) != QMessageBox::Yes)
    {
        return;
    }
    if (m_burnProcess){
        QLOG_DEBUG() << "Closing Flashing Process";
        m_burnProcess->terminate();
        m_burnProcess->deleteLater();
    }

}

void ApmFirmwareConfig::uasDisconnected()
{
    ui.stackedWidget->setCurrentIndex(0);
}

void ApmFirmwareConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
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
        connect(uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        connect(uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
        uasConnected();
    }
}

void ApmFirmwareConfig::connectButtonClicked()
{

}

void ApmFirmwareConfig::disconnectButtonClicked()
{

}

void ApmFirmwareConfig::hideBetaLabels()
{
    for (int i=0;i<m_betaButtonLabelList.size();i++)
    {
        m_betaButtonLabelList[i]->hide();
    }
    ui.warningLabel->hide();
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

void ApmFirmwareConfig::requestBetaFirmwares()
{
    m_betaFirmwareChecked = true;
    m_trunkFirmwareChecked = false;
    showBetaLabels();
    QNetworkReply *reply1 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-heli/git-version.txt")));
    QNetworkReply *reply2 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-quad/git-version.txt")));
    QNetworkReply *reply3 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-hexa/git-version.txt")));
    QNetworkReply *reply4 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-octa/git-version.txt")));
    QNetworkReply *reply5 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-octa-quad/git-version.txt")));
    QNetworkReply *reply6 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-tri/git-version.txt")));
    QNetworkReply *reply7 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/beta/apm2-y6/git-version.txt")));
    QNetworkReply *reply8 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Plane/beta/apm2/git-version.txt")));
    QNetworkReply *reply9 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Rover/beta/apm2/git-version.txt")));

    m_buttonToUrlMap[ui.roverPushButton] = "http://firmware.diydrones.com/Rover/beta/apm2/APMrover2.hex";
    m_buttonToUrlMap[ui.planePushButton] = "http://firmware.diydrones.com/Plane/beta/apm2/ArduPlane.hex";
    m_buttonToUrlMap[ui.copterPushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-heli/ArduCopter.hex";
    m_buttonToUrlMap[ui.hexaPushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-hexa/ArduCopter.hex";
    m_buttonToUrlMap[ui.octaQuadPushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-octa-quad/ArduCopter.hex";
    m_buttonToUrlMap[ui.octaPushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-octa/ArduCopter.hex";
    m_buttonToUrlMap[ui.quadPushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-quad/ArduCopter.hex";
    m_buttonToUrlMap[ui.triPushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-tri/ArduCopter.hex";
    m_buttonToUrlMap[ui.y6PushButton] = "http://firmware.diydrones.com/Copter/beta/apm2-y6/ArduCopter.hex";

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
    QLOG_DEBUG() << "Getting Beta firmware...";
}

void ApmFirmwareConfig::requestFirmwares()
{
    m_betaFirmwareChecked = false;
    m_trunkFirmwareChecked = false;
    hideBetaLabels();
    QNetworkReply *reply1 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-heli/git-version.txt")));
    QNetworkReply *reply2 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-quad/git-version.txt")));
    QNetworkReply *reply3 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-hexa/git-version.txt")));
    QNetworkReply *reply4 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-octa/git-version.txt")));
    QNetworkReply *reply5 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-octa-quad/git-version.txt")));
    QNetworkReply *reply6 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-tri/git-version.txt")));
    QNetworkReply *reply7 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/stable/apm2-y6/git-version.txt")));
    QNetworkReply *reply8 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Plane/stable/apm2/git-version.txt")));
    QNetworkReply *reply9 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Rover/stable/apm2/git-version.txt")));

    m_buttonToUrlMap[ui.roverPushButton] = "http://firmware.diydrones.com/Rover/stable/apm2/APMrover2.hex";
    m_buttonToUrlMap[ui.planePushButton] = "http://firmware.diydrones.com/Plane/stable/apm2/ArduPlane.hex";
    m_buttonToUrlMap[ui.copterPushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-heli/ArduCopter.hex";
    m_buttonToUrlMap[ui.hexaPushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-hexa/ArduCopter.hex";
    m_buttonToUrlMap[ui.octaQuadPushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-octa-quad/ArduCopter.hex";
    m_buttonToUrlMap[ui.octaPushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-octa/ArduCopter.hex";
    m_buttonToUrlMap[ui.quadPushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-quad/ArduCopter.hex";
    m_buttonToUrlMap[ui.triPushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-tri/ArduCopter.hex";
    m_buttonToUrlMap[ui.y6PushButton] = "http://firmware.diydrones.com/Copter/stable/apm2-y6/ArduCopter.hex";

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
    QLOG_DEBUG() << "Getting Stable firmware...";
}

void ApmFirmwareConfig::betaFirmwareButtonClicked(bool betafirmwareenabled)
{
    QLOG_DEBUG() << "Beta FW Button clicked";

    if (betafirmwareenabled)
    {
        QMessageBox::information(this,tr("Warning"),tr("These are beta firmware downloads. Use at your own risk!!!"));
        ui.label->setText(tr("<h2>Beta Firmware</h2>"));
        ui.betaFirmwareButton->setText(tr("Stable Firmware"));
        requestBetaFirmwares();
    }
    else
    {
        ui.label->setText(tr("<h2>Firmware</h2>"));
        ui.betaFirmwareButton->setText(tr("Beta Firmware"));
        requestFirmwares();
    }
}
void ApmFirmwareConfig::trunkFirmwareButtonClicked()
{
    QMessageBox::information(this,tr("Warning"),tr("These are trunk firmware downloads. These should ONLY BE USED if you know what you're doing!!!"));
    ui.label->setText(tr("<h2>Trunk Firmware</h2>"));
    ui.betaFirmwareButton->setText(tr("Stable Firmware"));
    ui.betaFirmwareButton->setChecked(true);
    m_trunkFirmwareChecked = true;
    m_betaFirmwareChecked = false;
    //showBetaLabels();
    QNetworkReply *reply1 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-heli/git-version.txt")));
    QNetworkReply *reply2 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-quad/git-version.txt")));
    QNetworkReply *reply3 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-hexa/git-version.txt")));
    QNetworkReply *reply4 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-octa/git-version.txt")));
    QNetworkReply *reply5 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-octa-quad/git-version.txt")));
    QNetworkReply *reply6 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-tri/git-version.txt")));
    QNetworkReply *reply7 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Copter/latest/apm2-y6/git-version.txt")));
    QNetworkReply *reply8 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Plane/latest/apm2/git-version.txt")));
    QNetworkReply *reply9 = m_networkManager->get(QNetworkRequest(QUrl("http://firmware.diydrones.com/Rover/latest/apm2/git-version.txt")));

    m_buttonToUrlMap[ui.roverPushButton] = "http://firmware.diydrones.com/Rover/latest/apm2/APMrover2.hex";
    m_buttonToUrlMap[ui.planePushButton] = "http://firmware.diydrones.com/Plane/latest/apm2/ArduPlane.hex";
    m_buttonToUrlMap[ui.copterPushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-heli/ArduCopter.hex";
    m_buttonToUrlMap[ui.hexaPushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-hexa/ArduCopter.hex";
    m_buttonToUrlMap[ui.octaQuadPushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-octa-quad/ArduCopter.hex";
    m_buttonToUrlMap[ui.octaPushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-octa/ArduCopter.hex";
    m_buttonToUrlMap[ui.quadPushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-quad/ArduCopter.hex";
    m_buttonToUrlMap[ui.triPushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-tri/ArduCopter.hex";
    m_buttonToUrlMap[ui.y6PushButton] = "http://firmware.diydrones.com/Copter/latest/apm2-y6/ArduCopter.hex";

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
    QLOG_DEBUG() << "Getting Trunk firmware...";

}

void ApmFirmwareConfig::firmwareProcessFinished(int status)
{
    QLOG_DEBUG() << "firmwareProcessFinished: " << status;
    QProcess *proc = qobject_cast<QProcess*>(sender());
    if (!proc)
    {
        return;
    }
    if (status != 0)
    {
        //Error of some kind
        QMessageBox::information(0,tr("Error"),tr("An error has occured during the upload process. See window for details"));
        ui.textBrowser->setVisible(true);
        ui.showOutputCheckBox->setChecked(true);
        ui.textBrowser->setPlainText(ui.textBrowser->toPlainText().append("\n\nERROR!!\n" + proc->errorString()));
        QScrollBar *sb = ui.textBrowser->verticalScrollBar();
        if (sb)
        {
            sb->setValue(sb->maximum());
        }
        ui.statusLabel->setText(tr("Error during upload"));
    }
    else
    {
        //Ensure we're reading 100%
        ui.progressBar->setValue(100);
        if (!m_hasError)
        {
            ui.statusLabel->setText(tr("Upload complete"));
        }
    }
    //QLOG_DEBUG() << "Upload finished!" << QString::number(status);
    m_tempFirmwareFile->deleteLater(); //This will remove the temporary file.
    m_tempFirmwareFile = 0;
    ui.progressBar->setVisible(false);
    ui.cancelPushButton->setEnabled(false);
    ui.cancelPushButton->setVisible(false);

}
void ApmFirmwareConfig::firmwareProcessReadyRead()
{
    QProcess *proc = qobject_cast<QProcess*>(sender());
    QLOG_DEBUG() << "firmwareProcessReadyRead: " << proc;

    if (!proc)
    {
        return;
    }

    QString output = proc->readAllStandardError() + proc->readAllStandardOutput();
    if (output.contains("Writing"))
    {
        //firmwareStatus->resetProgress();
        ui.progressBar->setValue(0);
        ui.statusLabel->setText("Flashing");
    }
    else if (output.contains("Reading"))
    {
        ui.progressBar->setValue(50);
        ui.statusLabel->setText("Verifying");
    }
    if (output.startsWith("#"))
    {
        ui.progressBar->setValue(ui.progressBar->value()+1);

        ui.textBrowser->setPlainText(ui.textBrowser->toPlainText().append(output));
        QScrollBar *sb = ui.textBrowser->verticalScrollBar();
        if (sb)
        {
            sb->setValue(sb->maximum());
        }
    }
    else
    {
        if (output.contains("timeout"))
        {
            //Timeout, increment timeout timer.
            m_timeoutCounter++;
            if (m_timeoutCounter > 3)
            {
                //We've reached timeout
                QMessageBox::information(0,tr("Error"),tr("An error has occured during the upload process. Check to make sure you are connected to the correct serial port"));
                ui.statusLabel->setText(tr("Error during upload"));
                proc->terminate();
                proc->deleteLater();
                m_hasError = true;
            }
        }
        ui.textBrowser->setPlainText(ui.textBrowser->toPlainText().append(output + "\n"));
        QScrollBar *sb = ui.textBrowser->verticalScrollBar();
        if (sb)
        {
            sb->setValue(sb->maximum());
        }
    }

    QLOG_DEBUG() << "E:" << output;
    //QLOG_DEBUG() << "AVR Output:" << proc->readAllStandardOutput();
    //QLOG_DEBUG() << "AVR Output:" << proc->readAllStandardError();
}

void ApmFirmwareConfig::downloadFinished()
{
    QLOG_DEBUG() << "Download finished, flashing firmware";
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        return;
    }
    QByteArray hex = reply->readAll();
    m_tempFirmwareFile = new QTemporaryFile();
    m_tempFirmwareFile->open();
    m_tempFirmwareFile->write(hex);
    m_tempFirmwareFile->flush();
    m_tempFirmwareFile->close();
    //tempfirmware.fileName()
    ui.cancelPushButton->setEnabled(true);
    ui.cancelPushButton->setVisible(true);
    m_burnProcess = new QProcess(this);
    connect(m_burnProcess,SIGNAL(finished(int)),this,SLOT(firmwareProcessFinished(int)));
    connect(m_burnProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(firmwareProcessReadyRead()));
    connect(m_burnProcess,SIGNAL(readyReadStandardError()),this,SLOT(firmwareProcessReadyRead()));
    connect(m_burnProcess,SIGNAL(error(QProcess::ProcessError)),this,SLOT(firmwareProcessError(QProcess::ProcessError)));
    QList<QSerialPortInfo> portList =  QSerialPortInfo::availablePorts();


    foreach (const QSerialPortInfo &info, portList)
    {
        QLOG_DEBUG() << "PortName    : " << info.portName()
               << "Description : " << info.description();
        QLOG_DEBUG() << "Manufacturer: " << info.manufacturer();


    }

    //info.manufacturer() == "Arduino LLC (www.arduino.cc)"
    //info.description() == "%mega2560.name%"

    QLOG_DEBUG() << "Attempting to Open port";


    m_port= new QSerialPort(this);
    m_port->setPortName(m_settings.name);
    if (m_port->open(QIODevice::ReadWrite)) {
        if (m_port->setBaudRate(m_settings.baudRate)
                && m_port->setDataBits(m_settings.dataBits)
                && m_port->setParity(m_settings.parity)
                && m_port->setStopBits(m_settings.stopBits)
                && m_port->setFlowControl(m_settings.flowControl)) {
            QLOG_INFO() << "Open Terminal Console Serial Port";
            m_port->setDataTerminalReady(true);
            m_port->waitForBytesWritten(250);
            m_port->setDataTerminalReady(false);
            m_port->close();
        } else {
            m_port->close();
            QMessageBox::critical(this, tr("Error"), m_port->errorString());
            m_port->deleteLater();
            return;
        }
    } else {
        QMessageBox::critical(this, tr("Error"), m_port->errorString());
        m_port->deleteLater();
        return;
    }

    QLOG_INFO() << "Port Open fro FW Upload";

    QString avrdudeExecutable;
    QStringList stringList;

    ui.statusLabel->setText(tr("Flashing"));
#ifdef Q_OS_WIN
    stringList = QStringList() << "-Cavrdude/avrdude.conf" << "-pm2560"
                               << "-cstk500" << QString("-P").append(m_settings.name)
                               << QString("-Uflash:w:").append(m_tempFirmwareFile->fileName()).append(":i");

    avrdudeExecutable = "avrdude/avrdude.exe";
#endif
#ifdef Q_OS_MAC
    stringList = QStringList() << "-v" << "-pm2560"
                                           << "-cstk500" << QString("-P/dev/cu.").append(m_settings.name)
                                           << QString("-Uflash:w:").append(m_tempFirmwareFile->fileName()).append(":i");
    avrdudeExecutable = "/usr/local/CrossPack-AVR/bin/avrdude";
#endif

    // Start the Flashing
    QLOG_DEBUG() << avrdudeExecutable << stringList;
    m_burnProcess->start(avrdudeExecutable,stringList);
    m_timeoutCounter=0;
    m_hasError=false;
    ui.progressBar->setValue(0);
}
void ApmFirmwareConfig::firmwareProcessError(QProcess::ProcessError error)
{
    QLOG_DEBUG() << "Error:" << error;
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

        /*bool foundconnected = false;
        for (int i=0;i<LinkManager::instance()->getLinks().size();i++)
        {
            if (LinkManager::instance()->getLinks()[i]->isConnected())
            {
                //This is likely the serial link we want.
                SerialLink *link = qobject_cast<SerialLink*>(LinkManager::instance()->getLinks()[i]);
                if (!link)
                {
                    QLOG_DEBUG() << "Eror, trying to program over a non serial link. This should not happen";
                    return;
                }
                if (!(QMessageBox::question(this,tr("WARNING"),tr("You are about to upload new firmware to your board. This will disconnect you if you are currently connected. Be sure the MAV is on the ground, and connected over USB/Serial link.\n\nDo you wish to proceed?"),QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes))
                {
                    return;
                }

                m_detectedComPort = link->getPortName();
                link->requestReset();
                foundconnected = true;
                link->disconnect();
                link->wait(1000); // Wait 1 second for it to disconnect.
            }
        }
        if (!foundconnected)
        {
            QMessageBox::information(0,tr("Error"),tr("You must be connected to a MAV over serial link to flash firmware. Please connect to a MAV then try again"));
            return;
        }*/
        //Try to connect before downloading:
        if (m_uas)
        {
            //Active UAS. Ensure it's not connected over Serial
            for (int i=0;i<m_uas->getLinks()->size();i++)
            {
                SerialLink *testlink = qobject_cast<SerialLink*>(m_uas->getLinks()->at(i));
                if (testlink)
                {
                    //It's a serial link
                    if (testlink->isConnected())
                    {
                        //Error out, we don't want to attempt firmware upload when there is a serial link active.
                        QMessageBox::information(this,"Error","You cannot load new firmware while connected via MAVLink. Please press the Disconnect button at top right to end the current MAVLink session and enable the firmware loading screen.");
                        return;
                    }
                }
            }
        }
        QLOG_DEBUG() << "creating new serial port" << m_settings.name;
        m_port = new QSerialPort(this);
        QLOG_DEBUG() << m_port;
        m_port->setPortName(m_settings.name);
        if (m_port->open(QIODevice::ReadWrite)) {
            if (m_port->setBaudRate(m_settings.baudRate)
                    && m_port->setDataBits(m_settings.dataBits)
                    && m_port->setParity(m_settings.parity)
                    && m_port->setStopBits(m_settings.stopBits)
                    && m_port->setFlowControl(m_settings.flowControl)) {
                QLOG_INFO() << "Open Firmware Upload Serial Port";
                m_port->close();
            } else {
                m_port->close();
                QMessageBox::critical(this, tr("Error"), m_port->errorString());
                m_port->deleteLater();
                return;
            }
        } else {
            QMessageBox::critical(this, tr("Error"), m_port->errorString());
            m_port->deleteLater();
            return;
        }
        m_port->deleteLater();
        ui.progressBar->setVisible(true);

        QLOG_DEBUG() << "Go download:" << m_buttonToUrlMap[senderbtn];
        QNetworkReply *reply = m_networkManager->get(QNetworkRequest(QUrl(m_buttonToUrlMap[senderbtn])));
        //http://firmware.diydrones.com/Plane/stable/apm2/ArduPlane.hex
        connect(reply,SIGNAL(finished()),this,SLOT(downloadFinished()));

        connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(firmwareListError(QNetworkReply::NetworkError)));
        connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(firmwareDownloadProgress(qint64,qint64)));
        ui.statusLabel->setText("Downloading");
    }
}
void ApmFirmwareConfig::setLink(int index)
{
    if (ui.linkComboBox->itemData(index).toStringList().size() > 0)
    {
        m_settings.name = ui.linkComboBox->itemData(index).toStringList()[0];
#ifdef Q_OS_WIN
        ui.comPortNameLabel->setText(ui.linkComboBox->itemData(index).toStringList()[1] + "\n" + ui.linkComboBox->itemData(index).toStringList()[2]);
#else
        ui.comPortNameLabel->setText(ui.linkComboBox->itemData(index).toStringList()[0] + "\n" + ui.linkComboBox->itemData(index).toStringList()[2]);
#endif
        //QLOG_INFO() << "Changed Link to:" << m_settings.name;
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

    QLOG_DEBUG() << "firmwareListFinished error: " << reply->error() << reply->errorString();
    QString cmpstr = "";
    QString labelstr = "";
    if (m_betaFirmwareChecked)
    {
        cmpstr = "beta";
        labelstr = "BETA ";
    }
    else if (m_trunkFirmwareChecked)
    {
        cmpstr = "latest";
        labelstr = "TRUNK ";
    }
    else
    {
        cmpstr = "stable";
        labelstr = "";
    }

    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-heli",cmpstr,&outstr))
    {
        ui.copterLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-quad",cmpstr,&outstr))
    {
        ui.quadLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-hexa",cmpstr,&outstr))
    {
        ui.hexaLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-octa-quad",cmpstr,&outstr))
    {
        ui.octaQuadLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-octa",cmpstr,&outstr))
    {
        ui.octaLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-tri",cmpstr,&outstr))
    {
        ui.triLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"apm2-y6",cmpstr,&outstr))
    {
        ui.y6Label->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"Plane",cmpstr,&outstr))
    {
        ui.planeLabel->setText(labelstr + outstr);
        return;
    }
    if (stripVersionFromGitReply(reply->url().toString(),replystr,"Rover",cmpstr,&outstr))
    {
        ui.roverLabel->setText(labelstr + outstr);
        return;
    }
    //QLOG_DEBUG() << "Match not found for:" << reply->url();
    //QLOG_DEBUG() << "Git version line:" <<  replystr;
}

ApmFirmwareConfig::~ApmFirmwareConfig()
{
}

