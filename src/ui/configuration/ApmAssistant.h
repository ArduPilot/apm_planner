#ifndef APMASSISTANT_H
#define APMASSISTANT_H

#include <QWidget>
#include <QList>
#include <QString>
#include <QUrl>
#include <QPushButton>

namespace Ui {
class ApmAssistant;
}

class ApmAssistant : public QWidget
{
    Q_OBJECT

public:
    explicit ApmAssistant(QWidget *parent = 0);
    ~ApmAssistant();

signals:
    void showAction(const QString& objectName);

public slots:
    void viewChanged(QWidget* widget);

private slots:
    void anchorClicked(QUrl url);

private:
    Ui::ApmAssistant *ui;
};

#endif // APMASSISTANT_H
