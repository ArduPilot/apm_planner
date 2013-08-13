#ifndef UASQUICKVIEWITEM_H
#define UASQUICKVIEWITEM_H

#include <QWidget>
#include <QLabel>
class UASQuickViewItem : public QWidget
{
    Q_OBJECT
public:
    explicit UASQuickViewItem(QWidget *parent = 0);
    virtual void setValue(double value)=0;
    virtual void setTitle(QString title)=0;
    virtual int minValuePixelSize()=0;
    virtual QString value()=0;
    virtual QString title()=0;
    virtual void setValuePixelSize(int size)=0;
protected:
    void mouseDoubleClickEvent(QMouseEvent *event);
signals:
    void showSelectDialog(QString current);
};

#endif // UASQUICKVIEWITEM_H
