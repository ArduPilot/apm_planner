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
    ui(new Ui::QGCMapTool)
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
        disconnect(&m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()));
    }
    m_uas = uas;
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateOverlay()));
    m_timer.start();
}

void QGCMapTool::updateOverlay()
{
    UAS* uas = dynamic_cast<UAS*>(m_uas);
    ui->latitudeLabel->setText(tr("LAT: %1").arg(m_uas->getLatitude()));
    ui->longitudeLabel->setText(tr("LON: %1").arg(m_uas->getLongitude()));
    ui->satsLabel->setText(tr("SATS: %1").arg(uas->getSatelliteCount()));

    double hdop = uas->getGpsHdop();
    QString stringHdop = QString::number(hdop,'g',2);
    ui->hdopLabel->setText(tr("HDOP: %1").arg(stringHdop));
}
