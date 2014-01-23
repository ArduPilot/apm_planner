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
#include "QsLog.h"
#include "FlightModeConfig.h"
#include "ArduPilotMegaMAV.h"
#include "ApmUiHelpers.h"

#include "QGCCore.h"

FlightModeConfig::FlightModeConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    ui.mode1SimpleCheckBox->setVisible(false);
    ui.mode2SimpleCheckBox->setVisible(false);
    ui.mode3SimpleCheckBox->setVisible(false);
    ui.mode4SimpleCheckBox->setVisible(false);
    ui.mode5SimpleCheckBox->setVisible(false);
    ui.mode6SimpleCheckBox->setVisible(false);
    connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
    initConnections();
}

FlightModeConfig::~FlightModeConfig()
{
}


void FlightModeConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(modeChanged(int,QString,QString)),this,SLOT(modeChanged(int,QString,QString)));
        disconnect(m_uas,SIGNAL(remoteControlChannelRawChanged(int,float)),this,SLOT(remoteControlChannelRawChanged(int,float)));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas) return;
    connect(m_uas,SIGNAL(modeChanged(int,QString,QString)),this,SLOT(modeChanged(int,QString,QString)));
    connect(m_uas,SIGNAL(remoteControlChannelRawChanged(int,float)),this,SLOT(remoteControlChannelRawChanged(int,float)));
    QStringList itemlist;

    if (m_uas->isFixedWing())
    {
        ApmUiHelpers::addPlaneModes(ui.mode1ComboBox);
        ApmUiHelpers::addPlaneModes(ui.mode2ComboBox);
        ApmUiHelpers::addPlaneModes(ui.mode3ComboBox);
        ApmUiHelpers::addPlaneModes(ui.mode4ComboBox);
        ApmUiHelpers::addPlaneModes(ui.mode5ComboBox);
        ApmUiHelpers::addPlaneModes(ui.mode6ComboBox);

        ui.mode6ComboBox->setEnabled(false);
    }
    else if (m_uas->isGroundRover())
    {
        ui.mode6ComboBox->setEnabled(false); // [TODO] ???

        ApmUiHelpers::addRoverModes(ui.mode1ComboBox);
        ApmUiHelpers::addRoverModes(ui.mode2ComboBox);
        ApmUiHelpers::addRoverModes(ui.mode3ComboBox);
        ApmUiHelpers::addRoverModes(ui.mode4ComboBox);
        ApmUiHelpers::addRoverModes(ui.mode5ComboBox);
        ApmUiHelpers::addRoverModes(ui.mode6ComboBox);
    }
    else if (m_uas->isMultirotor())
    {
        ApmUiHelpers::addCopterModes(ui.mode1ComboBox);
        ApmUiHelpers::addCopterModes(ui.mode2ComboBox);
        ApmUiHelpers::addCopterModes(ui.mode3ComboBox);
        ApmUiHelpers::addCopterModes(ui.mode4ComboBox);
        ApmUiHelpers::addCopterModes(ui.mode5ComboBox);
        ApmUiHelpers::addCopterModes(ui.mode6ComboBox);

        ui.mode6ComboBox->setEnabled(true);
        ui.mode1SimpleCheckBox->setVisible(true);
        ui.mode2SimpleCheckBox->setVisible(true);
        ui.mode3SimpleCheckBox->setVisible(true);
        ui.mode4SimpleCheckBox->setVisible(true);
        ui.mode5SimpleCheckBox->setVisible(true);
        ui.mode6SimpleCheckBox->setVisible(true);
    }
}
void FlightModeConfig::modeChanged(int sysId, QString status, QString description)
{
    //Unused?
}
void FlightModeConfig::saveButtonClicked()
{
    QGCUASParamManager* pm = m_uas->getParamManager();
    QString modeString;
    int mode1 = ui.mode1ComboBox->itemData(ui.mode1ComboBox->currentIndex()).toInt();
    int mode2 = ui.mode2ComboBox->itemData(ui.mode2ComboBox->currentIndex()).toInt();
    int mode3 = ui.mode3ComboBox->itemData(ui.mode3ComboBox->currentIndex()).toInt();
    int mode4 = ui.mode4ComboBox->itemData(ui.mode4ComboBox->currentIndex()).toInt();
    int mode5 = ui.mode5ComboBox->itemData(ui.mode5ComboBox->currentIndex()).toInt();
    int mode6 = ui.mode6ComboBox->itemData(ui.mode6ComboBox->currentIndex()).toInt();

    if (m_uas->isFixedWing() || m_uas->isMultirotor())
    {
        modeString = "FLTMODE";

        if(m_uas->isMultirotor()){
            int value = 0;
            value += (ui.mode1SimpleCheckBox->isChecked() ? 1 : 0);
            value += (ui.mode2SimpleCheckBox->isChecked() ? 1 : 0) << 1;
            value += (ui.mode3SimpleCheckBox->isChecked() ? 1 : 0) << 2;
            value += (ui.mode4SimpleCheckBox->isChecked() ? 1 : 0) << 3;
            value += (ui.mode5SimpleCheckBox->isChecked() ? 1 : 0) << 4;
            value += (ui.mode6SimpleCheckBox->isChecked() ? 1 : 0) << 5;
            pm->setParameter(1,"SIMPLE",value);
        }

    }
    else if (m_uas->isGroundRover())
    {
        modeString = "MODE";
    }

    pm->setParameter(1,modeString + "1",mode1);
    pm->setParameter(1,modeString + "2",mode2);
    pm->setParameter(1,modeString + "3",mode3);
    pm->setParameter(1,modeString + "4",mode4);
    pm->setParameter(1,modeString + "5",mode5);
    pm->setParameter(1,modeString + "6",mode6);

}

