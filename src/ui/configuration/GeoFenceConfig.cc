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

#include "GeoFenceConfig.h"
#include "QsLog.h"

#define TO_METERS 100.0f
#define TO_CENTIMETERS 100.0f

GeoFenceConfig::GeoFenceConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    disableControls();

    // Init Connections
//    connectAllSignals();

    AP2ConfigWidget::initConnections();
}

void GeoFenceConfig::connectAllSignals()
{
    connect(ui.enableCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(enabledChangedState(int)));
    connect(ui.typeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(fenceTypeChanged(int)));
    connect(ui.actionComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(fenceActionChanged(int)));
    connect(ui.maxAltSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedMaxAlt()));
    connect(ui.rtlAltSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedRtlAlt()));
    connect(ui.radiusSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedRadius()));
    connect(ui.finalAltSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedFinalRtlAlt()));
}

void GeoFenceConfig::disconnectAllSignals()
{
    // Make disconnections
    disconnect(ui.enableCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(enabledChangedState(int)));
    disconnect(ui.typeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(fenceTypeChanged(int)));
    disconnect(ui.actionComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(fenceActionChanged(int)));
    disconnect(ui.maxAltSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedMaxAlt()));
    disconnect(ui.rtlAltSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedRtlAlt()));
    disconnect(ui.radiusSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedRadius()));
    disconnect(ui.finalAltSpinBox, SIGNAL(editingFinished()),
            this, SLOT(valueChangedFinalRtlAlt()));
}

void GeoFenceConfig::activeUASSet(UASInterface *uas)
{
    AP2ConfigWidget::activeUASSet(uas);
}

GeoFenceConfig::~GeoFenceConfig()
{
}

void GeoFenceConfig::enabledChangedState(int state)
{
    QLOG_DEBUG() << "Fence Status is set to " << state;

    if (!m_uas) {
        // disconnect previous connections.
        showNullMAVErrorMessageBox();
        return;
    }

    switch(state) {
    case Qt::Checked:
        m_uas->getParamManager()->setParameter(1,"FENCE_ENABLE", 1);
        break;
    case Qt::Unchecked:
        m_uas->getParamManager()->setParameter(1,"FENCE_ENABLE", 0);
        break;
    case Qt::PartiallyChecked:
    default:
        QLOG_ERROR() << "Error State";
    }
}

void GeoFenceConfig::fenceTypeChanged(int index)
{
    QLOG_DEBUG() << "GeoFenceConfigfenceTypeChanged:" << index;
    m_uas->getParamManager()->setParameter(1,"FENCE_TYPE", index);
}

void GeoFenceConfig::fenceActionChanged(int index)
{
    QLOG_DEBUG() << "GeoFenceConfig fenceActionChanged:" << index;
    m_uas->getParamManager()->setParameter(1,"FENCE_ACTION", index);
}

void GeoFenceConfig::valueChangedMaxAlt()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedMaxAlt:" << ui.maxAltSpinBox->value();
    m_uas->getParamManager()->setParameter(1,"FENCE_ALT_MAX", static_cast<float>(ui.maxAltSpinBox->value()));
}

void GeoFenceConfig::valueChangedRtlAlt()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedRtlAlt:" << ui.rtlAltSpinBox->value();
    m_uas->getParamManager()->setParameter(1,"RTL_ALT", static_cast<float>(ui.rtlAltSpinBox->value()*TO_CENTIMETERS));
}

void GeoFenceConfig::valueChangedFinalRtlAlt()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedRtlAlt:" << ui.finalAltSpinBox->value();
    m_uas->getParamManager()->setParameter(1,"RTL_ALT_FINAL", static_cast<float>(ui.finalAltSpinBox->value()*TO_CENTIMETERS));
}

void GeoFenceConfig::valueChangedRadius()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedRadius:" << ui.radiusSpinBox->value();
    m_uas->getParamManager()->setParameter(1,"FENCE_RADIUS", static_cast<float>(ui.radiusSpinBox->value()));
}

void GeoFenceConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(component);

    if (uas != m_uas->getUASID())
        return;

    disconnectAllSignals();

    if (parameterName.contains("FENCE_ENABLE")) {
        QLOG_DEBUG() << "Update FENCE_ENABLE" << value;

        if (value.toInt() == 1) {
            ui.enableCheckBox->setChecked(true);
            enableControls();
        } else {
            ui.enableCheckBox->setChecked(false);
            disableControls();
        }

    } else if(parameterName.contains("FENCE_TYPE")) {
        QLOG_DEBUG() << "Update FENCE_TYPE" << value;
        ui.typeComboBox->setCurrentIndex(value.toInt());

    } else if(parameterName.contains("FENCE_ACTION")) {
        QLOG_DEBUG() << "Update FENCE_ACTION" << value;
        ui.actionComboBox->setCurrentIndex(value.toInt());

    } else if(parameterName.contains("FENCE_ALT_MAX")) {
        QLOG_DEBUG() << "Update FENCE_ALT_MAX" << value;
        ui.maxAltSpinBox->setValue(value.toDouble());

    } else if(parameterName.contains("FENCE_RADIUS")) {
        QLOG_DEBUG() << "Update FENCE_RADIUS" << value;
        ui.radiusSpinBox->setValue(value.toDouble());

    } else if(parameterName.contains("FENCE_MARGIN")) {
        QLOG_DEBUG() << "Update FENCE_MARGIN" << value;
        // Currently not part of the 'users' settings
        // See advanced view to change.

    } else if(parameterName.contains("RTL_ALT_FINAL")) {
        QLOG_DEBUG() << "Update RTL_ALT_FINAL" << value.toDouble();
        double finalRtlAlt = (value.toDouble()/TO_METERS);
        ui.finalAltSpinBox->setValue(finalRtlAlt);

    } else if(parameterName.contains("RTL_ALT")) {
        QLOG_DEBUG() << "Update RTL_ALT" << value.toDouble();
        double rtlAlt = (value.toDouble()/TO_METERS);
        ui.rtlAltSpinBox->setValue(rtlAlt);

    }

    connectAllSignals();

}

void GeoFenceConfig::enableControls()
{
    QLOG_DEBUG() << "GeoFence: Enable controls";
    ui.actionComboBox->setDisabled(false);
    ui.typeComboBox->setDisabled(false);
    ui.maxAltSpinBox->setDisabled(false);
    ui.rtlAltSpinBox->setDisabled(false);
    ui.radiusSpinBox->setDisabled(false);
    ui.finalAltSpinBox->setDisabled(false);

}

void GeoFenceConfig::disableControls()
{
    QLOG_DEBUG() << "Disable Controls";
    ui.actionComboBox->setDisabled(true);
    ui.typeComboBox->setDisabled(true);
    ui.maxAltSpinBox->setDisabled(true);
    ui.rtlAltSpinBox->setDisabled(true);
    ui.radiusSpinBox->setDisabled(true);
    ui.finalAltSpinBox->setDisabled(true);
}

void GeoFenceConfig::showEvent(QShowEvent *)
{
    // Update of Parameters of show.
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_ENABLE");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_TYPE");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_ACTION");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_ALT_MAX");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_RADIUS");
//    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_MARGIN"); Not Being used currently
    m_uas->getParamManager()->requestParameterUpdate(1,"RTL_ALT");
    m_uas->getParamManager()->requestParameterUpdate(1,"RTL_ALT_FINAL");
}


