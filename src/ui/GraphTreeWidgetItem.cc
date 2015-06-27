#include "GraphTreeWidgetItem.h"

GraphTreeWidgetItem::GraphTreeWidgetItem(QStringList list) : QTreeWidgetItem(list)
{
}
bool GraphTreeWidgetItem::operator< (const QTreeWidgetItem &other ) const
{
    if (this->parent())
    {
        return false;
    }
    return QTreeWidgetItem::operator <(other);
}
