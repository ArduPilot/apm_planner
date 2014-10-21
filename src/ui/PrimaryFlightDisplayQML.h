/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 Bill Bonney <billbonney@communistech.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#ifndef PRIMARYFLIGHTDISPLAYQML_H
#define PRIMARYFLIGHTDISPLAYQML_H

#include <UASInterface.h>
#include <QWidget>
#include <QtQuick\QQuickView>
#include "GStreamerPlayer.h"


namespace Ui {
class PrimaryFlightDisplayQML;
}

class PrimaryFlightDisplayQML : public QWidget
{
    Q_OBJECT

public:
    explicit PrimaryFlightDisplayQML(QWidget *parent = 0, bool enableGStreamer = false);
    ~PrimaryFlightDisplayQML();

public slots:
    void setActiveUAS(UASInterface *uas);
    void uasTextMessage(int uasid, int componentid, int severity, QString text);
    void updateNavMode(int uasid, int mode, const QString& text);
    void topLevelChanged(bool topLevel);
    void dockLocationChanged(Qt::DockWidgetArea area);

signals:
    void fullScreenModeChanged ();

public:
    Q_PROPERTY(bool fullScreenMode READ isFullScreenMode WRITE setFullScreenMode NOTIFY fullScreenModeChanged)
    void setFullScreenMode(bool value) {
        this->m_fullScreenMode = value;
        QWidget *p = dynamic_cast<QWidget*>(this->parent());
        if (!value &&  p->isFullScreen()) {
            p->showNormal();
            emit fullScreenModeChanged();
        }

        if (value && !p->isFullScreen()) {
            p->showFullScreen();
            emit fullScreenModeChanged();
        }
    }
    bool isFullScreenMode() const {
        QWidget *p = dynamic_cast<QWidget*>(this->parent());
        return p->isFullScreen();
    }

    GStreamerPlayer * player() { return m_player; }

    void InitializeDisplay();
    void InitializeDisplayWithVideo();

private:

    void showEvent(QShowEvent *)
    {
        if (m_enableGStreamer && m_wasHidden)
        {
            InitializeDisplayWithVideo();
            m_wasHidden = false;
        }
    }

    void hideEvent(QHideEvent *)
    {
        m_wasHidden = true;
    }

    Ui::PrimaryFlightDisplayQML *ui;

    QQuickView* m_declarativeView;
    UASInterface *m_uasInterface;
    GStreamerPlayer *m_player;
    QWidget *m_viewcontainer;
    QString m_pipelineString;

    bool m_enableGStreamer;
    bool m_fullScreenMode;
    bool m_wasHidden;

};

#endif // PRIMARYFLIGHTDISPLAYQML_H
