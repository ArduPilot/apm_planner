#include "QGCMissionDoSetReverse.h"
#include "ui_QGCMissionDoSetReverse.h"
#include "WaypointEditableView.h"

QGCMissionDoSetReverse::QGCMissionDoSetReverse(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionDoSetReverse)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param1SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam1(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param1Broadcast(double)),this->ui->param1SpinBox,SLOT(setValue(double)));
}

QGCMissionDoSetReverse::~QGCMissionDoSetReverse()
{
    delete ui;
}
