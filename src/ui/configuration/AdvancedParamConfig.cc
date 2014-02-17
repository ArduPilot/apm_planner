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

#include "AdvancedParamConfig.h"

#include "QGCMouseWheelEventFilter.h"

AdvancedParamConfig::AdvancedParamConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    initConnections();
}

AdvancedParamConfig::~AdvancedParamConfig()
{
}
void AdvancedParamConfig::addRange(QString title,QString description,QString param,double min,double max,double increment)
{
    ParamWidget *widget = new ParamWidget(param,ui.scrollAreaWidgetContents);
    connect(widget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(doubleValueChanged(QString,double)));
    connect(widget,SIGNAL(intValueChanged(QString,int)),this,SLOT(intValueChanged(QString,int)));
    m_paramToWidgetMap[param] = widget;
    widget->setupDouble(title + "(" + param + ")",description,0,min,max,increment);
    ui.verticalLayout->addWidget(widget);
    widget->installEventFilter(QGCMouseWheelEventFilter::getFilter());
    widget->show();
    if (m_paramToValueMap.contains(param))
    {
        widget->setValue(m_paramToValueMap.value(param).toDouble());
        m_paramToValueMap.remove(param);
    }
}

void AdvancedParamConfig::addCombo(QString title,QString description,QString param,QList<QPair<int,QString> > valuelist)
{
    ParamWidget *widget = new ParamWidget(param,ui.scrollAreaWidgetContents);
    connect(widget,SIGNAL(doubleValueChanged(QString,double)),this,SLOT(doubleValueChanged(QString,double)));
    connect(widget,SIGNAL(intValueChanged(QString,int)),this,SLOT(intValueChanged(QString,int)));
    m_paramToWidgetMap[param] = widget;
    widget->setupCombo(title + "(" + param + ")",description,valuelist);
    ui.verticalLayout->addWidget(widget);
    widget->installEventFilter(QGCMouseWheelEventFilter::getFilter());
    widget->show();
    if (m_paramToValueMap.contains(param))
    {
        widget->setValue(m_paramToValueMap.value(param).toDouble());
        m_paramToValueMap.remove(param);
    }
}
void AdvancedParamConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (m_paramToWidgetMap.contains(parameterName))
    {
        if (value.type() == QVariant::Double || value.type() == QMetaType::Float)
        {
            m_paramToWidgetMap[parameterName]->setValue(value.toDouble());
        }
        else
        {
            m_paramToWidgetMap[parameterName]->setValue(value.toInt());
        }
    }
    else
    {
        m_paramToValueMap[parameterName] = value;
    }
}
void AdvancedParamConfig::doubleValueChanged(QString param,double value)
{
    if (!m_uas)
    {
        this->showNullMAVErrorMessageBox();
    }
    m_uas->getParamManager()->setParameter(1,param,value);
}

void AdvancedParamConfig::intValueChanged(QString param,int value)
{
    if (!m_uas)
    {
        this->showNullMAVErrorMessageBox();
    }
    m_uas->getParamManager()->setParameter(1,param,value);
}
