
#include "ApmUiHelpers.h"
#include "ArduPilotMegaMAV.h"

void ApmUiHelpers::addRoverModes(QComboBox* comboBox)
{
    comboBox->clear();
    for (int count = 0; count < ApmRover::modeCount; count++){
        QString mode = ApmRover::stringForMode(count);
        if (!(mode.contains("Undefined", Qt::CaseInsensitive)
              || mode.contains("Reserved", Qt::CaseInsensitive))){
            // If mode valid add it and mode Id to comboBox
            comboBox->addItem(ApmRover::stringForMode(count), count);
        }
    }
}

void ApmUiHelpers::addPlaneModes(QComboBox* comboBox)
{
    comboBox->clear();
    for (int count = 0; count < ApmPlane::modeCount; count++){
        QString mode = ApmPlane::stringForMode(count);
        if (!(mode.contains("Undefined", Qt::CaseInsensitive)
              ||(mode.contains("Reserved", Qt::CaseInsensitive)))){
            // If mode valid add it and mode Id to comboBox
            comboBox->addItem(ApmPlane::stringForMode(count), count);
        }
    }
}

void ApmUiHelpers::addCopterModes(QComboBox* comboBox)
{
    comboBox->clear();
    for (int count = 0; count < ApmCopter::modeCount; count++){
        QString mode = ApmCopter::stringForMode(count);
        if (!(mode.contains("Undefined", Qt::CaseInsensitive)
              ||(mode.contains("Reserved", Qt::CaseInsensitive)))){
            // If mode valid add it and mode Id to comboBox
            comboBox->addItem(ApmCopter::stringForMode(count), count);
        }
    }
}
