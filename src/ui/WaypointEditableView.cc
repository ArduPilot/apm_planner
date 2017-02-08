/*===================================================================
 * QGroundControl Open Source Ground Control Station

(c) 2009, 2010, 2014 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.
======================================================================*/

/**
 * @file
 *   @brief Displays one waypoint
 *
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *   @author Benjamin Knecht <mavteam@student.ethz.ch>
 *   @author Petri Tanskanen <mavteam@student.ethz.ch>
 *   @author Bill Bonney <billbonney@communistech.com>
 */

#include "logging.h"
#include "WaypointEditableView.h"
#include "ui_WaypointEditableView.h"

// NAV Commands
#include "mission/QGCMissionNavWaypoint.h"
#include "mission/QGCMissionNavLoiterUnlim.h"
#include "mission/QGCMissionNavLoiterTurns.h"
#include "mission/QGCMissionNavLoiterTime.h"
#include "mission/QGCMissionNavReturnToLaunch.h"
#include "mission/QGCMissionNavLand.h"
#include "mission/QGCMissionNavTakeoff.h"
#include "mission/QGCMissionNavSweep.h"
#include "mission/QGCMissionNavContinueChangeAlt.h"
#include "mission/QGCMissionNavLoiterToAlt.h"
// Condition Commands
#include "mission/QGCMissionConditionDelay.h"
#include "mission/QGCMissionConditionYaw.h"
#include "mission/QGCMissionConditionDistance.h"
// DO commands
#include "mission/QGCMissionDoJump.h"
#include "mission/QGCMissionDoSetServo.h"
#include "mission/QGCMissionDoRepeatServo.h"
#include "mission/QGCMissionDoDigicamControl.h"
#include "mission/QGCMissionDoMountControl.h"
#include "mission/QGCMissionDoSetCamTriggDist.h"
#include "mission/QGCMissionDoSetRelay.h"
#include "mission/QGCMissionDoSetROI.h"
#include "mission/QGCMissionDoRepeatRelay.h"
#include "mission/QGCMissionDoSetHome.h"
#include "mission/QGCMissionDoChangeSpeed.h"
#include "mission/QGCMissionDoStartSearch.h"
#include "mission/QGCMissionDoFinishSearch.h"
#include "mission/QGCMissionDoSetReverse.h"
// Other
#include "mission/QGCMissionOther.h"

#include "QGCMouseWheelEventFilter.h"

#include <QDoubleSpinBox>
#include <cmath>
#include <qmath.h>

