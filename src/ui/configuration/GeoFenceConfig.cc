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

GeoFenceConfig::GeoFenceConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    // Init Connections
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
}

void GeoFenceConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas) {
        // disconnect previous connections.

    }

    m_uas = uas;
    // Read Update of Parameters
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_ENABLE");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_TYPE");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_ACTION");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_ALT_MAX");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_RADIUS");
    m_uas->getParamManager()->requestParameterUpdate(1,"FENCE_MARGIN");
}

GeoFenceConfig::~GeoFenceConfig()
{
}

void GeoFenceConfig::fenceTypeChanged(int index)
{
    QLOG_DEBUG() << "GeoFenceConfigfenceTypeChanged:" << index;
}

void GeoFenceConfig::fenceActionChanged(int index)
{
    QLOG_DEBUG() << "GeoFenceConfig fenceActionChanged:" << index;
}

void GeoFenceConfig::valueChangedMaxAlt()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedMaxAlt:" << ui.maxAltSpinBox->value();
}

void GeoFenceConfig::valueChangedRtlAlt()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedRtlAlt:" << ui.rtlAltSpinBox->value();
}

void GeoFenceConfig::valueChangedRadius()
{
    QLOG_DEBUG() << "GeoFenceConfig valueChangedRadius:" << ui.radiusSpinBox->value();
}

void GeoFenceConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (parameterName.contains("FENCE_ENABLE")) {
        QLOG_DEBUG() << "Update FENCE_ENABLE" << value;

    } else if(parameterName.contains("FENCE_TYPE")) {
        QLOG_DEBUG() << "Update FENCE_TYPE" << value;

    } else if(parameterName.contains("FENCE_ACTION")) {
        QLOG_DEBUG() << "Update FENCE_ACTION" << value;

    } else if(parameterName.contains("FENCE_ALT_MAX")) {
        QLOG_DEBUG() << "Update FENCE_ALT_MAX" << value;

    } else if(parameterName.contains("FENCE_RADIUS")) {
        QLOG_DEBUG() << "Update FENCE_RADIUS" << value;

    } else if(parameterName.contains("FENCE_MARGIN")) {
        QLOG_DEBUG() << "Update FENCE_MARGIN" << value;

    }

}
