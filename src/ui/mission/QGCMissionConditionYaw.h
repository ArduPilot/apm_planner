#ifndef QGCMISSIONCONDITIONYAW_H
#define QGCMISSIONCONDITIONYAW_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionConditionYaw;
}

class QGCMissionConditionYaw : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionConditionYaw(WaypointEditableView* WEV);
    ~QGCMissionConditionYaw();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionConditionYaw *ui;
};

#endif // QGCMISSIONCONDITIONYAW_H
