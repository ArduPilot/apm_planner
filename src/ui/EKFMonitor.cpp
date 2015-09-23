/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2015 Bill Bonney <billbonney@communistech.com>

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
#include "EKFMonitor.h"

#include "configuration.h"
#include "MainWindow.h"

#include <QVBoxLayout>
#include <QQmlContext>
#include <QQuickItem>
#include <QQmlEngine>

EKFMonitor::EKFMonitor(QWidget *parent) :
    QWidget(parent),
    m_declarativeView(NULL),
    m_uasInterface(NULL)
{
    QUrl url = QUrl::fromLocalFile(QGC::shareDirectory() + "/qml/EKFMonitor.qml");
    QLOG_DEBUG() << url;
    if (!QFile::exists(QGC::shareDirectory() + "/qml/EKFMonitor.qml"))
    {
        QMessageBox::information(0,"Error", "" + QGC::shareDirectory() + "/qml/EKFMonitor.qml" + " not found. Please reinstall the application and try again");
        exit(-1);
    }
    m_declarativeView = new QQuickView();
    m_declarativeView->engine()->addImportPath("qml/"); //For local or win32 builds
    m_declarativeView->engine()->addImportPath(QGC::shareDirectory() +"/qml"); //For installed linux builds
    m_declarativeView->setSource(url);
    QSurfaceFormat format = m_declarativeView->format();
    format.setSamples(16);
    m_declarativeView->setFormat(format);

    QLOG_DEBUG() << "QML Status:" << m_declarativeView->status();
    m_declarativeView->setResizeMode(QQuickView::SizeRootObjectToView);
    QVBoxLayout* layout = new QVBoxLayout();
    QWidget *viewcontainer = QWidget::createWindowContainer(m_declarativeView);
    layout->addWidget(viewcontainer);
    setLayout(layout);
    setContentsMargins(0,0,0,0);
    show();

    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this,
            SLOT(setActiveUAS(UASInterface*)), Qt::UniqueConnection);
    setActiveUAS(UASManager::instance()->getActiveUAS());

}

EKFMonitor::~EKFMonitor()
{
    delete m_declarativeView;
}

void EKFMonitor::setActiveUAS(UASInterface *uas)
{
    m_uasInterface = uas;

    if (m_uasInterface) {
        connect(uas,SIGNAL(),this,SLOT(uasTextMessage(int,int,int,QString)));

        VehicleOverview* vehicleOverview = LinkManager::instance()->getUasObject(uas->getUASID())->getVehicleOverview();
        if (vehicleOverview) {
            m_declarativeView->rootContext()->setContextProperty("vehicleOverview", vehicleOverview);
        } else {
            QLOG_ERROR() << "EKFMonitor::setActiveUAS() Invalid vehicleOverview!";
        }

        QMetaObject::invokeMethod(m_declarativeView->rootObject(),"activeUasSet");
    }
}
