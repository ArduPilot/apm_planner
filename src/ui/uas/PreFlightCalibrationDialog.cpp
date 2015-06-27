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
#include <QRadioButton>
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
        m_widgets.append(NULL);
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
            QStringList paramList;

            if (m_uasInterface->isMultirotor()){
                paramList << "Gyro" // param 1 // Gyro calibration: 0: no, 1: yes
                         << "unused" // param 2 // Magnetometer calibration: 0: no, 1: yes
                         << "Barometer" // param 3 // Ground pressure: 0: no, 1: yes
                         << "Radio Trim"; // param 4 // Radio calibration: 0: no, 1: yes
    //                     << "3D Accel Calibration"; // param 5 // Accelerometer calibration: 0: no, 1: yes
    //                     << "" // param 6 // | Empty|
    //                     << ""; // param 7 // | Empty|
                addParamCheckBoxes("APM:Copter", paramList, true);

            } else if (m_uasInterface->isFixedWing()
                       ||m_uasInterface->isGroundRover()) {
                paramList << "Gyro & 1D Accel" // param 1
                         << "unused" // param 2
                         << "Barometer & Airspeed" // param 3
                         << "Radio Trim"; // param 4
    //                     << "3D Accel Calibration"; // param 5 // not supported here. use Intial Config View
    //                     << "" // param 6 // | Empty|
    //                     << ""; // param 7 // | Empty|
                addParamCheckBoxes("APM:Plane", paramList, true);

            } else {
                addParamSpinBoxes();
            }


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

    QVBoxLayout *vLayout = new QVBoxLayout;

    for (int i = 0; i < 7; ++i) {
        QString text = "Parameter " + QString::number(i+1);
        QWidget *paramSpinBox = createParamWidget(text, m_widgets, m_ui->groupBox, new QDoubleSpinBox());
        vLayout->addWidget(paramSpinBox);
        signalMapper->setMapping(m_widgets[i], i);
        paramSpinBox->show();
        connect(m_widgets[i], SIGNAL(valueChanged(double)), signalMapper, SLOT(map()));
    }
    m_ui->groupBox->setLayout(vLayout);
    m_ui->groupBox->show();
}

void PreFlightCalibrationDialog::addParamCheckBoxes(const QString& vehicleName, const QStringList &parameterList, bool exclusive)
{
    m_ui->groupBox->setTitle(vehicleName);

    QVBoxLayout *vLayout = new QVBoxLayout;

    for(int count = 0; count < parameterList.length(); count++) {
        QString parameterName = parameterList[count];
        QAbstractButton *paramBox = NULL;
        if (exclusive){
            paramBox = new QRadioButton(parameterName);
        } else {
            paramBox = new QCheckBox(parameterName);
        }
        if(paramBox->text().contains("unused")){
            paramBox->setDisabled(true);
            paramBox->setHidden(true);
        } else {
            vLayout->addWidget(paramBox);
            m_widgets[count] = paramBox;
            paramBox->show();
        }

    }
    m_ui->groupBox->setLayout(vLayout);
    m_ui->groupBox->show();
}

QWidget* PreFlightCalibrationDialog::createParamWidget(const QString& paramName, QList<QWidget*>& list, QWidget* parent, QWidget* controlWidget )
{
    QWidget *widget = new QWidget(parent);
    QHBoxLayout *paramLayout = new QHBoxLayout(widget);
    QLabel *paramLabel = new QLabel(paramName);
    paramLayout->addWidget(controlWidget);
    paramLayout->addWidget(paramLabel);
    list.append(controlWidget);
    return widget;
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
    QLOG_INFO() << "MAV_CMD_PREFLIGHT_CALIBRATION";

    UASActionsWidget* uasaction = qobject_cast<UASActionsWidget*>(parent());

    if (!(uasaction && (uasaction->preFlightWarningBox(this) == QMessageBox::Ok))){
        return;
    }

    m_uasInterface->executeCommand(MAV_CMD_PREFLIGHT_CALIBRATION, 1 /*confirm*/,
                                       getCheckValue(m_widgets[0]), // param 1 // Gyro calibration: 0: no, 1: yes
                                       getCheckValue(m_widgets[1]), // param 2 // Magnetometer calibration: 0: no, 1: yes
                                       getCheckValue(m_widgets[2]), // param 3 // Ground pressure: 0: no, 1: yes
                                       getCheckValue(m_widgets[3]), // param 4 // Radio calibration: 0: no, 1: yes
                                       getCheckValue(m_widgets[4]), // param 5 // Accelerometer calibration: 0: no, 1: yes
                                       getCheckValue(m_widgets[5]), // param 6 // | Empty|
                                       getCheckValue(m_widgets[6]), // param 7 // | Empty|
                                       MAV_COMP_ID_PRIMARY);
}

float PreFlightCalibrationDialog::getCheckValue(QWidget* widget)
{
    if (qobject_cast<QAbstractButton*>(widget)) {
        if (qobject_cast<QAbstractButton*>(widget)->isChecked()){
            return 1.0f; // Yes
        } else {
            return 0.0f; // No
        }
    } else {
        float result = qobject_cast<QDoubleSpinBox*>(widget) == NULL ?
                    0.0f : qobject_cast<QDoubleSpinBox*>(widget)->value();
        return result;
    }
}

void PreFlightCalibrationDialog::spinBoxChanged(int index)
{
    QLOG_DEBUG() << " spin box index changed : " << index;
}
