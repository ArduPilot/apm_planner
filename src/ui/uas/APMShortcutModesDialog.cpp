#include "APMShortcutModesDialog.h"
#include "ui_APMShortcutModesDialog.h"

APMShortcutModesDialog::APMShortcutModesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::APMShortcutModesDialog)
{
    ui->setupUi(this);
    opt1ComboBox()->addItem("none", -1);
    opt2ComboBox()->addItem("none", -1);
    opt3ComboBox()->addItem("none", -1);
    opt4ComboBox()->addItem("none", -1);
}

APMShortcutModesDialog::~APMShortcutModesDialog()
{
    delete ui;
}

QComboBox* APMShortcutModesDialog::opt1ComboBox()
{
    return ui->opt1ModeComboBox;
}

QComboBox* APMShortcutModesDialog::opt2ComboBox()
{
    return ui->opt2ModeComboBox;
}

QComboBox* APMShortcutModesDialog::opt3ComboBox()
{
    return ui->opt3ModeComboBox;
}

QComboBox* APMShortcutModesDialog::opt4ComboBox()
{
    return ui->opt4ModeComboBox;
}


