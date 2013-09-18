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

#include "SonarConfig.h"
#include <QMessageBox>

#include "QGCCore.h"

SonarConfig::SonarConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    ui.sonarTypeComboBox->addItem("XL-EZ0 / XL-EZ4");
    ui.sonarTypeComboBox->addItem("LV-EZ0");
    ui.sonarTypeComboBox->addItem("XL-EZL0");
    ui.sonarTypeComboBox->addItem("HRLV");
    connect(ui.enableCheckBox,SIGNAL(clicked(bool)),this,SLOT(checkBoxToggled(bool)));
    connect(ui.sonarTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sonarTypeChanged(int)));

    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    initConnections();
}

SonarConfig::~SonarConfig()
{
}
void SonarConfig::checkBoxToggled(bool enabled)
{
    if (enabled)
    {
        ui.sonarTypeComboBox->setEnabled(false);
    }
    if (!m_uas)
    {
        QMessageBox::information(0,tr("Error"),tr("Please connect to a MAV before attempting to set configuration"));
        return;
    }
    m_uas->getParamManager()->setParameter(1,"SONAR_ENABLE",ui.enableCheckBox->isChecked() ? 1 : 0);
}
void SonarConfig::sonarTypeChanged(int index)
{
    if (!m_uas)
    {
        QMessageBox::information(0,tr("Error"),tr("Please connect to a MAV before attempting to set configuration"));
        return;
    }
    m_uas->getParamManager()->setParameter(1,"SONAR_TYPE",ui.sonarTypeComboBox->currentIndex());
}

void SonarConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName == "SONAR_ENABLE")
    {
        if (value.toInt() == 0)
        {
            //Disabled
            ui.enableCheckBox->setChecked(false);
            ui.sonarTypeComboBox->setEnabled(false);
        }
        else
        {
            ui.enableCheckBox->setChecked(true);
            ui.sonarTypeComboBox->setEnabled(true);
        }
    }
    else if (parameterName == "SONAR_TYPE")
    {
        disconnect(ui.sonarTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sonarTypeChanged(int)));
        ui.sonarTypeComboBox->setCurrentIndex(value.toInt());
        connect(ui.sonarTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sonarTypeChanged(int)));
    }
}
