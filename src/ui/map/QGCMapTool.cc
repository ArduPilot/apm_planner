#include "QsLog.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "UAS.h"
#include "QGCMapTool.h"
#include "ui_QGCMapTool.h"

#include <QAction>
#include <QMenu>

QGCMapTool::QGCMapTool(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCMapTool),
    m_uas(NULL)
{
    ui->setupUi(this);

    // Connect map and toolbar
    ui->toolBar->setMap(ui->map);
    // Connect zoom slider and map
    ui->zoomSlider->setMinimum(ui->map->MinZoom());
    ui->zoomSlider->setMaximum(ui->map->MaxZoom());
    ui->zoomSlider->setValue(ui->map->ZoomReal());
    connect(ui->zoomSlider, SIGNAL(valueChanged(int)), ui->map, SLOT(SetZoom(int)));
    connect(ui->map, SIGNAL(zoomChanged(int)), this, SLOT(setZoom(int)));

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));

    if (UASManager::instance()->getActiveUAS())
    {
        activeUASSet(UASManager::instance()->getActiveUAS());
    }
}

void QGCMapTool::setZoom(int zoom)
{
    if (ui->zoomSlider->value() != zoom)
    {
        ui->zoomSlider->setValue(zoom);
    }
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

void QGCMapTool::activeUASSet(UASInterface *uas)
{
    QLOG_INFO() << "QGCMapTool::activeUASSet";
    if (uas == NULL) {
        QLOG_ERROR() << "uas object NULL";
        return;
    }

    if (m_uas){
        disconnect(m_uas, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)),
                this, SLOT(globalPositionUpdate(UASInterface*,double,double,double,quint64)));
        disconnect(m_uas, SIGNAL(gpsRawChanged(UASInterface*,double,double,double,double,int,quint64)),
                this, SLOT(gpsRawUpate()));
    }
    m_uas = uas;

    connect(m_uas, SIGNAL(globalPositionChanged(UASInterface*,double,double,double,quint64)),
            this, SLOT(globalPositionUpdate()));
    connect(m_uas, SIGNAL(gpsRawChanged(UASInterface*,double,double,double,double,int,quint64)),
            this, SLOT(gpsRawUpdate()));
}

void QGCMapTool::globalPositionUpdate()
{
    ui->latitudeLabel->setText(tr("LAT: %1").arg(m_uas->getLatitude()));
    ui->longitudeLabel->setText(tr("LON: %1").arg(m_uas->getLongitude()));
}

void QGCMapTool::gpsRawUpdate()
{
    UAS* uas = dynamic_cast<UAS*>(m_uas);
    ui->satsLabel->setText(tr("SATS: %1").arg(uas->getSatelliteCount()));

    ui->fixLabel->setText(tr("FIX: %1").arg(uas->getGpsFixString()));

    double hdop = uas->getGpsHdop();
    QString stringHdop = QString::number(hdop,'g',2);
    ui->hdopLabel->setText(tr("HDOP: %1").arg(stringHdop));
}

