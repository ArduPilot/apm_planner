#ifndef PREFLIGHTCALIBRATIONDIALOG_H
#define PREFLIGHTCALIBRATIONDIALOG_H

#include <QDialog>

class UASInterface;
class QDoubleSpinBox;

namespace Ui {
class PreFlightCalibrationDialog;
}

class PreFlightCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreFlightCalibrationDialog(QWidget *parent = 0);
    ~PreFlightCalibrationDialog();

public slots:
    void activeUASSet(UASInterface* uasInterface);

private slots:
    void dialogAccepted();
    void dialogRejected();

    void spinBoxChanged(int index);

private:
    void addArdupilotMegaOptions();
    void addParamSpinBoxes();
    void addParamCheckBoxes(const QString &vehicleName, const QStringList &parameterList, bool exclusive);
    QWidget* createParamWidget(const QString &paramName, QList<QWidget*> &list, QWidget *parent, QWidget *controlWidget);

    void sendPreflightCalibrationMessage();
    float getCheckValue(QWidget *widget);

private:
    Ui::PreFlightCalibrationDialog *m_ui;

    UASInterface* m_uasInterface;
    QList<QWidget*> m_widgets;
    QList<float> m_param;
};

#endif // PREFLIGHTCALIBRATIONDIALOG_H
