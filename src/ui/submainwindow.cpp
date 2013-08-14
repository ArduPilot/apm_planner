#include "submainwindow.h"
#include "QsLog.h"


SubMainWindow::SubMainWindow(QWidget *parent) : QMainWindow(parent)
{
    QLOG_INFO() << "Creating SubMainWindow: " <<  this;
}

void SubMainWindow::setObjectName(const QString &name)
{
    QLOG_INFO() << "setting objectName: " << name;
    QMainWindow::setObjectName(name);
}

SubMainWindow::~SubMainWindow()
{
    QLOG_INFO() << "Closing SubMainWindow: " <<  this << objectName();
}

