/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

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

#ifndef RANGEFINDERCONFIG_H
#define RANGEFINDERCONFIG_H

#include <QWidget>
#include "AP2ConfigWidget.h"
#include "ui_RangeFinderConfig.h"

class RangeFinderConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit RangeFinderConfig(QWidget *parent = 0);
    ~RangeFinderConfig();
    void enableUi();
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void rangeFinderTypeChanged(int index);
    void gainSliderChanged(int value);
    void sendParameterUpdates();

    void activeUASSet(UASInterface *uas);
    void rangeFinderUpdate(UASInterface *uas, double distance, double voltage);

private:
    void enableUi(bool state);

private:
    Ui::RangeFinderConfig ui;
};

#endif // SONARCONFIG_H
