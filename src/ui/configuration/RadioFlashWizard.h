#ifndef RADIOFLASHWIZARD_H
#define RADIOFLASHWIZARD_H

#include <QWizard>

class QProcess;

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
    void selectPage(int index);

    void portSelectionShown();
    void flashRadio();
    void standardOutputReady();
    void processFinished(int exitCode);

private:
    Ui::RadioFlashWizard *ui;

    QProcess* m_updateProcess;

    QString m_portName;
    int m_baudRate;
    QString m_firmwareImage;
};

#endif // RADIOFLASHWIZARD_H
