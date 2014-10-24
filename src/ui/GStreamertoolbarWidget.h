#ifndef GSTREAMERTOOLBARWIDGET_H
#define GSTREAMERTOOLBARWIDGET_H

#include <QWidget>
#include "GStreamerPlayer.h"

// Helper for adjusting gstreamer plugin settings
class GStreamerToolBarWidget : public QWidget
{
    Q_OBJECT
public:
    GStreamerToolBarWidget(QPoint pos, GStreamerPlayer *player, QWidget *parent = NULL) : QWidget(parent), m_player(player)
    {
        setWindowFlags(windowFlags() | Qt::Popup);
        createLayout();
        int newYPos = pos.y() + 100;
        move(pos.x(), newYPos);
    }

    void createLayout();

private slots:
    void brightnessValueChanged(int value);
    void contrastValueChanged(int value);
    void hueValueChanged(int value);
    void satValueChanged(int value);

private:
    GStreamerPlayer *m_player;

};

#endif // GSTREAMERTOOLBARWIDGET_H
