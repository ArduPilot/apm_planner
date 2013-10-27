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
 *   @brief Firmware select and upload
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef APMFIRMWARECONFIG_H
#define APMFIRMWARECONFIG_H

#include <QWidget>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QTemporaryFile>
#include <QProcess>
#include <QXmlStreamReader>
#include <QMessageBox>
#include <QProcess>
#include <QScrollBar>
#include <QProgressDialog>
#include <QTimer>

#include "UASInterface.h"
#include "UASManager.h"

#include "qserialport.h"
#include "SerialSettingsDialog.h"
#include "ui_ApmFirmwareConfig.h"
#include "PX4FirmwareUploader.h"

class ApmFirmwareConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit ApmFirmwareConfig(QWidget *parent = 0);
    ~ApmFirmwareConfig();
signals:
    void showBlankingScreen();
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots:
    void firmwareListFinished();
    void firmwareListError(QNetworkReply::NetworkError error);
    void flashButtonClicked();
    void betaFirmwareButtonClicked();
    void stableFirmwareButtonClicked();
    void downloadFinished();
    void trunkFirmwareButtonClicked();
    void firmwareProcessFinished(int status);
    void firmwareProcessReadyRead();
    void firmwareProcessError(QProcess::ProcessError error);
    void firmwareDownloadProgress(qint64 received,qint64 total);
    void requestFirmwares(QString type,QString autopilot);
    void connectButtonClicked();
    void disconnectButtonClicked();
    void setLink(int index);
    void activeUASSet(UASInterface *uas);
    void uasConnected();
    void uasDisconnected();
    void cancelButtonClicked();
    void populateSerialPorts();
    void requestDeviceReplug();
    void devicePlugDetected();
    void px4Error(QString error);
    void px4Finished();
    void px4Terminated();
    void px4StatusUpdate(QString update);
    void px4DebugUpdate(QString update);
    void px4UnplugTimerTick();

    void flashCustomFirmware();
    void flashFirmware(QString filename);

private:
    QProgressDialog *m_replugRequestMessageBox;
    QTimer *m_px4UnplugTimer;
    PX4FirmwareUploader *m_px4uploader;
    QString m_firmwareType;
    QString m_autopilotType;
    bool m_isPx4;
    int m_timeoutCounter;
    bool m_hasError;
    QPointer<QProcess> m_burnProcess;
    QPointer<UASInterface> m_uas;
    SerialSettings m_settings;
    QPointer<QSerialPort> m_port;
    void addBetaLabel(QWidget *parent);
    void hideBetaLabels();
    void showBetaLabels();
    //ApmFirmwareStatus *firmwareStatus;
    QString m_detectedComPort;
    QPointer<QTemporaryFile> m_tempFirmwareFile;
    QPointer<QNetworkAccessManager> m_networkManager;
    QList<QLabel*> m_betaButtonLabelList;
    bool stripVersionFromGitReply(QString url,QString reply,QString type,QString stable,QString *out);
    bool m_betaFirmwareChecked;
    bool m_trunkFirmwareChecked;
    QMap<QPushButton*,QString> m_buttonToUrlMap;
    Ui::ApmFirmwareConfig ui;
    class FirmwareDef
    {
    public:
        QString url;
        QString url2560;
        QString url25602;
        QString urlpx4;
        QString type;
        QString name;
        QString desc;
        int version;
    };
    QList<FirmwareDef> m_firmwareList;
    QPointer<QTimer> m_timer;
};

#endif // APMFIRMWARECONFIG_H

