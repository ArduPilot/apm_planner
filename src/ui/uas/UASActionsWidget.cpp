#include "QsLog.h"
#include "ArduPilotMegaMAV.h"
#include "UASActionsWidget.h"
#include <QMessageBox>
#include <UAS.h>

void UASActionsWidget::setupApmPlaneModes()
{
    QLOG_INFO() << "UASActionWidget: Set for Plane";
    ui.modeComboBox->addItem("Manual", ApmPlane::MANUAL);
    ui.modeComboBox->addItem("Circle", ApmPlane::CIRCLE);
    ui.modeComboBox->addItem("Stabilize", ApmPlane::STABILIZE);
    ui.modeComboBox->addItem("Training", ApmPlane::TRAINING);
    ui.modeComboBox->addItem("FBW A", ApmPlane::FLY_BY_WIRE_A);
    ui.modeComboBox->addItem("FBW B", ApmPlane::FLY_BY_WIRE_B);
    ui.modeComboBox->addItem("Auto", ApmPlane::AUTO);
    ui.modeComboBox->addItem("RTL", ApmPlane::RTL);
    ui.modeComboBox->addItem("Loiter", ApmPlane::LOITER);
    ui.modeComboBox->addItem("Guided", ApmPlane::GUIDED);
}

void UASActionsWidget::setupApmCopterModes()
{
    QLOG_INFO() << "UASActionWidget: set for Copter";
    ui.modeComboBox->addItem("Stablize", ApmCopter::STABILIZE);
    ui.modeComboBox->addItem("Acro", ApmCopter::ACRO);
    ui.modeComboBox->addItem("Alt Hold", ApmCopter::ALT_HOLD);
    ui.modeComboBox->addItem("Auto", ApmCopter::AUTO);
    ui.modeComboBox->addItem("Guided", ApmCopter::GUIDED);
    ui.modeComboBox->addItem("Loiter", ApmCopter::LOITER);
    ui.modeComboBox->addItem("RTL", ApmCopter::RTL);
    ui.modeComboBox->addItem("Circle", ApmCopter::CIRCLE);
    ui.modeComboBox->addItem("Position", ApmCopter::POSITION);
    ui.modeComboBox->addItem("Land", ApmCopter::LAND);
    ui.modeComboBox->addItem("Loiter", ApmCopter::OF_LOITER);
    ui.modeComboBox->addItem("Toy A", ApmCopter::TOY_A);
    ui.modeComboBox->addItem("Toy M", ApmCopter::TOY_M);
    ui.modeComboBox->addItem("Sport", ApmCopter::SPORT);
}

void UASActionsWidget::setupApmRoverModes()
{
    QLOG_INFO() << "UASActionWidget: Setfor Rover";
    ui.modeComboBox->addItem("Manual", ApmRover::MANUAL);
    ui.modeComboBox->addItem("Learning", ApmRover::LEARNING);
    ui.modeComboBox->addItem("Steering", ApmRover::STEERING);
    ui.modeComboBox->addItem("Hold", ApmRover::HOLD);
    ui.modeComboBox->addItem("Auto", ApmRover::AUTO);
    ui.modeComboBox->addItem("RTL", ApmRover::RTL);
    ui.modeComboBox->addItem("Guided", ApmRover::GUIDED);
    ui.modeComboBox->addItem("Initializing", ApmRover::INITIALIZING);
}

UASActionsWidget::UASActionsWidget(QWidget *parent) : QWidget(parent)
{
    QLOG_INFO() << "UASActionsWidget creating " << this;
    m_uas = NULL;
    ui.setupUi(this);
    connect(ui.changeAltitudeButton,SIGNAL(clicked()),this,SLOT(changeAltitudeClicked()));
    connect(ui.changeSpeedButton,SIGNAL(clicked()),this,SLOT(changeSpeedClicked()));
    connect(ui.goToWaypointButton,SIGNAL(clicked()),this,SLOT(goToWaypointClicked()));
    connect(ui.armDisarmButton,SIGNAL(clicked()),this,SLOT(armButtonClicked()));
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));

    if (UASManager::instance()->getActiveUAS())
    {
        activeUASSet(UASManager::instance()->getActiveUAS());
    }

    // Setup Action Combo Box
    ui.actionComboBox->addItem("Loiter Unlimited", MAV_CMD_NAV_LOITER_UNLIM);
    ui.actionComboBox->addItem("Return To Launch", MAV_CMD_NAV_RETURN_TO_LAUNCH);
    ui.actionComboBox->addItem("Preflight Calibration", MAV_CMD_PREFLIGHT_CALIBRATION);
    ui.actionComboBox->addItem("Mission Start", MAV_CMD_MISSION_START);
    ui.actionComboBox->addItem("Preflight Reboot", MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN);

    connect(ui.exeActionButton, SIGNAL(clicked()),
            this, SLOT(setAction()));
}

