#ifndef QGCMISSIONDOSETREVERSE_H
#define QGCMISSIONDOSETREVERSE_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoSetReverse;
}

class QGCMissionDoSetReverse : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoSetReverse(WaypointEditableView* WEV);
    ~QGCMissionDoSetReverse();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoSetReverse *ui;
};

#endif // QGCMISSIONDOSETREVERSE_H
