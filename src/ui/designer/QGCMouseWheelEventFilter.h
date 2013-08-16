#ifndef QGCMOUSEWHEELEVENTFILTER_H
#define QGCMOUSEWHEELEVENTFILTER_H

#include <QObject>

class QGCMouseWheelEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit QGCMouseWheelEventFilter(QObject *parent = 0);
    static QGCMouseWheelEventFilter *getFilter();
    bool eventFilter(QObject *obj, QEvent *event);
signals:
    
public slots:
    
};

#endif // QGCMOUSEWHEELEVENTFILTER_H
