#include "QGCMouseWheelEventFilter.h"

#include <QEvent>
#include <QAbstractSpinBox>
#include <QComboBox>
#include <QAbstractSpinBox>
#include "QGCCore.h"
QGCMouseWheelEventFilter::QGCMouseWheelEventFilter(QObject *parent) : QObject(parent)
{
}
bool QGCMouseWheelEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    if(qobject_cast<QComboBox*>(obj) || qobject_cast<QAbstractSpinBox*>(obj) || qobject_cast<QAbstractSlider*>(obj))
    {
        if(event->type() == QEvent::Wheel)
        {
            event->ignore();
            return true;
        }
    }
    return QObject::eventFilter(obj, event);
}
QGCMouseWheelEventFilter *QGCMouseWheelEventFilter::getFilter()
{
    return qobject_cast<QGCCore*>(QGCCore::instance())->getMouseWheelFilter();
}
