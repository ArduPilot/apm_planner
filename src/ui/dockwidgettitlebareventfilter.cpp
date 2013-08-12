#include "dockwidgettitlebareventfilter.h"

#include <QEvent>
DockWidgetTitleBarEventFilter::DockWidgetTitleBarEventFilter(QObject *parent) : QObject(parent)
{
}
bool DockWidgetTitleBarEventFilter::eventFilter(QObject *object,QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)
    {
        return true;
    }
    return QObject::eventFilter(object,event);
}
