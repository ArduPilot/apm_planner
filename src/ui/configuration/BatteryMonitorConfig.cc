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
#include <QPushButton>
#include "QsLog.h"

BatteryPreset::BatteryPreset(const QString &title, BatteryMonitor batteryMonitor,
                             int voltPin, float voltDivider,
                             int currPin, float ampsPerVolt):
    m_title(title),
    m_battMonitorType(batteryMonitor),
    m_voltPin(voltPin),
    m_voltageDivider(voltDivider),
    m_currentPin(currPin),
    m_ampsPerVolt(ampsPerVolt)
{
}

inline QString BatteryPreset::title() const
{
    return m_title;
}

inline BatteryPreset::BatteryMonitor BatteryPreset::batteryMonitor()
{
    return m_battMonitorType;
}

inline int BatteryPreset::voltagePin()
{
    return m_voltPin;
}

inline float BatteryPreset::voltageDivider()
{
    return m_voltageDivider;
}

inline int BatteryPreset::currentPin()
{
    return m_currentPin;
}

inline float BatteryPreset::ampsPerVolt()
{
    return m_ampsPerVolt;
}

BatteryMonitorConfig::BatteryMonitorConfig(QWidget *parent) : AP2ConfigWidget(parent),
    m_maxVoltOut(3.3)
{
    ui.setupUi(this);


    connect(ui.calcDividerLineEdit,SIGNAL(editingFinished()),this,SLOT(calcDividerSet()));
    connect(ui.measuredVoltsLineEdit,SIGNAL(editingFinished()),this,SLOT(measuredVoltsSet()));
    connect(ui.measuredVoltsLineEdit_2,SIGNAL(editingFinished()),this,SLOT(measuredVoltsSetBasic()));
    connect(ui.ampsPerVoltsLineEdit,SIGNAL(editingFinished()),this,SLOT(ampsPerVoltSet()));
    connect(ui.battCapacityLineEdit,SIGNAL(editingFinished()),this,SLOT(batteryCapacitySet()));
    connect(ui.measuredCurrentLineEdit,SIGNAL(editingFinished()),this,SLOT(measuredCurrentSet()));
    connect(ui.measuredCurrentLineEdit_2,SIGNAL(editingFinished()),this,SLOT(measuredCurrentSetBasic()));

    initConnections();

    // Setup presets
    setupPresetSelectionTable();
}

