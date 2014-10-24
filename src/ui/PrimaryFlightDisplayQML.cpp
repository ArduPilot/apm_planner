/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 Bill Bonney <billbonney@communistech.com>

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

#include "QsLog.h"
#include "PrimaryFlightDisplayQML.h"
#include "ui_PrimaryFlightDisplayQML.h"
#include "configuration.h"
#include "MainWindow.h"
#include <QVBoxLayout>
#include <QtQml/QQmlContext>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlEngine>
#include <QGst/Init>
#include <QGst/Quick/VideoSurface>

#define ToRad(x) (x*0.01745329252)      // *pi/180
#define ToDeg(x) (x*57.2957795131)      // *180/pi

QDialog * PrimaryFlightDisplayQML::s_primaryFlightDisplayDialog = NULL;

PrimaryFlightDisplayQML::PrimaryFlightDisplayQML(QWidget *parent) :
    QWidget(parent),
    m_declarativeView(NULL),
    m_player(NULL),
    m_viewcontainer(NULL),
    m_showToolAction(NULL),
    m_videoEnabled(false),
    m_topMostMode(false)
{

    // Get GStreamer pipeline string if specified
    QSettings settings;
    settings.beginGroup("QGC_GSTREAMER");
    m_pipelineString = settings.value("GSTREAMER_PIPELINE_STRING").toString();
    m_enableGStreamer = settings.value("PFD_ENABLE_GSTREAMER").toBool();

    settings.endGroup();

    // Default to non-video display until user selects
    InitializeDisplay();

    show();

    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this,
            SLOT(setActiveUAS(UASInterface*)), Qt::UniqueConnection);


}

PrimaryFlightDisplayQML::~PrimaryFlightDisplayQML()
{
    delete m_player;
    m_player = NULL;
}

void PrimaryFlightDisplayQML::setActiveUAS(UASInterface *uas)
{
    if (m_uasInterface) {
        disconnect(uas,SIGNAL(textMessageReceived(int,int,int,QString)),
                this,SLOT(uasTextMessage(int,int,int,QString)));

        disconnect(uas, SIGNAL(navModeChanged(int, int, QString)),
                   this, SLOT(updateNavMode(int, int, QString)));

    }
    m_uasInterface = uas;

    if (m_uasInterface) {
        connect(uas,SIGNAL(textMessageReceived(int,int,int,QString)),
                this,SLOT(uasTextMessage(int,int,int,QString)));
        connect(uas, SIGNAL(navModeChanged(int, int, QString)),
                this, SLOT(updateNavMode(int, int, QString)));

        VehicleOverview *obj = LinkManager::instance()->getUasObject(uas->getUASID())->getVehicleOverview();
        RelPositionOverview *rel = LinkManager::instance()->getUasObject(uas->getUASID())->getRelPositionOverview();
        m_declarativeView->rootContext()->setContextProperty("vehicleoverview",obj);
        m_declarativeView->rootContext()->setContextProperty("relpositionoverview",rel);
        m_declarativeView->rootContext()->setContextProperty("abspositionoverview",LinkManager::instance()->getUasObject(uas->getUASID())->getAbsPositionOverview());
        QMetaObject::invokeMethod(m_declarativeView->rootObject(),"activeUasSet");
    }
}

void PrimaryFlightDisplayQML::uasTextMessage(int uasid, int componentid, int severity, QString text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);
    if (text.contains("PreArm") || severity == 3)
    {
        QObject *root = m_declarativeView->rootObject();
        root->setProperty("statusMessage", text);
        root->setProperty("showStatusMessage", true);
    }
}

void PrimaryFlightDisplayQML::updateNavMode(int uasid, int mode, const QString& text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(mode);
    QObject *root = m_declarativeView->rootObject();
    root->setProperty("navMode", text);
}

// QtGStreamer and the Qt docking system don't play well, so when we dock/undock, we need to reload (TODO: fix this)
void PrimaryFlightDisplayQML::topLevelChanged(bool topLevel)
{
    if (m_videoEnabled && topLevel) InitializeDisplayWithVideo();
}

