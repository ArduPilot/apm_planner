/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#include "QsLog.h"
#include <QMessageBox>

#include "CameraGimbalConfig.h"

CameraGimbalConfig::CameraGimbalConfig(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);

    ui.camTriggerGroupBox->setEnabled(false);
    ui.tiltGroupBox->setEnabled(false);
    ui.rollGroupBox->setEnabled(false);
    ui.panGroupBox->setEnabled(false);

    addOutputRcChannels(ui.tiltChannelComboBox);
    addOutputRcChannels(ui.rollChannelComboBox);
    addOutputRcChannels(ui.panChannelComboBox);
    addOutputRcChannels(ui.camTriggerOutChannelComboBox);

    addInputRcChannels(ui.tiltInputChannelComboBox);
    addInputRcChannels(ui.rollInputChannelComboBox);
    addInputRcChannels(ui.panInputChannelComboBox);

    addTriggerTypes(ui.camTriggerTypeComboBox);

    initConnections();
}

void CameraGimbalConfig::initConnections()
{
    connectSignals();
    AP2ConfigWidget::initConnections();
}

void CameraGimbalConfig::connectSignals()
{
    connect(ui.tiltServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    connect(ui.tiltServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    connect(ui.tiltAngleMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    connect(ui.tiltAngleMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    connect(ui.tiltChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTilt(int)));
    connect(ui.tiltInputChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTilt()));
    connect(ui.tiltReverseCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateTilt()));
    connect(ui.tiltStabilizeCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateTilt()));

    connect(ui.rollServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    connect(ui.rollServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    connect(ui.rollAngleMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    connect(ui.rollAngleMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    connect(ui.rollChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateRoll(int)));
    connect(ui.rollInputChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateRoll()));
    connect(ui.rollReverseCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateRoll()));
    connect(ui.rollStabilizeCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateRoll()));

    connect(ui.panServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    connect(ui.panServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    connect(ui.panAngleMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    connect(ui.panAngleMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    connect(ui.panChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePan(int)));
    connect(ui.panInputChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePan()));
    connect(ui.panReverseCheckBox,SIGNAL(clicked(bool)),this,SLOT(updatePan()));
    connect(ui.panStabilizeCheckBox,SIGNAL(clicked(bool)),this,SLOT(updatePan()));

    connect(ui.camTriggerServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    connect(ui.camTriggerServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    connect(ui.camTriggerOnPwmSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    connect(ui.camTriggerOffPwmSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    connect(ui.camTriggerDurationSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    connect(ui.camTriggerOutChannelComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(updateCameraTriggerOutputCh(int)));
    connect(ui.camTriggerTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateCameraTriggerSettings()));

    connect(ui.retractXSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRetractAngles()));
    connect(ui.retractYSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRetractAngles()));
    connect(ui.retractZSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRetractAngles()));

    connect(ui.controlXSpinBox,SIGNAL(editingFinished()),this,SLOT(updateControlAngles()));
    connect(ui.controlYSpinBox,SIGNAL(editingFinished()),this,SLOT(updateControlAngles()));
    connect(ui.controlZSpinBox,SIGNAL(editingFinished()),this,SLOT(updateControlAngles()));

    connect(ui.neutralXSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNeutralAngles()));
    connect(ui.neutralYSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNeutralAngles()));
    connect(ui.neutralZSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNeutralAngles()));
}

