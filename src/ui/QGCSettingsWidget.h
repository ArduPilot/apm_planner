#ifndef QGCSETTINGSWIDGET_H
#define QGCSETTINGSWIDGET_H

#include "UASInterface.h"
#include <QDialog>

namespace Ui
{
class QGCSettingsWidget;
}

class QGCSettingsWidget : public QDialog
{
    Q_OBJECT

public:
    QGCSettingsWidget(QWidget *parent = 0, Qt::WindowFlags flags = Qt::Sheet);
    ~QGCSettingsWidget();
protected:
    void showEvent(QShowEvent *evt);
private slots:
    void setLogDir();
    void setMAVLinkLogDir();
    void setParamDir();
    void setAppDataDir();
    void setMissionsDir();
    void ratesChanged();
    void setBetaRelease(bool state);
    void setHideDonateButton(bool state);
    void setMavlinkSeverityCompatibility(bool state);

    void setActiveUAS(UASInterface *uas);

private:
    void setDataRateLineEdits();

private:
    Ui::QGCSettingsWidget *ui;
    bool m_init;
    UASInterface *m_uas;
};

#endif // QGCSETTINGSWIDGET_H
