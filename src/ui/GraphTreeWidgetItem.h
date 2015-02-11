#ifndef GRAPHTREEWIDGETITEM_H
#define GRAPHTREEWIDGETITEM_H

#include <QTreeWidgetItem>

class GraphTreeWidgetItem : public QTreeWidgetItem
{
public:
    explicit GraphTreeWidgetItem(QStringList list = QStringList());
    bool operator< (const QTreeWidgetItem &other ) const;
signals:

public slots:

};

#endif // GRAPHTREEWIDGETITEM_H