void FlightModeConfig::remoteControlChannelRawChanged(int chan,float val)
{
    if (chan == 4)
    {
        //Channel 5 (0 array) is the mode switch.
        ///TODO: Make this configurable
        if (val <= 1230)
        {
            ui.mode1Label->setStyleSheet("background-color: rgb(0, 255, 0);color: rgb(0, 0, 0);");
            ui.mode2Label->setStyleSheet("");
            ui.mode3Label->setStyleSheet("");
            ui.mode4Label->setStyleSheet("");
            ui.mode5Label->setStyleSheet("");
            ui.mode6Label->setStyleSheet("");
        }
        else if (val <= 1360)
        {
            ui.mode1Label->setStyleSheet("");
            ui.mode2Label->setStyleSheet("background-color: rgb(0, 255, 0);color: rgb(0, 0, 0);");
            ui.mode3Label->setStyleSheet("");
            ui.mode4Label->setStyleSheet("");
            ui.mode5Label->setStyleSheet("");
            ui.mode6Label->setStyleSheet("");
        }
        else if (val <= 1490)
        {
            ui.mode1Label->setStyleSheet("");
            ui.mode2Label->setStyleSheet("");
            ui.mode3Label->setStyleSheet("background-color: rgb(0, 255, 0);color: rgb(0, 0, 0);");
            ui.mode4Label->setStyleSheet("");
            ui.mode5Label->setStyleSheet("");
            ui.mode6Label->setStyleSheet("");
        }
        else if (val <=1620)
        {
            ui.mode1Label->setStyleSheet("");
            ui.mode2Label->setStyleSheet("");
            ui.mode3Label->setStyleSheet("");
            ui.mode4Label->setStyleSheet("background-color: rgb(0, 255, 0);color: rgb(0, 0, 0);");
            ui.mode5Label->setStyleSheet("");
            ui.mode6Label->setStyleSheet("");
        }
        else if (val <=1749)
        {
            ui.mode1Label->setStyleSheet("");
            ui.mode2Label->setStyleSheet("");
            ui.mode3Label->setStyleSheet("");
            ui.mode4Label->setStyleSheet("");
            ui.mode5Label->setStyleSheet("background-color: rgb(0, 255, 0);color: rgb(0, 0, 0);");
            ui.mode6Label->setStyleSheet("");
        }
        else
        {
            ui.mode1Label->setStyleSheet("");
            ui.mode2Label->setStyleSheet("");
            ui.mode3Label->setStyleSheet("");
            ui.mode4Label->setStyleSheet("");
            ui.mode5Label->setStyleSheet("");
            ui.mode6Label->setStyleSheet("background-color: rgb(0, 255, 0);color: rgb(0, 0, 0);");
        }
    }
}

void FlightModeConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    // Filter Log based on parameters we are interested in.
    if (parameterName.startsWith("FLTMODE")||parameterName.startsWith("MODE"))
         QLOG_DEBUG() << "FlightModeConfig:: name:" << parameterName << " value:" << value;

    if ((parameterName == "FLTMODE1") || (parameterName == "MODE1"))
    {
        int index = ui.mode1ComboBox->findData(value.toInt());
        ui.mode1ComboBox->setCurrentIndex(index);
    }
    else if ((parameterName == "FLTMODE2") || (parameterName == "MODE2"))
    {
        int index = ui.mode2ComboBox->findData(value.toInt());
        ui.mode2ComboBox->setCurrentIndex(index);
    }
    else if ((parameterName == "FLTMODE3") || (parameterName == "MODE3"))
    {
        int index = ui.mode3ComboBox->findData(value.toInt());
        ui.mode3ComboBox->setCurrentIndex(index);
    }
    else if ((parameterName == "FLTMODE4") || (parameterName == "MODE4"))
    {
        int index = ui.mode4ComboBox->findData(value.toInt());
        ui.mode4ComboBox->setCurrentIndex(index);
    }
    else if ((parameterName == "FLTMODE5") || (parameterName == "MODE5"))
    {
        int index = ui.mode5ComboBox->findData(value.toInt());
        ui.mode5ComboBox->setCurrentIndex(index);
    }
    else if ((parameterName == "FLTMODE6") || (parameterName == "MODE6"))
    {
        int index = ui.mode6ComboBox->findData(value.toInt());
        ui.mode6ComboBox->setCurrentIndex(index);
    }


    if (m_uas->isMultirotor())
    {
        if (parameterName == "SIMPLE")
        {
            int newval = value.toInt();
            ui.mode1SimpleCheckBox->setChecked(newval >> 0 & 1);
            ui.mode2SimpleCheckBox->setChecked(newval >> 1 & 1);
            ui.mode3SimpleCheckBox->setChecked(newval >> 2 & 1);
            ui.mode4SimpleCheckBox->setChecked(newval >> 3 & 1);
            ui.mode5SimpleCheckBox->setChecked(newval >> 4 & 1);
            ui.mode6SimpleCheckBox->setChecked(newval >> 5 & 1);
        }
    }
}
