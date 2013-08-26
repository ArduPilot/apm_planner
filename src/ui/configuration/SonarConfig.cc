#include "SonarConfig.h"
#include <QMessageBox>

#include "QGCCore.h"

SonarConfig::SonarConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    ui.sonarTypeComboBox->addItem("XL-EZ0 / XL-EZ4");
    ui.sonarTypeComboBox->addItem("LV-EZ0");
    ui.sonarTypeComboBox->addItem("XL-EZL0");
    ui.sonarTypeComboBox->addItem("HRLV");
    connect(ui.enableCheckBox,SIGNAL(clicked(bool)),this,SLOT(checkBoxToggled(bool)));
    connect(ui.sonarTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sonarTypeChanged(int)));

    foreach (QObject *obj,this->children())
    {
        if (qobject_cast<QAbstractSlider*>(obj) || qobject_cast<QComboBox*>(obj) || qobject_cast<QAbstractSpinBox*>(obj))
        {
            obj->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }

    initConnections();
}

SonarConfig::~SonarConfig()
{
}
void SonarConfig::checkBoxToggled(bool enabled)
{
    if (enabled)
    {
        ui.sonarTypeComboBox->setEnabled(false);
    }
    if (!m_uas)
    {
        QMessageBox::information(0,tr("Error"),tr("Please connect to a MAV before attempting to set configuration"));
        return;
    }
    m_uas->getParamManager()->setParameter(1,"SONAR_ENABLE",ui.enableCheckBox->isChecked() ? 1 : 0);
}
void SonarConfig::sonarTypeChanged(int index)
{
    if (!m_uas)
    {
        QMessageBox::information(0,tr("Error"),tr("Please connect to a MAV before attempting to set configuration"));
        return;
    }
    m_uas->getParamManager()->setParameter(1,"SONAR_TYPE",ui.sonarTypeComboBox->currentIndex());
}

void SonarConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName == "SONAR_ENABLE")
    {
        if (value.toInt() == 0)
        {
            //Disabled
            ui.enableCheckBox->setChecked(false);
            ui.sonarTypeComboBox->setEnabled(false);
        }
        else
        {
            ui.enableCheckBox->setChecked(true);
            ui.sonarTypeComboBox->setEnabled(true);
        }
    }
    else if (parameterName == "SONAR_TYPE")
    {
        disconnect(ui.sonarTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sonarTypeChanged(int)));
        ui.sonarTypeComboBox->setCurrentIndex(value.toInt());
        connect(ui.sonarTypeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(sonarTypeChanged(int)));
    }
}
