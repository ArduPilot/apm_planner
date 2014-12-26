#include "RadioFlashWizard.h"
#include "ui_RadioFlashWizard.h"

#include "QsLog.h"

RadioFlashWizard::RadioFlashWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::RadioFlashWizard)
{
    ui->setupUi(this);

    connect(ui->flashPushButton, SIGNAL(clicked()), this, SLOT(flashRadio()));
}

RadioFlashWizard::~RadioFlashWizard()
{
    delete ui;
}

void RadioFlashWizard::accept()
{

}

void RadioFlashWizard::flashRadio()
{
    QLOG_DEBUG() << "Flash Radio:";


}
