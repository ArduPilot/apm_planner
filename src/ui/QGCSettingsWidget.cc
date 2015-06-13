#include <QSettings>

#include "QGCSettingsWidget.h"
#include "MainWindow.h"
#include "ui_QGCSettingsWidget.h"

#include "LinkManager.h"
#include "MAVLinkProtocol.h"
#include "MAVLinkSettingsWidget.h"
#include "GAudioOutput.h"
#include "ArduPilotMegaMAV.h"

#include <QFileDialog>

QGCSettingsWidget::QGCSettingsWidget(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags),
    ui(new Ui::QGCSettingsWidget)
{
    m_init = false;
    ui->setupUi(this);

    // Add all protocols
    /*QList<ProtocolInterface*> protocols = LinkManager::instance()->getProtocols();
    foreach (ProtocolInterface* protocol, protocols) {
        MAVLinkProtocol* mavlink = dynamic_cast<MAVLinkProtocol*>(protocol);
        if (mavlink) {
            MAVLinkSettingsWidget* msettings = new MAVLinkSettingsWidget(mavlink, this);
            ui->tabWidget->addTab(msettings, "MAVLink");
        }
    }*/

    this->window()->setWindowTitle(tr("APM Planner 2 Settings"));


}

void QGCSettingsWidget::showEvent(QShowEvent *evt)
{
    if (!m_init)
    {
        m_init = true;
        // Audio preferences
        ui->audioMuteCheckBox->setChecked(GAudioOutput::instance()->isMuted());
        connect(ui->audioMuteCheckBox, SIGNAL(toggled(bool)), GAudioOutput::instance(), SLOT(mute(bool)));
        connect(GAudioOutput::instance(), SIGNAL(mutedChanged(bool)), ui->audioMuteCheckBox, SLOT(setChecked(bool)));

        // Reconnect
        ui->reconnectCheckBox->setChecked(MainWindow::instance()->autoReconnectEnabled());
        connect(ui->reconnectCheckBox, SIGNAL(clicked(bool)), MainWindow::instance(), SLOT(enableAutoReconnect(bool)));

        // Low power mode
        ui->lowPowerCheckBox->setChecked(MainWindow::instance()->lowPowerModeEnabled());
        connect(ui->lowPowerCheckBox, SIGNAL(clicked(bool)), MainWindow::instance(), SLOT(enableLowPowerMode(bool)));

        //Dock widget title bars
        ui->titleBarCheckBox->setChecked(MainWindow::instance()->dockWidgetTitleBarsEnabled());
        connect(ui->titleBarCheckBox,SIGNAL(clicked(bool)),MainWindow::instance(),SLOT(enableDockWidgetTitleBars(bool)));

        ui->heartbeatCheckBox->setChecked(MainWindow::instance()->heartbeatEnabled());
        connect(ui->heartbeatCheckBox,SIGNAL(clicked(bool)),MainWindow::instance(),SLOT(enableHeartbeat(bool)));

        ui->mavlinkLoggingCheckBox->setChecked(LinkManager::instance()->loggingEnabled());
        connect(ui->mavlinkLoggingCheckBox,SIGNAL(clicked(bool)),LinkManager::instance(),SLOT(enableLogging(bool)));

        ui->logDirEdit->setText(QGC::logDirectory());

        ui->appDataDirEdit->setText((QGC::appDataDirectory()));
        ui->paramDirEdit->setText(QGC::parameterDirectory());
        ui->mavlinkLogDirEdit->setText((QGC::MAVLinkLogDirectory()));
        ui->missionsDirEdit->setText((QGC::missionDirectory()));

        connect(ui->logDirSetButton, SIGNAL(clicked()), this, SLOT(setLogDir()));
        connect(ui->appDirSetButton, SIGNAL(clicked()), this, SLOT(setAppDataDir()));
        connect(ui->paramDirSetButton, SIGNAL(clicked()), this, SLOT(setParamDir()));
        connect(ui->mavlinkDirSetButton, SIGNAL(clicked()), this, SLOT(setMAVLinkLogDir()));
        connect(ui->missionsSetButton, SIGNAL(clicked()), this, SLOT(setMissionsDir()));

        // Style
        MainWindow::QGC_MAINWINDOW_STYLE style = (MainWindow::QGC_MAINWINDOW_STYLE)MainWindow::instance()->getStyle();
        switch (style) {
        case MainWindow::QGC_MAINWINDOW_STYLE_NATIVE:
            ui->nativeStyle->setChecked(true);
            break;
        case MainWindow::QGC_MAINWINDOW_STYLE_INDOOR:
            ui->indoorStyle->setChecked(true);
            break;
        case MainWindow::QGC_MAINWINDOW_STYLE_OUTDOOR:
            ui->outdoorStyle->setChecked(true);
            break;
        }
        connect(ui->nativeStyle, SIGNAL(clicked()), MainWindow::instance(), SLOT(loadNativeStyle()));
        connect(ui->indoorStyle, SIGNAL(clicked()), MainWindow::instance(), SLOT(loadIndoorStyle()));
        connect(ui->outdoorStyle, SIGNAL(clicked()), MainWindow::instance(), SLOT(loadOutdoorStyle()));

        connect(ui->extra1LineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));
        connect(ui->extra2LineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));
        connect(ui->extra3LineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));
        connect(ui->positionLineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));
        connect(ui->extStatusLineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));
        connect(ui->rcChannelDataLineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));
        connect(ui->rawSensorLineEdit, SIGNAL(editingFinished()), this, SLOT(ratesChanged()));

        connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(setActiveUAS(UASInterface*)));
        setActiveUAS(UASManager::instance()->getActiveUAS());

        setDataRateLineEdits();

        QSettings settings;
        settings.beginGroup("AUTO_UPDATE");
        if(!settings.value("RELEASE_TYPE", "stable").toString().contains("stable")){
            ui->enableBetaReleaseCheckBox->setChecked(true);
        }
        settings.endGroup();
        connect(ui->enableBetaReleaseCheckBox, SIGNAL(clicked(bool)), this, SLOT(setBetaRelease(bool)));
    }
}

