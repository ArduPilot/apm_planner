#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include <QWidget>
#include <QsLogDestConsole.h>
#include "ui_DebugOutput.h"
#define define2string_p(x) #x
#define define2string(x) define2string_p(x)
class DebugOutput : public QWidget, public QsLogging::Destination
{
    Q_OBJECT
    
public:
    explicit DebugOutput(QWidget *parent = 0);
    ~DebugOutput();
    void write(const QString& message, QsLogging::Level level);
    bool isValid() { return true; }
private slots:
    void onTopCheckBoxChecked(bool checked);
private:
    Ui::DebugOutput ui;
};

#endif // DEBUGOUTPUT_H
