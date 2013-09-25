#ifndef SETUPWARNINGMESSAGE_H
#define SETUPWARNINGMESSAGE_H

#include <QWidget>

namespace Ui {
class SetupWarningMessage;
}

class SetupWarningMessage : public QWidget
{
    Q_OBJECT

public:
    explicit SetupWarningMessage(QWidget *parent = 0);
    ~SetupWarningMessage();

private:
    Ui::SetupWarningMessage *ui;
};

#endif // SETUPWARNINGMESSAGE_H
