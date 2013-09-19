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

#include "AdvParameterList.h"
#include "QsLog.h"
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
AdvParameterList::AdvParameterList(QWidget *parent) : AP2ConfigWidget(parent),
    m_paramDownloadState(starting),
    m_paramDownloadCount(0)
{
    ui.setupUi(this);
    connect(ui.refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));
    connect(ui.writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui.loadPushButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    connect(ui.savePushButton,SIGNAL(clicked()),this,SLOT(saveButtonClicked()));
    connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));

    ui.tableWidget->setColumnCount(4);
    ui.tableWidget->verticalHeader()->hide();
    ui.tableWidget->setColumnWidth(0,200);
    ui.tableWidget->setColumnWidth(1,100);
    ui.tableWidget->setColumnWidth(2,100);
    ui.tableWidget->setColumnWidth(3,800);
    ui.tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem("Param"));
    ui.tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem("Value"));
    ui.tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem("Unit"));
    ui.tableWidget->setHorizontalHeaderItem(3,new QTableWidgetItem("Description"));
    ui.tableWidget->horizontalHeaderItem (3)->setTextAlignment(Qt::AlignLeft);

    ui.paramProgressBar->setRange(0,100);

    initConnections();
}
void AdvParameterList::tableWidgetItemChanged(QTableWidgetItem* item)
{
	if (!ui.tableWidget->item(item->row(),1) || !ui.tableWidget->item(item->row(),0))
    {
        //Invalid item, something has gone awry.
        return;
    }
    m_origBrushList.append(ui.tableWidget->item(item->row(),0)->text());
    QBrush brush = QBrush(QColor::fromRgb(100,255,100));
    item->setBackground(brush);
    ui.tableWidget->item(item->row(),0)->setBackground(brush);
    m_modifiedParamMap[ui.tableWidget->item(item->row(),0)->text()] = item->text().toDouble();
}
void AdvParameterList::writeButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    for (QMap<QString,double>::const_iterator i = m_modifiedParamMap.constBegin();i!=m_modifiedParamMap.constEnd();i++)
    {
        QLOG_DEBUG() << "setParam:" << i.key() << "value:" << i.value();
        m_uas->getParamManager()->setParameter(1,i.key(),i.value());
        m_waitingParamList.append(i.key());
    }
    m_modifiedParamMap.clear();
}

AdvParameterList::~AdvParameterList()
{
}
void AdvParameterList::refreshButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    m_uas->getParamManager()->requestParameterList();
    m_paramDownloadState = starting;
}

void AdvParameterList::setParameterMetaData(QString name,QString humanname,QString description,QString unit)
{
    m_paramToNameMap[name] = humanname;
    m_paramToDescriptionMap[name] = description;
    m_paramToUnitMap[name] = unit;
}
void AdvParameterList::loadButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    QString filename = QFileDialog::getOpenFileName(this,"Open File");
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,"Error","Unable to open file");
        return;
    }
    QString filestr = file.readAll();
    file.close();
    QStringList filesplit = filestr.split("\r\n");

    foreach (QString fileline,filesplit)
    {
        if (fileline.startsWith("#"))
        {
            //Comment
        }
        else
        {
            QStringList linesplit = fileline.split(",");
            if (linesplit.size() == 2)
            {
                if (m_paramValueMap.contains(linesplit[0]))
                {
                    m_paramValueMap[linesplit[0]]->setText(linesplit[1]);
                }

            }
        }
    }
}

void AdvParameterList::saveButtonClicked()
{
    QString filename = QFileDialog::getSaveFileName(this,"Save File");
    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QMessageBox::information(this,"Error","Unable to open file");
        return;
    }
    QString fileheader = QInputDialog::getText(this,"Input file header","Header at beginning of file:");

    file.write(QString("#NOTE: " + QDateTime::currentDateTime().toString("M/d/yyyy h:m:s AP")
                       + ": " + fileheader + "\r\n").toAscii());

    QList<QString> paramnamelist = m_uas->getParamManager()->getParameterNames(1);
    for (int i=0;i<paramnamelist.size();i++)
    {
        QVariant value;
        m_uas->getParamManager()->getParameterValue(1,paramnamelist[i],value);
        if (value.type() == QVariant::Double || value.type() == QMetaType::Float)
        {
            file.write(paramnamelist[i].append(",").append(QString::number(value.toDouble(),'f',6)).append("\r\n").toAscii());
        }
        else
        {
            file.write(paramnamelist[i].append(",").append(QString::number(value.toInt())).append("\r\n").toAscii());
        }
    }
    file.flush();
    file.close();
}

