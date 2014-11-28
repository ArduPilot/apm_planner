#include "QsLog.h"
#include "PreFlightCalibrationDialog.h"
#include "ui_PreFlightCalibrationDialog.h"

// Local Includes
#include "UASInterface.h"
#include "UASManager.h"
#include "UASActionsWidget.h"

// System Includes
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QSignalMapper>
#include <QMessageBox>

PreFlightCalibrationDialog::PreFlightCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PreFlightCalibrationDialog)
{
    m_ui->setupUi(this);

    for (int count = 0; count < 7; count++){
        m_param.append(0.0f);
    }

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    connect(m_ui->buttonBox, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
    connect(m_ui->buttonBox, SIGNAL(rejected()), this, SLOT(dialogRejected()));
    activeUASSet(UASManager::instance()->getActiveUAS());

}

PreFlightCalibrationDialog::~PreFlightCalibrationDialog()
{
    delete m_ui;
}

void PreFlightCalibrationDialog::activeUASSet(UASInterface *uasInterface)
{
    if (m_uasInterface)
    {
        // Disconnect any signals to current uas
    }

    m_uasInterface = uasInterface;

    if (m_uasInterface){
        // Connect any signals to new active uas

        // Set dialog to display options based on connect uas
        switch(uasInterface->getAutopilotType()){
        case MAV_AUTOPILOT_ARDUPILOTMEGA:{
            addArdupilotMegaOptions();
        }
            break;
        case MAV_AUTOPILOT_GENERIC:
        default:
            addParamSpinBoxes();
        }
    }
}

void PreFlightCalibrationDialog::addParamSpinBoxes()
{
    m_ui->groupBox->setTitle("Generic Vehicle");

    QSignalMapper* signalMapper = new QSignalMapper(this);
    connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(spinBoxChanged(int)));

    QVBoxLayout *vLayout = new QVBoxLayout();
    m_ui->groupBox->setLayout(vLayout);

    for (int i = 0; i < 7; ++i) {
        QString text = "Parameter " + QString::number(i);
        vLayout->addWidget(addParamSpinBox(text, m_widgets));
        signalMapper->setMapping(m_widgets[i], i);
        connect(m_widgets[i], SIGNAL(valueChanged(double)), signalMapper, SLOT(map()));
    }
}

QWidget* PreFlightCalibrationDialog::addParamSpinBox(const QString& paramName, QList<QDoubleSpinBox*>& list)
{
    QDoubleSpinBox *paramSpinBox = new QDoubleSpinBox();
    list.append(paramSpinBox);
    return paramSpinBox;
}

void PreFlightCalibrationDialog::addArdupilotMegaOptions()
{
    // [TODO] for now add the generic option for APM
    addParamSpinBoxes();
}

void PreFlightCalibrationDialog::dialogAccepted()
{
    QLOG_DEBUG() << "Preflight Dialog Accepted";
    sendPreflightCalibrationMessage();
    accept();
}

void PreFlightCalibrationDialog::dialogRejected()
{
    QLOG_DEBUG() << "Preflight Dialog Dismissed";
    reject();
}

void PreFlightCalibrationDialog::sendPreflightCalibrationMessage()
{
//    Q_ASSERT(command == MAV_CMD_PREFLIGHT_CALIBRATION);
    QLOG_INFO() << "MAV_CMD_PREFLIGHT_CALIBRATION";

    UASActionsWidget* uasaction = qobject_cast<UASActionsWidget*>(parent());

    if (uasaction && (uasaction->preFlightWarningBox(this) == QMessageBox::Ok)){
        m_uasInterface->executeCommand(MAV_CMD_PREFLIGHT_CALIBRATION, 1 /*confirm*/,
                                       m_widgets[0]->value(), // param 1 // Gyro calibration: 0: no, 1: yes
                                       m_widgets[1]->value(), // param 2 // Magnetometer calibration: 0: no, 1: yes
                                       m_widgets[2]->value(), // param 3 // Ground pressure: 0: no, 1: yes
                                       m_widgets[3]->value(), // param 4 // Radio calibration: 0: no, 1: yes
                                       m_widgets[4]->value(), // param 5 // Accelerometer calibration: 0: no, 1: yes
                                       m_widgets[5]->value(), // param 6 // | Empty|
                                       m_widgets[6]->value(), // param 7 // | Empty|
                                       MAV_COMP_ID_ALL);
    }
}

void PreFlightCalibrationDialog::spinBoxChanged(int index)
{
    QLOG_DEBUG() << " spin box index changed : " << index;
}
