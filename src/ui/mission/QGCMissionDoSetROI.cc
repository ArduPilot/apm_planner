#include "QGCMissionDoSetROI.h"
#include "ui_QGCMissionDoSetROI.h"
#include "WaypointEditableView.h"

QGCMissionDoSetROI::QGCMissionDoSetROI(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionDoSetROI)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param5SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam5(double)));
    connect(this->ui->param6SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam6(double)));
    connect(this->ui->param7SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam7(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param5Broadcast(double)),this->ui->param5SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param6Broadcast(double)),this->ui->param6SpinBox,SLOT(setValue(double)));
    connect(WEV,SIGNAL(param7Broadcast(double)),this->ui->param7SpinBox,SLOT(setValue(double)));
}

QGCMissionDoSetROI::~QGCMissionDoSetROI()
{
    delete ui;
}
