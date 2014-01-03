#ifndef UASACTIONSWIDGET_H
#define UASACTIONSWIDGET_H

#include <QWidget>
#include "ui_UASActionsWidget.h"
#include <UASManager.h>
#include <UASInterface.h>
class UASActionsWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit UASActionsWidget(QWidget *parent = 0);
    ~UASActionsWidget();

    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void activeUASSet(UASInterface *uas);
    void uasConnected();
    void uasDisconnected();

    void armButtonClicked();
    void armingChanged(bool state);
    void currentWaypointChanged(quint16 wpid);
    void updateWaypointList();
    void goToWaypointClicked();
    void changeAltitudeClicked();
    void changeSpeedClicked();
    void setMode();   //[TODO] create a new ardupilot mode types
    void setShortcutMode();
    void setAction();
    void setRTLMode();

    void parameterChanged(int uas, int component, int parameterCount,
                          int parameterId, QString parameterName, QVariant value);
    void heartbeatReceived(UASInterface*);

private:
    void setupApmCopterModes();
    void setupApmPlaneModes();
    void setupApmRoverModes();

    void sendApmPlaneCommand(MAV_CMD command);
    void sendApmCopterCommand(MAV_CMD command);
    void sendApmRoverCommand(MAV_CMD command);

    bool activeUas();

    int preFlightWarningBox();
    int modeChangeWarningBox(const QString& modeString);

    void configureModeButtonEnableDisable();

    void saveApmSettings();
    void loadApmSettings();

private:
    Ui::UASActionsWidget ui;
    UAS *m_uas;
    quint16 m_last_wpid;
};

#endif // UASACTIONSWIDGET_H
