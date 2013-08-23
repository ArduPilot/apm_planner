#include "DebugOutput.h"

#include <QScrollBar>

DebugOutput::DebugOutput(QWidget *parent) : QWidget(parent), QsLogging::Destination()
{
    ui.setupUi(this);
    ui.hashLineEdit->setText(define2string(GIT_HASH));
    ui.commitLineEdit->setText(define2string(GIT_COMMIT));
}

DebugOutput::~DebugOutput()
{
}
void DebugOutput::write(const QString& message, QsLogging::Level level)
{
    ui.textBrowser->append(message);
    if (ui.autoScrollCheckBox->isChecked())
    {
        QScrollBar *sb = ui.textBrowser->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