QGCSettingsWidget::~QGCSettingsWidget()
{
    delete ui;
}

void QGCSettingsWidget::setLogDir()
{
    QFileDialog dlg(this, "Set log output directory");
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setDirectory(QGC::logDirectory());

    if(dlg.exec() == QDialog::Accepted) {
        QDir dir = dlg.directory();
        QString name = dir.absolutePath();
        QGC::setLogDirectory(name);
        ui->logDirEdit->setText(name);
    }
}

void QGCSettingsWidget::setMAVLinkLogDir()
{
    QFileDialog dlg(this, "Set tlog output directory");
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setDirectory(QGC::MAVLinkLogDirectory());

    if(dlg.exec() == QDialog::Accepted) {
        QDir dir = dlg.directory();
        QString name = dir.absolutePath();
        QGC::setMAVLinkLogDirectory(name);
        ui->mavlinkLogDirEdit->setText(name);
    }
}

void QGCSettingsWidget::setParamDir()
{
    QFileDialog dlg(this, "Set parameters directory");
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setDirectory(QGC::parameterDirectory());

    if(dlg.exec() == QDialog::Accepted) {
        QDir dir = dlg.directory();
        QString name = dir.absolutePath();
        QGC::setParameterDirectory(name);
        ui->paramDirEdit->setText(name);
    }
}

void QGCSettingsWidget::setAppDataDir()
{
    QFileDialog dlg(this, "Set application data directory");
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setDirectory(QGC::appDataDirectory());

    if(dlg.exec() ==  QDialog::Accepted) {
        QDir dir = dlg.directory();
        QString name = dir.absolutePath();
        QGC::setAppDataDirectory(name);
        ui->appDataDirEdit->setText(name);
    }
}

void QGCSettingsWidget::setMissionsDir()
{
    QFileDialog dlg(this, "Set missions directory");
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setDirectory(QGC::missionDirectory());

    if(dlg.exec() ==  QDialog::Accepted) {
        QDir dir = dlg.directory();
        QString name = dir.absolutePath();
        QGC::setMissionDirectory(name);
        ui->missionsDirEdit->setText(name);
    }
}

void QGCSettingsWidget::setActiveUAS(UASInterface *uas)
{
    if (m_uas){
        m_uas = NULL;
    }

    if (uas != NULL){
        m_uas = uas;
    }
}

void QGCSettingsWidget::setDataRateLineEdits()
{
    QSettings settings;
    settings.beginGroup("DATA_RATES");
    ui->extStatusLineEdit->setText(settings.value("EXT_SYS_STATUS",2).toString());
    ui->positionLineEdit->setText(settings.value("POSITION",3).toString());
    ui->extra1LineEdit->setText(settings.value("EXTRA1",10).toString());
    ui->extra2LineEdit->setText(settings.value("EXTRA2",10).toString());
    ui->extra3LineEdit->setText(settings.value("EXTRA3",2).toString());

    ui->rawSensorLineEdit->setText(settings.value("RAW_SENSOR_DATA",2).toString());
    ui->rcChannelDataLineEdit->setText(settings.value("RC_CHANNEL_DATA",2).toString());
    settings.endGroup();
}

void QGCSettingsWidget::ratesChanged()
{
    QSettings settings;
    settings.beginGroup("DATA_RATES");
    bool ok;
    int conversion = ui->extStatusLineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("EXT_SYS_STATUS",conversion);
    }

    conversion = ui->positionLineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("POSITION",conversion);
    }

    conversion = ui->extra1LineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("EXTRA1", conversion);
    }

    conversion = ui->extra2LineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("EXTRA2", conversion);
    }

    conversion = ui->extra3LineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("EXTRA3", conversion);
    }

    conversion = ui->rawSensorLineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("RAW_SENSOR_DATA", conversion);
    }

    conversion = ui->rcChannelDataLineEdit->text().toInt(&ok);
    if (ok){
        settings.setValue("RC_CHANNEL_DATA", conversion);
    }
    settings.endGroup();
    settings.sync();

    setDataRateLineEdits();

    if (m_uas) {
        ArduPilotMegaMAV *mav = dynamic_cast<ArduPilotMegaMAV*>(m_uas);
        if (mav != NULL){
            mav->RequestAllDataStreams();
        }
    }
}

void QGCSettingsWidget::setBetaRelease(bool state)
{
    QString type;
    QSettings settings;
    settings.beginGroup("AUTO_UPDATE");
    if (state == true){
        type = "beta";
    } else {
        type = "stable";
    }
    settings.setValue("RELEASE_TYPE", type);
    settings.sync();
}
