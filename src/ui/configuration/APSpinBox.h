#ifndef APSPINBOX_H
#define APSPINBOX_H

#include <QSpinBox>

class APSpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit APSpinBox(QWidget *parent = 0);
protected:
    void keyReleaseEvent(QKeyEvent *evt);
signals:
    void returnPressed();
public slots:
    
};

#endif // APSPINBOX_H
