#ifndef SUBMAINWINDOW_H
#define SUBMAINWINDOW_H

#include <QMainWindow>

class SubMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit SubMainWindow(QWidget *parent = 0);
    virtual ~SubMainWindow();

    void setObjectName(const QString &name);

signals:
	
public slots:
	
};

#endif // SUBMAINWINDOW_H
