#ifndef QGCMAPTOOL_H
#define QGCMAPTOOL_H

class UASInterface;
#include <QWidget>
#include <QMenu>
#include <QTimer>

namespace Ui {
    class QGCMapTool;
}

class QGCMapTool : public QWidget
{
    Q_OBJECT

public:
    explicit QGCMapTool(QWidget *parent = 0);
    ~QGCMapTool();

public slots:
    void setMapZoom(int zoom);
    /** @brief Update slider zoom from map change */
    void setZoom(int zoom);

signals:
    void visibilityChanged(bool visible);

private slots:
    void activeUASSet(UASInterface *uasInterface);
    void globalPositionUpdate();
    void gpsHdopChanged(double value, const QString&);
    void gpsFixChanged(int, const QString&);
    void satelliteCountChanged(int value, const QString&);

private:
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);

private:
    Ui::QGCMapTool *ui;

    UASInterface* m_uasInterface;
};

#endif // QGCMAPTOOL_H
