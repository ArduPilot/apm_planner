#include "UASQuickViewItem.h"
#include <QVBoxLayout>
#include <QDebug>
UASQuickViewItem::UASQuickViewItem(QWidget *parent) : QWidget(parent)
{

}
void UASQuickViewItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    emit showSelectDialog(title());
}
