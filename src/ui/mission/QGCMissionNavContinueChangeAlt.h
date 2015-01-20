#ifndef QGCMissionNavContinueChangeAlt_H
#define QGCMissionNavContinueChangeAlt_H

#include <QWidget>
#include "WaypointEditableView.h"

namespace Ui {
    class QGCMissionNavContinueChangeAlt;
}

class QGCMissionNavContinueChangeAlt : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMissionNavContinueChangeAlt(WaypointEditableView* WEV);
    ~QGCMissionNavContinueChangeAlt();

protected:
    WaypointEditableView* WEV;

private:
    Ui::QGCMissionNavContinueChangeAlt *ui;
};

#endif // QGCMissionNavContinueChangeAlt_H
