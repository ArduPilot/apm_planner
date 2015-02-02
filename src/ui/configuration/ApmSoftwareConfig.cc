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

#include "ApmSoftwareConfig.h"
#include "QsLog.h"
#include <QXmlStreamReader>
#include <QDir>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>

ApmSoftwareConfig::ApmSoftwareConfig(QWidget *parent) : QWidget(parent),
    m_paramDownloadState(none),
    m_paramDownloadCount(0)
{
    m_uas=0;
    ui.setupUi(this);

    ui.flightModesButton->setVisible(false);
    ui.geoFenceButton->setVisible(false);
    ui.standardParamButton->setVisible(false);
    ui.advancedParamButton->setVisible(false);
    ui.advParamListButton->setVisible(false);
    ui.arduCopterPidButton->setVisible(false);
    ui.arduCopterPidButton->setText(tr("Extended Tuning"));
    ui.arduRoverPidButton->setVisible(false);
    ui.arduRoverPidButton->setText(tr("Tuning"));
    ui.arduPlanePidButton->setVisible(false);
    ui.arduPlanePidButton->setText(tr("Tuning"));
    ui.basicPidButton->setVisible(false);

    m_flightConfig = new FlightModeConfig(this);
    ui.stackedWidget->addWidget(m_flightConfig);
    m_buttonToConfigWidgetMap[ui.flightModesButton] = m_flightConfig;
    connect(ui.flightModesButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_geoFenceConfig = new GeoFenceConfig(this);
    ui.stackedWidget->addWidget(m_geoFenceConfig);
    m_buttonToConfigWidgetMap[ui.geoFenceButton] = m_geoFenceConfig;
    connect(ui.geoFenceButton, SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_standardParamConfig = new StandardParamConfig(this);
    ui.stackedWidget->addWidget(m_standardParamConfig);
    m_buttonToConfigWidgetMap[ui.standardParamButton] = m_standardParamConfig;
    connect(ui.standardParamButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_advancedParamConfig = new AdvancedParamConfig(this);
    ui.stackedWidget->addWidget(m_advancedParamConfig);
    m_buttonToConfigWidgetMap[ui.advancedParamButton] = m_advancedParamConfig;
    connect(ui.advancedParamButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_advParameterList = new AdvParameterList(this);
    ui.stackedWidget->addWidget(m_advParameterList);
    m_buttonToConfigWidgetMap[ui.advParamListButton] = m_advParameterList;
    connect(ui.advParamListButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_basicPidConfig = new BasicPidConfig(this);
    ui.stackedWidget->addWidget(m_basicPidConfig);
    m_buttonToConfigWidgetMap[ui.basicPidButton] = m_basicPidConfig;
    connect(ui.basicPidButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_arduCopterPidConfig = new ArduCopterPidConfig(this);
    ui.stackedWidget->addWidget(m_arduCopterPidConfig);
    m_buttonToConfigWidgetMap[ui.arduCopterPidButton] = m_arduCopterPidConfig;
    connect(ui.arduCopterPidButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_arduPlanePidConfig = new ArduPlanePidConfig(this);
    ui.stackedWidget->addWidget(m_arduPlanePidConfig);
    m_buttonToConfigWidgetMap[ui.arduPlanePidButton] = m_arduPlanePidConfig;
    connect(ui.arduPlanePidButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_arduRoverPidConfig = new ArduRoverPidConfig(this);
    ui.stackedWidget->addWidget(m_arduRoverPidConfig);
    m_buttonToConfigWidgetMap[ui.arduRoverPidButton] = m_arduRoverPidConfig;
    connect(ui.arduRoverPidButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

    m_settingsConfig = new QGCSettingsWidget(this);
    ui.stackedWidget->addWidget(m_settingsConfig);
    m_buttonToConfigWidgetMap[ui.plannerConfigButton] = m_settingsConfig;
    connect(ui.plannerConfigButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));
    ui.stackedWidget->setCurrentWidget(m_buttonToConfigWidgetMap[ui.plannerConfigButton]);

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());

    QNetworkAccessManager *man = new QNetworkAccessManager(this);
    QNetworkReply *reply = man->get(QNetworkRequest(QUrl("http://autotest.diydrones.com/Parameters/apm.pdef.xml")));
    connect(reply,SIGNAL(finished()),this,SLOT(apmParamNetworkReplyFinished()));

    // Setup Parameter Progress bars
    ui.globalParamProgressBar->setRange(0,100);

    QSettings settings;
    settings.beginGroup("QGC_MAINWINDOW");
    if (settings.contains("ADVANCED_MODE"))
    {
        m_isAdvancedMode = settings.value("ADVANCED_MODE").toBool();
    }

    connect(&m_populateTimer,SIGNAL(timeout()),this,SLOT(populateTimerTick()));
}

void ApmSoftwareConfig::advModeChanged(bool state)
{
    m_isAdvancedMode = state;
    if(m_uas){
        uasConnected();
    }

}

void ApmSoftwareConfig::apmParamNetworkReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
    {
        return;
    }
    if (reply->error() != 0 || reply->bytesAvailable() == 0)
    {
        //Error condition, don't attempt to rewrite the file
        QLOG_ERROR() << "ApmSoftwareConfig::apmParamNetworkReplyFinished()" << "Unable to retrieve pdef.xml file! Error num:" << reply->error() << ":" << reply->errorString();
        return;
    }
    QByteArray apmpdef = reply->readAll();
    m_apmPdefFilename = QDir(QGC::appDataDirectory()).filePath("apm.pdef.xml");
    QFile file(m_apmPdefFilename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QLOG_ERROR() << "ApmSoftwareConfig::apmParamNetworkReplyFinished()" << "Unable to open" << file.fileName() << "for writing";
        return;
    }
    file.write(apmpdef);
    file.flush();
    file.close();

}

ApmSoftwareConfig::~ApmSoftwareConfig()
{
}
void ApmSoftwareConfig::activateStackedWidget()
{
    if (m_buttonToConfigWidgetMap.contains(sender()))
    {
        ui.stackedWidget->setCurrentWidget(m_buttonToConfigWidgetMap[sender()]);
    }
}
void ApmSoftwareConfig::uasDisconnected()
{
    ui.flightModesButton->setVisible(false);
    ui.geoFenceButton->setVisible(false);
    ui.standardParamButton->setVisible(false);
    ui.advancedParamButton->setVisible(false);
    ui.advParamListButton->setVisible(false);
    ui.arduCopterPidButton->setVisible(false);
    ui.arduRoverPidButton->setVisible(false);
    ui.arduPlanePidButton->setVisible(false);
    ui.basicPidButton->setVisible(false);
}

void ApmSoftwareConfig::uasConnected()
{
    ui.flightModesButton->setVisible(true);
    ui.standardParamButton->setVisible(true);
    ui.advancedParamButton->setVisible(true);
    ui.advParamListButton->setVisible(true);

    if (m_uas->isFixedWing())
    {
        ui.geoFenceButton->setVisible(false); // TODO - enable when plane geo fence implemented
        ui.arduPlanePidButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(false);
        ui.basicPidButton->setVisible(false);
        ui.advParamListButton->setVisible(m_isAdvancedMode);
        ui.advancedParamButton->setVisible(m_isAdvancedMode);
    }
    else if (m_uas->isMultirotor())
    {
        ui.geoFenceButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(m_isAdvancedMode);
        ui.arduPlanePidButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(false);
        ui.basicPidButton->setVisible(true);
        ui.advParamListButton->setVisible(m_isAdvancedMode);
        ui.advancedParamButton->setVisible(m_isAdvancedMode);
    }
    else if (m_uas->isGroundRover())
    {
        ui.geoFenceButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(false);
        ui.arduPlanePidButton->setVisible(false);
        ui.basicPidButton->setVisible(false);
        ui.advParamListButton->setVisible(m_isAdvancedMode);
        ui.advancedParamButton->setVisible(m_isAdvancedMode);
    }
}

void ApmSoftwareConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));

        disconnect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
                this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));
        m_uas = 0;
    }
    if (!uas)
    {
        if (m_populateTimer.isActive())
        {
            m_populateTimer.stop();
        }
        return;
    }
    m_uas = uas;
    connect(uas,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    connect(m_uas,SIGNAL(parameterChanged(int,int,int,int,QString,QVariant)),
            this,SLOT(parameterChanged(int,int,int,int,QString,QVariant)));

    ui.flightModesButton->setVisible(true);
    ui.standardParamButton->setVisible(true);
    ui.advancedParamButton->setVisible(true);
    ui.advParamListButton->setVisible(true);

    QString compare = "";
    if (uas->isFixedWing())
    {
        compare = "ArduPlane";
    }
    else if (uas->isMultirotor())
    {
        compare = "ArduCopter";
    }
    else if (uas->isGroundRover())
    {
        compare = "APMRover2";
    }

    uasConnected();

#ifdef Q_OS_WIN
    QString appDataDir = QString(getenv("USERPROFILE")).replace("\\","/");
#else
    QString appDataDir = getenv("HOME");
#endif
    m_apmPdefFilename = QDir(appDataDir + "/apmplanner2").filePath("apm.pdef.xml");
    if (!QFile::exists(m_apmPdefFilename))
    {
        QDir autopilotdir(QGC::shareDirectory() + "/files/" + uas->getAutopilotTypeName().toLower());
        m_apmPdefFilename = autopilotdir.absolutePath() + "/arduplane.pdef.xml";
    }

    QFile xmlfile(m_apmPdefFilename);
    if (xmlfile.exists() && !xmlfile.open(QIODevice::ReadOnly))
    {
        return;
    }

    QXmlStreamReader xml(xmlfile.readAll());
    xmlfile.close();

    //TODO: Testing to ensure that incorrectly formated XML won't break this.
    //Also, move this into the Param Manager, as it should handle all metadata.
    while (!xml.atEnd())
    {
        if (xml.isStartElement() && xml.name() == "paramfile")
        {
            xml.readNext();
            while ((xml.name() != "paramfile") && !xml.atEnd())
            {
                QString valuetype = "";
                if (xml.isStartElement() && (xml.name() == "vehicles" || xml.name() == "libraries")) //Enter into the vehicles loop
                {
                    valuetype = xml.name().toString();
                    xml.readNext();
                    while ((xml.name() != valuetype) && !xml.atEnd())
                    {
                        if (xml.isStartElement() && xml.name() == "parameters") //This is a parameter block
                        {
                            QString parametersname = "";
                            if (xml.attributes().hasAttribute("name"))
                            {
                                    parametersname = xml.attributes().value("name").toString();
                            }

                            xml.readNext();
                            while ((xml.name() != "parameters") && !xml.atEnd())
                            {
                                if (xml.isStartElement() && xml.name() == "param")
                                {
                                    QString humanname = xml.attributes().value("humanName").toString();
                                    QString name = xml.attributes().value("name").toString();
                                    QString tab= xml.attributes().value("user").toString();
                                    QString range = "";
                                    if (name.contains(":"))
                                    {
                                        name = name.split(":")[1].toUpper();
                                    }
                                    QString docs = xml.attributes().value("documentation").toString();

                                    int type = -1; //Type of item
                                    QMap<QString,QString> fieldmap;
                                    QList<QPair<QString,QString> > valuemap;
                                    xml.readNext();
                                    while ((xml.name() != "param") && !xml.atEnd())
                                    {
                                        if (xml.isStartElement() && xml.name() == "values")
                                        {
                                            type = 1; //1 is a combobox
                                            int paramcount = 0;
                                            xml.readNext();
                                            while ((xml.name() != "values") && !xml.atEnd())
                                            {
                                                if (xml.isStartElement() && xml.name() == "value")
                                                {

                                                    QString code = xml.attributes().value("code").toString();
                                                    QString arg = xml.readElementText();
                                                    valuemap.append(QPair<QString,QString>(code,arg));
                                                    paramcount++;
                                                }
                                                xml.readNext();
                                            }
                                        }
                                        if (xml.isStartElement() && xml.name() == "field")
                                        {
                                            type = 2; //2 is a slider
                                            QString fieldtype = xml.attributes().value("name").toString();
                                            QString text = xml.readElementText();
                                            fieldmap[fieldtype] = text;
                                        }
                                        xml.readNext();
                                    }
                                    if (type == -1)
                                    {
                                        //Nothing inside! Assume it's a value, give it a default range.
                                        type = 2;
                                        QString fieldtype = "Range";
                                        QString text = "0 100"; //TODO: Determine a better way of figuring out default ranges.
                                        fieldmap[fieldtype] = text;
                                    }
                                    if (type == 2)
                                    {
                                        if (fieldmap.contains("Range"))
                                        {
                                            float min = 0;
                                            float max = 0;
                                            //Some range fields list "0-10" and some list "0 10". Handle both.
                                            if (fieldmap["Range"].split(" ").size() > 1)
                                            {
                                                min = fieldmap["Range"].split(" ")[0].trimmed().toFloat();
                                                max = fieldmap["Range"].split(" ")[1].trimmed().toFloat();
                                            }
                                            else if (fieldmap["Range"].split("-").size() > 1)
                                            {
                                                min = fieldmap["Range"].split("-")[0].trimmed().toFloat();
                                                max = fieldmap["Range"].split("-")[1].trimmed().toFloat();
                                            }
                                            range = QString("%1 to %2").arg(min).arg(max);
                                        }
                                    }
                                    QString units = "";
                                    if (fieldmap.contains("Units"))
                                    {
                                        units = fieldmap["Units"];
                                    }

                                    //Right here we have a single param in memory
                                    if (valuemap.size() > 0)
                                    {
                                        QList<QPair<int,QString> > valuelist;
                                        for (int i=0;i<valuemap.size();i++)
                                        {
                                            valuelist.append(QPair<int,QString>(valuemap[i].first.toInt(),valuemap[i].second));
                                        }
                                        if (compare == parametersname || valuetype == "libraries")
                                        {
                                            if (tab == "Standard")
                                            {
                                                ParamConfig c;
                                                c.name = humanname;
                                                c.docs = docs;
                                                c.param = name;
                                                c.valuelist = valuelist;
                                                c.isAdvanced = false;
                                                c.isRange = false;
                                                m_paramConfigList.append(c);
                                            }
                                            else if (tab == "Advanced")
                                            {
                                                ParamConfig c;
                                                c.name = humanname;
                                                c.docs = docs;
                                                c.param = name;
                                                c.valuelist = valuelist;
                                                c.isAdvanced = true;
                                                c.isRange = false;
                                                m_paramConfigList.append(c);
                                            }
                                            m_advParameterList->setParameterMetaData(name,humanname,docs,units,range);
                                        }
                                    }
                                    else if (fieldmap.size() > 0)
                                    {
                                        float min = 0;
                                        float max = 65535;
                                        float increment = 655.35; //Starting increment of 1%.
                                        if (fieldmap.contains("Range"))
                                        {
                                            //Some range fields list "0-10" and some list "0 10". Handle both.
                                            if (fieldmap["Range"].split(" ").size() > 1)
                                            {
                                                min = fieldmap["Range"].split(" ")[0].trimmed().toFloat();
                                                max = fieldmap["Range"].split(" ")[1].trimmed().toFloat();
                                            }
                                            else if (fieldmap["Range"].split("-").size() > 1)
                                            {
                                                min = fieldmap["Range"].split("-")[0].trimmed().toFloat();
                                                max = fieldmap["Range"].split("-")[1].trimmed().toFloat();
                                            }
                                            increment = (max - min) / 100.0; //1% of total range increment
                                            range = QString("%1 to %2").arg(min).arg(max);
                                        }
                                        if (compare == parametersname || valuetype == "libraries")
                                        {
                                            if (tab == "Standard")
                                            {
                                                ParamConfig c;
                                                c.name = humanname;
                                                c.docs = docs;
                                                c.param = name;
                                                c.min = min;
                                                c.max = max;
                                                c.increment = increment;
                                                c.isAdvanced = false;
                                                c.isRange = true;
                                                m_paramConfigList.append(c);
                                            }
                                            else if (tab == "Advanced")
                                            {
                                                ParamConfig c;
                                                c.name = humanname;
                                                c.docs = docs;
                                                c.param = name;
                                                c.min = min;
                                                c.max = max;
                                                c.increment = increment;
                                                c.isAdvanced = true;
                                                c.isRange = true;
                                                m_paramConfigList.append(c);
                                            }
                                            m_advParameterList->setParameterMetaData(name,humanname,docs,units,range);
                                        }
                                    }

                                }
                                xml.readNext();
                            }
                        }
                        xml.readNext();
                    }

                }
                xml.readNext();
            }
        }
        xml.readNext();
    }

    m_populateTimer.start(1);

}
void ApmSoftwareConfig::populateTimerTick()
{
    if (m_paramConfigList.size() == 0)
    {
        m_populateTimer.stop();
        m_advancedParamConfig->allParamsAdded();
        m_standardParamConfig->allParamsAdded();
        if (m_uas)
        {
            //Set all the new parameters to their proper values.
            //By the time this is hit, the param manager already has a full set of parameters from the vehicle,
            //no need to re-request them.
            QList<QString> paramnames = m_uas->getParamManager()->getParameterNames(1);
            if (paramnames.size() == 0)
            {
                //No param names, params are likely not yet done. Wait a second and refresh.
                QLOG_DEBUG() << "ApmSoftwareConfig::populateTimerTick() - No Param names from param manager. Sleeping for one second...";
                m_populateTimer.start(1000);
                return;
            }
            for (int i=0;i<paramnames.size();i++)
            {
                m_advancedParamConfig->parameterChanged(m_uas->getUASID(),m_uas->getComponentId(),paramnames.at(i),m_uas->getParamManager()->getParameterValue(1,paramnames.at(i)));
                m_standardParamConfig->parameterChanged(m_uas->getUASID(),m_uas->getComponentId(),paramnames.at(i),m_uas->getParamManager()->getParameterValue(1,paramnames.at(i)));
            }
        }
        return;
    }
    if (m_paramConfigList.at(0).isRange)
    {
        if (m_paramConfigList.at(0).isAdvanced)
        {
            m_advancedParamConfig->addRange(m_paramConfigList.at(0).name,m_paramConfigList.at(0).docs,m_paramConfigList.at(0).param,m_paramConfigList.at(0).min,m_paramConfigList.at(0).max,m_paramConfigList.at(0).increment);
        }
        else
        {
            m_standardParamConfig->addRange(m_paramConfigList.at(0).name,m_paramConfigList.at(0).docs,m_paramConfigList.at(0).param,m_paramConfigList.at(0).min,m_paramConfigList.at(0).max,m_paramConfigList.at(0).increment);
        }
    }
    else
    {
        if (m_paramConfigList.at(0).isAdvanced)
        {
            m_advancedParamConfig->addCombo(m_paramConfigList.at(0).name,m_paramConfigList.at(0).docs,m_paramConfigList.at(0).param,m_paramConfigList.at(0).valuelist);
        }
        else
        {
            m_standardParamConfig->addCombo(m_paramConfigList.at(0).name,m_paramConfigList.at(0).docs,m_paramConfigList.at(0).param,m_paramConfigList.at(0).valuelist);

        }
    }
    m_paramConfigList.removeAt(0);

}

void ApmSoftwareConfig::writeParameter(int component, QString parameterName, QVariant value)
{
    QLOG_DEBUG() << "ASC writeParameter";
}

void ApmSoftwareConfig::readParameter(int component, QString parameterName, QVariant value)
{
    QLOG_DEBUG() << "ASC readParameter";
}

void ApmSoftwareConfig::parameterChanged(int uas, int component, int parameterCount, int parameterId, QString parameterName, QVariant value)
{
    QString countString;
    // Create progress of downloading all parameters for UI
    switch (m_paramDownloadState){
    case none:
        if (parameterId == UINT16_MAX){
            // This is an ACK package, not a full read
            break;
        } else if ((parameterId == 0) && (parameterCount != UINT16_MAX)) {
            // Its a new download List, Start from zero.
            ui.globalParamStateLabel->setText(tr("Downloading Params..."));
        } else {
            break;
        }

        // Otherwise, trigger progress bar update.
    case startRead:
        QLOG_INFO() << "Starting Global Param Progress Bar Updating sys:" << uas;
        m_paramDownloadCount = 1;

        countString = QString::number(m_paramDownloadCount) + "/"
                        + QString::number(parameterCount);
        QLOG_TRACE() << "Global Param Progress Bar: " << countString
                     << "paramId:" << parameterId << "name:" << parameterName
                     << "paramValue:" << value;
        ui.globalParamProgressLabel->setText(countString);
        ui.globalParamProgressBar->setValue((m_paramDownloadCount/(float)parameterCount)*100.0);

        m_paramDownloadState = readingParams;
        break;

    case readingParams:
        m_paramDownloadCount++;
        countString = QString::number(m_paramDownloadCount) + "/"
                        + QString::number(parameterCount);
        QLOG_TRACE() << "Param Progress Bar: " << countString
                     << "paramId:" << parameterId << "name:" << parameterName
                     << "paramValue:" << value;
        ui.globalParamProgressLabel->setText(countString);
        ui.globalParamProgressBar->setValue((m_paramDownloadCount/(float)parameterCount)*100.0);

        if (m_paramDownloadCount == parameterCount){
            m_paramDownloadState = none;
            ui.globalParamStateLabel->setText(tr("Params Downloaded"));
        }
        break;

    case completed:
        QLOG_INFO() << "Global Finished Downloading Params" << m_paramDownloadCount;
        m_paramDownloadState = none;
        break;

    default:
        ; // Do Nothing
    }
}