WaypointEditableView::WaypointEditableView(Waypoint* wp, QWidget* parent) :
    QWidget(parent),
    viewMode(QGC_WAYPOINTEDITABLEVIEW_MODE_DEFAULT),
    m_missionWidget(NULL),
    m_ui(new Ui::WaypointEditableView)
{
    m_ui->setupUi(this);

    this->wp = wp;
    connect(wp, SIGNAL(destroyed(QObject*)), this, SLOT(deleted(QObject*)));

    // CUSTOM COMMAND WIDGET
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setSpacing(2);
    layout->setContentsMargins(4, 0 ,4 ,0);
    m_ui->customActionWidget->setLayout(layout);

    m_ui->comboBox_action->setMinimumSize(140,0);

    // add actions
    if(wp->getId() == 0){
        // For APM WP0 is the home location
        m_ui->comboBox_action->addItem(tr("HOME"),MAV_CMD_NAV_WAYPOINT);
        this->setEnabled(false);

    } else {
        // NAV Commands
//        m_ui->comboBox_action->addItem(tr("NAV commands"));
        m_ui->comboBox_action->addItem(tr("Waypoint"),MAV_CMD_NAV_WAYPOINT);
        m_ui->comboBox_action->addItem(tr("Spline Waypoint"),MAV_CMD_NAV_SPLINE_WAYPOINT);
        m_ui->comboBox_action->addItem(tr("TakeOff"),MAV_CMD_NAV_TAKEOFF);
        m_ui->comboBox_action->addItem(tr("Loiter Unlim."),MAV_CMD_NAV_LOITER_UNLIM);
        m_ui->comboBox_action->addItem(tr("Loiter Time"),MAV_CMD_NAV_LOITER_TIME);
        m_ui->comboBox_action->addItem(tr("Loiter Turns"),MAV_CMD_NAV_LOITER_TURNS);
        m_ui->comboBox_action->addItem(tr("Ret. to Launch"),MAV_CMD_NAV_RETURN_TO_LAUNCH);
        m_ui->comboBox_action->addItem(tr("Land"),MAV_CMD_NAV_LAND);
        m_ui->comboBox_action->addItem(tr("Change Alt & cont."),MAV_CMD_NAV_CONTINUE_AND_CHANGE_ALT);
        m_ui->comboBox_action->addItem(tr("Loiter to Alt."),MAV_CMD_NAV_LOITER_TO_ALT);
        //m_ui->comboBox_action->addItem(tr("NAV: Target"),MAV_CMD_NAV_TARGET);

        // IF Commands
        m_ui->comboBox_action->insertSeparator(9999);
//        m_ui->comboBox_action->addItem(tr("Condition Commands"));
        m_ui->comboBox_action->addItem(tr("Condition Delay"),MAV_CMD_CONDITION_DELAY);
        m_ui->comboBox_action->addItem(tr("Condition Yaw"),MAV_CMD_CONDITION_YAW);
        m_ui->comboBox_action->addItem(tr("Condition Distance"),MAV_CMD_CONDITION_DISTANCE);

        // DO Commands
        m_ui->comboBox_action->insertSeparator(9999);
//        m_ui->comboBox_action->addItem(tr("DO commands"));
        m_ui->comboBox_action->addItem(tr("Jump to Index"),MAV_CMD_DO_JUMP);
        m_ui->comboBox_action->addItem(tr("Set Reverse"),MAV_CMD_DO_SET_REVERSE);
        m_ui->comboBox_action->addItem(tr("Set Servo"), MAV_CMD_DO_SET_SERVO);
        m_ui->comboBox_action->addItem(tr("Repeat Servo"), MAV_CMD_DO_REPEAT_SERVO);
        m_ui->comboBox_action->addItem(tr("Digicam Control"), MAV_CMD_DO_DIGICAM_CONTROL);
        m_ui->comboBox_action->addItem(tr("Set Relay"), MAV_CMD_DO_SET_RELAY);
        m_ui->comboBox_action->addItem(tr("Repeat Relay"), MAV_CMD_DO_REPEAT_RELAY);
        m_ui->comboBox_action->addItem(tr("Set Cam Trigg Dist"), MAV_CMD_DO_SET_CAM_TRIGG_DIST);
        m_ui->comboBox_action->addItem(tr("Change Speed"), MAV_CMD_DO_CHANGE_SPEED);
        m_ui->comboBox_action->addItem(tr("Set Home"), MAV_CMD_DO_SET_HOME);
        m_ui->comboBox_action->addItem(tr("Mount Control"), MAV_CMD_DO_MOUNT_CONTROL);
        m_ui->comboBox_action->addItem(tr("Set ROI"), MAV_CMD_DO_SET_ROI);

#ifdef MAVLINK_ENABLED_PIXHAWK
        m_ui->comboBox_action->addItem(tr("NAV: Sweep"),MAV_CMD_NAV_SWEEP);
        m_ui->comboBox_action->addItem(tr("Do: Start Search"),MAV_CMD_DO_START_SEARCH);
        m_ui->comboBox_action->addItem(tr("Do: Finish Search"),MAV_CMD_DO_FINISH_SEARCH);
#endif
        m_ui->comboBox_action->addItem(tr("Other"), MAV_CMD_ENUM_END);
    }

    // add frames
    m_ui->comboBox_frame->addItem("Abs.Alt",MAV_FRAME_GLOBAL);
    m_ui->comboBox_frame->addItem("Rel.Alt", MAV_FRAME_GLOBAL_RELATIVE_ALT);
    m_ui->comboBox_frame->addItem("Ter.Alt", MAV_FRAME_GLOBAL_TERRAIN_ALT); // A relative alt above terrain.
//    m_ui->comboBox_frame->addItem("Local(NED)",MAV_FRAME_LOCAL_NED); // [TODO] Not supported on APM
    m_ui->comboBox_frame->addItem("Mission",MAV_FRAME_MISSION);

    // Initialize view correctly
    m_currentAction = wp->getAction();
    m_missionWidget = createActionWidget(m_currentAction);
    m_ui->customActionWidget->layout()->addWidget(m_missionWidget);
    updateValues();

    // Check for mission frame
    if (wp->getFrame() == MAV_FRAME_MISSION)
    {
        // [ToDo] APM does not set the MAV_FRAME_MISSION, so should check a dictionary
        // of mission commands and mark them up in the GCS.
        m_ui->comboBox_action->setCurrentIndex(m_ui->comboBox_action->count()-1);
    }

    connect(m_ui->upButton, SIGNAL(clicked()), this, SLOT(moveUp()));
    connect(m_ui->downButton, SIGNAL(clicked()), this, SLOT(moveDown()));
    connect(m_ui->topButton, SIGNAL(clicked()), this, SLOT(moveTop()));
    connect(m_ui->bottomButton, SIGNAL(clicked()), this, SLOT(moveBottom()));
    connect(m_ui->removeButton, SIGNAL(clicked()), this, SLOT(remove()));

    connect(m_ui->autoContinue, SIGNAL(stateChanged(int)), this, SLOT(changedAutoContinue(int)));
    connect(m_ui->selectedBox, SIGNAL(stateChanged(int)), this, SLOT(changedCurrent(int)));
    connect(m_ui->comboBox_action, SIGNAL(activated(int)), this, SLOT(changedAction(int)));
    connect(m_ui->comboBox_frame, SIGNAL(activated(int)), this, SLOT(changedFrame(int)));

}

