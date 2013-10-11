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

#include "BatteryMonitorConfig.h"
#include <QMessageBox>
#include "QsLog.h"

BatteryMonitorConfig::BatteryMonitorConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_maxVoltOut(3.3)
{
    ui.setupUi(this);
    ui.monitorComboBox->addItem(tr("0: Disabled"),0);
    ui.monitorComboBox->addItem(tr("3: Battery Volts"),3);
    ui.monitorComboBox->addItem(tr("4: Voltage and Current"),4);

    ui.sensorComboBox->addItem(tr("0: Other"),0);
    ui.sensorComboBox->addItem(tr("1: AttoPilot 45A"),1);
    ui.sensorComboBox->addItem(tr("2: AttoPilot 90A"),2);
    ui.sensorComboBox->addItem(tr("3: AttoPilot 180A"),3);
    ui.sensorComboBox->addItem(tr("4: 3DR Power Module"),4);
    ui.sensorComboBox->addItem(tr("5: 3DR 4 in 1 ESC"),5);

    ui.apmVerComboBox->addItem(tr("0: APM1"));
    ui.apmVerComboBox->addItem(tr("1: APM2 - 2.5 non 3DR"));
    ui.apmVerComboBox->addItem(tr("2: APM2.5 - 3DR Power Module"));
    ui.apmVerComboBox->addItem(tr("3: PX4"));
    ui.apmVerComboBox->addItem(tr("4: Pixhawk - 3DR 4in1 ESC"));
    ui.apmVerComboBox->addItem(tr("5: Pixhawk - 3DR Power Module"));

    ui.alertOnLowCheckBox->setVisible(false); //Unimpelemented, but TODO.


    connect(ui.monitorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
    connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sensorCurrentIndexChanged(int)));
    connect(ui.apmVerComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(apmVerCurrentIndexChanged(int)));

    connect(ui.calcDividerLineEdit,SIGNAL(editingFinished()),this,SLOT(calcDividerSet()));
    connect(ui.measuredVoltsLineEdit,SIGNAL(editingFinished()),this,SLOT(measuredVoltsSet()));
    connect(ui.ampsPerVoltsLineEdit,SIGNAL(editingFinished()),this,SLOT(ampsPerVoltSet()));
    connect(ui.battCapacityLineEdit,SIGNAL(editingFinished()),this,SLOT(batteryCapacitySet()));
    connect(ui.measuredCurrentLineEdit,SIGNAL(editingFinished()),this,SLOT(measuredCurrentSet()));


    initConnections();
}
void BatteryMonitorConfig::measuredVoltsSet()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    bool ok = false;
    float measured = ui.measuredVoltsLineEdit->text().toFloat(&ok);
    if (!ok)
    {
        QMessageBox::information(0,"Error","Invalid number entered for measured voltage. Please try again");
        return;
    }
    float calced = ui.calcVoltsLineEdit->text().toFloat(&ok);
    float divider = ui.calcDividerLineEdit->text().toFloat(&ok);
    float newval = (measured * divider)/ calced;
    disconnect(ui.calcDividerLineEdit,SIGNAL(editingFinished()),this,SLOT(calcDividerSet()));
    ui.calcDividerLineEdit->setText(QString::number(newval,'f',6));
    m_uas->getParamManager()->setParameter(1,m_voltDividerParam,newval);
    connect(ui.calcDividerLineEdit,SIGNAL(editingFinished()),this,SLOT(calcDividerSet()));
}

void BatteryMonitorConfig::measuredCurrentSet()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    bool ok = false;
    float measured = ui.measuredCurrentLineEdit->text().toFloat(&ok);
    QLOG_DEBUG() << "Measured Current: " << measured;
    if (!ok)
    {
        QMessageBox::information(0,"Error","Invalid number entered for measured current. Please try again");
        return;
    }
    float calced = ui.batteryCurrentLineEdit->text().toFloat(&ok);
    float divider = ui.ampsPerVoltsLineEdit->text().toFloat(&ok);
    float newval = (measured * divider)/ calced;
    ui.ampsPerVoltsLineEdit->setText(QString::number(newval,'f',6));
    m_uas->getParamManager()->setParameter(1,m_ampPerVoltParam,newval);
}

void BatteryMonitorConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(batteryChanged(UASInterface*,double,double,double,int)),this,SLOT(batteryChanged(UASInterface*,double,double,double,int)));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas)
    {
        return;
    }
    connect(uas,SIGNAL(batteryChanged(UASInterface*,double,double,double,int)),this,SLOT(batteryChanged(UASInterface*,double,double,double,int)));

}
void BatteryMonitorConfig::alertOnLowClicked(bool checked)
{
}

void BatteryMonitorConfig::calcDividerSet()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    bool ok = false;

    float newval = ui.calcDividerLineEdit->text().toFloat(&ok);
    if (!ok)
    {
        //Error parsing;
        QMessageBox::information(0,"Error","Invalid number entered for voltage divider. Please try again");
        return;
    }
    m_uas->getParamManager()->setParameter(1,m_voltDividerParam,newval);
}
void BatteryMonitorConfig::ampsPerVoltSet()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    bool ok = false;
    float newval = ui.ampsPerVoltsLineEdit->text().toFloat(&ok);
    if (!ok)
    {
        //Error parsing;
        QMessageBox::information(0,"Error","Invalid number entered for amps per volts. Please try again");
        return;
    }
    m_uas->getParamManager()->setParameter(1,m_ampPerVoltParam,newval);
}
void BatteryMonitorConfig::batteryCapacitySet()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    bool ok = false;
    float newval = ui.battCapacityLineEdit->text().toFloat(&ok);
    if (!ok)
    {
        //Error parsing;
        QMessageBox::information(0,"Error","Invalid number entered for amps per volts. Please try again");
        return;
    }
    m_uas->getParamManager()->setParameter(1,"BATT_CAPACITY",newval);
}

