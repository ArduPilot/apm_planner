#include "QGCMissionDoSetServo.h"
#include "ui_QGCMissionDoSetServo.h"
#include "WaypointEditableView.h"

QGCMissionDoSetServo::QGCMissionDoSetServo(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionDoSetServo)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param1SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam1(double)));
    connect(this->ui->param2SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam2(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param1Broadcast(double)),this->ui->param1SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param2Broadcast(double)),this->ui->param2SpinBox,SLOT(setValue(double)));
}

QGCMissionDoSetServo::~QGCMissionDoSetServo()
{
    delete ui;
}
