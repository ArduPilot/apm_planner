#include "QGCMissionNavContinueChangeAlt.h"
#include "ui_QGCMissionNavContinueChangeAlt.h"
#include "WaypointEditableView.h"

QGCMissionNavContinueChangeAlt::QGCMissionNavContinueChangeAlt(WaypointEditableView* WEV) :
    QWidget(WEV),
    ui(new Ui::QGCMissionNavContinueChangeAlt)
{
    ui->setupUi(this);
    this->WEV = WEV;
    //Using UI to change WP:
    connect(this->ui->param7SpinBox, SIGNAL(valueChanged(double)),WEV,SLOT(changedParam7(double)));

    //Reading WP to update UI:
    connect(WEV,SIGNAL(param7Broadcast(double)),this->ui->param7SpinBox,SLOT(setValue(double)));
}

QGCMissionNavContinueChangeAlt::~QGCMissionNavContinueChangeAlt()
{
    delete ui;
}