void UASActionsWidget::activeUASSet(UASInterface *uas)
{
    QLOG_INFO() << "UASActionWidget::activeUASSet";
    if (uas == NULL) {
        QLOG_WARN() << "uas object NULL";
    }
    m_uas = dynamic_cast<UAS*>(uas);

    connect(m_uas->getWaypointManager(),SIGNAL(waypointEditableListChanged()),this,SLOT(updateWaypointList()));
    connect(m_uas->getWaypointManager(),SIGNAL(currentWaypointChanged(quint16)),this,SLOT(currentWaypointChanged(quint16)));
    connect(m_uas,SIGNAL(armingChanged(bool)),this,SLOT(armingChanged(bool)));

    armingChanged(m_uas->isArmed());
    updateWaypointList();

    switch (uas->getAutopilotType()) {
        case MAV_AUTOPILOT_ARDUPILOTMEGA:
        {
            int systemType = uas->getSystemType();
            switch(systemType) {
            case MAV_TYPE_FIXED_WING:
                setupApmPlaneModes();
                break;

            case MAV_TYPE_QUADROTOR:
            case MAV_TYPE_OCTOROTOR:
            case MAV_TYPE_HELICOPTER:
            case MAV_TYPE_TRICOPTER:
                setupApmCopterModes();
                break;

            case MAV_TYPE_GROUND_ROVER:
                setupApmRoverModes();
                break;

            default:
                QLOG_WARN() << "UASActionWidget: Unsupported System Type" << systemType;
            }

            // Setup Final Connections
            connect(ui.setModeButton, SIGNAL(clicked()),
                    this, SLOT(setMode()));

        } break;
        case MAV_AUTOPILOT_PX4:
        {
            // [TODO] PX4 flight controller go here
        }
        case MAV_AUTOPILOT_GENERIC:
        default:
        {
            // [TODO] Generic, and other flight controllers
        }
    }
}

void UASActionsWidget::armButtonClicked()
{
    QLOG_INFO() << "UASActionsWidget::armButtonClicked";

    if (m_uas)
    {
        if (m_uas->isArmed())
        {
            QLOG_INFO() << "UAS:: Attempt to Disarm System";
            m_uas->disarmSystem();

        }
        else
        {
            QLOG_INFO() << "UAS:: Attempt to Arm System";
            m_uas->armSystem();
        }
    }
}

void UASActionsWidget::armingChanged(bool state)
{
    QLOG_INFO() << "Arming State Changed: " << (state?"ARMED":"DISARM");

    //TODO:
    //Figure out why arm/disarm is in UAS.h and not part of the interface, and fix.
    if (state)
    {
        ui.armDisarmButton->setText("DISARM\nCurrently Armed");
    }
    else
    {
        ui.armDisarmButton->setText("ARM\nCurrently Disarmed");
    }

}

void UASActionsWidget::currentWaypointChanged(quint16 wpid)
{
    if (m_last_wpid != wpid) {
        m_last_wpid = wpid;
        QLOG_INFO() << "Waypoint Changed to: " << wpid;
        ui.currentWaypointLabel->setText("Current: " + QString::number(wpid));
    }
}

void UASActionsWidget::updateWaypointList()
{
    QLOG_INFO() << "updateWaypointList: ";

    ui.waypointListComboBox->clear();
    for (int i=0;i<m_uas->getWaypointManager()->getWaypointEditableList().size();i++)
    {
        QLOG_INFO() << "  WP:" << i;
        ui.waypointListComboBox->addItem(QString::number(i));
    }
}

UASActionsWidget::~UASActionsWidget()
{
}

void UASActionsWidget::goToWaypointClicked()
{
    if (!m_uas)
    {
        return;
    }
    QLOG_INFO() << "Go to Waypoint" << ui.waypointListComboBox->currentIndex();
    m_uas->getWaypointManager()->setCurrentWaypoint(ui.waypointListComboBox->currentIndex());
}

void UASActionsWidget::changeAltitudeClicked()
{
    QLOG_WARN() << "changeAltitudeClicked: Not implemented yet.";
//    QMessageBox::information(0,"Error","No implemented yet.");
}