void CameraGimbalConfig::disconnectSignals()
{
    disconnect(ui.tiltServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    disconnect(ui.tiltServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    disconnect(ui.tiltAngleMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    disconnect(ui.tiltAngleMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateTilt()));
    disconnect(ui.tiltChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTilt(int)));
    disconnect(ui.tiltInputChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateTilt()));
    disconnect(ui.tiltReverseCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateTilt()));
    disconnect(ui.tiltStabilizeCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateTilt()));

    disconnect(ui.rollServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    disconnect(ui.rollServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    disconnect(ui.rollAngleMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    disconnect(ui.rollAngleMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRoll()));
    disconnect(ui.rollChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateRoll(int)));
    disconnect(ui.rollInputChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateRoll()));
    disconnect(ui.rollReverseCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateRoll()));
    disconnect(ui.rollStabilizeCheckBox,SIGNAL(clicked(bool)),this,SLOT(updateRoll()));

    disconnect(ui.panServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    disconnect(ui.panServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    disconnect(ui.panAngleMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    disconnect(ui.panAngleMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updatePan()));
    disconnect(ui.panChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePan(int)));
    disconnect(ui.panInputChannelComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(updatePan()));
    disconnect(ui.panReverseCheckBox,SIGNAL(clicked(bool)),this,SLOT(updatePan()));
    disconnect(ui.panStabilizeCheckBox,SIGNAL(clicked(bool)),this,SLOT(updatePan()));

    disconnect(ui.camTriggerServoMinSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    disconnect(ui.camTriggerServoMaxSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    disconnect(ui.camTriggerOnPwmSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    disconnect(ui.camTriggerOffPwmSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    disconnect(ui.camTriggerDurationSpinBox,SIGNAL(editingFinished()),this,SLOT(updateCameraTriggerSettings()));
    disconnect(ui.camTriggerOutChannelComboBox,SIGNAL(currentIndexChanged(int)),
            this,SLOT(updateCameraTriggerOutputCh(int)));
    disconnect(ui.camTriggerTypeComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updateCameraTriggerSettings()));

    disconnect(ui.retractXSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRetractAngles()));
    disconnect(ui.retractYSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRetractAngles()));
    disconnect(ui.retractZSpinBox,SIGNAL(editingFinished()),this,SLOT(updateRetractAngles()));

    disconnect(ui.controlXSpinBox,SIGNAL(editingFinished()),this,SLOT(updateControlAngles()));
    disconnect(ui.controlYSpinBox,SIGNAL(editingFinished()),this,SLOT(updateControlAngles()));
    disconnect(ui.controlZSpinBox,SIGNAL(editingFinished()),this,SLOT(updateControlAngles()));

    disconnect(ui.neutralXSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNeutralAngles()));
    disconnect(ui.neutralYSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNeutralAngles()));
    disconnect(ui.neutralZSpinBox,SIGNAL(editingFinished()),this,SLOT(updateNeutralAngles()));
}


void CameraGimbalConfig::addOutputRcChannels(QComboBox* comboBox)
{
    comboBox->addItem(tr("Disable"), 0);
    comboBox->addItem("RC5", 5);
    comboBox->addItem("RC6", 6);
    comboBox->addItem("RC7", 7);
    comboBox->addItem("RC8", 8);
    comboBox->addItem("RC9", 9);
    comboBox->addItem("RC10", 10);
    comboBox->addItem("RC11", 11);
    comboBox->addItem("RC12", 12);

}

void CameraGimbalConfig::addInputRcChannels(QComboBox* comboBox)
{
    comboBox->addItem(tr("None", 0));
    comboBox->addItem("RC5", 5);
    comboBox->addItem("RC6", 6);
    comboBox->addItem("RC7", 7);
    comboBox->addItem("RC8", 8);
}

void CameraGimbalConfig::addTriggerTypes(QComboBox *comboBox)
{
    comboBox->addItem("Servo", CAM_TRIGG_TYPE_SERVO);
    comboBox->addItem("Relay", CAM_TRIGG_TYPE_RELAY);
}

void CameraGimbalConfig::activeUASSet(UASInterface *uas)
{
    AP2ConfigWidget::activeUASSet(uas);
}

void CameraGimbalConfig::showEvent(QShowEvent *)
{
    requestParameterUpdate();
}

