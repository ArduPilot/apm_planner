#ifndef COMPASSONBOARDCALIBRATIONDIALOG_H
#define COMPASSONBOARDCALIBRATIONDIALOG_H

#include <mavlink_types.h>
extern mavlink_status_t m_mavlink_status[MAVLINK_COMM_NUM_BUFFERS];  // defined in src/main.cc
#include <mavlink.h>

#include <QDialog>
#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QScatter3DSeries>
#include <QtDataVisualization/QScatterDataProxy>

using namespace QtDataVisualization;

class QCustomPlot;
class UASInterface;

namespace Ui {
class CompassOnboardCalibrationDialog;
}

class CompassOnboardCalibrationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CompassOnboardCalibrationDialog(QWidget *parent = 0);
    ~CompassOnboardCalibrationDialog();

    void closeEvent(QCloseEvent *);

public slots:
    void activeUASSet(UASInterface* uas);
    void compassCalibrationProgress(UASInterface* uas, mavlink_mag_cal_progress_t &mag_cal_progress);
    void compassCalibrationReport(UASInterface* uas, mavlink_mag_cal_report_t &mag_cal_report);
    void textMessageReceived(int uasid, int componentid, int severity, QString text);

    void startCalibration();
    void stopCalibration();
    void okButtonClicked();

    void cancelCalibration();

private:
    void addDataVisualization();

private:
    Ui::CompassOnboardCalibrationDialog *ui;

    UASInterface* m_uasInterface = nullptr;
    bool m_compassCalibrationComplete[2] = {false};

    QScatterDataArray* m_pointDataArray = nullptr;
    QScatterDataProxy* m_proxy = nullptr;
    QScatter3DSeries* m_series = nullptr;
};

#endif // COMPASSONBOARDCALIBRATIONDIALOG_H
