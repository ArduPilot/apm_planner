#ifndef QGCMISSIONDOREPEATRELAY_H
#define QGCMISSIONDOREPEATRELAY_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoRepeatRelay;
}

class QGCMissionDoRepeatRelay : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoRepeatRelay(WaypointEditableView* WEV);
    ~QGCMissionDoRepeatRelay();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoRepeatRelay *ui;
};

#endif // QGCMISSIONDOREPEATRELAY_H
