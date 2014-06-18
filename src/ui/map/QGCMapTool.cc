#include "QsLog.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "UAS.h"
#include "QGCMapTool.h"
#include "ui_QGCMapTool.h"

#include <QAction>
#include <QMenu>

const static int MapToolZoomFactor = 10; // This may need to be different for win/linux/mac

QGCMapTool::QGCMapTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCMapTool),
    m_uasInterface(NULL)
{
    ui->setupUi(this);

    // Connect map and toolbar
    ui->toolBar->setMap(ui->map);
    // Connect zoom slider and map
    ui->zoomSlider->setMinimum(ui->map->MinZoom() * MapToolZoomFactor);
    ui->zoomSlider->setMaximum(ui->map->MaxZoom() * MapToolZoomFactor);
    setZoom(ui->map->ZoomReal());

    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setMapZoom(int)));
    connect(ui->map, SIGNAL(zoomChanged(int)), this, SLOT(setZoom(int)));

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));

    if (UASManager::instance()->getActiveUAS())
    {
        activeUASSet(UASManager::instance()->getActiveUAS());
    }
}

void QGCMapTool::setMapZoom(int zoom)
{
    ui->map->SetZoom(zoom/MapToolZoomFactor);
}

void QGCMapTool::setZoom(int zoom)
{
    ui->zoomSlider->setValue(zoom * MapToolZoomFactor);
    ui->zoomLabel->setText("ZOOM: " + QString::number(zoom));
}

void QGCMapTool::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit visibilityChanged(true);
}

void QGCMapTool::hideEvent(QHideEvent* event)
{
    QWidget::hideEvent(event);
    emit visibilityChanged(false);
}

QGCMapTool::~QGCMapTool()
{
    delete ui;
}

void QGCMapTool::activeUASSet(UASInterface *uasInterface)
{
    QLOG_INFO() << "QGCMapTool::activeUASSet";
    if (uasInterface == NULL) {
        QLOG_ERROR() << "uas object NULL";
        return;
    }
    UAS* uas = NULL;
    if (m_uasInterface){
        uas = qobject_cast<UAS*>(m_uasInterface);
        disconnect(m_uasInterface, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)),
                this, SLOT(globalPositionUpdate(UASInterface*,double,double,double,quint64)));
        disconnect(uas, SIGNAL(gpsHdopChanged(double,QString)), this, SLOT(gpsHdopChanged(int,QString)));
        disconnect(uas, SIGNAL(gpsFixChanged(int,QString)),this, SLOT(gpsFixChanged(int,QString)));
        disconnect(uas, SIGNAL(satelliteCountChanged(int,QString)),
                this, SLOT(satelliteCountChanged(double,QString)));
    }
    m_uasInterface = uasInterface;
    uas = qobject_cast<UAS*>(m_uasInterface);
    connect(m_uasInterface, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)),
            this, SLOT(globalPositionUpdate()));
    connect(uas, SIGNAL(gpsHdopChanged(double,QString)), this, SLOT(gpsHdopChanged(double,QString)));
    connect(uas, SIGNAL(gpsFixChanged(int,QString)), SLOT(gpsFixChanged(int,QString)));
    connect(uas, SIGNAL(satelliteCountChanged(int,QString)),
            this, SLOT(satelliteCountChanged(int,QString)));


}

void QGCMapTool::globalPositionUpdate()
{
    ui->latitudeLabel->setText(tr("LAT: %1").arg(m_uasInterface->getLatitude()));
    ui->longitudeLabel->setText(tr("LON: %1").arg(m_uasInterface->getLongitude()));
}

void QGCMapTool::gpsHdopChanged(double value, const QString &)
{
    QString stringHdop = QString::number(value,'g',2);
    ui->hdopLabel->setText(tr("HDOP: %1").arg(stringHdop));
}

void QGCMapTool::gpsFixChanged(int, const QString &)
{
    UAS* uas = dynamic_cast<UAS*>(m_uasInterface);
    ui->fixLabel->setText(tr("FIX: %1").arg(uas->getGpsFixString()));
}

void QGCMapTool::satelliteCountChanged(int value, const QString &)
{
    ui->satsLabel->setText(tr("SATS: %1").arg(value));
}
