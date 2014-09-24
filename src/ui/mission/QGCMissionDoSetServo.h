#ifndef QGCMISSIONDOSETSERVO_H
#define QGCMISSIONDOSETSERVO_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoSetServo;
}

class QGCMissionDoSetServo: public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoSetServo(WaypointEditableView* WEV);
    ~QGCMissionDoSetServo();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoSetServo *ui;
};

#endif // QGCMISSIONDOSETSERVO_H
