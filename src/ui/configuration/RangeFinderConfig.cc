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
#include "RangeFinderConfig.h"
#include <QMessageBox>

#include "QGCCore.h"

RangeFinderConfig::RangeFinderConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    // Disable scroll wheel from easily triggering settings change
    QList<QWidget*> widgetList = this->findChildren<QWidget*>();
    for (int i=0;i<widgetList.size();i++)
    {
        if (qobject_cast<QComboBox*>(widgetList[i]) || qobject_cast<QAbstractSpinBox*>(widgetList[i]) || qobject_cast<QAbstractSlider*>(widgetList[i]))
        {
            widgetList[i]->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    // Setup Ui
    ui.typeComboBox->addItem("None", 0 );
    ui.typeComboBox->addItem("Analog", 1 );
    ui.typeComboBox->addItem("APM2-MaxbotixI2C", 2 );
    ui.typeComboBox->addItem("APM2-PulsedLightI2C", 3) ;
    ui.typeComboBox->addItem("PX4-I2C", 4 );
    ui.typeComboBox->addItem("PX4-PWM", 5 );

    ui.functionComboBox->addItem("Linear", 0);
    ui.functionComboBox->addItem("Inverted", 1);
    ui.functionComboBox->addItem("Hyperbolic", 2);

    ui.gainSlider->setMinimum(001);
    ui.gainSlider->setMaximum(200);

    enableUi(false);

    AP2ConfigWidget::initConnections();
}

RangeFinderConfig::~RangeFinderConfig()
{
}

void RangeFinderConfig::activeUASSet(UASInterface *uas)
{
    if(m_uas){
        disconnect(m_uas, SIGNAL(rangeFinderUpdate(UASInterface*,double,double)),
                   this, SLOT(rangeFinderUpdate(UASInterface*,double,double)));

        disconnect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));

        disconnect(ui.functionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sendParameterUpdates()));
        disconnect(ui.gainSlider, SIGNAL(valueChanged(int)), this, SLOT(gainSliderChanged(int)));
        disconnect(ui.gainSlider, SIGNAL(sliderReleased()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.maxDistanceEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.minDistanceEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.offsetEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.inputPinEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.rmetricCheckBox, SIGNAL(stateChanged(int)), this, SLOT(sendParameterUpdates()));
        disconnect(ui.scalingEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.settleTimeEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        disconnect(ui.stopPinEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
    }
    m_uas = uas;
    if(m_uas){
        connect(m_uas, SIGNAL(rangeFinderUpdate(UASInterface*,double,double)),
                this, SLOT(rangeFinderUpdate(UASInterface*,double,double)));

        connect(m_uas,SIGNAL(parameterChanged(int,int,QString,QVariant)),
                   this,SLOT(parameterChanged(int,int,QString,QVariant)));

        connect(ui.typeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(rangeFinderTypeChanged(int)));
        connect(ui.functionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sendParameterUpdates()));
        connect(ui.gainSlider, SIGNAL(valueChanged(int)), this, SLOT(gainSliderChanged(int)));
        connect(ui.gainSlider, SIGNAL(sliderReleased()), this, SLOT(sendParameterUpdates()));
        connect(ui.maxDistanceEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        connect(ui.minDistanceEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        connect(ui.offsetEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        connect(ui.inputPinEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        connect(ui.rmetricCheckBox, SIGNAL(stateChanged(int)), this, SLOT(sendParameterUpdates()));
        connect(ui.scalingEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        connect(ui.settleTimeEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
        connect(ui.stopPinEdit, SIGNAL(editingFinished()), this, SLOT(sendParameterUpdates()));
    }
}

void RangeFinderConfig::sendParameterUpdates()
{
    if (!m_uas)
    {
        QMessageBox::information(0,tr("Error"),tr("Please connect to a MAV before attempting to set configuration"));
        return;
    }

    QGCUASParamManager* pm = m_uas->getParamManager();
    pm->setParameter(1,"RNGFND_TYPE", ui.typeComboBox->currentIndex());
    pm->setParameter(1,"RNGFND_FUNCTION", ui.functionComboBox->currentIndex());
    pm->setParameter(1,"RNGFND_GAIN", ui.gainSlider->value()/100.0);
    pm->setParameter(1,"RNGFND_MAX_CM", ui.maxDistanceEdit->text().toDouble());
    pm->setParameter(1,"RNGFND_MIN_CM", ui.minDistanceEdit->text().toDouble());
    pm->setParameter(1,"RNGFND_OFFSET", ui.offsetEdit->text().toDouble());
    pm->setParameter(1,"RNGFND_PIN", ui.inputPinEdit->text().toInt());
    int rmetric = ui.rmetricCheckBox->checkState() == Qt::Checked ? 1 : 0;
    pm->setParameter(1,"RNGFND_RMETRIC" ,rmetric);
    pm->setParameter(1,"RNGFND_SCALING", ui.scalingEdit->text().toDouble());
    pm->setParameter(1,"RNGFND_SETTLE_MS", ui.settleTimeEdit->text().toDouble());
    pm->setParameter(1,"RNGFND_STOP_PIN", ui.stopPinEdit->text().toInt());
}

void RangeFinderConfig::enableUi(bool state)
{
    ui.functionComboBox->setEnabled(state);
    ui.gainSlider->setEnabled(state);
    ui.maxDistanceEdit->setEnabled(state);
    ui.minDistanceEdit->setEnabled(state);
    ui.offsetEdit->setEnabled(state);
    ui.inputPinEdit->setEnabled(state);
    ui.rmetricCheckBox->setEnabled(state);
    ui.scalingEdit->setEnabled(state);
    ui.settleTimeEdit->setEnabled(state);
    ui.stopPinEdit->setEnabled(state);
}

void RangeFinderConfig::rangeFinderTypeChanged(int index)
{
    Q_UNUSED(index);
    sendParameterUpdates();
}

void RangeFinderConfig::gainSliderChanged(int value)
{
    ui.gainLabel->blockSignals(true);
    ui.gainLabel->setText(QString::number(value/100.0, 'g', 2));
    ui.gainLabel->blockSignals(false);
}

void RangeFinderConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if (parameterName.startsWith("RNGFND")){
        QLOG_DEBUG() << "RNGFND param:" << parameterName << " value:" << value;

        if (parameterName == "RNGFND_TYPE") {
            ui.typeComboBox->blockSignals(true);
            ui.typeComboBox->setCurrentIndex(value.toInt());
            ui.typeComboBox->blockSignals(false);

            if (value.toInt() == 0) {
                enableUi(false);
            } else {
                enableUi(true);
            }


        } else if (parameterName == "RNGFND_FUNCTION") {
            ui.functionComboBox->blockSignals(true);
            ui.functionComboBox->setCurrentIndex(value.toInt());
            ui.functionComboBox->blockSignals(false);

        } else if (parameterName == "RNGFND_GAIN") {
            ui.gainSlider->blockSignals(true);
            ui.gainLabel->blockSignals(true);
            ui.gainSlider->setValue(value.toDouble()*100);
            ui.gainLabel->setText(QString::number(value.toDouble(), 'g', 2));
            ui.gainSlider->blockSignals(false);
            ui.gainLabel->blockSignals(false);

        } else if (parameterName == "RNGFND_MAX_CM") {
            ui.maxDistanceEdit->blockSignals(true);
            ui.maxDistanceEdit->setText(value.toString());
            ui.maxDistanceEdit->blockSignals(false);

        } else if (parameterName == "RNGFND_MIN_CM") {
            ui.minDistanceEdit->blockSignals(true);
            ui.minDistanceEdit->setText(value.toString());
            ui.minDistanceEdit->blockSignals(false);

        } else if (parameterName == "RNGFND_OFFSET") {
            ui.offsetEdit->blockSignals(true);
            ui.offsetEdit->setText(value.toString());
            ui.offsetEdit->blockSignals(false);

        } else if (parameterName == "RNGFND_PIN") {
            ui.inputPinEdit->blockSignals(true);
            ui.inputPinEdit->setText(value.toString());
            ui.inputPinEdit->blockSignals(false);

        } else if (parameterName == "RNGFND_STOP_PIN") {
            ui.stopPinEdit->blockSignals(true);
            ui.stopPinEdit->setText(value.toString());
            ui.stopPinEdit->blockSignals(false);

        } else if (parameterName == "RNGFND_RMETRIC") {
            ui.rmetricCheckBox->blockSignals(true);
            ui.rmetricCheckBox->setChecked(value.toBool());
            ui.rmetricCheckBox->blockSignals(false);

        } else if (parameterName == "RNGFND_SCALING") {
            ui.scalingEdit->blockSignals(true);
            ui.scalingEdit->setText(value.toString());
            ui.scalingEdit->blockSignals(false);

        } else if (parameterName == "RNGFND_SETTLE_MS") {
            ui.settleTimeEdit->blockSignals(true);
            ui.settleTimeEdit->setText(value.toString());
            ui.settleTimeEdit->blockSignals(false);
        }

    }
}

void RangeFinderConfig::rangeFinderUpdate(UASInterface *uas, double distance, double voltage)
{
    Q_UNUSED(uas);
    ui.rangeLineEdit->setText(QString::number(distance, 'g', 2) + "m");
    ui.voltageLineEdit->setText(QString::number(voltage, 'g', 3));
}
