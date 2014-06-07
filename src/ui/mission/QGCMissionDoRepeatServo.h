#ifndef QGCMISSIONDOREPEATSERVO_H
#define QGCMISSIONDOREPEATSERVO_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoRepeatServo;
}

class QGCMissionDoRepeatServo : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoRepeatServo(WaypointEditableView* WEV);
    ~QGCMissionDoRepeatServo();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoRepeatServo *ui;
};

#endif // QGCMISSIONDOREPEATSERVO_H
