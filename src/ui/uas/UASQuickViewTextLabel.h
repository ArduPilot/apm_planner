#ifndef UASQUICKVIEWTEXTLABEL_H
#define UASQUICKVIEWTEXTLABEL_H

#include <QLabel>

class UASQuickViewTextLabel : public QLabel
{
    Q_OBJECT
public:
    explicit UASQuickViewTextLabel(QWidget *parent = 0);
    void setFontColor(QColor color);
    void setFontSize(int pixelsize);
private:
    QString m_styleSheetString;
    QColor m_fontColor;
    int m_fontSize;
signals:
    
public slots:
    
};

#endif // UASQUICKVIEWTEXTLABEL_H
