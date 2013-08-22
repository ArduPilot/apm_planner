#include "APSpinBox.h"
#include <QKeyEvent>

APSpinBox::APSpinBox(QWidget *parent) : QSpinBox(parent)
{
}
void APSpinBox::keyReleaseEvent(QKeyEvent *evt)
{
    if (evt->key() == Qt::Key_Enter)
    {
        emit returnPressed();
    }
}
