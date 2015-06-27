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
    ui.sensorComboBox->setEnabled(false);
    ui.pinComboBox->setEnabled(false);
    connect(ui.hardwareComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(hardwareSelectComboBoxChanged(int)));
    connect(ui.pinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(pinSpinBoxValueChanged(int)));
}

AirspeedConfig::~AirspeedConfig()
{
}
void AirspeedConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

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
    else if (parameterName == "ARSPD_PIN")
    {
        disconnect(ui.pinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(pinSpinBoxValueChanged(int)));
        ui.pinSpinBox->setValue(value.toInt());
        connect(ui.pinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(pinSpinBoxValueChanged(int)));
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
void AirspeedConfig::hardwareSelectComboBoxChanged(int index)
{
    if (index == 1) //APM1.X
    {
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
        ui.sensorComboBox->clear();
        ui.sensorComboBox->addItem("Choose One");
        ui.sensorComboBox->addItem("Analog Sensor");
        ui.sensorComboBox->setEnabled(true);
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->setEnabled(false);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
    }
    else if (index == 2) //APM2.X
    {
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
        ui.sensorComboBox->clear();
        ui.sensorComboBox->addItem("Choose One");
        ui.sensorComboBox->addItem("Analog Sensor");
        ui.sensorComboBox->addItem("EagleTree I2C");
        ui.sensorComboBox->addItem("MEAS I2C");
        ui.sensorComboBox->addItem("Pixhawk I2C");
        ui.sensorComboBox->setEnabled(true);
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->setEnabled(false);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
    }
    else if (index == 3) //PX4
    {
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
        ui.sensorComboBox->clear();
        ui.sensorComboBox->addItem("Choose One");
        ui.sensorComboBox->addItem("Analog Sensor");
        ui.sensorComboBox->addItem("EagleTree I2C");
        ui.sensorComboBox->addItem("MEAS I2C");
        ui.sensorComboBox->addItem("Pixhawk I2C");
        ui.sensorComboBox->setEnabled(true);
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->setEnabled(false);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
    }
    else if (index == 4) //Pixhawk
    {
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
        ui.sensorComboBox->clear();
        ui.sensorComboBox->addItem("Choose One");
        ui.sensorComboBox->addItem("Analog Sensor");
        ui.sensorComboBox->addItem("EagleTree I2C");
        ui.sensorComboBox->addItem("MEAS I2C");
        ui.sensorComboBox->addItem("Pixhawk I2C");
        ui.sensorComboBox->setEnabled(true);
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->setEnabled(false);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
    }
    else
    {
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
        ui.sensorComboBox->clear();
        ui.sensorComboBox->setEnabled(false);
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->setEnabled(false);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorSelectComboBoxChanged(int)));
    }
}

void AirspeedConfig::sensorSelectComboBoxChanged(int index)
{
    if (index == 1) //Analog Sensor
    {
        if (ui.hardwareComboBox->currentIndex() == 1) //APM1.X
        {
            disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
            ui.pinComboBox->clear();
            ui.pinComboBox->addItem("Choose One");
            ui.pinComboBox->addItem("64");
            ui.pinComboBox->setEnabled(true);
            connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        }
        else if (ui.hardwareComboBox->currentIndex() == 2) //APM2.X
        {
            disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
            ui.pinComboBox->clear();
            ui.pinComboBox->addItem("Choose One");
            ui.pinComboBox->addItem("0");
            ui.pinComboBox->addItem("1");
            ui.pinComboBox->addItem("2");
            ui.pinComboBox->addItem("3");
            ui.pinComboBox->addItem("4");
            ui.pinComboBox->addItem("5");
            ui.pinComboBox->addItem("6");
            ui.pinComboBox->addItem("7");
            ui.pinComboBox->addItem("8");
            ui.pinComboBox->addItem("9");
            ui.pinComboBox->setEnabled(true);
            connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        }
        else if (ui.hardwareComboBox->currentIndex() == 3) //PX4
        {
            disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
            ui.pinComboBox->clear();
            ui.pinComboBox->addItem("Choose One");
            ui.pinComboBox->addItem("11");
            ui.pinComboBox->setEnabled(true);
            connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        }
        else if (ui.hardwareComboBox->currentIndex() == 4) //Pixhawk
        {
            disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
            ui.pinComboBox->clear();
            ui.pinComboBox->addItem("Choose One");
            ui.pinComboBox->addItem("15");
            ui.pinComboBox->setEnabled(true);
            connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        }
    }
    else if (index == 2 || index == 3 || index == 4) //I2C Sensor
    {
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->addItem("Choose One");
        ui.pinComboBox->addItem("65");
        ui.pinComboBox->setEnabled(true);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
    }
    else
    {
        disconnect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
        ui.pinComboBox->clear();
        ui.pinComboBox->setEnabled(false);
        connect(ui.pinComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(pinSelectComboBoxChanged(int)));
    }
}
void AirspeedConfig::pinSelectComboBoxChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    if (ui.pinComboBox->itemText(index) != "Choose One")
    {
        bool ok = false;
        int pinnumber = ui.pinComboBox->itemText(index).toInt(&ok);
        if (!ok)
        {
            //For some reason, the conversion failed. An unexpected case
            //Don't want to set the pin to 0 for no reason, return on failure.
            return;
        }
        disconnect(ui.pinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(pinSpinBoxValueChanged(int)));
        ui.pinSpinBox->setValue(pinnumber);
        connect(ui.pinSpinBox,SIGNAL(valueChanged(int)),this,SLOT(pinSpinBoxValueChanged(int)));
        m_uas->setParameter(1,"ARSPD_PIN",pinnumber);

    }
}
void AirspeedConfig::pinSpinBoxValueChanged(int value)
{
    m_uas->setParameter(1,"ARSPD_PIN",value);
}
