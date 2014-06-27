#ifndef QGCMISSIONDOSETRELAY_H
#define QGCMISSIONDOSETRELAY_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoSetRelay;
}

class QGCMissionDoSetRelay : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoSetRelay(WaypointEditableView* WEV);
    ~QGCMissionDoSetRelay();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoSetRelay *ui;
};

#endif // QGCMISSIONDOSETRELAY_H
