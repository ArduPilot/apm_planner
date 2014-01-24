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

#include "QsLog.h"
#include "LoadParameterConfig.h"
#include "ui_LoadParameterConfig.h"
#include "DownloadRemoteParamsDialog.h"
#include "ParamCompareDialog.h"

LoadParameterConfig::LoadParameterConfig(QWidget *parent) :
    AP2ConfigWidget(parent),
    ui(new Ui::LoadParameterConfig)
{
    ui->setupUi(this);
    initConnections();
}

LoadParameterConfig::~LoadParameterConfig()
{
    delete ui;
}

void LoadParameterConfig::parameterChanged(int uas, int component, int parameterCount, int parameterId,
                                            QString parameterName, QVariant value)
{
    // Create a parameter list model for comparison feature
    // [TODO] This needs to move to the global parameter model.

    if (m_parameterList.contains(parameterName)){
        UASParameter* param = m_parameterList.value(parameterName);
        param->setValue(value); // This also sets the modified bit
    } else {
        // create a new entry
        UASParameter* param = new UASParameter(parameterName,component,value,parameterId);
        m_parameterList.insert(parameterName, param);
    }
}

void LoadParameterConfig::showEvent(QShowEvent *)
{

}

void LoadParameterConfig::hideEvent(QHideEvent *)
{

}

void LoadParameterConfig::paramButtonClicked()
{
    DownloadRemoteParamsDialog* dialog = new DownloadRemoteParamsDialog(this->parentWidget(), true);

    if(dialog->exec() == QDialog::Accepted) {
        // Pull the selected file and
        // modify the parameters on the adv param list.
        QLOG_DEBUG() << "Remote File Downloaded";
        QLOG_DEBUG() << "Trigger auto load or compare of the downloaded file";

        // Bring up the compare dialog
        m_paramFileToCompare = dialog->getDownloadedFileName();
        QTimer::singleShot(300, this, SLOT(activateCompareDialog()));
    }
    delete dialog;
    dialog = NULL;
}

void LoadParameterConfig::activateCompareDialog()
{
    QLOG_DEBUG() << "Compare Params to File";

    ParamCompareDialog* dialog = new ParamCompareDialog(m_parameterList, m_paramFileToCompare, this);
    dialog->setAcceptButtonLabel(tr("Write Params"));

    if(dialog->exec() == QDialog::Accepted) {
        // Apply the selected parameters
        foreach(UASParameter* param, m_parameterList){
            // Apply changes to ParamManager
            if(param->isModified()){
                m_uas->getParamManager()->setParameter(param->component(),param->name(),param->value());
            }
        }
    }
    delete dialog;
    dialog = NULL;
}
