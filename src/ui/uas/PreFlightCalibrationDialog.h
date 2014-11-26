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
    QDoubleSpinBox* addParamSpinBox(const QString &paramName, QWidget* parent);
    void sendPreflightCalibrationMessage();

private:
    Ui::PreFlightCalibrationDialog *m_ui;

    UASInterface* m_uasInterface;
    QList<QDoubleSpinBox*> widgets;
};

#endif // PREFLIGHTCALIBRATIONDIALOG_H
