#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include <QWidget>
#include <QsLogDestConsole.h>
#include "ui_DebugOutput.h"

class DebugOutput : public QWidget, public QsLogging::Destination
{
    Q_OBJECT
    
public:
    explicit DebugOutput(QWidget *parent = 0);
    ~DebugOutput();
    void write(const QString& message, QsLogging::Level level);
    bool isValid() { return true; }
private:
    Ui::DebugOutput ui;
};

#endif // DEBUGOUTPUT_H
