#ifndef QGCMISSIONDOSETHOME_H
#define QGCMISSIONDOSETHOME_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionDoSetHome;
}

class QGCMissionDoSetHome : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionDoSetHome(WaypointEditableView* WEV);
    ~QGCMissionDoSetHome();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionDoSetHome *ui;
};

#endif // QGCMISSIONDOSETHOME_H