void UASActionsWidget::changeSpeedClicked()
{
    if (!m_uas)
    {
        return;
    }

    QLOG_INFO() << "Change System Speed " << (float)ui.altitudeSpinBox->value() * 100;

    if (m_uas->getSystemType() == MAV_TYPE_QUADROTOR)
    {
        QLOG_INFO() << "APMCopter: setting WP_SPEED_MAX";
        m_uas->setParameter(1,"WP_SPEED_MAX",QVariant(((float)ui.altitudeSpinBox->value() * 100)));
        return;
    }
    else if (m_uas->getSystemType() == MAV_TYPE_FIXED_WING)
    {
        QVariant variant;
        if (m_uas->getParamManager()->getParameterValue(1,"ARSPD_ENABLE",variant))
        {
            if (variant.toInt() == 1)
            {
                QLOG_INFO() << "APMPlane: ARSPD_ENABLED setting TRIM_ARSPD_CN";
                m_uas->setParameter(1,"TRIM_ARSPD_CN",QVariant(((float)ui.altitudeSpinBox->value() * 100)));
                return;
            }

        }
        QLOG_INFO() << "APMPlane: setting TRIM_ARSPD_CN";
        m_uas->setParameter(1,"TRIM_ARSPD_CN",QVariant(((float)ui.altitudeSpinBox->value() * 100)));
    }
}

void UASActionsWidget::setMode()
{
    QLOG_INFO() << "    UASActionsWidget::setAction()";

    if (m_uas == NULL) {
        QLOG_INFO() << "No Active UAS";
        return;
    }

    QLOG_INFO() << "Set Mode to " << ui.modeComboBox->currentIndex();
    QLOG_DEBUG() << "Set Mode: To Be Implemented";

//    mavlink_message_t msg;
//    mavlink_msg_set_mode_pack(m_uas->getUASID(),
//                                MAV_MODE_FLAG_CUSTOM_MODE_ENABLED, // Target Component
//                                ui.actionComboBox->currentValue());
//    sendMessage(msg);

}



void UASActionsWidget::setAction()
{
    QLOG_INFO() << "UASActionsWidget::setAction()";

    if (m_uas == NULL) {
        QLOG_INFO() << "No Active UAS";
        return;
    }


    QLOG_INFO() << "Set Action to " << ui.actionComboBox->currentIndex();

    int currentIndex = ui.actionComboBox->currentIndex();
    MAV_CMD currentCommand = static_cast<MAV_CMD>(ui.actionComboBox->itemData(currentIndex).toInt());

    switch (m_uas->getAutopilotType()) {
        case MAV_AUTOPILOT_ARDUPILOTMEGA: {
            int systemType = m_uas->getSystemType();
            switch(systemType) {
            case MAV_TYPE_FIXED_WING:
                sendApmPlaneCommand(currentCommand);
                break;

            case MAV_TYPE_QUADROTOR:
            case MAV_TYPE_OCTOROTOR:
            case MAV_TYPE_HELICOPTER:
            case MAV_TYPE_TRICOPTER:
                sendApmCopterCommand(currentCommand);
                break;

            case MAV_TYPE_GROUND_ROVER:
                sendApmRoverCommand(currentCommand);
                break;

            default:
                QLOG_WARN() << "UASActionWidget: Unsupported System Type" << systemType;
            }
        } break;

        case MAV_AUTOPILOT_PX4: {
            // [TODO] PX4 flight controller go here
        }

        case MAV_AUTOPILOT_GENERIC:
        default: {
            // [TODO] Generic, and other flight controllers here (or own sections as above)
        }
    }
}


