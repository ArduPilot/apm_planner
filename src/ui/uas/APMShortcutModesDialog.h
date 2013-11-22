#ifndef APMSHORTCUTMODESDIALOG_H
#define APMSHORTCUTMODESDIALOG_H

#include <QDialog>
#include <QComboBox>

namespace Ui {
class APMShortcutModesDialog;
}

class APMShortcutModesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit APMShortcutModesDialog(QWidget *parent = 0);
    ~APMShortcutModesDialog();

    QComboBox* opt1ComboBox();
    QComboBox* opt2ComboBox();
    QComboBox* opt3ComboBox();
    QComboBox* opt4ComboBox();

private:
    Ui::APMShortcutModesDialog *ui;
};

#endif // APMSHORTCUTMODESDIALOG_H
