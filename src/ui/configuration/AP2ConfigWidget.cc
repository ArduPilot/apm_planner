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

#include <QMessageBox>
#include "AP2ConfigWidget.h"

AP2ConfigWidget::AP2ConfigWidget(QWidget *parent) : QWidget(parent)
{
    m_uas = 0;
}
void AP2ConfigWidget::initConnections()
{
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
}

void AP2ConfigWidget::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));
        disconnect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
                this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));
        m_uas = 0;
    }
    if (!uas) return;
    m_uas = uas;
    connect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
            this,SLOT(parameterChanged(int,int,QString,QVariant)));
    connect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
            this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));
}

void AP2ConfigWidget::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);
    Q_UNUSED(parameterName);
    Q_UNUSED(value);
}

void AP2ConfigWidget::parameterChanged(int uas, int component, int parameterCount, int parameterId, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);
    Q_UNUSED(parameterName);
    Q_UNUSED(value);
    Q_UNUSED(parameterCount);
    Q_UNUSED(parameterId);
}

bool AP2ConfigWidget::showNullMAVErrorMessageBox()
{
    if (!m_uas) {
        QMessageBox::information(0,tr("Error"),
                                 tr("Please connect to a MAV before attempting to set configuration"));
        return true;
    }
    return false;
}
