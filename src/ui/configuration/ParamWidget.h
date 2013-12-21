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

#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

#include <QWidget>
#include "ui_ParamWidget.h"

class ParamWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit ParamWidget(QString param,QWidget *parent = 0);
    ~ParamWidget();
    void setupInt(QString title,QString description,int value,int min,int max);
    void setupDouble(QString title,QString description,double value,double min,double max,double increment);
    void setupCombo(QString title,QString description,QList<QPair<int,QString> > list);
    void setValue(double value);
signals:
    void doubleValueChanged(QString param,double value);
    void intValueChanged(QString param,int value);
private slots:
    void doubleSpinEditFinished();
    void intSpinEditFinished();
    void comboIndexChanged(int index);
    void valueSliderPressed();
    void valueSliderReleased();
    void valueSliderChanged();
private:
    QPalette doubleSpinBoxPalette;
    QPalette intSpinBoxPalette;
    bool m_valueChanged;
    QString m_param;
    enum VIEWTYPE
    {
        INT,
        DOUBLE,
        COMBO
    };
    double m_min;
    double m_max;
    double m_dvalue;
    int m_ivalue;
    VIEWTYPE type;
    QList<QPair<int,QString> > m_valueList;
    Ui::ParamWidget ui;
};

#endif // PARAMWIDGET_H
