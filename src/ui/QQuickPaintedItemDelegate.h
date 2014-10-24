#ifndef QQuickPaintedItemDelegate_H
#define QQuickPaintedItemDelegate_H

#include <QQuickItem>
#include <QQuickPaintedItem>

class QQuickPaintedItemDelegate : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit QQuickPaintedItemDelegate(QQuickItem *parent = 0);
    void paint(QPainter *painter);

signals:
    void paintSignal(QPainter *painter);
};

#endif // QQuickPaintedItemDelegate_H
