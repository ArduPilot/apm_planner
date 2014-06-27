#ifndef UASRAWSTATUSVIEW_H
#define UASRAWSTATUSVIEW_H

#include <QWidget>
#include "MAVLinkDecoder.h"
#include "ui_UASRawStatusView.h"
#include "UASInterface.h"
class UASRawStatusView : public QWidget
{
    Q_OBJECT
    
public:
    explicit UASRawStatusView(QWidget *parent = 0);
    ~UASRawStatusView();
    void addSource(MAVLinkDecoder *decoder);
private slots:
    void updateTableTimerTick();
    void valueChanged(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant value, const quint64 msec);
    void activeUASSet(UASInterface* uas);
protected:
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
private:
    UASInterface *m_uas;
    QMap<QString,double> valueMap;
    QMap<QString,QTableWidgetItem*> nameToUpdateWidgetMap;
    Ui::UASRawStatusView ui;
    QTimer *m_updateTimer; //This time triggers a reorganization of the cells, for when new cells are added
    bool m_tableDirty;
};

#endif // UASRAWSTATUSVIEW_H