void WaypointEditableView::moveUp()
{
    emit moveUpWaypoint(wp);
}

void WaypointEditableView::moveDown()
{
    emit moveDownWaypoint(wp);
}

void WaypointEditableView::moveTop()
{
    emit moveTopWaypoint(wp);
}

void WaypointEditableView::moveBottom()
{
    emit moveBottomWaypoint(wp);
}


void WaypointEditableView::remove()
{
    emit removeWaypoint(wp);
    deleteLater();
}

void WaypointEditableView::changedAutoContinue(int state)
{
    if (state == 0)
        wp->setAutocontinue(false);
    else
        wp->setAutocontinue(true);
}

/**
 * @param index The index of the combo box of the action entry, NOT the action ID
 */
void WaypointEditableView::changedAction(int index)
{
    // set waypoint action
    int actionID = m_ui->comboBox_action->itemData(index).toUInt();
    if (actionID == QVariant::Invalid || actionID == MAV_CMD_ENUM_END)
    {
        viewMode = QGC_WAYPOINTEDITABLEVIEW_MODE_DIRECT_EDITING;
    }
    else //(actionID < MAV_CMD_ENUM_END && actionID >= 0)
    {
        viewMode = QGC_WAYPOINTEDITABLEVIEW_MODE_DEFAULT;
        MAV_CMD action = (MAV_CMD) actionID;
        wp->setAction(action);
    }
    // change the view
    updateActionView(actionID);
    updateValues();
}

void WaypointEditableView::disableMouseScrollWheel(const QWidget *parentWidget)
{
    QList<QWidget*> widgetList = parentWidget->findChildren<QWidget*>();

    foreach (QWidget *widget, widgetList)
    {
        if (qobject_cast<QAbstractSlider*>(widget) || qobject_cast<QComboBox*>(widget)
                || qobject_cast<QAbstractSpinBox*>(widget))
        {
            widget->installEventFilter(QGCMouseWheelEventFilter::getFilter());
        }
    }
}

void WaypointEditableView::updateActionView(int actionID)
{
    if (m_missionWidget) m_missionWidget->hide();

    if (m_missionWidget && (m_currentAction != actionID)){
        delete m_missionWidget;
        m_missionWidget = NULL;
    }

    if (m_missionWidget == NULL){
        m_missionWidget = createActionWidget(actionID);
        m_ui->customActionWidget->layout()->addWidget(m_missionWidget);
        m_currentAction = actionID;
    }

    if(m_missionWidget) {
        m_missionWidget->show();
    }
}