void UASActionsWidget::sendApmPlaneCommand(MAV_CMD command)
{
    switch(command) {

    case MAV_CMD_NAV_LOITER_UNLIM: {
        // Loiter around this MISSION an unlimited amount of time
        Q_ASSERT(command == MAV_CMD_NAV_LOITER_UNLIM);
        QLOG_INFO() << "MAV_CMD_NAV_LOITER_UNLIM";

        int confirm = 1;    // [TODO] Verify This is what ArduPlane Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_NAV_RETURN_TO_LAUNCH: {
        /* Return to launch location |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
        Q_ASSERT(command == MAV_CMD_NAV_RETURN_TO_LAUNCH);
        QLOG_INFO() << "MAV_CMD_NAV_RETURN_TO_LAUNCH";

        int confirm = 1;    // [TODO] Verify This is what ArduPlane Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_CALIBRATION: {
        // Trigger calibration. This command will be only accepted if in pre-flight mode.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_CALIBRATION);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_CALIBRATION";

        int confirm = 1;
        float param1 = 1.0; // Gyro calibration: 0: no, 1: yes
        float param2 = 0.0; // Magnetometer calibration: 0: no, 1: yes
        float param3 = 0.0; // Ground pressure: 0: no, 1: yes
        float param4 = 0.0; // Radio calibration: 0: no, 1: yes
        float param5 = 0.0; // Accelerometer calibration: 0: no, 1: yes
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_MISSION_START: {
        // start running a mission last_item:
        Q_ASSERT(command == MAV_CMD_MISSION_START);
        QLOG_INFO() << "MAV_CMD_MISSION_START";


        int confirm = 1;
        float param1 = 1.0; // first_item: the first mission item to run
        float param2 = 0.0; // the last mission item to run (after this item is run, the mission ends)|
        float param3 = 0.0; // | Empty|
        float param4 = 0.0; // | Empty|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN: {
        // Request the reboot or shutdown of system components.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN";

        int confirm = 1;
        float param1 = 1.0; // | 0: Do nothing for autopilot 1: Reboot autopilot, 2: Shutdown autopilot.
        float param2 = 1.0; // | 0: Do nothing for onboard computer, 1: Reboot onboard computer, 2: Shutdown onboard computer.
        float param3 = 0.0; // | Reserved|
        float param4 = 0.0; // | Reserved|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    default:
        QLOG_INFO() << "sendApmPlaneCommand: Unknown Command " << command;
    }
}

void UASActionsWidget::sendApmCopterCommand(MAV_CMD command)
{
    switch(command) {

    case MAV_CMD_NAV_LOITER_UNLIM: {
        // Loiter around this MISSION an unlimited amount of time
        Q_ASSERT(command == MAV_CMD_NAV_LOITER_UNLIM);
        QLOG_INFO() << "MAV_CMD_NAV_LOITER_UNLIM";

        int confirm = 1;    // [TODO] Verify This is what ArduCopter Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_NAV_RETURN_TO_LAUNCH: {
        /* Return to launch location |Empty| Empty| Empty| Empty| Empty| Empty| Empty|  */
        Q_ASSERT(command == MAV_CMD_NAV_RETURN_TO_LAUNCH);
        QLOG_INFO() << "MAV_CMD_NAV_RETURN_TO_LAUNCH";

        int confirm = 1;    // [TODO] Verify This is what ArduCopter Does.
        float param1 = 0.0; // Empty
        float param2 = 0.0; // Empty
        float param3 = 0.0; // [NOT USED] Radius around MISSION, in meters. If positive loiter clockwise, else counter-clockwise
        float param4 = 0.0; // Desired yaw angle.|
        float param5 = 0.0; // Latitude
        float param6 = 0.0; // Longitude
        float param7 = 0.0; // Altitude
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_CALIBRATION: {
        // Trigger calibration. This command will be only accepted if in pre-flight mode.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_CALIBRATION);
        QLOG_INFO() << "MAV_CMD_PREFLIGHT_CALIBRATION";

        int confirm = 1;
        float param1 = 1.0; // Gyro calibration: 0: no, 1: yes
        float param2 = 0.0; // Magnetometer calibration: 0: no, 1: yes
        float param3 = 0.0; // Ground pressure: 0: no, 1: yes
        float param4 = 0.0; // Radio calibration: 0: no, 1: yes
        float param5 = 0.0; // Accelerometer calibration: 0: no, 1: yes
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);
    } break;

    case MAV_CMD_MISSION_START: {
        // start running a mission last_item:
        Q_ASSERT(command == MAV_CMD_MISSION_START);
        QLOG_INFO() << "MAV_CMD_MISSION_START";

        int confirm = 1;
        float param1 = 1.0; // first_item: the first mission item to run
        float param2 = 0.0; // the last mission item to run (after this item is run, the mission ends)|
        float param3 = 0.0; // | Empty|
        float param4 = 0.0; // | Empty|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN: {
        // Request the reboot or shutdown of system components.
        Q_ASSERT(command == MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN);
        QLOG_INFO() << "MAV_CMD_MISSION_START";

        int confirm = 1;
        float param1 = 1.0; // | 0: Do nothing for autopilot 1: Reboot autopilot, 2: Shutdown autopilot.
        float param2 = 1.0; // | 0: Do nothing for onboard computer, 1: Reboot onboard computer, 2: Shutdown onboard computer.
        float param3 = 0.0; // | Reserved|
        float param4 = 0.0; // | Reserved|
        float param5 = 0.0; // | Empty|
        float param6 = 0.0; // | Empty|
        float param7 = 0.0; // | Empty|
        int component = MAV_COMP_ID_ALL;
        m_uas->executeCommand(command,
                              confirm, param1, param2, param3,
                              param4, param5, param6, param7, component);

    } break;

    default:
        QLOG_INFO() << "sendApmCopterCommand: Unknown Command " << command;
    }
}

void UASActionsWidget::sendApmRoverCommand(MAV_CMD command)
{
    // [TODO] :(
    QLOG_INFO() << "UASActionWidget::sendApmRoverCommand";
    QLOG_INFO() << "to be implemented";
}

void UASActionsWidget::setAutoMode()
{
    QLOG_INFO() << "UASActionWidget::setAutoMode";
    QLOG_INFO() << "to be implemented";
}

void UASActionsWidget::setManualMode()
{
    QLOG_INFO() << "UASActionsWidget::setManualMode()";
    QLOG_INFO() << "to be implemented";
}

void UASActionsWidget::setRTLMode()
{
    QLOG_INFO() << "UASActionsWidget::setManualMode()";
    QLOG_INFO() << "to be implemented";
}

