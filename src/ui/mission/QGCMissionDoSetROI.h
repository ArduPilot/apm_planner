#ifndef QGCMISSIONDOSETROI_H
#define QGCMISSIONDOSETROI_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoSetROI;
}

class QGCMissionDoSetROI : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoSetROI(WaypointEditableView* WEV);
    ~QGCMissionDoSetROI();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoSetROI *ui;
};

#endif // QGCMISSIONDOSETROI_H