QWidget* WaypointEditableView::createActionWidget(int action)
{
    QWidget* missionWidget;
    switch(action) {
    case MAV_CMD_NAV_WAYPOINT:
    case MAV_CMD_NAV_SPLINE_WAYPOINT:
        missionWidget = new QGCMissionNavWaypoint(this);
        break;
    case MAV_CMD_NAV_LOITER_UNLIM:
        missionWidget = new QGCMissionNavLoiterUnlim(this);
        break;
    case MAV_CMD_NAV_LOITER_TURNS:
        missionWidget = new QGCMissionNavLoiterTurns(this);
        break;
    case MAV_CMD_NAV_LOITER_TIME:
        missionWidget = new QGCMissionNavLoiterTime(this);
        break;
    case MAV_CMD_NAV_RETURN_TO_LAUNCH:
        missionWidget = new QGCMissionNavReturnToLaunch(this);
        break;
    case MAV_CMD_NAV_LAND:
        missionWidget = new QGCMissionNavLand(this);
        break;
    case MAV_CMD_NAV_TAKEOFF:
        missionWidget = new QGCMissionNavTakeoff(this);
        break;
    case MAV_CMD_NAV_CONTINUE_AND_CHANGE_ALT:
        missionWidget = new QGCMissionNavContinueChangeAlt(this);
        break;
    case MAV_CMD_NAV_LOITER_TO_ALT:
        missionWidget = new QGCMissionNavLoiterToAlt(this);
        break;
    case MAV_CMD_CONDITION_DELAY:
        missionWidget = new QGCMissionConditionDelay(this);
        break;
    case MAV_CMD_CONDITION_YAW:
        missionWidget = new QGCMissionConditionYaw(this);
        break;
    case MAV_CMD_CONDITION_DISTANCE:
        missionWidget = new QGCMissionConditionDistance(this);
        break;
    case MAV_CMD_DO_JUMP:
        missionWidget = new QGCMissionDoJump(this);
        break;
    case MAV_CMD_DO_SET_SERVO:
        missionWidget = new QGCMissionDoSetServo(this);
        break;
    case MAV_CMD_DO_REPEAT_SERVO:
        missionWidget = new QGCMissionDoRepeatServo(this);
        break;
    case MAV_CMD_DO_DIGICAM_CONTROL:
        missionWidget = new QGCMissionDoDigicamControl(this);
        break;
    case MAV_CMD_DO_SET_ROI:
        missionWidget = new QGCMissionDoSetROI(this);
        break;
    case MAV_CMD_DO_CHANGE_SPEED:
        missionWidget = new QGCMissionDoChangeSpeed(this);
        break;
    case MAV_CMD_DO_SET_HOME:
        missionWidget = new QGCMissionDoSetHome(this);
        break;
    case MAV_CMD_DO_SET_CAM_TRIGG_DIST:
        missionWidget = new QGCMissionDoSetCamTriggDist(this);
        break;
    case MAV_CMD_DO_SET_RELAY:
        missionWidget = new QGCMissionDoSetRelay(this);
        break;
    case MAV_CMD_DO_REPEAT_RELAY:
        missionWidget = new QGCMissionDoRepeatRelay(this);
        break;
    case MAV_CMD_DO_MOUNT_CONTROL:
        missionWidget = new QGCMissionDoMountControl(this);
        break;
    case MAV_CMD_DO_SET_REVERSE:
        missionWidget = new QGCMissionDoSetReverse(this);
        break;

 #ifdef MAVLINK_ENABLED_PIXHAWK
    case MAV_CMD_NAV_SWEEP:
        missionWidget = new QGCMissionNavSweep(this);
        break;
    case MAV_CMD_DO_START_SEARCH:
        missionWidget = new QGCMissionDoStartSearch(this);
        break;
    case MAV_CMD_DO_FINISH_SEARCH:
        missionWidget = new QGCMissionDoFinishSearch(this);
        break;
#endif
    case MAV_CMD_ENUM_END:
    default:
        missionWidget = new QGCMissionOther(this);
    }
    // Make sure the mouse or trackpad scrolling doesn't
    // change a value when you hover over it
    disableMouseScrollWheel(this);
    return missionWidget;
}

void WaypointEditableView::deleted(QObject* waypoint)
{
    Q_UNUSED(waypoint);
}

