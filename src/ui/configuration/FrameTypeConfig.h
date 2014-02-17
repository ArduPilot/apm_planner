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
 *   @brief Airframe type configuration widget header.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *
 */

#ifndef FRAMETYPECONFIG_H
#define FRAMETYPECONFIG_H

#include "UASParameter.h"
#include <QWidget>
#include "ui_FrameTypeConfig.h"
#include "UASInterface.h"
#include "UASManager.h"
#include "QGCUASParamManager.h"
#include "AP2ConfigWidget.h"
class FrameTypeConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit FrameTypeConfig(QWidget *parent = 0);
    ~FrameTypeConfig();

public slots:
    void paramButtonClicked();
    void activateCompareDialog();

private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void parameterChanged(int uas, int component, int parameterCount, int parameterId,
                           QString parameterName, QVariant value);
    void xFrameSelected();
    void plusFrameSelected();
    void vFrameSelected();
    void hFrameSelected();
    void newY6FrameSelected();

private:
    Ui::FrameTypeConfig ui;

    QMap<QString, UASParameter*> m_parameterList;
    QString m_paramFileToCompare;
};

#endif // FRAMETYPECONFIG_H
