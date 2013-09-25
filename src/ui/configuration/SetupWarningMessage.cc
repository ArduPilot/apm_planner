#include "SetupWarningMessage.h"
#include "ui_SetupWarningMessage.h"

SetupWarningMessage::SetupWarningMessage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetupWarningMessage)
{
    ui->setupUi(this);
}

SetupWarningMessage::~SetupWarningMessage()
{
    delete ui;
}
