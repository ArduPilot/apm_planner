#include <QSettings>

#include "QGCSettingsWidget.h"
#include "MainWindow.h"
#include "ui_QGCSettingsWidget.h"

#include "LinkManager.h"
#include "MAVLinkProtocol.h"
#include "MAVLinkSettingsWidget.h"
#include "GAudioOutput.h"

//, Qt::WindowFlags flags

QGCSettingsWidget::QGCSettingsWidget(QWidget *parent, Qt::WindowFlags flags) :
    QDialog(parent, flags),
    ui(new Ui::QGCSettingsWidget)
{
    m_init = false;
    ui->setupUi(this);

    // Add all protocols
    QList<ProtocolInterface*> protocols = LinkManager::instance()->getProtocols();
    foreach (ProtocolInterface* protocol, protocols) {
        MAVLinkProtocol* mavlink = dynamic_cast<MAVLinkProtocol*>(protocol);
        if (mavlink) {
            MAVLinkSettingsWidget* msettings = new MAVLinkSettingsWidget(mavlink, this);
            ui->tabWidget->addTab(msettings, "MAVLink");
        }
    }

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

        ui->logDirEdit->setText(QGC::logDirectory());

        ui->appDataDirEdit->setText((QGC::appDataDirectory()));
        ui->paramDirEdit->setText(QGC::parameterDirectory());
        ui->mavlinkLogDirEdit->setText((QGC::MAVLinkLogDirectory()));

        connect(ui->logDirSetButton, SIGNAL(clicked()), this, SLOT(setLogDir()));
        connect(ui->appDirSetButton, SIGNAL(clicked()), this, SLOT(setAppDataDir()));
        connect(ui->paramDirSetButton, SIGNAL(clicked()), this, SLOT(setParamDir()));
        connect(ui->mavlinkDirSetButton, SIGNAL(clicked()), this, SLOT(setMAVLinkLogDir()));

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
    }
}

QGCSettingsWidget::~QGCSettingsWidget()
{
    delete ui;
}

void QGCSettingsWidget::setLogDir()
{
    QFileDialog dlg(this);
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
    QFileDialog dlg(this);
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
    QFileDialog dlg(this);
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
    QFileDialog dlg(this);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setDirectory(QGC::appDataDirectory());

    if(dlg.exec() ==  QDialog::Accepted) {
        QDir dir = dlg.directory();
        QString name = dir.absolutePath();
        QGC::setAppDataDirectory(name);
        ui->appDataDirEdit->setText(name);
    }
}
