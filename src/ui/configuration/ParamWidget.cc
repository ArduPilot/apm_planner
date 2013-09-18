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

#include "ParamWidget.h"

#include "QGCCore.h"

ParamWidget::ParamWidget(QString param,QWidget *parent) : QWidget(parent)
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

    m_param = param;

    connect(ui.doubleSpinBox,SIGNAL(editingFinished()),this,SLOT(doubleSpinEditFinished()));
    connect(ui.doubleSpinBox,SIGNAL(returnPressed()),this,SLOT(doubleSpinEditFinished()));
    connect(ui.intSpinBox,SIGNAL(editingFinished()),this,SLOT(intSpinEditFinished()));
    connect(ui.intSpinBox,SIGNAL(returnPressed()),this,SLOT(intSpinEditFinished()));
    connect(ui.valueComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboIndexChanged(int)));
    //connect(ui.valueSlider,SIGNAL(sliderReleased()),this,SLOT(valueSliderReleased()));
    connect(ui.valueSlider,SIGNAL(sliderPressed()),this,SLOT(valueSliderPressed()));
    connect(ui.valueSlider,SIGNAL(sliderReleased()),this,SLOT(valueSliderReleased()));
    connect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
}
void ParamWidget::doubleSpinEditFinished()
{
    disconnect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderReleased()));
    ui.valueSlider->setValue(((ui.doubleSpinBox->value() - m_min) / (m_max - m_min)) * (double)ui.valueSlider->maximum());
    connect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderReleased()));
    emit doubleValueChanged(m_param,ui.doubleSpinBox->value());
}

void ParamWidget::intSpinEditFinished()
{
    disconnect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
    ui.valueSlider->setValue(((ui.intSpinBox->value() - m_min) / (m_max - m_min)) * (double)ui.valueSlider->maximum());
    connect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
    emit intValueChanged(m_param,ui.intSpinBox->value());
}

void ParamWidget::comboIndexChanged(int index)
{
    emit intValueChanged(m_param,m_valueList[index].first);
}
void ParamWidget::valueSliderPressed()
{
    disconnect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
}

void ParamWidget::valueSliderReleased()
{
    valueSliderChanged();
    connect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
}

void ParamWidget::valueSliderChanged()
{
    //Set the spin box, and emit a signal.
    if (type == INT)
    {
        ui.intSpinBox->setValue((((double)ui.valueSlider->value() / (double)ui.valueSlider->maximum()) * (m_max - m_min)) + m_min);
        if (!ui.valueSlider->isSliderDown())
        {
            emit intValueChanged(m_param,ui.intSpinBox->value());
        }
    }
    else if (type == DOUBLE)
    {
        ui.doubleSpinBox->setValue((((double)ui.valueSlider->value() / (double)ui.valueSlider->maximum()) * (m_max - m_min)) + m_min);
        if (!ui.valueSlider->isSliderDown())
        {
            emit doubleValueChanged(m_param,ui.doubleSpinBox->value());
        }
    }
}

ParamWidget::~ParamWidget()
{
}

void ParamWidget::setupInt(QString title,QString description,int value,int min,int max)
{
    type = INT;
    ui.titleLabel->setText("<h3>" + title + "</h3>");
    ui.descriptionLabel->setText(description);
    ui.valueComboBox->hide();
    ui.valueSlider->show();
    ui.intSpinBox->show();
    ui.doubleSpinBox->hide();
    ui.minLabel->show();
    ui.maxLabel->show();
    if (min == 0 && max == 0)
    {
        m_min = 0;
        m_max = 65535;
    }
    else
    {
        m_min = min;
        m_max = max;
    }
    ui.intSpinBox->setMinimum(m_min);
    ui.intSpinBox->setMaximum(m_max);
    ui.minLabel->setText(QString::number(m_min));
    ui.maxLabel->setText(QString::number(m_max));
}

void ParamWidget::setupDouble(QString title,QString description,double value,double min,double max,double increment)
{
    type = DOUBLE;
    ui.titleLabel->setText("<h3>" + title + "</h3>");
    ui.descriptionLabel->setText(description);
    ui.valueComboBox->hide();
    ui.valueSlider->show();
    ui.intSpinBox->hide();
    ui.doubleSpinBox->show();
    ui.minLabel->show();
    ui.maxLabel->show();
    if (min == 0 && max == 0)
    {
        m_min = 0;
        m_max = 65535;
    }
    else
    {
        m_min = min;
        m_max = max;
    }
    QString minstr = "";
    QString maxstr = "";
    if (m_max > 100)
    {
        maxstr = QString::number(m_max,'f',0);
        minstr = QString::number(m_min,'f',0);
    }
    else if (m_max > 10)
    {
        maxstr = QString::number(m_max,'f',1);
        minstr = QString::number(m_min,'f',1);
    }
    else if (m_max > 1)
    {
        maxstr = QString::number(m_max,'f',2);
        minstr = QString::number(m_min,'f',2);
    }
    else
    {
        maxstr = QString::number(m_max,'f',3);
        minstr = QString::number(m_min,'f',3);
    }
    ui.doubleSpinBox->setDecimals(3);
    ui.minLabel->setText(minstr);
    ui.maxLabel->setText(maxstr);
    //ui.doubleSpinBox->setSingleStep(increment);
    ui.valueSlider->setSingleStep(increment);
    ui.valueSlider->setPageStep(increment * 10);
    ui.doubleSpinBox->setSingleStep(increment);
    ui.doubleSpinBox->setMinimum(m_min);
    ui.doubleSpinBox->setMaximum(m_max);
}

void ParamWidget::setupCombo(QString title,QString description,QList<QPair<int,QString> > list)
{
    type = COMBO;
    ui.titleLabel->setText("<h3>" + title + "</h3>");
    ui.descriptionLabel->setText(description);
    ui.valueComboBox->show();
    ui.valueSlider->hide();
    ui.intSpinBox->hide();
    ui.doubleSpinBox->hide();
    ui.minLabel->hide();
    ui.maxLabel->hide();
    m_valueList = list;
    ui.valueComboBox->clear();
    disconnect(ui.valueComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboIndexChanged(int)));
    for (int i=0;i<m_valueList.size();i++)
    {
        ui.valueComboBox->addItem(m_valueList[i].second);
    }
    connect(ui.valueComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboIndexChanged(int)));
}

void ParamWidget::setValue(double value)
{
    if (type == INT)
    {
        disconnect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
        ui.intSpinBox->setValue(value);
        ui.valueSlider->setValue(((value - m_min) / (m_max - m_min)) * (double)ui.valueSlider->maximum());
        connect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
    }
    else if (type == DOUBLE)
    {
        disconnect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
        ui.doubleSpinBox->setValue(value);
        ui.valueSlider->setValue(((value - m_min) / (m_max - m_min)) * (double)ui.valueSlider->maximum());
        connect(ui.valueSlider,SIGNAL(valueChanged(int)),this,SLOT(valueSliderChanged()));
    }
    else if (type == COMBO)
    {
        for (int i=0;i<m_valueList.size();i++)
        {
            if ((int)value == m_valueList[i].first)
            {
                disconnect(ui.valueComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboIndexChanged(int)));
                ui.valueComboBox->setCurrentIndex(i);
                connect(ui.valueComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(comboIndexChanged(int)));
                return;
            }
        }
    }
}
