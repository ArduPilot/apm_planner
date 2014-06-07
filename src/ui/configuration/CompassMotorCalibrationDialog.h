#ifndef COMPASSMOTORCALIBRATIONDIALOG_H
#define COMPASSMOTORCALIBRATIONDIALOG_H

#include <mavlink.h>
#include <QDialog>

class QCustomPlot;
class UASInterface;

namespace Ui {
class CompassMotorCalibrationDialog;
}

class CompassMotorCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompassMotorCalibrationDialog(QWidget *parent = 0);
    ~CompassMotorCalibrationDialog();

    void closeEvent(QCloseEvent *);

public slots:
    void activeUASSet(UASInterface* uas);
    void compassMotCalibration(mavlink_compassmot_status_t* compassmot_status);
    void textMessageReceived(int uasid, int componentid, int severity, QString text);

    void startCalibration();
    void stopCalibration();
    void okButtonClicked();

    void cancelCalibration();

private:
    Ui::CompassMotorCalibrationDialog *ui;

    UASInterface* m_uasInterface;
    double x_scalar;
    double y_scalar;
    double z_scalar;
};

#endif // COMPASSMOTORCALIBRATIONDIALOG_H
