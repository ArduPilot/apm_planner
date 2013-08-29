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

#include <QXmlStreamReader>
#include <QDir>
#include <QFile>

#include "ApmSoftwareConfig.h"


ApmSoftwareConfig::ApmSoftwareConfig(QWidget *parent) : QWidget(parent)
{
    m_uas=0;
    ui.setupUi(this);

    ui.flightModesButton->setVisible(false);
    ui.standardParamButton->setVisible(false);
    ui.advancedParamButton->setVisible(false);
    ui.advParamListButton->setVisible(false);
    ui.arduCopterPidButton->setVisible(false);
    ui.arduRoverPidButton->setVisible(false);
    ui.arduPlanePidButton->setVisible(false);

    m_flightConfig = new FlightModeConfig(this);
    ui.stackedWidget->addWidget(m_flightConfig);
    m_buttonToConfigWidgetMap[ui.flightModesButton] = m_flightConfig;
    connect(ui.flightModesButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));

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

    QWidget *temp = new QWidget(this);
    ui.stackedWidget->addWidget(temp);
    m_buttonToConfigWidgetMap[ui.plannerConfigButton] = temp;
    connect(ui.plannerConfigButton,SIGNAL(clicked()),this,SLOT(activateStackedWidget()));
    ui.stackedWidget->setCurrentWidget(m_buttonToConfigWidgetMap[ui.plannerConfigButton]);

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
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
    ui.standardParamButton->setVisible(false);
    ui.advancedParamButton->setVisible(false);
    ui.advParamListButton->setVisible(false);
    ui.arduCopterPidButton->setVisible(false);
    ui.arduRoverPidButton->setVisible(false);
    ui.arduPlanePidButton->setVisible(false);
}

void ApmSoftwareConfig::uasConnected()
{
    ui.flightModesButton->setVisible(true);
    ui.standardParamButton->setVisible(true);
    ui.advancedParamButton->setVisible(true);
    ui.advParamListButton->setVisible(true);
    if (m_uas->getSystemType() == MAV_TYPE_FIXED_WING)
    {
        ui.arduPlanePidButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(false);
    }
    else if (m_uas->getSystemType() == MAV_TYPE_QUADROTOR)
    {
        ui.arduCopterPidButton->setVisible(true);
        ui.arduPlanePidButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(false);
    }
    else if (m_uas->getSystemType() == MAV_TYPE_GROUND_ROVER)
    {
        ui.arduRoverPidButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(false);
        ui.arduPlanePidButton->setVisible(false);
    }
}

