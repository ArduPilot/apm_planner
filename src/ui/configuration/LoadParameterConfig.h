/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 *   @brief Load Parameters configuration widget.
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 */

#ifndef LOADPARAMETERCONFIG_H
#define LOADPARAMETERCONFIG_H

#include "UASParameter.h"
#include "AP2ConfigWidget.h"
#include <QWidget>

namespace Ui {
class LoadParameterConfig;
}

class LoadParameterConfig : public AP2ConfigWidget
{
    Q_OBJECT

public:
    explicit LoadParameterConfig(QWidget *parent = 0);
    ~LoadParameterConfig();

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);

public slots:
    void paramButtonClicked();
    void activateCompareDialog();

    void parameterChanged(int uas, int component, int parameterCount, int parameterId,
                                             QString parameterName, QVariant value);

private:
    Ui::LoadParameterConfig *ui;

    QMap<QString, UASParameter*> m_parameterList;
    QString m_paramFileToCompare;
};

#endif // LOADPARAMETERCONFIG_H
