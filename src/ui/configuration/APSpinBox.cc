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

#include "APSpinBox.h"
#include <QKeyEvent>

APSpinBox::APSpinBox(QWidget *parent) : QSpinBox(parent)
{
}
void APSpinBox::keyReleaseEvent(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Enter)
    {
        emit returnPressed();
    }
    else if (evt->key() == Qt::Key_Escape)
    {
        this->setValue(m_savedValue);
    }
    else
    {
        QSpinBox::keyReleaseEvent(evt);
    }
}
void APSpinBox::focusInEvent(QFocusEvent *evt)
{
    m_savedValue = this->value();
    QSpinBox::focusInEvent(evt);
}