void ApmSoftwareConfig::activeUASSet(UASInterface *uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
        m_uas = 0;
    }
    if (!uas)
    {
        return;
    }
    m_uas = uas;
    connect(uas,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));

    ui.flightModesButton->setVisible(true);
    ui.standardParamButton->setVisible(true);
    ui.advancedParamButton->setVisible(true);
    ui.advParamListButton->setVisible(true);

    QString compare = "";
    if (uas->getSystemType() == MAV_TYPE_FIXED_WING)
    {
        ui.arduPlanePidButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(false);
        compare = "ArduPlane";
    }
    else if (uas->getSystemType() == MAV_TYPE_QUADROTOR)
    {
        ui.arduCopterPidButton->setVisible(true);
        ui.arduPlanePidButton->setVisible(false);
        ui.arduRoverPidButton->setVisible(false);
        compare = "ArduCopter";
    }
    else if (uas->getSystemType() == MAV_TYPE_GROUND_ROVER)
    {
        ui.arduRoverPidButton->setVisible(true);
        ui.arduCopterPidButton->setVisible(false);
        ui.arduPlanePidButton->setVisible(false);
        compare = "APMRover2";
    }


    QDir autopilotdir(qApp->applicationDirPath() + "/files/" + uas->getAutopilotTypeName().toLower());
    QFile xmlfile(autopilotdir.absolutePath() + "/arduplane.pdef.xml");
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

                            QVariantMap genset;
                            QVariantMap advset;

                            QString setname = parametersname;
                            xml.readNext();
                            int genarraycount = 0;
                            int advarraycount = 0;
                            while ((xml.name() != "parameters") && !xml.atEnd())
                            {
                                if (xml.isStartElement() && xml.name() == "param")
                                {
                                    QString humanname = xml.attributes().value("humanName").toString();
                                    QString name = xml.attributes().value("name").toString();
                                    QString tab= xml.attributes().value("user").toString();
                                    if (tab == "Advanced")
                                    {
                                        advset["title"] = parametersname;
                                    }
                                    else
                                    {
                                        genset["title"] = parametersname;
                                    }
                                    if (name.contains(":"))
                                    {
                                        name = name.split(":")[1].toUpper();
                                    }
                                    QString docs = xml.attributes().value("documentation").toString();
                                    //paramTooltips[name] = name + " - " + docs;

                                    int type = -1; //Type of item
                                    QMap<QString,QString> fieldmap;
                                    QList<QPair<QString,QString> > valuemap;
                                    xml.readNext();
                                    while ((xml.name() != "param") && !xml.atEnd())
                                    {
                                        if (xml.isStartElement() && xml.name() == "values")
                                        {
                                            type = 1; //1 is a combobox
                                            if (tab == "Advanced")
                                            {
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "TYPE"] = "COMBO";
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_COMBOBOX_DESCRIPTION"] = humanname;
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_COMBOBOX_PARAMID"] = name;
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_COMBOBOX_COMPONENTID"] = 1;
                                            }
                                            else
                                            {
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "TYPE"] = "COMBO";
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_COMBOBOX_DESCRIPTION"] = humanname;
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_COMBOBOX_PARAMID"] = name;
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_COMBOBOX_COMPONENTID"] = 1;
                                            }
                                            int paramcount = 0;
                                            xml.readNext();
                                            while ((xml.name() != "values") && !xml.atEnd())
                                            {
                                                if (xml.isStartElement() && xml.name() == "value")
                                                {

                                                    QString code = xml.attributes().value("code").toString();
                                                    QString arg = xml.readElementText();
                                                    valuemap.append(QPair<QString,QString>(code,arg));
                                                    if (tab == "Advanced")
                                                    {
                                                        advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_COMBOBOX_ITEM_" + QString::number(paramcount) + "_TEXT"] = arg;
                                                        advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_COMBOBOX_ITEM_" + QString::number(paramcount) + "_VAL"] = code.toInt();
                                                    }
                                                    else
                                                    {
                                                        genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_COMBOBOX_ITEM_" + QString::number(paramcount) + "_TEXT"] = arg;
                                                        genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_COMBOBOX_ITEM_" + QString::number(paramcount) + "_VAL"] = code.toInt();
                                                    }
                                                    paramcount++;
                                                }
                                                xml.readNext();
                                            }
                                            if (tab == "Advanced")
                                            {
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_COMBOBOX_COUNT"] = paramcount;
                                            }
                                            else
                                            {
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_COMBOBOX_COUNT"] = paramcount;
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
                                        if (tab == "Advanced")
                                        {
                                            advset[setname + "\\" + QString::number(advarraycount) + "\\" + "TYPE"] = "SLIDER";
                                            advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_SLIDER_DESCRIPTION"] = humanname;
                                            advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_SLIDER_PARAMID"] = name;
                                            advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_SLIDER_COMPONENTID"] = 1;
                                        }
                                        else
                                        {
                                            genset[setname + "\\" + QString::number(genarraycount) + "\\" + "TYPE"] = "SLIDER";
                                            genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_SLIDER_DESCRIPTION"] = humanname;
                                            genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_SLIDER_PARAMID"] = name;
                                            genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_SLIDER_COMPONENTID"] = 1;
                                        }
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
                                            if (tab == "Advanced")
                                            {
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_SLIDER_MIN"] = min;
                                                advset[setname + "\\" + QString::number(advarraycount) + "\\" + "QGC_PARAM_SLIDER_MAX"] = max;
                                            }
                                            else
                                            {
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_SLIDER_MIN"] = min;
                                                genset[setname + "\\" + QString::number(genarraycount) + "\\" + "QGC_PARAM_SLIDER_MAX"] = max;
                                            }
                                        }
                                    }
                                    QString units = "";
                                    if (fieldmap.contains("Units"))
                                    {
                                        units = fieldmap["Units"];
                                    }

                                    if (tab == "Advanced")
                                    {
                                        advarraycount++;
                                        advset["count"] = advarraycount;
                                    }
                                    else
                                    {
                                        genarraycount++;
                                        genset["count"] = genarraycount;
                                    }
                                    //Right here we have a single param in memory
                                    if (valuemap.size() > 0)
                                    {
                                        QList<QPair<int,QString> > valuelist;
                                        for (int i=0;i<valuemap.size();i++)
                                        {
                                            valuelist.append(QPair<int,QString>(valuemap[i].first.toInt(),valuemap[i].second));
                                        }
                                        if (compare == parametersname)
                                        {
                                            if (tab == "Standard")
                                            {
                                                m_standardParamConfig->addCombo(humanname,docs,name,valuelist);
                                            }
                                            else if (tab == "Advanced")
                                            {
                                                m_advancedParamConfig->addCombo(humanname,docs,name,valuelist);
                                            }
                                            m_advParameterList->setParameterMetaData(name,humanname,docs,units);
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
                                        }
                                        if (compare == parametersname)
                                        {
                                            if (tab == "Standard")
                                            {
                                                m_standardParamConfig->addRange(humanname,docs,name,min,max,increment);
                                            }
                                            else if (tab == "Advanced")
                                            {
                                                m_advancedParamConfig->addRange(humanname,docs,name,min,max,increment);
                                            }
                                            m_advParameterList->setParameterMetaData(name,humanname,docs,units);
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

}
