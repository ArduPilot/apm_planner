#include "QGCMissionDoDigicamControl.h"
#include "ui_QGCMissionDoDigicamControl.h"
#include "WaypointEditableView.h"

QGCMissionDoDigicamControl::QGCMissionDoDigicamControl(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionDoDigicamControl)
{
    ui->setupUi(this);
    this->WEV = WEV;
}

QGCMissionDoDigicamControl::~QGCMissionDoDigicamControl()
{
    delete ui;
}
