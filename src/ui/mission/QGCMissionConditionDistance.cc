#include "QGCMissionConditionDistance.h"
#include "ui_QGCMissionConditionDistance.h"
#include "WaypointEditableView.h"

QGCMissionConditionDistance::QGCMissionConditionDistance(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionConditionDistance)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param1SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam1(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param1Broadcast(double)),this->ui->param1SpinBox,SLOT(setValue(double)));
}

QGCMissionConditionDistance::~QGCMissionConditionDistance()
{
    delete ui;
}
