/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013-2017 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
/**
 * @file
 *   @brief Airframe type configuration widget source.
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Arne Wischmann <wischmann-a@gmx.de>
 *
 */

#include "logging.h"
#include "FrameTypeConfig.h"
#include "DownloadRemoteParamsDialog.h"
#include "ParamCompareDialog.h"
#include "ArduPilotMegaMAV.h"

FrameTypeConfig::FrameTypeConfig(QWidget *parent) :
    AP2ConfigWidget(parent),
    m_frameClass(FRAME_INIT_VALUE),
    m_frameType(FRAME_INIT_VALUE)
{
    ui.setupUi(this);
    connect(ui.LoadParametersButton, SIGNAL(clicked()), this, SLOT(paramButtonClicked()));

    initConnections();
}

FrameTypeConfig::~FrameTypeConfig()
{
}

void FrameTypeConfig::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    Q_UNUSED(uas);
    Q_UNUSED(component);

    if(!m_currentFirmwareVersion.isValid())
    {
        ArduPilotMegaMAV *myMav = qobject_cast<ArduPilotMegaMAV*>(m_uas);
        if(myMav)
        {
            m_currentFirmwareVersion = myMav->getFirmwareVersion();

            if(m_currentFirmwareVersion.isValid() && m_currentFirmwareVersion.majorNumber() >= 3 && m_currentFirmwareVersion.minorNumber() >= 5)
            {
                QLOG_DEBUG() << "FrameTypeConfig::parameterChanged - Using new frame config!";
                FrameTypeConfigNew *p_ConfigNew = new FrameTypeConfigNew(m_uas, this);
                connect(this, SIGNAL(detectedNewFrameType(int, int)), p_ConfigNew, SLOT(setFrameType(int, int)));
                ui.scrollArea->setWidget(p_ConfigNew);
                p_ConfigNew->show();
            }
            else if(m_currentFirmwareVersion.isValid())
            {
                QLOG_DEBUG() << "FrameTypeConfig::parameterChanged - Using old frame config!";
                FrameTypeConfigOld *p_ConfigOld = new FrameTypeConfigOld(m_uas, this);
                connect(this, SIGNAL(detectedOldFrameType(int)), p_ConfigOld, SLOT(setFrameType(int)));
                ui.scrollArea->setWidget(p_ConfigOld);
                p_ConfigOld->show();
            }
        }
    }

    // TODO handle reconnecting
    // TODO handle reordering of messages -> frame is set before Version is valid
    if (parameterName == "FRAME")
    {
        // ArduPilot < V3.5.0 uses only "FRAME" parameter to set the farme type
        emit detectedOldFrameType(value.toInt());
    }
    else if(parameterName == "FRAME_CLASS")
    {
        m_frameClass = value.toInt();
        if(m_frameType != FRAME_INIT_VALUE)
        {
            emit detectedNewFrameType(m_frameClass, m_frameType);
        }
    }
    else if(parameterName == "FRAME_TYPE")
    {
        m_frameType = value.toInt();
        if(m_frameClass != FRAME_INIT_VALUE)
        {
            emit detectedNewFrameType(m_frameClass, m_frameType);
        }
    }
}

void FrameTypeConfig::parameterChanged(int uas, int component, int parameterCount,
                                       int parameterId, QString parameterName, QVariant value)
{
    Q_UNUSED(uas)
    Q_UNUSED(parameterCount)

    // Create a parameter list model for comparison feature
    // [TODO] This needs to move to the global parameter model.

    if (m_parameterList.contains(parameterName)){
        UASParameter* param = m_parameterList.value(parameterName);
        param->setValue(value); // This also sets the modified bit
    } else {
        // create a new entry
        UASParameter* param = new UASParameter(parameterName,component,value,parameterId);
        m_parameterList.insert(parameterName, param);
    }
}

void FrameTypeConfig::paramButtonClicked()
{
    DownloadRemoteParamsDialog* dialog = new DownloadRemoteParamsDialog(this->parentWidget(), true);

    if(dialog->exec() == QDialog::Accepted) {
        // Pull the selected file and
        // modify the parameters on the adv param list.
        QLOG_DEBUG() << "Remote File Downloaded";
        QLOG_DEBUG() << "Trigger auto load or compare of the downloaded file";

        // Bring up the compare dialog
        m_paramFileToCompare = dialog->getDownloadedFileName();
        QTimer::singleShot(300, this, SLOT(activateCompareDialog()));
    }
    delete dialog;
    dialog = NULL;
}

