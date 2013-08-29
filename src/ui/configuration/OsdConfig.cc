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

#include "OsdConfig.h"
#include <QMessageBox>

OsdConfig::OsdConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    connect(ui.enablePushButton,SIGNAL(clicked()),this,SLOT(enableButtonClicked()));
    initConnections();

}

OsdConfig::~OsdConfig()
{
}
void OsdConfig::enableButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->setParameter(1,"SR0_EXT_STAT",2);
    m_uas->getParamManager()->setParameter(1,"SR0_EXTRA1",10);
    m_uas->getParamManager()->setParameter(1,"SR0_EXTRA2",10);
    m_uas->getParamManager()->setParameter(1,"SR0_EXTRA3",2);
    m_uas->getParamManager()->setParameter(1,"SR0_POSITION",3);
    m_uas->getParamManager()->setParameter(1,"SR0_RAW_CTRL",2);
    m_uas->getParamManager()->setParameter(1,"SR0_RAW_SENS",2);
    m_uas->getParamManager()->setParameter(1,"SR0_RC_CHAN",2);
}
