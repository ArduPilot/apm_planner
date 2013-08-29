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

/**
 * @file
 *   @brief Accelerometer calibration for ApmPlane
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef APMPLANELEVEL_H
#define APMPLANELEVEL_H

#include <QWidget>
#include "ui_ApmPlaneLevel.h"
#include "AP2ConfigWidget.h"

class ApmPlaneLevel : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit ApmPlaneLevel(QWidget *parent = 0);
    ~ApmPlaneLevel();
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void levelClicked();
    void manualCheckBoxToggled(bool checked);
private:
    Ui::ApmPlaneLevel ui;
};

#endif // APMPLANELEVEL_H
