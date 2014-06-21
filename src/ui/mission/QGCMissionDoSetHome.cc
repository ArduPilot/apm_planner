#include "QGCMissionDoSetHome.h"
#include "ui_QGCMissionDoSetHome.h"
#include "WaypointEditableView.h"

QGCMissionDoSetHome::QGCMissionDoSetHome(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionDoSetHome)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param1SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam1(double)));
    connect(this->ui->param5SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam5(double)));
    connect(this->ui->param6SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam6(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param1Broadcast(double)),this->ui->param1SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param5Broadcast(double)),this->ui->param5SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param6Broadcast(double)),this->ui->param6SpinBox,SLOT(setValue(double)));
}

QGCMissionDoSetHome::~QGCMissionDoSetHome()
{
    delete ui;
}
