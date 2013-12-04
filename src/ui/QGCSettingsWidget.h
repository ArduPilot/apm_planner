#ifndef QGCSETTINGSWIDGET_H
#define QGCSETTINGSWIDGET_H

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

public slots:

private:
    Ui::QGCSettingsWidget *ui;
    bool m_init;
};

#endif // QGCSETTINGSWIDGET_H