void CameraGimbalConfig::requestParameterUpdate()
{
    if (!m_uas) return;
    // The List of Params we care about

    for (int channelCount=5; channelCount <= 11; ++channelCount) {
        QString function = "RC?_FUNCTION";
        function.replace(2,1,QString::number(channelCount));
        QString min = "RC?_MIN";
        min.replace(2,1,QString::number(channelCount));
        QString max = "RC?_MAX";
        max.replace(2,1,QString::number(channelCount));
        QString trim = "RC?_TRIM";
        trim.replace(2,1,QString::number(channelCount));
        QString rev = "RC?_REV";
        rev.replace(2,1,QString::number(channelCount));
        QString dz =  "RC?_DZ";
        dz.replace(2,1,QString::number(channelCount));

        m_cameraParams << function << min << max << trim << rev << dz;
    }

    m_cameraParams << "MNT_RETRACT_X"
            << "MNT_RETRACT_Y"
            << "MNT_RETRACT_Z"
            << "MNT_NEUTRAL_X"

            << "MNT_NEUTRAL_Y"
            << "MNT_NEUTRAL_Z"

            << "MNT_CONTROL_X"
            << "MNT_CONTROL_Y"
            << "MNT_CONTROL_Z"

            << "MNT_STAB_TILT"
            << "MNT_STAB_PAN"

            << "MNT_RC_IN_ROLL"
            << "MNT_ANGMIN_ROL"
            << "MNT_ANGMAX_ROL"

            << "MNT_RC_IN_TILT"
            << "MNT_ANGMIN_TIL"
            << "MNT_ANGMAX_TIL"

            << "MNT_RC_IN_PAN"
            << "MNT_ANGMIN_PAN"
            << "MNT_ANGMAX_PAN"

            << "MNT_JSTICK_SPD"

            << "MNT2_STAB_TILT"
            << "MNT2_STAB_PAN"

            << "MNT2_RC_IN_ROLL"
            << "MNT2_ANGMIN_ROL"
            << "MNT2_ANGMAX_ROL"

            << "MNT2_RC_IN_TILT"
            << "MNT2_ANGMIN_TIL"
            << "MNT2_ANGMAX_TIL"

            << "MNT2_RC_IN_PAN"
            << "MNT2_ANGMIN_PAN"
            << "MNT2_ANGMAX_PAN"

            << "MNT2_JSTICK_SPD";

    qDebug() << "cameraParams" << m_cameraParams;

    QGCUASParamManager *pm = m_uas->getParamManager();
    foreach(QString parameter, m_cameraParams) {
        pm->requestParameterUpdate(1, parameter);
    };
}

void CameraGimbalConfig::updateRetractAngles()
{
    if (showNullMAVErrorMessageBox())
        return;

    m_uas->getParamManager()->setParameter(1,"MNT_RETRACT_X",ui.retractXSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"MNT_RETRACT_Y",ui.retractYSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"MNT_RETRACT_Z",ui.retractZSpinBox->value());
}

void CameraGimbalConfig::updateNeutralAngles()
{
    if (showNullMAVErrorMessageBox())
        return;

    m_uas->getParamManager()->setParameter(1,"MNT_NEUTRAL_X",ui.neutralXSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"MNT_NEUTRAL_Y",ui.neutralYSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"MNT_NEUTRAL_Z",ui.neutralZSpinBox->value());
}

void CameraGimbalConfig::updateControlAngles()
{
    if (showNullMAVErrorMessageBox())
        return;

    m_uas->getParamManager()->setParameter(1,"MNT_CONTROL_X",ui.controlXSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"MNT_CONTROL_Y",ui.controlYSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"MNT_CONTROL_Z",ui.controlZSpinBox->value());
}

void CameraGimbalConfig::updateTilt(int index)
{
    m_newTiltPrefix = ui.tiltChannelComboBox->itemText(index);
    if(index == 0) {
        // Disable Tilt Controls
        ui.tiltGroupBox->setEnabled(false);
    }else {
        ui.tiltGroupBox->setEnabled(true);
    }
    updateTilt();
}

void CameraGimbalConfig::updatePan(int index)
{
    m_newPanPrefix = ui.panChannelComboBox->itemText(index);
    if(index == 0) {
        // Disable Pan Controls
        ui.panGroupBox->setEnabled(false);
    } else {
        ui.panGroupBox->setEnabled(true);
    }
    updatePan();
}

void CameraGimbalConfig::updateRoll(int index)
{
    m_newRollPrefix = ui.rollChannelComboBox->itemText(index);
    if(index == 0) {
        // Disable Roll Controls
        ui.rollGroupBox->setEnabled(false);
    } else {
        ui.rollGroupBox->setEnabled(true);
    }
    updateRoll();

}

void CameraGimbalConfig::updateTilt()
{
    updateCameraGimbalParams(m_tiltPrefix, m_newTiltPrefix, "TILT", RC_FUNCTION::mount_tilt,
                         ui.tiltChannelComboBox, ui.tiltInputChannelComboBox,
                         ui.tiltServoMinSpinBox, ui.tiltServoMaxSpinBox, ui.tiltReverseCheckBox,
                         ui.tiltAngleMinSpinBox, ui.tiltAngleMaxSpinBox, ui.tiltStabilizeCheckBox);
}