void FrameTypeConfig::activateCompareDialog()
{
    QLOG_DEBUG() << "Compare Params to File";

    ParamCompareDialog* dialog = new ParamCompareDialog(m_parameterList, m_paramFileToCompare, this);
    dialog->setAcceptButtonLabel(tr("Write Params"));

    if(dialog->exec() == QDialog::Accepted) {
        // Apply the selected parameters
        foreach(UASParameter* param, m_parameterList){
            // Apply changes to ParamManager
            if(param->isModified()){
                m_uas->getParamManager()->setParameter(param->component(),param->name(),param->value());
            }
        }
    }
    delete dialog;
    dialog = NULL;
}


//*********************************************************************************


FrameTypeConfigOld::FrameTypeConfigOld(UASInterface *uasInterface, QWidget *parent) :
    QWidget(parent),
    m_uasInterface(uasInterface),   // TODO check what happens on disconnecting MAV
    m_frameType(FrameTypeConfig::FRAME_INIT_VALUE)
{
    ui.setupUi(this);

    //Disable until we get a FRAME parameter.
    enableButtons(false);

    connect(ui.plusRadioButton,SIGNAL(clicked()),this,SLOT(plusFrameSelected()));
    connect(ui.xRadioButton,SIGNAL(clicked()),this,SLOT(xFrameSelected()));
    connect(ui.vRadioButton,SIGNAL(clicked()),this,SLOT(vFrameSelected()));
    connect(ui.hRadioButton,SIGNAL(clicked()),this,SLOT(hFrameSelected()));
    connect(ui.newY6radioButton,SIGNAL(clicked()),this,SLOT(newY6FrameSelected()));

}

FrameTypeConfigOld::~FrameTypeConfigOld()
{
}

void FrameTypeConfigOld::setFrameType(int frameType)
{
    QLOG_DEBUG() << "FrameTypeConfigOld::setFrameType - frameType:" << frameType;
    m_frameType = frameType;
    enableButtons(true);

    switch(m_frameType){
        case FRAME_PLUS:
            ui.plusRadioButton->setChecked(true);
            break;
        case FRAME_X:
            ui.xRadioButton->setChecked(true);
            break;
        case FRAME_V:
            ui.vRadioButton->setChecked(true);
            break;
        case FRAME_H:
            ui.hRadioButton->setChecked(true);
            break;
        case FRAME_NEWY6:
            ui.newY6radioButton->setChecked(true);
            break;
        default:
            QLOG_ERROR() << "Unknown Frame Type" << m_frameType;
    }
}

void FrameTypeConfigOld::xFrameSelected()
{
    if (!m_uasInterface)
    {
        QLOG_ERROR() << "No MAV connected - cannot set FRAME type";
        enableButtons(false);
        return;
    }
    m_uasInterface->getParamManager()->setParameter(1, "FRAME", QVariant(FRAME_X));
}

void FrameTypeConfigOld::hFrameSelected()
{
    if (!m_uasInterface)
    {
        QLOG_ERROR() << "No MAV connected - cannot set FRAME type";
        enableButtons(false);
        return;
    }
    m_uasInterface->getParamManager()->setParameter(1, "FRAME", QVariant(FRAME_H));
}

void FrameTypeConfigOld::plusFrameSelected()
{
    if (!m_uasInterface)
    {
        QLOG_ERROR() << "No MAV connected - cannot set FRAME type";
        enableButtons(false);
        return;
    }
    m_uasInterface->getParamManager()->setParameter(1, "FRAME", QVariant(FRAME_PLUS));
}

void FrameTypeConfigOld::vFrameSelected()
{
    if (!m_uasInterface)
    {
        QLOG_ERROR() << "No MAV connected - cannot set FRAME type";
        enableButtons(false);
        return;
    }
    m_uasInterface->getParamManager()->setParameter(1, "FRAME", QVariant(FRAME_V));
}

void FrameTypeConfigOld::newY6FrameSelected()
{
    if (!m_uasInterface)
    {
        QLOG_ERROR() << "No MAV connected - cannot set FRAME type";
        enableButtons(false);
        return;
    }
    m_uasInterface->getParamManager()->setParameter(1, "FRAME", QVariant(FRAME_NEWY6));
}

void FrameTypeConfigOld::enableButtons(bool enabled)
{
    ui.xRadioButton->setEnabled(enabled);
    ui.vRadioButton->setEnabled(enabled);
    ui.plusRadioButton->setEnabled(enabled);
    ui.hRadioButton->setEnabled(enabled);
    ui.newY6radioButton->setEnabled(enabled);
}

//***************************************************************************************

