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

#include "OpticalFlowConfig.h"
#include <QMessageBox>

OpticalFlowConfig::OpticalFlowConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    connect(ui.enableCheckBox,SIGNAL(clicked(bool)),this,SLOT(enableCheckBoxClicked(bool)));
    initConnections();
}

OpticalFlowConfig::~OpticalFlowConfig()
{
}
void OpticalFlowConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas)
    Q_UNUSED(component)

    if (parameterName == "FLOW_ENABLE")
    {
        if (value.toInt() == 0)
        {
            ui.enableCheckBox->setChecked(false);
        }
        else
        {
            ui.enableCheckBox->setChecked(true);
        }
    }
}

void OpticalFlowConfig::enableCheckBoxClicked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"FLOW_ENABLE",checked ? 1 : 0);
}
