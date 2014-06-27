#ifndef QGCMISSIONDOMOUNTCONTROL_H
#define QGCMISSIONDOMOUNTCONTROL_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoMountControl;
}

class QGCMissionDoMountControl : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoMountControl(WaypointEditableView* WEV);
    ~QGCMissionDoMountControl();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoMountControl *ui;
};

#endif // QGCMISSIONDOMOUNTCONTROL_H
