#ifndef APMTOOLBAR_H
#define APMTOOLBAR_H

#include <QAction>
#include <QDeclarativeView>
#include "UASInterface.h"

class LinkInterface;

class APMToolBar : public QDeclarativeView
{
    Q_OBJECT
public:
    explicit APMToolBar(QWidget *parent = 0);
    ~APMToolBar();

    void setFlightViewAction(QAction *action);
    void setFlightPlanViewAction(QAction *action);
    void setInitialSetupViewAction(QAction *action);
    void setConfigTuningViewAction(QAction *action);
    void setSimulationViewAction(QAction *action);
    void setTerminalViewAction(QAction *action);
    void setConnectMAVAction(QAction *action);
    
signals:
    void triggerFlightView();
    void triggerFlightPlanView();
    void triggerInitialSetupView();
    void triggerConfigTuningView();
    void triggerSimulationView();
    void triggerTerminalView();

    void MAVConnected(bool connected);

public slots:
    void selectFlightView();
    void selectFlightPlanView();
    void selectInitialSetupView();
    void selectConfigTuningView();
    void selectSimulationView();
    void selectTerminalView();

    void connectMAV();
    void showConnectionDialog();
    void setConnection(bool connection);

    void activeUasSet(UASInterface *uas);
    void armingChanged(int sysId, QString armingState);
    void armingChanged(bool armed);

    void updateLinkDisplay(LinkInterface *newLink);

private:
    UASInterface *m_uas;
};

#endif // APMTOOLBAR_H