void WaypointEditableView::changedFrame(int index)
{
    // set waypoint action
    MAV_FRAME frame = (MAV_FRAME)m_ui->comboBox_frame->itemData(index).toUInt();
    wp->setFrame(frame);
    updateValues();
}

void WaypointEditableView::changedCurrent(int state)
{
    if (state == 0)
    {
        if (wp->getCurrent() == true) //User clicked on the waypoint, that is already current
        {
            m_ui->selectedBox->setChecked(true);
            m_ui->selectedBox->setCheckState(Qt::Checked);
        }
        else
        {
            m_ui->selectedBox->setChecked(false);
            m_ui->selectedBox->setCheckState(Qt::Unchecked);
        }
    }
    else
    {
        wp->setCurrent(true);
        emit changeCurrentWaypoint(wp->getId());   //the slot changeCurrentWaypoint() in WaypointList sets all other current flags to false
    }
}

void WaypointEditableView::blockAllSpinBoxSignals(const bool shallBlock)
{
    QObjectList allChildrenToBlock(children());
    allChildrenToBlock.append(m_ui->customActionWidget->children());

    for (int j = 0; j  < allChildrenToBlock.size(); ++j)
    {
        // Store only QGCToolWidgetItems
        QDoubleSpinBox* spin = dynamic_cast<QDoubleSpinBox*>(allChildrenToBlock.at(j));
        if (spin)
        {
            //QLOG_DEBUG() << "DEACTIVATED SPINBOX #" << j;
            spin->blockSignals(shallBlock);
        }
        else
        {
            // Store only QGCToolWidgetItems
            QWidget* item = dynamic_cast<QWidget*>(allChildrenToBlock.at(j));
            if (item)
            {
                //QLOG_DEBUG() << "FOUND WIDGET BOX";
                for (int k = 0; k  < item->children().size(); ++k)
                {
                    // Store only QGCToolWidgetItems
                    QDoubleSpinBox* spin = dynamic_cast<QDoubleSpinBox*>(item->children().at(k));
                    if (spin)
                    {
                        //QLOG_DEBUG() << "DEACTIVATED SPINBOX #" << k;
                        spin->blockSignals(shallBlock);
                    }
                }
            }
        }
    }
}