void BatteryMonitorConfig::monitorCurrentIndexChanged(int index)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (index == 0) //Battery Monitor Disabled
    {
        QLOG_DEBUG() << "Battery Monitor Disabled";
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",-1);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",-1);
        m_uas->getParamManager()->setParameter(1,"BATT_MONITOR",0);
        ui.sensorComboBox->setEnabled(false);
        ui.apmVerComboBox->setEnabled(false);
        ui.measuredVoltsLineEdit->setEnabled(false);
        ui.measuredVoltsLineEdit->setEnabled(false);
        ui.calcDividerLineEdit->setEnabled(false);
        ui.calcVoltsLineEdit->setEnabled(false);
        ui.ampsPerVoltsLineEdit->setEnabled(false);
    }
    else if (index == 1) //Monitor voltage only
    {
        QLOG_DEBUG() << "Monitor Voltage only";
        m_uas->getParamManager()->setParameter(1,"BATT_MONITOR",3);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",-1);
        ui.sensorComboBox->setEnabled(false);
        ui.apmVerComboBox->setEnabled(true);
        ui.measuredVoltsLineEdit->setEnabled(true);
        ui.calcDividerLineEdit->setEnabled(true);
        ui.calcVoltsLineEdit->setEnabled(false);
        ui.ampsPerVoltsLineEdit->setEnabled(false);
        apmVerCurrentIndexChanged(ui.apmVerComboBox->currentIndex());
    }
    else if (index == 2) //Monitor voltage and current
    {
        QLOG_DEBUG() << "Monitor Voltage & Current";
        m_uas->getParamManager()->setParameter(1,"BATT_MONITOR",4);
        ui.sensorComboBox->setEnabled(true);
        ui.apmVerComboBox->setEnabled(true);
        ui.measuredVoltsLineEdit->setEnabled(true);
        ui.calcDividerLineEdit->setEnabled(true);
        ui.calcVoltsLineEdit->setEnabled(false);
        ui.ampsPerVoltsLineEdit->setEnabled(true);
        apmVerCurrentIndexChanged(ui.apmVerComboBox->currentIndex());
    }


}
void BatteryMonitorConfig::sensorCurrentIndexChanged(int index)
{
    QLOG_DEBUG() << "sensorCurrentIndexChanged" << index;
    m_maxVoltOut = 3.3;
    float maxvolt = 0.0;
    float maxamps = 0.0;
    float mvpervolt = 0.0;
    float mvperamp = 0.0;
    float topvolt = 0.0;
    float topamps = 0.0;

    if (index == 1)
    {
        //atto 45 see https://www.sparkfun.com/products/10643
        maxvolt = 13.6;
        maxamps = 44.7;
    }
    else if (index == 2)
    {
        //atto 90 see https://www.sparkfun.com/products/9028
        maxvolt = 51.8;
        maxamps = 89.4;
    }
    else if (index == 3)
    {
        //atto 180 see https://www.sparkfun.com/products/10644
        maxvolt = 51.8;
        maxamps = 178.8;
    }
    else if (index == 4)
    {
        //3dr
        maxvolt = 50.0;
        maxamps = 90.0;
        m_maxVoltOut = 5.0;
    }
    else if (index == 5)
    {
        //3dr 4 in 1
        maxvolt = 39.67;
        maxamps = 56.1;
    }
    mvpervolt = calculatemVPerVolt(m_maxVoltOut,maxvolt);
    mvperamp = calculatemVPerAmp(m_maxVoltOut,maxamps);
    if (index == 0)
    {
        //Other
        ui.ampsPerVoltsLineEdit->setEnabled(true);
        ui.calcDividerLineEdit->setEnabled(true);
        ui.measuredVoltsLineEdit->setEnabled(true);
    }
    else
    {
        topvolt = (maxvolt * mvpervolt) / 1000.0;
        topamps = (maxamps * mvperamp) / 1000.0;
        ui.calcDividerLineEdit->setText(QString::number(maxvolt/topvolt));
        ui.ampsPerVoltsLineEdit->setText(QString::number(maxamps / topamps));
        m_uas->getParamManager()->setParameter(1,m_ampPerVoltParam,(float)(maxamps/topamps));
        m_uas->getParamManager()->setParameter(1,m_voltDividerParam,(float)(maxvolt/topvolt));
        ui.ampsPerVoltsLineEdit->setEnabled(false);
        ui.calcDividerLineEdit->setEnabled(false);
        ui.measuredVoltsLineEdit->setEnabled(false);

    }
}
float BatteryMonitorConfig::calculatemVPerAmp(float maxvoltsout,float maxamps)
{
    QLOG_DEBUG() << "calculatemVPerAmp: maxvoltsout" << maxvoltsout << "maxamps:" << maxamps;
    return (1000.0 * (maxvoltsout/maxamps));
}

float BatteryMonitorConfig::calculatemVPerVolt(float maxvoltsout,float maxvolts)
{
    QLOG_DEBUG() << "calculatemVPerVolt: maxvoltsout" << maxvoltsout << "maxvolts:" << maxvolts;
    return (1000.0 * (maxvoltsout/maxvolts));
}