void CameraGimbalConfig::updateRoll()
{
    updateCameraGimbalParams(m_rollPrefix, m_newRollPrefix, "ROLL", RC_FUNCTION::mount_roll,
                         ui.rollChannelComboBox, ui.rollInputChannelComboBox,
                         ui.rollServoMinSpinBox, ui.rollServoMaxSpinBox, ui.rollReverseCheckBox,
                         ui.rollAngleMinSpinBox, ui.rollAngleMaxSpinBox, ui.rollStabilizeCheckBox);
}

void CameraGimbalConfig::updatePan()
{
    updateCameraGimbalParams(m_panPrefix, m_newPanPrefix, "PAN", RC_FUNCTION::mount_pan,
                          ui.panChannelComboBox, ui.panInputChannelComboBox,
                          ui.panServoMinSpinBox, ui.panServoMaxSpinBox, ui.panReverseCheckBox,
                          ui.panAngleMinSpinBox, ui.panAngleMaxSpinBox, ui.panStabilizeCheckBox);
}

void CameraGimbalConfig::updateCameraGimbalParams(QString& chPrefix, const QString& newChPrefix,
                                        const QString& mountType, int rcFunction,
                                        QComboBox *outputChCombo, QComboBox* inputChCombo,
                                        QSpinBox* servoMin, QSpinBox* servoMax, QCheckBox* servoReverse,
                                        QSpinBox* angleMin, QSpinBox* angleMax, QCheckBox* stabilize)
{
    if (showNullMAVErrorMessageBox())
        return;

    QGCUASParamManager *pm = m_uas->getParamManager();

    if (!chPrefix.isEmpty() && (!newChPrefix.isEmpty())
            && (newChPrefix != chPrefix)){
        //We need to disable the old assignment first if chnaged
        QLOG_DEBUG() << "Set old " << chPrefix << " disabled";
        pm->setParameter(1, chPrefix + "_FUNCTION", RC_FUNCTION::Disabled);
        outputChCombo->setCurrentIndex(0);
    }

    chPrefix = newChPrefix;

    if (outputChCombo->currentIndex() == 0) {
        //Disabled
        return;
    }

    QString channel = outputChCombo->itemData(outputChCombo->currentIndex()).toString();

    // component is currently ignored by APM, so MAV_COMP_ID_CAMERA seems ideal.
    QLOG_DEBUG() << "Setting RC Parameters Channel:" << channel << "function:" << rcFunction
             << " min:" << servoMin->value() << " max:" << servoMax->value()
             << " reverse:" << servoReverse->checkState() << "comp:" << 1;
    pm->setParameter(1, "RC" + channel + "_FUNCTION", rcFunction);
    pm->setParameter(1, "RC" + channel + "_MIN", servoMin->value());
    pm->setParameter(1, "RC" + channel + "_MAX", servoMax->value());

    if(servoReverse->checkState() == Qt::Checked){
        pm->setParameter(1, "RC" + channel + "_REV", -1.0);
    } else {
        pm->setParameter(1, "RC" + channel + "_REV", 1.0);
    }

    int inChannel = inputChCombo->itemData(inputChCombo->currentIndex()).toInt();

    QLOG_DEBUG() << "Setting Mount Parameters input Channel:" << inChannel << " type: " << mountType
             << " minAngle:" << angleMin->value() << " maxAngle:" << angleMax->value()
             << "comp:" << 1;

    pm->setParameter(1, "MNT_RC_IN_" + mountType , inChannel);
    QString type = mountType;
    type.resize(3);// makes the string either ROL or TIL
    pm->setParameter(1, "MNT_ANGMIN_" + type, QVariant(angleMin->value() * 100).toInt()); // centiDegrees
    pm->setParameter(1, "MNT_ANGMAX_" + type, QVariant(angleMax->value() * 100).toInt()); // centiDegrees
    pm->setParameter(1, "MNT_STAB_" + mountType, QVariant(stabilize->isChecked()?1:0)); // Enable Stabilization
}


