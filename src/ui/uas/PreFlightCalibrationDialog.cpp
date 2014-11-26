#include "QsLog.h"
#include "PreFlightCalibrationDialog.h"
#include "ui_PreFlightCalibrationDialog.h"

// Local Includes
#include "UASInterface.h"
#include "UASManager.h"

// System Includes
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>
#include <QSignalMapper>

PreFlightCalibrationDialog::PreFlightCalibrationDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::PreFlightCalibrationDialog)
{
    m_ui->setupUi(this);

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

    QSignalMapper* signalMapper = new QSignalMapper();
    connect(signalMapper, SIGNAL(mapped(int)), this, SIGNAL(spinBoxChanged(int)));

    for (int i = 0; i < 7; ++i) {
        QString text = "Parameter" + QString::number(i);
        widgets.append(addParamSpinBox(text,m_ui->groupBox));
        signalMapper->setMapping(widgets[i], i);
        connect(widgets[i], SIGNAL(valueChanged(double)), signalMapper, SLOT(map()));
    }
}

QDoubleSpinBox* PreFlightCalibrationDialog::addParamSpinBox(const QString& paramName, QWidget *parent)
{
    QHBoxLayout *paramLayout = new QHBoxLayout(parent);
    QDoubleSpinBox *paramSpinBox = new QDoubleSpinBox();
    QLabel *paramLabel = new QLabel(paramName);
    paramLayout->addWidget(paramSpinBox);
    paramLayout->addWidget(paramLabel);
    return paramSpinBox;
}

void PreFlightCalibrationDialog::addArdupilotMegaOptions()
{

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

//    if (preFlightWarningBox() == QMessageBox::Abort)
//        return;

    int confirm = 1;
    float param1 = 1.0; // Gyro calibration: 0: no, 1: yes
    float param2 = 0.0; // Magnetometer calibration: 0: no, 1: yes
    float param3 = 0.0; // Ground pressure: 0: no, 1: yes
    float param4 = 0.0; // Radio calibration: 0: no, 1: yes
    float param5 = 0.0; // Accelerometer calibration: 0: no, 1: yes
    float param6 = 0.0; // | Empty|
    float param7 = 0.0; // | Empty|
    int component = MAV_COMP_ID_ALL;
    m_uasInterface->executeCommand(MAV_CMD_PREFLIGHT_CALIBRATION,
                          confirm, param1, param2, param3,
                          param4, param5, param6, param7, component);
}

void PreFlightCalibrationDialog::spinBoxChanged(int index)
{
    QLOG_DEBUG() << " spin box index changed : " << index;
}
