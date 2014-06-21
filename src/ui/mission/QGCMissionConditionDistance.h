#ifndef QGCMISSIONCONDITIONDISTANCE_H
#define QGCMISSIONCONDITIONDISTANCE_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionConditionDistance;
}

class QGCMissionConditionDistance : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionConditionDistance(WaypointEditableView* WEV);
    ~QGCMissionConditionDistance();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionConditionDistance *ui;
};

#endif // QGCMISSIONCONDITIONDISTANCE_H
