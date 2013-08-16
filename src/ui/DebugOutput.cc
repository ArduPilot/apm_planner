#include "DebugOutput.h"

#include <QScrollBar>

DebugOutput::DebugOutput(QWidget *parent) : QWidget(parent), QsLogging::Destination()
{
    ui.setupUi(this);
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