void BatteryMonitorConfig::apmVerCurrentIndexChanged(int index)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (index == 0) //APM1
    {
        QLOG_DEBUG() << "APM1 settings";
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",0);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",1);
    }
    else if (index == 1) //APM2
    {
        QLOG_DEBUG() << "APM2 settings";
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",1);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",2);
    }
    else if (index == 2) //APM2.5
    {
        QLOG_DEBUG() << "APM2.5 Power Module settings";
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",13);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",12);
    }
    else if (index == 3) //PX4
    {
        QLOG_DEBUG() << "PX4 settings";
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",100);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",101);
        m_uas->getParamManager()->setParameter(1,m_voltDividerParam,1);
        ui.calcDividerLineEdit->setText("1");
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
        ui.sensorComboBox->setCurrentIndex(0); //PX4 must be other
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
    }
    else if (index == 4) //Pixhawk - 4in1 ESC
    {
        QLOG_DEBUG() << "Pixhawk settings";
        m_uas->getParamManager()->setParameter(1,"BATT_MONITOR",4);
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",2);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",3);
        m_uas->getParamManager()->setParameter(1,m_ampPerVoltParam,(float)17);
        m_uas->getParamManager()->setParameter(1,m_voltDividerParam,(float)12.02);
        ui.calcDividerLineEdit->setText("12.02");
        ui.ampsPerVoltsLineEdit->setText("17.0");
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
        ui.sensorComboBox->setCurrentIndex(0); //Pixhawk using 3DR Power Module
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
    }
    else if (index == 5) //Pixhawk 3DR Power Module
    {
        QLOG_DEBUG() << "Pixhawk settings";
        m_uas->getParamManager()->setParameter(1,"BATT_MONITOR",4);
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",2);
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",3);
        m_uas->getParamManager()->setParameter(1,m_ampPerVoltParam,(float)17);
        m_uas->getParamManager()->setParameter(1,m_voltDividerParam,(float)10.1);
        ui.calcDividerLineEdit->setText("10.1");
        ui.ampsPerVoltsLineEdit->setText("17.0");
        disconnect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
        ui.sensorComboBox->setCurrentIndex(0); //Pixhawk using 3DR Power Module
        connect(ui.sensorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
    }
}

BatteryMonitorConfig::~BatteryMonitorConfig()
{
}
void BatteryMonitorConfig::batteryChanged(UASInterface* uas, double voltage, double current, double percent, int seconds)
{
    ui.calcVoltsLineEdit->setText(QString::number(voltage,'f',2));
    if (ui.measuredVoltsLineEdit->text() == "")
    {
        ui.measuredVoltsLineEdit->setText(ui.calcVoltsLineEdit->text());
    }
    ui.batteryCurrentLineEdit->setText(QString::number(current,'f',2));
}

void BatteryMonitorConfig::checkSensorType()
{
//    if (m_savedAmpsPerVolts < 14 && m_savedAmpsPerVolts > 13 && m_savedVoltDivider < 5 && m_savedVoltDivider > 4)
//    {
//        //atto45
//        QLOG_DEBUG() << "Setting ATTO 45 sensor";
//        ui.sensorComboBox->setCurrentIndex(1);
//    }
//    else if (m_savedAmpsPerVolts < 28 && m_savedAmpsPerVolts > 27 && m_savedVoltDivider < 16 && m_savedVoltDivider > 15)
//    {
//        //atto90
//        QLOG_DEBUG() << "Setting ATTO 90 sensor";
//        ui.sensorComboBox->setCurrentIndex(2);
//    }
//    else if (m_savedAmpsPerVolts < 55 && m_savedAmpsPerVolts > 54 && m_savedVoltDivider < 16 && m_savedVoltDivider > 15)
//    {
//        //atto180
//        QLOG_DEBUG() << "Setting ATTO 180 sensor";
//        ui.sensorComboBox->setCurrentIndex(3);
//    }
//    else if (m_savedAmpsPerVolts < 19 && m_savedAmpsPerVolts > 18 && m_savedVoltDivider < 11 && m_savedVoltDivider > 9)
//    {
//        //3dr
//        QLOG_DEBUG() << "Setting 3DR Power Module sensor";
//        ui.sensorComboBox->setCurrentIndex(4);
//    }
//    else if (m_savedAmpsPerVolts < 18 && m_savedAmpsPerVolts > 16 && m_savedVoltDivider < 13 && m_savedVoltDivider > 12)
//    {
//        //pixhawk with 3dr
//        QLOG_DEBUG() << "Setting 3DR with Pixhawk sensor";
//        ui.sensorComboBox->setCurrentIndex(4);
//    }
}

void BatteryMonitorConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName == "VOLT_DIVIDER")
    {
        QLOG_DEBUG() << "Received VOLT_DIVIDER parameter";
        m_voltDividerParam = parameterName;
        ui.calcDividerLineEdit->setText(QString::number(value.toFloat(),'f',6));
        m_savedVoltDivider = value.toFloat();
        checkSensorType();
    }
    else if (parameterName == "BATT_VOLT_MULT")
    {
        QLOG_DEBUG() << "Received BATT_VOLT_MULT parameter";
        m_voltDividerParam = parameterName;
        ui.calcDividerLineEdit->setText(QString::number(value.toFloat(),'f',6));
        m_savedVoltDivider = value.toFloat();
        checkSensorType();
    }
    else if (parameterName == "AMP_PER_VOLT")
    {
        QLOG_DEBUG() << "Received AMP_PER_VOLT parameter";
        m_ampPerVoltParam = parameterName;
        ui.ampsPerVoltsLineEdit->setText(QString::number(value.toFloat(),'f',4));
        m_savedAmpsPerVolts = value.toFloat();
        checkSensorType();
    }
    else if (parameterName == "BATT_AMP_PERVOLT")
    {
        QLOG_DEBUG() << "Received BATT_AMP_PERVOLT parameter";
        m_ampPerVoltParam = parameterName;
        ui.ampsPerVoltsLineEdit->setText(QString::number(value.toFloat(),'f',4));
        m_savedAmpsPerVolts = value.toFloat();
        checkSensorType();

    }
    else if (parameterName == "BATT_MONITOR")
    {
        QLOG_DEBUG() << "Received BATT_MONITOR combobox";
        disconnect(ui.monitorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
        if (value.toInt() == 0) //0: Disable
        {
            ui.monitorComboBox->setCurrentIndex(0);
        }
        else if (value.toInt() == 3) //3: Battery volts
        {
            ui.monitorComboBox->setCurrentIndex(1);
        }
        else if (value.toInt() == 4) //4: Voltage and Current
        {
            ui.monitorComboBox->setCurrentIndex(2);
        }
        connect(ui.monitorComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(monitorCurrentIndexChanged(int)));
    }
    else if (parameterName == "BATT_CAPACITY")
    {
        QLOG_DEBUG() << "Received BATT_CAPACITY" << QString::number(value.toFloat());
        ui.battCapacityLineEdit->setText(QString::number(value.toFloat()));
    }
    else if (parameterName == "BATT_VOLT_PIN")
    {
        int ivalue = value.toInt();
        QLOG_DEBUG() << "Received BATT_VOLT_PIN combo index to:" << ivalue;

        disconnect(ui.apmVerComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(apmVerCurrentIndexChanged(int)));
        if (ivalue == 0) //APM1
        {
            ui.apmVerComboBox->setCurrentIndex(0);
        }
        else if (ivalue == 1) //APM2
        {
            ui.apmVerComboBox->setCurrentIndex(1);
        }
        else if (ivalue == 2) //Pixhawk
        {
            ui.apmVerComboBox->setCurrentIndex(4);
        }
        else if (ivalue == 13) //APM2.5
        {
            ui.apmVerComboBox->setCurrentIndex(2);
        }
        else if (ivalue == 100) //PX4
        {
            ui.apmVerComboBox->setCurrentIndex(3);
        }
        connect(ui.apmVerComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(apmVerCurrentIndexChanged(int)));
    }
    else if (parameterName == "BATT_CURR_PIN")
    {
        QLOG_DEBUG() << "Received paramter BATT_CURR_PIN";
        //Unused at the moment, everything is off BATT_VOLT_PIN
    }
}
