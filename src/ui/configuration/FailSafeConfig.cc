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

#include "FailSafeConfig.h"

#include "QGCCore.h"

FailSafeConfig::FailSafeConfig(QWidget *parent) : AP2ConfigWidget(parent)
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

    ui.radio1In->setName("Radio 1");
    ui.radio1In->setMin(800);
    ui.radio1In->setMax(2200);
    ui.radio1In->setOrientation(Qt::Horizontal);
    ui.radio2In->setName("Radio 2");
    ui.radio2In->setMin(800);
    ui.radio2In->setMax(2200);
    ui.radio2In->setOrientation(Qt::Horizontal);
    ui.radio3In->setName("Radio 3");
    ui.radio3In->setMin(800);
    ui.radio3In->setMax(2200);
    ui.radio3In->setOrientation(Qt::Horizontal);
    ui.radio4In->setName("Radio 4");
    ui.radio4In->setMin(800);
    ui.radio4In->setMax(2200);
    ui.radio4In->setOrientation(Qt::Horizontal);
    ui.radio5In->setName("Radio 5");
    ui.radio5In->setMin(800);
    ui.radio5In->setMax(2200);
    ui.radio5In->setOrientation(Qt::Horizontal);
    ui.radio6In->setName("Radio 6");
    ui.radio6In->setMin(800);
    ui.radio6In->setMax(2200);
    ui.radio6In->setOrientation(Qt::Horizontal);
    ui.radio7In->setName("Radio 7");
    ui.radio7In->setMin(800);
    ui.radio7In->setMax(2200);
    ui.radio7In->setOrientation(Qt::Horizontal);
    ui.radio8In->setName("Radio 8");
    ui.radio8In->setMin(800);
    ui.radio8In->setMax(2200);
    ui.radio8In->setOrientation(Qt::Horizontal);

    ui.radio1Out->setName("Radio 1");
    ui.radio1Out->setMin(800);
    ui.radio1Out->setMax(2200);
    ui.radio1Out->setOrientation(Qt::Horizontal);
    ui.radio2Out->setName("Radio 2");
    ui.radio2Out->setMin(800);
    ui.radio2Out->setMax(2200);
    ui.radio2Out->setOrientation(Qt::Horizontal);
    ui.radio3Out->setName("Radio 3");
    ui.radio3Out->setMin(800);
    ui.radio3Out->setMax(2200);
    ui.radio3Out->setOrientation(Qt::Horizontal);
    ui.radio4Out->setName("Radio 4");
    ui.radio4Out->setMin(800);
    ui.radio4Out->setMax(2200);
    ui.radio4Out->setOrientation(Qt::Horizontal);
    ui.radio5Out->setName("Radio 5");
    ui.radio5Out->setMin(800);
    ui.radio5Out->setMax(2200);
    ui.radio5Out->setOrientation(Qt::Horizontal);
    ui.radio6Out->setName("Radio 6");
    ui.radio6Out->setMin(800);
    ui.radio6Out->setMax(2200);
    ui.radio6Out->setOrientation(Qt::Horizontal);
    ui.radio7Out->setName("Radio 7");
    ui.radio7Out->setMin(800);
    ui.radio7Out->setMax(2200);
    ui.radio7Out->setOrientation(Qt::Horizontal);
    ui.radio8Out->setName("Radio 8");
    ui.radio8Out->setMin(800);
    ui.radio8Out->setMax(2200);
    ui.radio8Out->setOrientation(Qt::Horizontal);

    ui.throttleFailSafeComboBox->addItem("DISABLED");
    ui.throttleFailSafeComboBox->addItem("RTL");
    ui.throttleFailSafeComboBox->addItem("Continue in AUTO");
    ui.throttleFailSafeComboBox->addItem("LAND");

    ui.batteryFailSafeComboBox->addItem("DISABLED");
    ui.batteryFailSafeComboBox->addItem("LAND");
    ui.batteryFailSafeComboBox->addItem("RTL");
    
    connect(ui.fsLongCheckBox,SIGNAL(clicked(bool)),this,SLOT(fsLongClicked(bool)));
    connect(ui.fsShortCheckBox,SIGNAL(clicked(bool)),this,SLOT(fsShortClicked(bool)));
    connect(ui.gcsCheckBox,SIGNAL(clicked(bool)),this,SLOT(gcsChecked(bool)));
    connect(ui.throttleActionCheckBox,SIGNAL(clicked(bool)),this,SLOT(throttleActionChecked(bool)));
    connect(ui.throttleCheckBox,SIGNAL(clicked(bool)),this,SLOT(throttleChecked(bool)));
    connect(ui.throttlePwmSpinBox,SIGNAL(editingFinished()),this,SLOT(throttlePwmChanged()));
    connect(ui.batteryVoltSpinBox,SIGNAL(editingFinished()),this,SLOT(batteryVoltChanged()));
    connect(ui.throttleFailSafeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(throttleFailSafeChanged(int)));
    connect(ui.batteryCapSpinBox,SIGNAL(editingFinished()),this,SLOT(batteryCapChanged()));
    connect(ui.batteryFailSafeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(batteryFailSafeChanged(int)));
    
    ui.armedLabel->setText("<h1>DISARMED</h1>");


    ui.modeLabel->setText("<h1>MODE</h1>");

    ui.label_7->setVisible(false);
    ui.batteryCapSpinBox->setVisible(false);


    initConnections();
}