void PrimaryFlightDisplayQML::dockLocationChanged(Qt::DockWidgetArea area)
{
    if (m_videoEnabled) InitializeDisplayWithVideo();
}

void PrimaryFlightDisplayQML::InitializeDisplayWithVideo()
{
    ResetDisplay();

    QString qml = QGC::shareDirectory() + "/qml/PrimaryFlightDisplayWithVideoQML.qml";
    QUrl url = QUrl::fromLocalFile(qml);
    QLOG_DEBUG() << url;
    if (!QFile::exists(qml))
    {
        QMessageBox::information(0,"Error", "" + qml + " not found. Please reinstall the application and try again");
        exit(-1);
    }
    m_declarativeView = new QQuickView();
    m_declarativeView->engine()->addImportPath("qml/"); //For local or win32 builds
    m_declarativeView->engine()->addImportPath("qml/quick2"); //For local or win32 builds
    m_declarativeView->engine()->addImportPath(QGC::shareDirectory() +"/qml"); //For installed linux builds

    QGst::Quick::VideoSurface *surface = new QGst::Quick::VideoSurface;
    m_declarativeView->rootContext()->setContextProperty(QLatin1String("videoSurface1"), surface);

    QLOG_DEBUG() << "QML Status:" << m_declarativeView->status();
    m_declarativeView->setResizeMode(QQuickView::SizeRootObjectToView);


    m_player = new GStreamerPlayer(m_declarativeView);
    m_player->setVideoSink(surface->videoSink());
    m_declarativeView->rootContext()->setContextProperty(QLatin1String("player"), m_player);
    m_declarativeView->rootContext()->setContextProperty(QLatin1String("container"), this);

    QLOG_DEBUG() << "GStreamer Pipeline String = " << m_pipelineString;
    m_player->initialize(m_pipelineString);
    m_player->play();

    m_declarativeView->setSource(url);

    QVBoxLayout* layout = new QVBoxLayout();
    m_viewcontainer = QWidget::createWindowContainer(m_declarativeView);
    layout->addWidget(m_viewcontainer);
    setLayout(layout);

    setActiveUAS(UASManager::instance()->getActiveUAS());
}

void PrimaryFlightDisplayQML::InitializeDisplay()
{
    ResetDisplay();

    QString qml = QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml";
    QUrl url = QUrl::fromLocalFile(qml);
    QLOG_DEBUG() << url;
    if (!QFile::exists(qml))
    {
        QMessageBox::information(0,"Error", "" + qml + " not found. Please reinstall the application and try again");
        exit(-1);
    }
    m_declarativeView = new QQuickView();
    m_declarativeView->engine()->addImportPath("qml/"); //For local or win32 builds
    m_declarativeView->engine()->addImportPath(QGC::shareDirectory() +"/qml"); //For installed linux builds
    m_declarativeView->engine()->addImportPath("qml/quick2"); //For local or win32 builds
    m_declarativeView->rootContext()->setContextProperty(QLatin1String("container"), this);
    m_declarativeView->setSource(url);

    QLOG_DEBUG() << "QML Status:" << m_declarativeView->status();
    m_declarativeView->setResizeMode(QQuickView::SizeRootObjectToView);
    QVBoxLayout* layout = new QVBoxLayout();
    m_viewcontainer = QWidget::createWindowContainer(m_declarativeView);
    layout->addWidget(m_viewcontainer);
    setLayout(layout);
    show();

    setActiveUAS(UASManager::instance()->getActiveUAS());
}

void PrimaryFlightDisplayQML::enableVideo(bool enabled)
{
    if (m_enableGStreamer)
    {
        if (enabled) InitializeDisplayWithVideo();
        else InitializeDisplay();
    }
}

