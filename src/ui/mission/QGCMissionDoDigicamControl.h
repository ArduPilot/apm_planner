#ifndef QGCMISSIONDODIGICAMCONTROL_H
#define QGCMISSIONDODIGICAMCONTROL_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoDigicamControl;
}

class QGCMissionDoDigicamControl : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoDigicamControl(WaypointEditableView* WEV);
    ~QGCMissionDoDigicamControl();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoDigicamControl *ui;
};

#endif // QGCMISSIONDODIGICAMCONTROL_H
