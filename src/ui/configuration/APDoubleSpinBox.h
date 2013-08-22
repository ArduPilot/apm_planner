#ifndef APDOUBLESPINBOX_H
#define APDOUBLESPINBOX_H

#include <QDoubleSpinBox>

class APDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit APDoubleSpinBox(QWidget *parent = 0);
protected:
    void keyReleaseEvent(QKeyEvent *evt);
signals:
    void returnPressed();
public slots:
    
};

#endif // APDOUBLESPINBOX_H