void BatteryMonitorConfig::setupPresetSelectionTable()
{
    // Setup preset manually in code for now
    // but this would be good ot read from a file

    // Iris
    BatteryPreset *preset = new BatteryPreset("3DR Iris\n4 in 1 ESC",
                                              BatteryPreset::Both,
                                              2, 12.02f,  // voltPin, voltDivider
                                              3, 17.0f);  // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("APM2.0\nAttoPilot 90A",
                               BatteryPreset::Both,
                               1, 51.8f/3.3f,  // voltPin, voltDivider
                               2, 89.4f/3.3f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("APM2.0\nAttoPilot 180A",
                               BatteryPreset::Both,
                               1, 51.8f/3.3f,   // voltPin, voltDivider
                               2, 178.8f/3.3f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("APM 2.5/6\nPower Module",
                               BatteryPreset::Both,
                               13, 10.01f,  // voltPin, voltDivider
                               12, 18.0f);  // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("APM2.5/6\nAttoPilot 90A",
                               BatteryPreset::Both,
                               1, 51.8f/3.3f,  // voltPin, voltDivider
                               2, 89.4f/3.3f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("APM2.5/6\nAttoPilot 180A",
                               BatteryPreset::Both,
                               1, 51.8f/3.3f,   // voltPin, voltDivider
                               2, 178.8f/3.3f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("Pixhawk\nPower Module 90A",
                               BatteryPreset::Both,
                               2, 10.0f,  // voltPin, voltDivider
                               3, 17.0f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("PX4\nAttoPilot 90A",
                               BatteryPreset::Both,
                               100, 51.8f/3.3f,  // voltPin, voltDivider
                               101, 89.4f/3.3f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("PX4\nAttoPilot 180A",
                               BatteryPreset::Both,
                               100, 51.8f/3.3f,   // voltPin, voltDivider
                               101, 178.8f/3.3f); // currPin, ampsPerVolt
    m_presetList.append(preset);

    preset = new BatteryPreset("Pixhawk\nPower Module HV",
                               BatteryPreset::Both,
                               2, 12.02,   // voltPin, voltDivider
                               3, 39.877); // currPin, ampsPerVolt
    m_presetList.append(preset);

    m_signalMapper = new QSignalMapper(this);

    for (int i = 0; i < m_presetList.size(); ++i) {
        QPushButton *button = new QPushButton(m_presetList[i]->title());
        connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
        m_signalMapper->setMapping(button, static_cast<QObject*>(m_presetList.at(i)));
        ui.presetGridLayout->addWidget(button, i / 3, i % 3);
    }

    connect(m_signalMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(setNewParameters(QObject*)));
}

void BatteryMonitorConfig::setNewParameters(QObject *object)
{
    BatteryPreset *preset = dynamic_cast<BatteryPreset*>(object);
    if(preset){
        QLOG_DEBUG() << QString().sprintf("Set battMon:%d voltPin:%d voltDiv:%2.3f currPin:%d aPerV:%2.2f",
                                         preset->batteryMonitor(), preset->voltagePin(), preset->voltageDivider(),
                                         preset->currentPin(), preset->ampsPerVolt());

        m_uas->getParamManager()->setParameter(1,"BATT_MONITOR",preset->batteryMonitor());
        m_uas->getParamManager()->setParameter(1,"BATT_VOLT_PIN",preset->voltagePin());
        m_uas->getParamManager()->setParameter(1,"BATT_CURR_PIN",preset->currentPin());

        m_uas->getParamManager()->setParameter(1,m_voltDividerParam,preset->voltageDivider());
        m_uas->getParamManager()->setParameter(1,m_ampPerVoltParam,preset->ampsPerVolt());
    }
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
    ui.measuredVoltsLineEdit_2->setText(ui.measuredVoltsLineEdit->text());
}
void BatteryMonitorConfig::measuredVoltsSetBasic()
{
    ui.measuredVoltsLineEdit->setText(ui.measuredVoltsLineEdit_2->text());
    measuredVoltsSet();
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
    ui.measuredCurrentLineEdit_2->setText(ui.measuredCurrentLineEdit->text());
}
void BatteryMonitorConfig::measuredCurrentSetBasic()
{
    ui.measuredCurrentLineEdit->setText(ui.measuredCurrentLineEdit_2->text());
    measuredCurrentSet();
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


BatteryMonitorConfig::~BatteryMonitorConfig()
{
}
void BatteryMonitorConfig::batteryChanged(UASInterface* uas, double voltage, double current, double percent, int seconds)
{
    Q_UNUSED(uas);
    Q_UNUSED(percent);
    Q_UNUSED(seconds);

    // Update Basic Battery Tab
    ui.battVoltageLineEdit->setText(QString::number(voltage,'f',2));
    ui.battCurrentLineEdit->setText(QString::number(current,'f',2));

    // Update Advanced Tab
    ui.calcVoltsLineEdit->setText(QString::number(voltage,'f',2));
    if (ui.measuredVoltsLineEdit->text() == "")
    {
        ui.measuredVoltsLineEdit->setText(ui.calcVoltsLineEdit->text());
        ui.measuredVoltsLineEdit_2->setText(ui.calcVoltsLineEdit->text());
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
    Q_UNUSED(uas);
    Q_UNUSED(component);

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
        ui.voltPinLineEdit->setText(QString::number(ivalue));
    }
    else if (parameterName == "BATT_CURR_PIN")
    {
        QLOG_DEBUG() << "Received paramter BATT_CURR_PIN";
        //Unused at the moment, everything is off BATT_VOLT_PIN
        int ivalue = value.toInt();
        ui.currPinLineEdit->setText(QString::number(ivalue));
    }
}