FrameTypeConfigNew::FrameTypeConfigNew(UASInterface *uasInterface, QWidget *parent) :
    QWidget(parent),
    m_uasInterface(uasInterface),
    m_frameClass(FrameTypeConfig::FRAME_INIT_VALUE),
    m_frameType(FrameTypeConfig::FRAME_INIT_VALUE)
{
    ui.setupUi(this);

    // Default - buttons disabled and Widges invisible
    enableClassButtons(false);
    enableTypeWidgets(false, false, false, false, false);

    // connect frame class buttons
    connect(ui.quadRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassQuadSelected()));
    connect(ui.hexaRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassHexaSelected()));
    connect(ui.octaRadionbtn, SIGNAL(clicked()), this, SLOT(FrameClassOctaSelected()));
    connect(ui.octaQuadRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassOctaQuadSelected()));
    connect(ui.Y6RadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassY6Selected()));
    connect(ui.heliRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassHeliSelected()));
    connect(ui.triRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassTriSelected()));
    connect(ui.singleRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassSingleSelected()));
    connect(ui.coaxRadioBtn, SIGNAL(clicked()), this, SLOT(FrameClassCoaxSelected()));

    // connect frame type buttons
    connect(ui.plusRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypePlusSelected()));
    connect(ui.XRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypeXSelected()));
    connect(ui.hRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypeHSelected()));
    connect(ui.vRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypeVSelected()));
    connect(ui.ATailRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypeATailSelected()));
    connect(ui.VTailRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypeVTailSelected()));
    connect(ui.Y6BRadioBtn, SIGNAL(clicked()), this, SLOT(FrameTypeY6BSelected()));

}

FrameTypeConfigNew::~FrameTypeConfigNew()
{
}

void FrameTypeConfigNew::setFrameType(int frameClass, int frameType)
{
    QLOG_DEBUG() << "FrameTypeConfigNew::setFrameType - frameClass:" << frameClass << " frameType:" << frameType;
    m_frameClass = frameClass;
    m_frameType  = frameType;

    enableClassButtons(true);

    switch(m_frameClass)
    {
        case FRAME_CLASS_QUAD:
            ui.quadRadioBtn->setChecked(true);
            enableTypeWidgets(true, true, true, true, false);
            break;
        case FRAME_CLASS_HEXA:
            ui.hexaRadioBtn->setChecked(true);
            enableTypeWidgets(true, true, false, false, false);
            break;
        case FRAME_CLASS_OCTA:
            ui.octaRadionbtn->setChecked(true);
            enableTypeWidgets(true, true, true, false, false);
            break;
        case FRAME_CLASS_OCTAQUAD:
            ui.octaQuadRadioBtn->setChecked(true);
            enableTypeWidgets(true, true, true, false, false);
            break;
        case FRAME_CLASS_Y6:
            ui.Y6RadioBtn->setChecked(true);
            enableTypeWidgets(false, true, false, false, true);
            break;
        case FRAME_CLASS_HELI:
            ui.heliRadioBtn->setChecked(true);
            enableTypeWidgets(true, false, false, false, false);
            break;
        case FRAME_CLASS_TRI:
            ui.triRadioBtn->setChecked(true);
            enableTypeWidgets(false, true, false, false, false);
            break;
        case FRAME_CLASS_SINGLE:
            ui.singleRadioBtn->setChecked(true);
            enableTypeWidgets(true, false, false, false, false);
            break;
        case FRAME_CLASS_COAX:
            ui.coaxRadioBtn->setChecked(true);
            enableTypeWidgets(true, false, false, false, false);
            break;
        case FRAME_CLASS_UNDEFINED:
            enableTypeWidgets(false, false, false, false, false);
            break;
    }

    switch(m_frameType)
    {
        case FRAME_TYPE_PLUS:
            ui.plusRadioBtn->setChecked(true);
            break;
        case FRAME_TYPE_X:
            ui.XRadioBtn->setChecked(true);
            break;
        case FRAME_TYPE_H:
            ui.hRadioBtn->setChecked(true);
            break;
        case FRAME_TYPE_V:
            ui.vRadioBtn->setChecked(true);
            break;
        case FRAME_TYPE_A_TAIL:
            ui.ATailRadioBtn->setChecked(true);
            break;
        case FRAME_TYPE_V_TAIL:
            ui.VTailRadioBtn->setChecked(true);
            break;
        case FRAME_TYPE_Y6B:
            ui.Y6BRadioBtn->setChecked(true);
            break;
    }
}

