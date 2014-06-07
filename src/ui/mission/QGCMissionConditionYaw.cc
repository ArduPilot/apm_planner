#include "QGCMissionConditionYaw.h"
#include "ui_QGCMissionConditionYaw.h"
#include "WaypointEditableView.h"

QGCMissionConditionYaw::QGCMissionConditionYaw(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionConditionYaw)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param1SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam1(double)));
    connect(this->ui->param2SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam2(double)));
    connect(this->ui->param3SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam3(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param1Broadcast(double)),this->ui->param1SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param2Broadcast(double)),this->ui->param2SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param3Broadcast(double)),this->ui->param3SpinBox,SLOT(setValue(double)));
}

QGCMissionConditionYaw::~QGCMissionConditionYaw()
{
    delete ui;
}
