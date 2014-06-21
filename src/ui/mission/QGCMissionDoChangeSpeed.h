#ifndef QGCMISSIONDOCHANGESPEED_H
#define QGCMISSIONDOCHANGESPEED_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoChangeSpeed;
}

class QGCMissionDoChangeSpeed : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoChangeSpeed(WaypointEditableView* WEV);
    ~QGCMissionDoChangeSpeed();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoChangeSpeed *ui;
};

#endif // QGCMISSIONDOCHANGESPEED_H
