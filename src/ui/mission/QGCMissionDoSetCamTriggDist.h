#ifndef QGCMISSIONDOSETCAMTRIGGDIST_H
#define QGCMISSIONDOSETCAMTRIGGDIST_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoSetCamTriggDist;
}

class QGCMissionDoSetCamTriggDist : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoSetCamTriggDist(WaypointEditableView* WEV);
    ~QGCMissionDoSetCamTriggDist();    

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoSetCamTriggDist *ui;
};

#endif // QGCMISSIONDOSETCAMTRIGGDIST_H
