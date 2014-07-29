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
#include <QDeclarativeContext>

#define ToRad(x) (x*0.01745329252)      // *pi/180
#define ToDeg(x) (x*57.2957795131)      // *180/pi

PrimaryFlightDisplayQML::PrimaryFlightDisplayQML(QWidget *parent) :
    QWidget(parent),
//    ui(new Ui::PrimaryFlightDisplayQML),
    m_declarativeView(NULL)
{
//    ui->setupUi(this);
    QUrl url = QUrl::fromLocalFile(QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml");
    QLOG_DEBUG() << url;
    if (!QFile::exists(QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml"))
    {
        QMessageBox::information(0,"Error", "" + QGC::shareDirectory() + "/qml/PrimaryFlightDisplayQML.qml" + " not found. Please reinstall the application and try again");
        exit(-1);
    }
    m_declarativeView = new QDeclarativeView(url);
    QLOG_DEBUG() << "QML Status:" << m_declarativeView->status();
    m_declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_declarativeView);
    setLayout(layout);
    show();

    // Connect with UAS
    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)), this,
            SLOT(setActiveUAS(UASInterface*)), Qt::UniqueConnection);
    setActiveUAS(UASManager::instance()->getActiveUAS());

}

PrimaryFlightDisplayQML::~PrimaryFlightDisplayQML()
{
//    delete ui;
}

void PrimaryFlightDisplayQML::setActiveUAS(UASInterface *uas)
{
    if (m_uasInterface) {
        disconnect(uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)),
                this, SLOT(attitudeChanged(UASInterface*, double, double, double, quint64)));
        disconnect(uas, SIGNAL(altitudeChanged(UASInterface*,double,double,double,quint64)),
                this, SLOT(altitudeChanged(UASInterface*,double,double,double,quint64)));
        disconnect(uas, SIGNAL(speedChanged(UASInterface*,double,double,quint64)),
                this, SLOT(speedChanged(UASInterface*,double,double,quint64)));
        disconnect(uas,SIGNAL(textMessageReceived(int,int,int,QString)),
                this,SLOT(uasTextMessage(int,int,int,QString)));
    }
    m_uasInterface = uas;

    if (m_uasInterface) {
        connect(uas, SIGNAL(attitudeChanged(UASInterface*,double,double,double,quint64)),
                this, SLOT(attitudeChanged(UASInterface*, double, double, double, quint64)));
        connect(uas, SIGNAL(altitudeChanged(UASInterface*,double,double,double,quint64)),
                this, SLOT(altitudeChanged(UASInterface*,double,double,double,quint64)));
        connect(uas, SIGNAL(speedChanged(UASInterface*,double,double,quint64)),
                this, SLOT(speedChanged(UASInterface*,double,double,quint64)));
        connect(uas,SIGNAL(textMessageReceived(int,int,int,QString)),
                this,SLOT(uasTextMessage(int,int,int,QString)));
        VehicleOverview *obj = LinkManager::instance()->getUasObject(uas->getUASID())->getVehicleOverview();
        RelPositionOverview *rel = LinkManager::instance()->getUasObject(uas->getUASID())->getRelPositionOverview();
        m_declarativeView->rootContext()->setContextProperty("vehicleoverview",obj);
        m_declarativeView->rootContext()->setContextProperty("relpositionoverview",rel);
        QMetaObject::invokeMethod(m_declarativeView->rootObject(),"uasObjectCreated");
    }
}

void PrimaryFlightDisplayQML::attitudeChanged(UASInterface *uas, double roll, double pitch, double yaw, quint64 usec)
{
    Q_UNUSED(uas);
    Q_UNUSED(usec);
    QObject *root = m_declarativeView->rootObject();
    root->setProperty("roll", ToDeg(roll));
    root->setProperty("pitch", ToDeg(pitch));
    double heading = ToDeg(yaw);
    if (heading<0)
        heading+=360;
    root->setProperty("heading", heading);
}

void PrimaryFlightDisplayQML::altitudeChanged(UASInterface *uas, double altitudeAMSL, double altitudeRelative,
                                              double climbRate, quint64 usec)
{
    Q_UNUSED(uas);
    Q_UNUSED(usec);
    QObject *root = m_declarativeView->rootObject();
    root->setProperty("altitudeRelative", altitudeRelative);
    root->setProperty("altitudeAMSL", altitudeAMSL);
    root->setProperty("climbRate", climbRate);
}

void PrimaryFlightDisplayQML::speedChanged(UASInterface *uas, double groundSpeed, double airSpeed, quint64 usec)
{
    Q_UNUSED(uas);
    Q_UNUSED(usec);
    QObject *root = m_declarativeView->rootObject();
    root->setProperty("airspeed", airSpeed);
    root->setProperty("groundspeed", groundSpeed);
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
