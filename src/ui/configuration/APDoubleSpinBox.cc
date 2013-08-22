#include "APDoubleSpinBox.h"
#include <QKeyEvent>
APDoubleSpinBox::APDoubleSpinBox(QWidget *parent) : QDoubleSpinBox(parent)
{
}
void APDoubleSpinBox::keyReleaseEvent(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Enter)
    {
        emit returnPressed();
    }
}
