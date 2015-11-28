#include "DebugOutput.h"

#include <QScrollBar>

DebugOutput::DebugOutput(QWidget *parent) : QWidget(parent), QsLogging::Destination()
{
    ui.setupUi(this);
    ui.hashLineEdit->setText(define2string(GIT_HASH));
    ui.commitLineEdit->setText(define2string(GIT_COMMIT));
    connect(ui.onTopCheckBox,SIGNAL(clicked(bool)),this,SLOT(onTopCheckBoxChecked(bool)));
    connect(ui.copyPushButton,SIGNAL(clicked()),this,SLOT(copyToClipboardButtonClicked()));
}

DebugOutput::~DebugOutput()
{
}
void DebugOutput::write(const QString& message, QsLogging::Level level)
{
    Q_UNUSED(level)
    ui.textBrowser->append(message);
    if (ui.autoScrollCheckBox->isChecked())
    {
        QScrollBar *sb = ui.textBrowser->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
void DebugOutput::onTopCheckBoxChecked(bool checked)
{
    if (checked)
    {
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
        this->show();
    }
}
void DebugOutput::copyToClipboardButtonClicked()
{
    ui.textBrowser->selectAll();
    ui.textBrowser->copy();

}
