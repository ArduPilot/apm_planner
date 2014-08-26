#ifndef QGCMISSIONNAVSPLINEWAYPOINT_H
#define QGCMISSIONNAVSPLINEWAYPOINT_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionNavSplineWaypoint;
}

class QGCMissionNavSplineWaypoint : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionNavSplineWaypoint(WaypointEditableView* WEV);
    ~QGCMissionNavSplineWaypoint();

public slots:
    void updateFrame(MAV_FRAME);

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionNavSplineWaypoint *ui;
};

#endif // QGCMISSIONNAVSPLINEWAYPOINT_H
