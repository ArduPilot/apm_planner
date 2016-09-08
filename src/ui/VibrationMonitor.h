/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2015 Bill Bonney <billbonney@communistech.com>

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
#ifndef VIBRATIONMONITOR_H
#define VIBRATIONMONITOR_H

#include <UASInterface.h>
#include <QWidget>
#include <QQuickView>

class VibrationMonitor : public QWidget
{
    Q_OBJECT
public:
    explicit VibrationMonitor(QWidget *parent = 0);
    ~VibrationMonitor();

private slots:
    void setActiveUAS(UASInterface *uas);

private:
    QQuickView* m_declarativeView;
    UASInterface *m_uasInterface;
};
#endif // VIBRATIONMONITOR_H