void FailSafeConfig::gcsChecked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->setParameter(1,"FS_GCS_ENABL",1);
    }
    else
    {
        m_uas->setParameter(1,"FS_GCS_ENABL",0);
    }
}

void FailSafeConfig::throttleActionChecked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->setParameter(1,"THR_FS_ACTION",1);
    }
    else
    {
        m_uas->setParameter(1,"THR_FS_ACTION",0);
    }
}

void FailSafeConfig::throttleChecked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->setParameter(1,"THR_FAILSAFE",1);
    }
    else
    {
        m_uas->setParameter(1,"THR_FAILSAFE",0);
    }
}

void FailSafeConfig::batteryVoltChanged()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->setParameter(1,m_lowVoltParam,ui.batteryVoltSpinBox->value());
}

void FailSafeConfig::batteryCapChanged()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->setParameter(1,"FS_BATT_MAH",ui.batteryCapSpinBox->value());
}

void FailSafeConfig::throttlePwmChanged()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (m_uas->isFixedWing())
    {
        m_uas->setParameter(1,"THR_FS_VALUE",ui.throttlePwmSpinBox->value());
    }
    else if (m_uas->isMultirotor())
    {
        m_uas->setParameter(1,"FS_THR_VALUE",ui.throttlePwmSpinBox->value());
    }
}

void FailSafeConfig::throttleFailSafeChanged(int index)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->setParameter(1,"FS_THR_ENABLE",index);
}

void FailSafeConfig::fsLongClicked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->setParameter(1,"FS_LONG_ACTN",1);
    }
    else
    {
        m_uas->setParameter(1,"FS_LONG_ACTN",0);
    }
}

void FailSafeConfig::fsShortClicked(bool checked)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (checked)
    {
        m_uas->setParameter(1,"FS_SHORT_ACTN",1);
    }
    else
    {
        m_uas->setParameter(1,"FS_SHORT_ACTN",0);
    }
}

void FailSafeConfig::batteryFailSafeChanged(int index)
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->setParameter(1,"FS_BATT_ENABLE",index);
}

FailSafeConfig::~FailSafeConfig()
{
}

void FailSafeConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(remoteControlChannelRawChanged(int,float)),this,SLOT(remoteControlChannelRawChanges(int,float)));
        disconnect(m_uas,SIGNAL(servoRawOutputChanged(uint64_t,float,float,float,float,float,float,float,float)),this,SLOT(hilActuatorsChanged(uint64_t,float,float,float,float,float,float,float,float)));
        disconnect(m_uas,SIGNAL(armingChanged(bool)),this,SLOT(armingChanged(bool)));
        disconnect(m_uas,SIGNAL(navModeChanged(int,int,QString)),this,SLOT(navModeChanged(int,int,QString)));
    }
    AP2ConfigWidget::activeUASSet(uas);
    if (!uas)
    {
        return;
    }
    connect(m_uas,SIGNAL(remoteControlChannelRawChanged(int,float)),this,SLOT(remoteControlChannelRawChanges(int,float)));
    connect(m_uas,SIGNAL(servoRawOutputChanged(uint64_t,float,float,float,float,float,float,float,float)),this,SLOT(hilActuatorsChanged(uint64_t,float,float,float,float,float,float,float,float)));
    connect(m_uas,SIGNAL(armingChanged(bool)),this,SLOT(armingChanged(bool)));
    connect(m_uas,SIGNAL(gpsLocalizationChanged(UASInterface*,int)),this,SLOT(gpsStatusChanged(UASInterface*,int)));
    connect(m_uas,SIGNAL(navModeChanged(int,int,QString)),this,SLOT(navModeChanged(int,int,QString)));

    if (m_uas->isFixedWing())
    {
        ui.batteryFailSafeComboBox->setVisible(false);
        ui.throttleFailSafeComboBox->setVisible(false);
        ui.batteryVoltSpinBox->setVisible(false);
        ui.label_6->setVisible(false);

        ui.throttlePwmSpinBox->setVisible(true); //Both

        ui.throttleCheckBox->setVisible(true);
        ui.throttleActionCheckBox->setVisible(true);
        ui.gcsCheckBox->setVisible(true);
        ui.fsLongCheckBox->setVisible(true);
        ui.fsShortCheckBox->setVisible(true);
    }
    else if (m_uas->isMultirotor())
    {
        ui.batteryFailSafeComboBox->setVisible(true);
        ui.throttleFailSafeComboBox->setVisible(true);
        ui.batteryVoltSpinBox->setVisible(true);
        ui.label_6->setVisible(true);

        ui.throttlePwmSpinBox->setVisible(true); //Both

        ui.throttleCheckBox->setVisible(false);
        ui.throttleActionCheckBox->setVisible(false);
        ui.gcsCheckBox->setVisible(false);
        ui.fsLongCheckBox->setVisible(false);
        ui.fsShortCheckBox->setVisible(false);
    }
    else
    {
        //Show all, just in case
        ui.batteryFailSafeComboBox->setVisible(true);
        ui.throttleFailSafeComboBox->setVisible(true);
        ui.batteryVoltSpinBox->setVisible(true);
        ui.throttlePwmSpinBox->setVisible(true); //Both
        ui.throttleCheckBox->setVisible(true);
        ui.throttleActionCheckBox->setVisible(true);
        ui.gcsCheckBox->setVisible(true);
        ui.fsLongCheckBox->setVisible(true);
        ui.fsShortCheckBox->setVisible(true);
    }

}
void FailSafeConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas)
    Q_UNUSED(component)

    //Arducopter
    if (parameterName == "FS_THR_ENABLE")
    {
        disconnect(ui.throttleFailSafeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(throttleFailSafeChanged(int)));
        ui.throttleFailSafeComboBox->setCurrentIndex(value.toInt());
        connect(ui.throttleFailSafeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(throttleFailSafeChanged(int)));
    }
    else if (parameterName == "FS_THR_VALUE")
    {
        ui.throttlePwmSpinBox->setValue(value.toFloat());
    }
    else if (parameterName == "FS_BATT_ENABLE")
    {
        disconnect(ui.batteryFailSafeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(batteryFailSafeChanged(int)));
        ui.batteryFailSafeComboBox->setCurrentIndex(value.toInt());
        connect(ui.batteryFailSafeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(batteryFailSafeChanged(int)));
    }
    else if (parameterName == "LOW_VOLT")
    {
        m_lowVoltParam = "LOW_VOLT";
        ui.batteryVoltSpinBox->setValue(value.toFloat());
    }
    else if (parameterName == "FS_BATT_VOLTAGE")
    {
        m_lowVoltParam = "FS_BATT_VOLTAGE";
        ui.batteryVoltSpinBox->setValue(value.toFloat());
    }
    else if (parameterName == "FS_BATT_MAH")
    {
        ui.label_7->setVisible(true);
        ui.batteryCapSpinBox->setVisible(true);
        ui.batteryCapSpinBox->setValue(value.toFloat());
    }
    //Arduplane
    else if (parameterName == "THR_FAILSAFE")
    {
        if (value.toInt() == 0)
        {
            ui.throttleCheckBox->setChecked(false);
        }
        else
        {
            ui.throttleCheckBox->setChecked(true);
        }
    }
    else if (parameterName == "THR_FS_VALUE")
    {
        ui.throttlePwmSpinBox->setValue(value.toFloat());
    }
    else if (parameterName == "THR_FS_ACTION")
    {
        if (value.toInt() == 0)
        {
            ui.throttleActionCheckBox->setChecked(false);
        }
        else
        {
            ui.throttleActionCheckBox->setChecked(true);
        }
    }
    else if (parameterName == "FS_GCS_ENABL")
    {
        if (value.toInt() == 0)
        {
            ui.gcsCheckBox->setChecked(false);
        }
        else
        {
            ui.gcsCheckBox->setChecked(true);
        }
    }
    else if (parameterName == "FS_SHORT_ACTN")
    {
        if (value.toInt() == 0)
        {
            ui.fsShortCheckBox->setChecked(false);
        }
        else
        {
            ui.fsShortCheckBox->setChecked(true);
        }
    }
    else if (parameterName == "FS_LONG_ACTN")
    {
        if (value.toInt() == 0)
        {
            ui.fsLongCheckBox->setChecked(false);
        }
        else
        {
            ui.fsLongCheckBox->setChecked(true);
        }
    }

}