void CameraGimbalConfig::updateCameraTriggerOutputCh(int index)
{
    if (showNullMAVErrorMessageBox())
        return;

    m_newTriggerPrefix = ui.camTriggerOutChannelComboBox->itemText(index);

    if (!m_triggerPrefix.isEmpty() && (m_triggerPrefix != m_newTriggerPrefix)){
        // We need to disable the old assignment first
        QLOG_DEBUG() << "Set old camera trigger out " << m_triggerPrefix << " to disabled";
        m_uas->getParamManager()->setParameter(1, m_triggerPrefix + "_FUNCTION", RC_FUNCTION::Disabled);
    }

    if(ui.camTriggerOutChannelComboBox->currentIndex() == 0) {
        // Disabled
        ui.camTriggerGroupBox->setEnabled(false);
        return;
    }

    m_triggerPrefix = m_newTriggerPrefix;

    QGCUASParamManager* pm = m_uas->getParamManager();
    pm->setParameter(1, m_triggerPrefix + "_FUNCTION", RC_FUNCTION::camera_trigger);
    updateCameraTriggerSettings(); // and set all the other related params
}

void CameraGimbalConfig::updateCameraTriggerSettings()
{
    if (showNullMAVErrorMessageBox())
        return;

    m_uas->getParamManager()->setParameter(1,"CAM_TRIGG_TYPE",ui.camTriggerTypeComboBox->currentIndex());
    m_uas->getParamManager()->setParameter(1, m_triggerPrefix + "_MIN",ui.camTriggerServoMinSpinBox->value());
    m_uas->getParamManager()->setParameter(1, m_triggerPrefix + "_MAX",ui.camTriggerServoMaxSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"CAM_SERVO_ON",ui.camTriggerOnPwmSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"CAM_SERVO_OFF",ui.camTriggerOffPwmSpinBox->value());
    m_uas->getParamManager()->setParameter(1,"CAM_DURATION",ui.camTriggerDurationSpinBox->value());

}

CameraGimbalConfig::~CameraGimbalConfig()
{

}

void CameraGimbalConfig::refreshMountParameters(QString mount, QString parameterName, QVariant &value)
{
    qDebug() << "refresh parameters " << mount;
    disconnectSignals();
    if (parameterName == "MNT_ANGMIN_TIL") //TILT
    {
        ui.tiltAngleMinSpinBox->setValue(value.toInt() / 100.0);
    }
    else if (parameterName == "MNT_ANGMAX_TIL")
    {
        ui.tiltAngleMaxSpinBox->setValue(value.toInt() / 100.0);
    }
    else if (parameterName == "MNT_RC_IN_TILT")
    {
        int index = ui.tiltInputChannelComboBox->findData(value.toInt());
        ui.tiltInputChannelComboBox->setCurrentIndex(index);
    }
    else if (parameterName == "MNT_ANGMIN_ROL") //ROLL
    {
        ui.rollAngleMinSpinBox->setValue(value.toInt() / 100.0);
    }
    else if (parameterName == "MNT_ANGMAX_ROL")
    {
        ui.rollAngleMaxSpinBox->setValue(value.toInt() / 100.0);
    }
    else if (parameterName == "MNT_RC_IN_ROLL")
    {
        int index = ui.rollInputChannelComboBox->findData(value.toInt());
        ui.rollInputChannelComboBox->setCurrentIndex(index);
    }
    else if (parameterName == "MNT_ANGMIN_PAN") //PAN
    {
        ui.panAngleMinSpinBox->setValue(value.toInt() / 100.0);
    }
    else if (parameterName == "MNT_ANGMAX_PAN")
    {
        ui.panAngleMaxSpinBox->setValue(value.toInt() / 100.0);
    }
    else if (parameterName == "MNT_RC_IN_PAN")
    {
        int index = ui.panInputChannelComboBox->findData(value.toInt());
        ui.panInputChannelComboBox->setCurrentIndex(index);
    }
    connectSignals();
}

void CameraGimbalConfig::refreshCameraTriggerParameters(QString parameterName, QVariant value)
{
    qDebug() << "refresh Camera parameters ";

    if (parameterName == "CAM_DURATION")
    {
        ui.camTriggerDurationSpinBox->setValue(value.toInt());
    }
    else if (parameterName == "CAM_SERVO_OFF")
    {
        ui.camTriggerOffPwmSpinBox->setValue(value.toInt());
    }
    else if (parameterName == "CAM_SERVO_ON")
    {
        ui.camTriggerOnPwmSpinBox->setValue(value.toInt());
    }
    else if (parameterName == "CAM_TRIGG_TYPE")
    {
        ui.camTriggerTypeComboBox->setCurrentIndex(value.toInt());
    }
}

void CameraGimbalConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    if (uas != m_uas->getUASID())
        return;

    // Silently ignore all params we don't care about
    if (!m_cameraParams.contains(parameterName))
        return;

    disconnectSignals();

    QLOG_DEBUG() << "Camera Gimbal Param Changed:" << m_uas->getUASName()
             << "param:" << parameterName << value << "compiD:" << component;

    if (parameterName.startsWith("MNT")){
        refreshMountParameters(parameterName.section('_',0), parameterName, value);

    } else if (parameterName.startsWith("CAM")){
        refreshCameraTriggerParameters(parameterName, value);

    } else if (parameterName.contains(QRegExp("RC[1-9][1-16]?_FUNCTION$"))){
        // Matches RCXX_FUNCTION for RC channels 1 - 16
        refreshRcFunctionComboxBox(parameterName, value);

    } else if (parameterName.startsWith(m_tiltPrefix) && !m_tiltPrefix.isEmpty()) {
        if (parameterName.endsWith("MIN"))
        {
            ui.tiltServoMinSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("MAX"))
        {
            ui.tiltServoMaxSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("REV"))
        {
            if (value.toInt() == -1)
            {
                ui.tiltReverseCheckBox->setChecked(true);
            }
            else
            {
                ui.tiltReverseCheckBox->setChecked(false);
            }
        }
    } else if (parameterName.startsWith(m_rollPrefix) && !m_rollPrefix.isEmpty()) {
        if (parameterName.endsWith("MIN"))
        {
            ui.rollServoMinSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("MAX"))
        {
            ui.rollServoMaxSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("REV"))
        {
            if (value.toInt() == -1)
            {
                ui.rollReverseCheckBox->setChecked(true);
            }
            else
            {
                ui.rollReverseCheckBox->setChecked(false);
            }
        }
    } else if (parameterName.startsWith(m_panPrefix) && !m_panPrefix.isEmpty()) {
        if (parameterName.endsWith("MIN"))
        {
            ui.panServoMinSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("MAX"))
        {
            ui.panServoMaxSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("REV"))
        {
            if (value.toInt() == -1 )
            {
                ui.panReverseCheckBox->setChecked(true);
            }
            else
            {
                ui.panReverseCheckBox->setChecked(false);
            }
        }
    } else if (parameterName.startsWith(m_triggerPrefix) && !m_triggerPrefix.isEmpty()) {
        if (parameterName.endsWith("MIN"))
        {
            ui.camTriggerServoMinSpinBox->setValue(value.toInt());
        }
        else if (parameterName.endsWith("MAX"))
        {
            ui.camTriggerServoMaxSpinBox->setValue(value.toInt());
        }
    }

    connectSignals();
}

void CameraGimbalConfig::refreshRcFunctionComboxBox(QString rcChannelName, QVariant &value)
{
    QStringList rcChannelId = rcChannelName.split("_");

    if (value.toInt() == RC_FUNCTION::camera_trigger)
    {
        ui.camTriggerOutChannelComboBox->setCurrentIndex(ui.camTriggerOutChannelComboBox->findText(rcChannelId[0]));
        m_triggerPrefix = rcChannelId[0];
        ui.camTriggerGroupBox->setEnabled(true);
    }
    else if (value.toInt() == RC_FUNCTION::mount_roll)
    {
        ui.rollChannelComboBox->setCurrentIndex(ui.rollChannelComboBox->findText(rcChannelId[0]));
        m_rollPrefix = rcChannelId[0];
        ui.rollGroupBox->setEnabled(true);
    }
    else if (value.toInt() == RC_FUNCTION::mount_tilt)
    {
        ui.tiltChannelComboBox->setCurrentIndex(ui.tiltChannelComboBox->findText(rcChannelId[0]));
        m_tiltPrefix = rcChannelId[0];
        ui.tiltGroupBox->setEnabled(true);
    }
    else if (value.toInt() == RC_FUNCTION::mount_pan)
    {
        ui.panChannelComboBox->setCurrentIndex(ui.panChannelComboBox->findText(rcChannelId[0]));
        m_panPrefix = rcChannelId[0];
        ui.panGroupBox->setEnabled(true);
    }
}


