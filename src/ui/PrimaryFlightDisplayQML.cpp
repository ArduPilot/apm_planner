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

#include "logging.h"
#include "PrimaryFlightDisplayQML.h"
#include "configuration.h"
#include "UASManager.h"
#include "LinkManager.h"

#include <QVBoxLayout>
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlEngine>
#include <QMessageBox>


PrimaryFlightDisplayQML::PrimaryFlightDisplayQML(QWidget *parent) : QWidget(parent)
{
    QUrl url = QUrl::fromLocalFile(QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml");
    QLOG_DEBUG() << url;
    if (!QFile::exists(QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml"))
    {
        QMessageBox::information(nullptr, "Error", QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml not found. Please reinstall the application and try again");
        exit(-1);
    }
    m_ptrDeclarativeView.reset(new QQuickView());
    m_ptrDeclarativeView->engine()->addImportPath("qml/"); //For local or win32 builds
    m_ptrDeclarativeView->engine()->addImportPath(QGC::shareDirectory() +"/qml"); //For installed linux builds
    m_ptrDeclarativeView->setSource(url);
    QSurfaceFormat format = m_ptrDeclarativeView->format();
    format.setSamples(4);
    m_ptrDeclarativeView->setFormat(format);

    QLOG_DEBUG() << "PrimaryFlightDisplay QML Status:" << m_ptrDeclarativeView->status();
    m_ptrDeclarativeView->setResizeMode(QQuickView::SizeRootObjectToView);
    auto *p_layout = new QVBoxLayout();
    QWidget *viewcontainer = QWidget::createWindowContainer(m_ptrDeclarativeView.get());
    p_layout->addWidget(viewcontainer);
    setLayout(p_layout);
    setContentsMargins(0,0,0,0);
    show();

    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this,
            SLOT(setActiveUAS(UASInterface*)), Qt::UniqueConnection);
    setActiveUAS(UASManager::instance()->getActiveUAS());

}


void PrimaryFlightDisplayQML::setActiveUAS(UASInterface *uas)
{
    if (mp_uasInterface != nullptr)
    {
        disconnect(mp_uasInterface,SIGNAL(textMessageReceived(int,int,int,QString)),this,SLOT(uasTextMessage(int,int,int,QString)));
    }
    mp_uasInterface = uas;

    if (mp_uasInterface != nullptr) {
        connect(uas,SIGNAL(textMessageReceived(int,int,int,QString)),
                this,SLOT(uasTextMessage(int,int,int,QString)));
        VehicleOverview* vehicleView = LinkManager::instance()->getUasObject(uas->getUASID())->getVehicleOverview();
        RelPositionOverview* relView = LinkManager::instance()->getUasObject(uas->getUASID())->getRelPositionOverview();
        AbsPositionOverview* absView = LinkManager::instance()->getUasObject(uas->getUASID())->getAbsPositionOverview();
        if (vehicleView != nullptr)
        {
            m_ptrDeclarativeView->rootContext()->setContextProperty("vehicleoverview", vehicleView);
        }
        else
        {
            QLOG_ERROR() << "PrimaryFlightDisplayQML::setActiveUAS() Invalid vehicleoverview!";
        }
        if (relView != nullptr)
        {
            m_ptrDeclarativeView->rootContext()->setContextProperty("relpositionoverview", relView);
        }
        else
        {
            QLOG_ERROR() << "PrimaryFlightDisplayQML::setActiveUAS() Invalid relpositionoverview!";
        }
        if (absView != nullptr)
        {
            m_ptrDeclarativeView->rootContext()->setContextProperty("abspositionoverview", absView);
        }
        else
        {
            QLOG_ERROR() << "PrimaryFlightDisplayQML::setActiveUAS() Invalid abspositionoverview!";
        }

        QMetaObject::invokeMethod(m_ptrDeclarativeView->rootObject(),"activeUasSet");
    }
}

void PrimaryFlightDisplayQML::uasTextMessage(int uasid, int componentid, int severity, const QString &text)
{
    Q_UNUSED(uasid);
    Q_UNUSED(componentid);
    if (text.contains("PreArm") || severity <= MAV_SEVERITY_CRITICAL)
    {
        QObject *root = m_ptrDeclarativeView->rootObject();
        root->setProperty("statusMessage", text);
        root->setProperty("showStatusMessage", true);
        root->setProperty("statusMessageColor", "red");
    } else if (severity <= MAV_SEVERITY_INFO ){
        QObject *root = m_ptrDeclarativeView->rootObject();
        root->setProperty("statusMessage", text);
        root->setProperty("showStatusMessage", true);
        root->setProperty("statusMessageColor", "darkgreen");
    }

}
