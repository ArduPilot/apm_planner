#include "UASQuickViewTextLabel.h"
#include <QDebug>
UASQuickViewTextLabel::UASQuickViewTextLabel(QWidget *parent) : QLabel(parent)
{
    m_styleSheetString = "UASQuickViewTextLabel{color:%1;font:%2;}";
}
void UASQuickViewTextLabel::setFontColor(QColor color)
{
    m_fontColor = color;
    this->setStyleSheet(m_styleSheetString.arg(m_fontColor.name(),QString::number(m_fontSize) + "px"));
}

void UASQuickViewTextLabel::setFontSize(int pixelsize)
{
    m_fontSize = pixelsize;
    this->setStyleSheet(m_styleSheetString.arg(m_fontColor.name(),QString::number(m_fontSize) + "px"));
}