void FrameTypeConfigNew::FrameClassQuadSelected()
{
    m_frameClass = FRAME_CLASS_QUAD;
    enableTypeWidgets(true, true, true, true, false);   // enable: plus, X, HV, AVTail
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassHexaSelected()
{
    m_frameClass = FRAME_CLASS_HEXA;
    enableTypeWidgets(true, true, false, false, false); // enable: plus, X
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassOctaSelected()
{
    m_frameClass = FRAME_CLASS_OCTA;
    enableTypeWidgets(true, true, true, false, false);  // enable: plus, X, HV
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassOctaQuadSelected()
{
    m_frameClass = FRAME_CLASS_OCTAQUAD;
    enableTypeWidgets(true, true, true, false, false);  // enable: plus, X, HV
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassY6Selected()
{
    m_frameClass = FRAME_CLASS_Y6;
    enableTypeWidgets(false, true, false, false, true); // enable: X, Y6B
    FrameTypeXSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassHeliSelected()
{
    m_frameClass = FRAME_CLASS_HELI;
    enableTypeWidgets(true, false, false, false, false);    // enable: plus
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassTriSelected()
{
    m_frameClass = FRAME_CLASS_TRI;
    enableTypeWidgets(false, true, false, false, false); // enable: X
    FrameTypeXSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassSingleSelected()
{
    m_frameClass = FRAME_CLASS_SINGLE;
    enableTypeWidgets(true, false, false, false, false);    // enable: plus
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameClassCoaxSelected()
{
    m_frameClass = FRAME_CLASS_COAX;
    enableTypeWidgets(true, false, false, false, false);    // enable: plus
    FrameTypePlusSelected();    // set default frame type to avoid unallowed selections when switching classes
}

void FrameTypeConfigNew::FrameTypePlusSelected()
{
    m_frameType = FRAME_TYPE_PLUS;
    writeFrameParams();
}

void FrameTypeConfigNew::FrameTypeXSelected()
{
    m_frameType = FRAME_TYPE_X;
    writeFrameParams();
}

void FrameTypeConfigNew::FrameTypeHSelected()
{
    m_frameType = FRAME_TYPE_H;
    writeFrameParams();
}

void FrameTypeConfigNew::FrameTypeVSelected()
{
    m_frameType = FRAME_TYPE_V;
    writeFrameParams();
}

void FrameTypeConfigNew::FrameTypeVTailSelected()
{
    m_frameType = FRAME_TYPE_V_TAIL;
    writeFrameParams();
}

void FrameTypeConfigNew::FrameTypeATailSelected()
{
    m_frameType = FRAME_TYPE_A_TAIL;
    writeFrameParams();
}

void FrameTypeConfigNew::FrameTypeY6BSelected()
{
    m_frameType = FRAME_TYPE_Y6B;
    writeFrameParams();
}

void FrameTypeConfigNew::enableClassButtons(bool enabled)
{
    ui.quadRadioBtn->setEnabled(enabled);
    ui.hexaRadioBtn->setEnabled(enabled);
    ui.octaRadionbtn->setEnabled(enabled);
    ui.octaQuadRadioBtn->setEnabled(enabled);
    ui.Y6RadioBtn->setEnabled(enabled);
    ui.heliRadioBtn->setEnabled(enabled);
    ui.triRadioBtn->setEnabled(enabled);
    ui.singleRadioBtn->setEnabled(enabled);
    ui.coaxRadioBtn->setEnabled(enabled);
}

void FrameTypeConfigNew::enableTypeWidgets(bool plus, bool X, bool HV, bool AVTail, bool Y6B)
{
    ui.plusWidget->setVisible(plus);
    ui.xWidget->setVisible(X);
    ui.HVWidget->setVisible(HV);
    ui.VATailWidget->setVisible(AVTail);
    ui.Y6Bwidget->setVisible(Y6B);
}

void FrameTypeConfigNew::writeFrameParams()
{
    if((m_frameClass != FrameTypeConfig::FRAME_INIT_VALUE) && (m_frameType != FrameTypeConfig::FRAME_INIT_VALUE))
    {
        if (!m_uasInterface)
        {
            QLOG_ERROR() << "No MAV connected - cannot set FRAME type";
            enableClassButtons(false);
            enableTypeWidgets(false, false, false, false, false);
            return;
        }
        QLOG_DEBUG() << "FrameTypeConfigNew::writeFrameParams - FRAME_CLASS:" << m_frameClass << " FRAME_TYPE:" << m_frameType;
        m_uasInterface->getParamManager()->setParameter(1, "FRAME_CLASS", QVariant(m_frameClass));
        m_uasInterface->getParamManager()->setParameter(1, "FRAME_TYPE", QVariant(m_frameType));
    }
}


