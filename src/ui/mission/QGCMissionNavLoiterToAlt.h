#ifndef QGCMISSIONNAVLOITERTOALT_H
#define QGCMISSIONNAVLOITERTOALT_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionNavLoiterToAlt;
}

class QGCMissionNavLoiterToAlt : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionNavLoiterToAlt(WaypointEditableView* WEV);
    ~QGCMissionNavLoiterToAlt();

public slots:
    void updateFrame(MAV_FRAME);

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionNavLoiterToAlt *ui;
};

#endif // QGCMISSIONNLOITERTOALT_H
