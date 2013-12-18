#ifndef APMUIHELPERS_H
#define APMUIHELPERS_H

#include <QComboBox>

class ApmUiHelpers
{
public:
    static void addRoverModes(QComboBox* comboBox);
    static void addPlaneModes(QComboBox* comboBox);
    static void addCopterModes(QComboBox* comboBox);

};

#endif // APMUIHELPERS_H