void WaypointEditableView::updateValues()
{
    // Check if we just lost the wp, delete the widget
    // accordingly
    if (!wp) {
        deleteLater();
        return;
    }

    // Deactivate all QDoubleSpinBox signals due to
    // unwanted rounding effects
    blockAllSpinBoxSignals(true);

    // update frame
    MAV_FRAME frame = wp->getFrame();
    int frame_index = m_ui->comboBox_frame->findData(frame);
    if (m_ui->comboBox_frame->currentIndex() != frame_index) {
        m_ui->comboBox_frame->setCurrentIndex(frame_index);
    }

    // Update action
    MAV_CMD action = wp->getAction();
    int action_index = m_ui->comboBox_action->findData(action);
    if (m_ui->comboBox_action->currentIndex() != action_index)
    {
        if (viewMode != QGC_WAYPOINTEDITABLEVIEW_MODE_DIRECT_EDITING)
        {
            // Set to "Other" action if it was -1
            if (action_index == -1)
            {
                action_index = m_ui->comboBox_action->findData(MAV_CMD_ENUM_END);
                viewMode = QGC_WAYPOINTEDITABLEVIEW_MODE_DIRECT_EDITING;
            }
            m_ui->comboBox_action->setCurrentIndex(action_index);
        }
    }

    emit commandBroadcast(wp->getAction());
    emit frameBroadcast(wp->getFrame());
    emit param1Broadcast(wp->getParam1());
    emit param2Broadcast(wp->getParam2());
    emit param3Broadcast(wp->getParam3());
    emit param4Broadcast(wp->getParam4());
    emit param5Broadcast(wp->getParam5());
    emit param6Broadcast(wp->getParam6());
    emit param7Broadcast(wp->getParam7());


    if (m_ui->selectedBox->isChecked() != wp->getCurrent())
    {
        m_ui->selectedBox->setChecked(wp->getCurrent());
    }
    if (m_ui->autoContinue->isChecked() != wp->getAutoContinue())
    {
        m_ui->autoContinue->setChecked(wp->getAutoContinue());
    }
    m_ui->idLabel->setText(QString("%1").arg(wp->getId()));

    static int lastId = -1;
    int currId = wp->getId() % 2;

    if (currId != lastId)
    {
#ifdef WAYPOINTEDIT_ALTERNATE_LINE_COLOR
        // QLOG_DEBUG() << "COLOR ID: " << currId;
        if (currId == 1)
        {
            //backGroundColor = backGroundColor.lighter(150);
            backGroundColor = QColor("#252528").lighter(150);
        }
        else
        {
            backGroundColor = QColor("#252528").lighter(250);
        }
        // QLOG_DEBUG() << "COLOR:" << backGroundColor.name();

        // Update color based on id
        QString groupBoxStyle = QString("QGroupBox {padding: 0px; margin: 0px; border: 0px; background-color: %1; }").arg(backGroundColor.name());
        QString labelStyle = QString("QWidget {background-color: %1; color: #DDDDDF; border-color: #EEEEEE; }").arg(backGroundColor.name());
        QString checkBoxStyle = QString("QCheckBox {background-color: %1; color: #454545; border-color: #EEEEEE; }"
                                        "QCheckBox::indicator:checked {"
                                            "background-color: #00BB00;}"
                                        "QCheckBox::indicator:checked:hover{"
                                            "background-color: #00FF00;}"
                                        "QCheckBox::indicator:checked:pressed {"
                                        "background-color: #005500;}"
//                                        "QCheckBox::indicator:unchecked {"
//                                            "background-color: #00BB00;}"
                                        "QCheckBox::indicator:unchecked:hover{"
                                            "background-color: #AAAAAA;}"
                                        "QCheckBox::indicator:unchecked:pressed {"
                                        "background-color: #00FF00;}").arg(backGroundColor.name());
        QString widgetSlotStyle = QString("QWidget {background-color: %1; color: #DDDDDF; border-color: #EEEEEE; } QSpinBox {background-color: #252528 } QDoubleSpinBox {background-color: #252528 } QComboBox {background-color: #252528 }").arg(backGroundColor.name()); //FIXME There should be a way to declare background color for widgetSlot without letting the children inherit this color. Here, background color for every widget-type (QSpinBox, etc.) has to be declared separately to overrule the coloring of QWidget.

        m_ui->autoContinue->setStyleSheet(checkBoxStyle);
        m_ui->selectedBox->setStyleSheet(checkBoxStyle);
        m_ui->idLabel->setStyleSheet(labelStyle);
        m_ui->groupBox->setStyleSheet(groupBoxStyle);
        m_ui->customActionWidget->setStyleSheet(widgetSlotStyle);
#endif
        lastId = currId;
    }

    // Activate all QDoubleSpinBox signals due to
    // unwanted rounding effects
    blockAllSpinBoxSignals(false);
}

void WaypointEditableView::setCurrent(bool state)
{
    if (m_ui){
        m_ui->selectedBox->blockSignals(true);
        m_ui->selectedBox->setChecked(state);
        m_ui->selectedBox->blockSignals(false);
    }
}


void WaypointEditableView::changedCommand(int mav_cmd_id)
{
    if (mav_cmd_id<MAV_CMD_ENUM_END)
    {
        wp->setAction(mav_cmd_id);
    }
}
void WaypointEditableView::changedParam1(double value)
{
    wp->setParam1(value);
}
void WaypointEditableView::changedParam2(double value)
{
    wp->setParam2(value);
}
void WaypointEditableView::changedParam3(double value)
{
    wp->setParam3(value);
}
void WaypointEditableView::changedParam4(double value)
{
    wp->setParam4(value);
}
void WaypointEditableView::changedParam5(double value)
{
    wp->setParam5(value);
}
void WaypointEditableView::changedParam6(double value)
{
    wp->setParam6(value);
}
void WaypointEditableView::changedParam7(double value)
{
    wp->setParam7(value);
}

WaypointEditableView::~WaypointEditableView()
{
    delete m_ui;
}

void WaypointEditableView::changeEvent(QEvent *e)
{
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
