#include "UASQuickViewItem.h"
#include <QVBoxLayout>
#include <QDebug>
UASQuickViewItem::UASQuickViewItem(QWidget *parent) : QWidget(parent)
{

}
void UASQuickViewItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    emit showSelectDialog(title());
}