void FailSafeConfig::armingChanged(bool armed)
{
    if (armed)
    {
        ui.armedLabel->setText("<h1>ARMED</h1>");
    }
    else
    {
        ui.armedLabel->setText("<h1>DISARMED</h1>");
    }
}

void FailSafeConfig::remoteControlChannelRawChanges(int chan,float value)
{
    if (chan == 0)
    {
        ui.radio1In->setValue(value);
    }
    else if (chan == 1)
    {
        ui.radio2In->setValue(value);
    }
    else if (chan == 2)
    {
        ui.radio3In->setValue(value);
    }
    else if (chan == 3)
    {
        ui.radio4In->setValue(value);
    }
    else if (chan == 4)
    {
        ui.radio5In->setValue(value);
    }
    else if (chan == 5)
    {
        ui.radio6In->setValue(value);
    }
    else if (chan == 6)
    {
        ui.radio7In->setValue(value);
    }
    else if (chan == 7)
    {
        ui.radio8In->setValue(value);
    }
}
void FailSafeConfig::hilActuatorsChanged(uint64_t time, float act1, float act2, float act3, float act4, float act5, float act6, float act7, float act8)
{
    Q_UNUSED(time)

    ui.radio1Out->setValue(act1);
    ui.radio2Out->setValue(act2);
    ui.radio3Out->setValue(act3);
    ui.radio4Out->setValue(act4);
    ui.radio5Out->setValue(act5);
    ui.radio6Out->setValue(act6);
    ui.radio7Out->setValue(act7);
    ui.radio8Out->setValue(act8);
}
void FailSafeConfig::gpsStatusChanged(UASInterface* uas,int fixtype)
{
    Q_UNUSED(uas)

    if (fixtype == 0 || fixtype == 1)
    {
        ui.gpsLabel->setText("<h1>GPS: No Fix</h1>");
    }
    else if (fixtype == 2)
    {
        ui.gpsLabel->setText("<h1>GPS: 2D Fix</h1>");
    }
    else if (fixtype == 3)
    {
        ui.gpsLabel->setText("<h1>GPS: 3D Fix</h1>");
    }
}
void FailSafeConfig::navModeChanged(int uasid, int mode, const QString& text)
{
    Q_UNUSED(uasid)
    Q_UNUSED(mode)

    ui.modeLabel->setText("<h1>" + text + "</h1>");
}