void PrimaryFlightDisplayQML::setTopMostMode(bool value) {

    m_topMostMode = value;

    if (!value && s_primaryFlightDisplayDialog != NULL) {

        if (m_showToolAction)
        {
            m_showToolAction->setChecked(false);
            m_showToolAction->trigger();
        }

        emit topMostModeChanged();
    }

    if (value && s_primaryFlightDisplayDialog == NULL) {

        // Hide dock version from user
        if (m_showToolAction)
        {
            m_showToolAction->setChecked(true);
            m_showToolAction->trigger();
        }

        // Create top most dialog
        s_primaryFlightDisplayDialog = new QDialog(NULL, Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint);
        s_primaryFlightDisplayDialog->resize(640,480);
        PrimaryFlightDisplayQML *child = new PrimaryFlightDisplayQML();
        child->setShowToolAction(m_showToolAction);
        s_primaryFlightDisplayDialog->setModal(false);
        s_primaryFlightDisplayDialog->setAttribute(Qt::WA_DeleteOnClose);
        s_primaryFlightDisplayDialog->setWindowTitle(tr("Primary Flight Display"));

        s_primaryFlightDisplayDialog->setLayoutDirection (Qt::LeftToRight);
        QBoxLayout *mainDialogLayout = new QBoxLayout(QBoxLayout::LeftToRight);
        mainDialogLayout->addWidget(child);
        mainDialogLayout->setMargin (0);

        s_primaryFlightDisplayDialog->setLayout(mainDialogLayout);
        mainDialogLayout->update();
        mainDialogLayout->activate();

        s_primaryFlightDisplayDialog->setObjectName(child->objectName());
        s_primaryFlightDisplayDialog->setMinimumHeight(320);
        s_primaryFlightDisplayDialog->setMinimumWidth(240);

        s_primaryFlightDisplayDialog->show();

        connect(child, SIGNAL(topMostModeChanged()), this, SLOT(onTopMostModeChanged()));

        child->show();

        emit topMostModeChanged();
    }
}

void PrimaryFlightDisplayQML::ResetDisplay()
{
    // Removes the current engine and widgets
    if (m_player)
    {
        delete m_player;
        m_player = NULL;
    }

    if (m_declarativeView)
    {
        m_declarativeView->hide();
        m_declarativeView->close();
        m_declarativeView->rootContext()->setContextProperty(QLatin1String("videoSurface1"), 0);
        m_declarativeView->engine()->clearComponentCache();
        m_declarativeView = NULL;
    }

    if (m_viewcontainer)
    {
        layout()->removeWidget(m_viewcontainer);
        m_viewcontainer->close();
        m_viewcontainer = NULL;
        delete layout();
    }
}

void PrimaryFlightDisplayQML::setVideoEnabled(bool value) {
    this->m_videoEnabled = value;
    this->enableVideo(value);
    emit videoEnabledChanged();
}

void PrimaryFlightDisplayQML::setFullScreenMode(bool value) {
    this->m_fullScreenMode = value;
    QWidget *p = dynamic_cast<QWidget*>(this->parent());
    if (!value &&  p->isFullScreen()) {
        p->showNormal();
        emit fullScreenModeChanged();
    }

    if (value && !p->isFullScreen()) {
        p->showFullScreen();
        emit fullScreenModeChanged();
    }
}

bool PrimaryFlightDisplayQML::isFullScreenMode() const {
    QWidget *p = dynamic_cast<QWidget*>(this->parent());
    return p->isFullScreen();
}

void PrimaryFlightDisplayQML::onTopMostModeChanged()
{
    // Will delete itself
    if (s_primaryFlightDisplayDialog)
    {
        s_primaryFlightDisplayDialog->close();
        s_primaryFlightDisplayDialog = NULL;
        m_topMostMode = false;
    }

    emit topMostModeChanged();
}

void PrimaryFlightDisplayQML::hideEvent(QHideEvent *event)
{
    if (m_videoEnabled) setVideoEnabled(false);

    QWidget::hideEvent(event);
}
