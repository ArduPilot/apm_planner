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
#include "UASInterface.h"
#include "UASManager.h"

#include "qserialport.h"
#include "SerialSettingsDialog.h"
#include "ui_ApmFirmwareConfig.h"

class ApmFirmwareConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit ApmFirmwareConfig(QWidget *parent = 0);
    ~ApmFirmwareConfig();
private slots:
    void firmwareListFinished();
    void firmwareListError(QNetworkReply::NetworkError error);
    void flashButtonClicked();
    void betaFirmwareButtonClicked(bool betafirmwareenabled);
    void downloadFinished();
    void firmwareProcessFinished(int status);
    void firmwareProcessReadyRead();
    void firmwareProcessError(QProcess::ProcessError error);
    void firmwareDownloadProgress(qint64 received,qint64 total);
    void requestFirmwares();
    void requestBetaFirmwares();
    void connectButtonClicked();
    void disconnectButtonClicked();
    void setLink(int index);
    void activeUASSet(UASInterface *uas);
    void uasConnected();
    void uasDisconnected();
    void cancelButtonClicked();
    void populateSerialPorts();
private:
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
};

#endif // APMFIRMWARECONFIG_H

