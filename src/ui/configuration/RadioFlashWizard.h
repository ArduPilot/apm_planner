#ifndef RADIOFLASHWIZARD_H
#define RADIOFLASHWIZARD_H

#include <QWizard>

namespace Ui {
class RadioFlashWizard;
}

class RadioFlashWizard : public QWizard
{
    Q_OBJECT

public:
    explicit RadioFlashWizard(QWidget *parent = 0);
    ~RadioFlashWizard();

    void accept();

private slots:
    void flashRadio();

private:
    Ui::RadioFlashWizard *ui;

    QString m_portName;
    int m_baudRate;
    QString m_firmwareImage;
};

#endif // RADIOFLASHWIZARD_H
