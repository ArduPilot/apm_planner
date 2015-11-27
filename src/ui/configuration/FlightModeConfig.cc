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

FlightModeConfig::FlightModeConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_modesUpdated(0),
    m_flightModeCh(5) // default is channel 5 (copter)
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

    for (int i = 0; i < 6; i++){
        m_changedModes.append(false);
    }

    connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
    ui.savePushButton->setEnabled(false);

    ui.progressBar->setRange(0,5);
    ui.progressBar->setValue(0);

    initConnections();
}

FlightModeConfig::~FlightModeConfig()
{
}


void FlightModeConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(remoteControlChannelRawChanged(int,float)),this,SLOT(remoteControlChannelRawChanged(int,float)));
        disconnect(ui.mode1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
        disconnect(ui.mode2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
        disconnect(ui.mode3ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
        disconnect(ui.mode4ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
        disconnect(ui.mode5ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
        disconnect(ui.mode6ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));

        disconnect(ui.mode1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
        disconnect(ui.mode2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
        disconnect(ui.mode3ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
        disconnect(ui.mode4ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
        disconnect(ui.mode5ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
        disconnect(ui.mode6ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));

        disconnect(ui.mode1SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
        disconnect(ui.mode2SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
        disconnect(ui.mode3SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
        disconnect(ui.mode4SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
        disconnect(ui.mode5SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
        disconnect(ui.mode6SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas) return;
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
        m_modeString = "FLT";
        m_flightModeCh = m_uas->getParamManager()->getParameterValue(1,"FLTMODE_CH").toInt();
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
        m_modeString = "";
        m_flightModeCh = m_uas->getParamManager()->getParameterValue(1,"FLTMODE_CH").toInt();
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
        m_modeString = "FLT";
        m_flightModeCh = 5; // ie. Channel 5
    }

    connect(ui.mode1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
    connect(ui.mode2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
    connect(ui.mode3ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
    connect(ui.mode4ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
    connect(ui.mode5ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));
    connect(ui.mode6ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(comboBoxChanged(int)));

    connect(ui.mode1ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
    connect(ui.mode2ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
    connect(ui.mode3ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
    connect(ui.mode4ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
    connect(ui.mode5ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));
    connect(ui.mode6ComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(enableSaveButton()));

    connect(ui.mode1SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
    connect(ui.mode2SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
    connect(ui.mode3SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
    connect(ui.mode4SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
    connect(ui.mode5SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
    connect(ui.mode6SimpleCheckBox, SIGNAL(clicked()), this, SLOT(enableSaveButton()));
}

void FlightModeConfig::enableSaveButton()
{
    if (isFlightModeChanged()){
            ui.savePushButton->setEnabled(true);
    }
}

bool FlightModeConfig::isFlightModeChanged()
{
    QGCUASParamManager* pm = m_uas->getParamManager();
    if((pm->getParameterValue(1, m_modeString + "MODE1") != ui.mode1ComboBox->itemData(ui.mode1ComboBox->currentIndex()))
      ||(pm->getParameterValue(1, m_modeString + "MODE2") != ui.mode2ComboBox->itemData(ui.mode2ComboBox->currentIndex()))
      ||(pm->getParameterValue(1, m_modeString + "MODE3") != ui.mode3ComboBox->itemData(ui.mode3ComboBox->currentIndex()))
      ||(pm->getParameterValue(1, m_modeString + "MODE4") != ui.mode4ComboBox->itemData(ui.mode4ComboBox->currentIndex()))
      ||(pm->getParameterValue(1, m_modeString + "MODE5") != ui.mode5ComboBox->itemData(ui.mode5ComboBox->currentIndex()))
      ||(pm->getParameterValue(1, m_modeString + "MODE6") != ui.mode6ComboBox->itemData(ui.mode6ComboBox->currentIndex()))){

        return true;
    }

    if(pm->getParameterValue(1, m_modeString + "MODE1").toInt() != getSimpleValue())
        return true;

    return false; // no changes

}

void FlightModeConfig::checkForComboxBoxChanged(QObject* sender, QComboBox *comboBox, int index,
                                                bool& modeChanged, const QString& param)
{
    QComboBox *senderComboBox = dynamic_cast<QComboBox*>(sender);
    QLOG_DEBUG() << "sender :" << senderComboBox;

    QGCUASParamManager* pm = m_uas->getParamManager();
    if ((senderComboBox == comboBox)&&(modeChanged == false)){
        if (pm->getParameterValue(1,param).toInt() != comboBox->itemData(index).toInt()){
            modeChanged = true;
            m_modesUpdated++;
            ui.progressBar->setMaximum(m_modesUpdated);
        } else {
            modeChanged = false;
            m_modesUpdated--;
            ui.progressBar->setMaximum(m_modesUpdated);
        }
    }
}

void FlightModeConfig::comboBoxChanged(int index)
{
    ui.progressBar->reset();

    checkForComboxBoxChanged(sender(), ui.mode1ComboBox, index, m_changedModes[0], m_modeString+"MODE1");
    checkForComboxBoxChanged(sender(), ui.mode2ComboBox, index, m_changedModes[1], m_modeString+"MODE2");
    checkForComboxBoxChanged(sender(), ui.mode3ComboBox, index, m_changedModes[2], m_modeString+"MODE3");
    checkForComboxBoxChanged(sender(), ui.mode4ComboBox, index, m_changedModes[3], m_modeString+"MODE4");
    checkForComboxBoxChanged(sender(), ui.mode5ComboBox, index, m_changedModes[4], m_modeString+"MODE5");
    checkForComboxBoxChanged(sender(), ui.mode6ComboBox, index, m_changedModes[5], m_modeString+"MODE6");
}

int FlightModeConfig::getSimpleValue()
{
    int value = 0;
    value += (ui.mode1SimpleCheckBox->isChecked() ? 1 : 0);
    value += (ui.mode2SimpleCheckBox->isChecked() ? 1 : 0) << 1;
    value += (ui.mode3SimpleCheckBox->isChecked() ? 1 : 0) << 2;
    value += (ui.mode4SimpleCheckBox->isChecked() ? 1 : 0) << 3;
    value += (ui.mode5SimpleCheckBox->isChecked() ? 1 : 0) << 4;
    value += (ui.mode6SimpleCheckBox->isChecked() ? 1 : 0) << 5;

    return value;
}

void FlightModeConfig::saveButtonClicked()
{
    QGCUASParamManager* pm = m_uas->getParamManager();

    int mode1 = ui.mode1ComboBox->itemData(ui.mode1ComboBox->currentIndex()).toInt();
    int mode2 = ui.mode2ComboBox->itemData(ui.mode2ComboBox->currentIndex()).toInt();
    int mode3 = ui.mode3ComboBox->itemData(ui.mode3ComboBox->currentIndex()).toInt();
    int mode4 = ui.mode4ComboBox->itemData(ui.mode4ComboBox->currentIndex()).toInt();
    int mode5 = ui.mode5ComboBox->itemData(ui.mode5ComboBox->currentIndex()).toInt();
    int mode6 = ui.mode6ComboBox->itemData(ui.mode6ComboBox->currentIndex()).toInt();

    if(m_uas->isMultirotor()){
        int value = getSimpleValue();
        pm->setParameter(1,"SIMPLE",value);
    }

    ui.progressBar->setValue(0);
    ui.progressBar->setMaximum(m_modesUpdated);

    pm->setParameter(1,m_modeString + "MODE1",mode1);
    pm->setParameter(1,m_modeString + "MODE2",mode2);
    pm->setParameter(1,m_modeString + "MODE3",mode3);
    pm->setParameter(1,m_modeString + "MODE4",mode4);
    pm->setParameter(1,m_modeString + "MODE5",mode5);
    pm->setParameter(1,m_modeString + "MODE6",mode6);

}

void FlightModeConfig::remoteControlChannelRawChanged(int chan,float val)
{
    if (chan == m_flightModeCh-1) // Zero indexed.
    {
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

void FlightModeConfig::updateModeComboBox(QComboBox* modeComboBox, QVariant value, bool& modeChanged)
{
    int index = modeComboBox->findData(value.toInt());
    modeComboBox->blockSignals(true);
    modeComboBox->setCurrentIndex(index);
    // stop progress bar
    if ((m_modesUpdated != 0) && (modeChanged==true)){
        ui.progressBar->setValue(ui.progressBar->value()+1);
        m_modesUpdated--;
        modeChanged = false;
    }
    modeComboBox->blockSignals(false);
}

void FlightModeConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    // Filter Log based on parameters we are interested in.
    if (parameterName.startsWith("FLTMODE")||parameterName.startsWith("MODE")){
        QLOG_DEBUG() << "FlightModeConfig:: name:" << parameterName << " value:" << value;
        ui.savePushButton->setEnabled(false);
    }

    if (parameterName == m_modeString + "MODE1")
    {
        updateModeComboBox(ui.mode1ComboBox, value, m_changedModes[0]);
    }
    else if ((parameterName == m_modeString + "MODE2"))
    {
        updateModeComboBox(ui.mode2ComboBox, value, m_changedModes[1]);
    }
    else if ((parameterName == m_modeString + "MODE3"))
    {
        updateModeComboBox(ui.mode3ComboBox, value, m_changedModes[2]);
    }
    else if ((parameterName == m_modeString + "MODE4"))
    {
        updateModeComboBox(ui.mode4ComboBox, value, m_changedModes[3]);
    }
    else if ((parameterName == m_modeString + "MODE5"))
    {
        updateModeComboBox(ui.mode5ComboBox, value, m_changedModes[4]);
    }
    else if ((parameterName == m_modeString + "MODE6"))
    {
        updateModeComboBox(ui.mode6ComboBox, value, m_changedModes[5]);
    } else if ((parameterName == "FLTMODE_CH")){
        m_flightModeCh = value.toInt();
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
            QTimer::singleShot(1000, this, SLOT(resetMaximum()));
            ui.savePushButton->setEnabled(false);
        }
    }
}

void FlightModeConfig::resetMaximum()
{
    ui.progressBar->setMaximum(5);
    ui.progressBar->setValue(5);
}
