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

#include "ApmPlaneLevel.h"
#include <QMessageBox>

ApmPlaneLevel::ApmPlaneLevel(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    connect(ui.levelPushButton,SIGNAL(clicked()),this,SLOT(levelClicked()));
    connect(ui.manualLevelCheckBox,SIGNAL(toggled(bool)),this,SLOT(manualCheckBoxToggled(bool)));
    initConnections();
}

ApmPlaneLevel::~ApmPlaneLevel()
{
}
void ApmPlaneLevel::levelClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    QMessageBox::information(0,"Warning","Be sure the plane is completely level, then click OK.");
    MAV_CMD command = MAV_CMD_PREFLIGHT_CALIBRATION;
    int confirm = 0;
    float param1 = 1.0; // Gyro & 1D Accel
    float param2 = 0.0; // Gyro & 1D Accel
    float param3 = 1.0; // Baro & Air Speed Cal
    float param4 = 0.0; // Radio Trim
    float param5 = 0.0; // 3D Accel Cal
    float param6 = 0.0;
    float param7 = 0.0;
    int component = 1;
    m_uas->executeCommand(command, confirm, param1, param2, param3, param4, param5, param6, param7, component);
    QMessageBox::information(0,"Warning","Leveling completed");
}

void ApmPlaneLevel::manualCheckBoxToggled(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->getParamManager()->setParameter(1,"MANUAL_LEVEL",1);
    }
    else
    {
        m_uas->getParamManager()->setParameter(1,"MANUAL_LEVEL",0);
    }
}
void ApmPlaneLevel::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas)
    Q_UNUSED(component)

    if (parameterName == "MANUAL_LEVEL")
    {
        if (value.toInt() == 1)
        {
            ui.manualLevelCheckBox->setChecked(true);
        }
        else
        {
            ui.manualLevelCheckBox->setChecked(false);
        }
    }
}
