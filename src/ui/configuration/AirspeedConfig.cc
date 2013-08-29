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

#include "AirspeedConfig.h"
#include <QMessageBox>

AirspeedConfig::AirspeedConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    connect(ui.enableCheckBox,SIGNAL(toggled(bool)),this,SLOT(enableCheckBoxClicked(bool)));
    connect(ui.useAirspeedCheckBox,SIGNAL(toggled(bool)),this,SLOT(useCheckBoxClicked(bool)));
    initConnections();
}

AirspeedConfig::~AirspeedConfig()
{
}
void AirspeedConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName == "ARSPD_ENABLE")
    {
        if (value.toInt() == 0)
        {
            disconnect(ui.enableCheckBox,SIGNAL(toggled(bool)),this,SLOT(enableCheckBoxClicked(bool)));
            ui.enableCheckBox->setChecked(false);
            connect(ui.enableCheckBox,SIGNAL(toggled(bool)),this,SLOT(enableCheckBoxClicked(bool)));
            ui.useAirspeedCheckBox->setEnabled(false);
        }
        else
        {
            disconnect(ui.enableCheckBox,SIGNAL(toggled(bool)),this,SLOT(enableCheckBoxClicked(bool)));
            ui.enableCheckBox->setChecked(true);
            connect(ui.enableCheckBox,SIGNAL(toggled(bool)),this,SLOT(enableCheckBoxClicked(bool)));
            ui.useAirspeedCheckBox->setEnabled(true);
        }
    }
    else if (parameterName == "ARSPD_USE")
    {
        if (value.toInt() == 0)
        {
            disconnect(ui.useAirspeedCheckBox,SIGNAL(toggled(bool)),this,SLOT(useCheckBoxClicked(bool)));
            ui.useAirspeedCheckBox->setChecked(false);
            connect(ui.useAirspeedCheckBox,SIGNAL(toggled(bool)),this,SLOT(useCheckBoxClicked(bool)));
        }
        else
        {
            disconnect(ui.useAirspeedCheckBox,SIGNAL(toggled(bool)),this,SLOT(useCheckBoxClicked(bool)));
            ui.useAirspeedCheckBox->setChecked(true);
            connect(ui.useAirspeedCheckBox,SIGNAL(toggled(bool)),this,SLOT(useCheckBoxClicked(bool)));
        }
    }
}

void AirspeedConfig::useCheckBoxClicked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->getParamManager()->setParameter(1,"ARSPD_USE",1);
    }
    else
    {
        m_uas->getParamManager()->setParameter(1,"ARSPD_USE",0);
    }
}

void AirspeedConfig::enableCheckBoxClicked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->getParamManager()->setParameter(1,"ARSPD_ENABLE",1);
    }
    else
    {
        m_uas->getParamManager()->setParameter(1,"ARSPD_ENABLE",0);
    }
}
