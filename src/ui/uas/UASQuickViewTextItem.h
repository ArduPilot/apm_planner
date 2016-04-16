#ifndef UASQUICKVIEWTEXTITEM_H
#define UASQUICKVIEWTEXTITEM_H

#include "UASQuickViewItem.h"
#include <QLabel>
#include <QSpacerItem>
#include "UASQuickViewTextLabel.h"
class UASQuickViewTextItem : public UASQuickViewItem
{
public:
    UASQuickViewTextItem(QWidget *parent=0);
    void setValue(double value);
    void setTitle(QString title);
    int minValuePixelSize();
    void setValuePixelSize(int size);
    QString value();
    QString title();
protected:
    void resizeEvent(QResizeEvent *event);
private:
    UASQuickViewTextLabel *titleLabel;
    UASQuickViewTextLabel *valueLabel;
};

#endif // UASQUICKVIEWTEXTITEM_H