void AdvParameterList::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    QLOG_DEBUG() << "APL::parameterChanged " << parameterName << ":" <<value.toFloat();
    disconnect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
    if (!m_paramValueMap.contains(parameterName))
    {
        ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);

        //Col 0, param name
        QTableWidgetItem *paramnameitem = new QTableWidgetItem(parameterName);
        paramnameitem->setFlags(paramnameitem->flags() ^ Qt::ItemIsEditable);
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,paramnameitem);

        //Col 1, param value
        QString valstr = "";
        if (value.type() == QMetaType::Float || value.type() == QVariant::Double)
        {
            valstr = QString::number(value.toFloat(),'f',4);
        }
        else
        {
            valstr = QString::number(value.toFloat(),'f',0);
        }
        QTableWidgetItem *valitem = new QTableWidgetItem(valstr);
        valitem->setFlags(valitem->flags() | Qt::ItemIsEditable);
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,1,valitem);

        //Col 2, param unit
        if (m_paramToUnitMap.contains(parameterName))
        {
            QTableWidgetItem *item = new QTableWidgetItem(m_paramToUnitMap[parameterName]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,2,item);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,2,item);
        }

        //Col 3, description
        QString desc = "";
        if (m_paramToNameMap.contains(parameterName))
        {
            desc += m_paramToNameMap[parameterName] + " - ";
        }
        else
        {
            desc = "";
        }

        if (m_paramToDescriptionMap.contains(parameterName))
        {
            desc += m_paramToDescriptionMap[parameterName];
        }
        QTableWidgetItem *item = new QTableWidgetItem(desc);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,3,item);


        m_paramValueMap[parameterName] = ui.tableWidget->item(ui.tableWidget->rowCount()-1,1);
        ui.tableWidget->sortByColumn(0,Qt::AscendingOrder);
    }

    if (m_origBrushList.contains(parameterName))
    {
        m_paramValueMap[parameterName]->setBackground(QBrush());
    ui.tableWidget->item(m_paramValueMap[parameterName]->row(),0)->setBackground(QBrush());
        m_origBrushList.removeAll(parameterName);
    }

    QString valstr = "";
    if (value.type() == QMetaType::Float || value.type() == QVariant::Double)
    {
        valstr = QString::number(value.toFloat(),'f',6);
    }
    else
    {
        valstr = QString::number(value.toInt(),'f',0);
    }
    m_paramValueMap[parameterName]->setText(valstr);
    connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));

}

void AdvParameterList::parameterChanged(int uas, int component, int parameterCount, int parameterId, QString parameterName, QVariant value)
{
    QString countString;
    // Create progress of downloading all parameters for UI
    switch (m_paramDownloadState){
    case starting:
        QLOG_INFO() << "Starting Param Progress Bar Updating sys:" << uas;
        m_paramDownloadCount = 1;

        countString = QString::number(m_paramDownloadCount) + "/"
                        + QString::number(parameterCount);
        QLOG_INFO() << "Param Progress Bar: " << countString
                     << "paramId:" << parameterId << "name:" << parameterName
                     << "paramValue:" << value;
        ui.progressLabel->setText(countString);
        ui.paramProgressBar->setValue((m_paramDownloadCount/(float)parameterCount)*100.0);

        m_paramDownloadState = refreshing;
        break;

    case refreshing:
        m_paramDownloadCount++;
        countString = QString::number(m_paramDownloadCount) + "/"
                        + QString::number(parameterCount);
        QLOG_INFO() << "Param Progress Bar: " << countString
                     << "paramId:" << parameterId << "name:" << parameterName
                     << "paramValue:" << value;
        ui.progressLabel->setText(countString);
        ui.paramProgressBar->setValue((m_paramDownloadCount/(float)parameterCount)*100.0);

        if (m_paramDownloadCount == parameterCount)
            m_paramDownloadState = completed;
        break;

    case completed:
        QLOG_INFO() << "Finished Downloading Params" << m_paramDownloadCount;
        m_paramDownloadState = none;
        break;

    case none:
    default:
        ; // Do Nothing
    }
}

