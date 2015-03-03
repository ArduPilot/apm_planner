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
#include "DownloadRemoteParamsDialog.h"
#include "ParamCompareDialog.h"
#include "QsLog.h"
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QProgressDialog>
#include <QDesktopServices>

#define ADV_TABLE_COLUMN_PARAM 0
#define ADV_TABLE_COLUMN_VALUE 1
#define ADV_TABLE_COLUMN_UNIT 2
#define ADV_TABLE_COLUMN_RANGE 3
#define ADV_TABLE_COLUMN_DESCRIPTION 4
#define ADV_TABLE_COLUMN_COUNT ADV_TABLE_COLUMN_DESCRIPTION + 1

AdvParameterList::AdvParameterList(QWidget *parent) : AP2ConfigWidget(parent),
    m_searchIndex(0),
    m_paramDownloadState(starting),
    m_paramDownloadCount(0),
    m_writingParams(false),
    m_paramsWritten(0),
    m_paramsToWrite(0),
    m_fileDialog(NULL)
{
    ui.setupUi(this);
    connect(ui.refreshPushButton, SIGNAL(clicked()),this, SLOT(refreshButtonClicked()));
    connect(ui.writePushButton, SIGNAL(clicked()),this, SLOT(writeButtonClicked()));
    connect(ui.loadPushButton, SIGNAL(clicked()),this, SLOT(loadButtonClicked()));
    connect(ui.savePushButton, SIGNAL(clicked()),this, SLOT(saveButtonClicked()));
    connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
    connect(ui.downloadRemoteButton, SIGNAL(clicked()),this, SLOT(downloadRemoteFiles()));
    connect(ui.compareButton,SIGNAL(clicked()),this, SLOT(compareButtonClicked()));

    connect(ui.searchLineEdit, SIGNAL(textEdited(QString)), this, SLOT(findStringInTable(QString)));
    connect(ui.nextItemButton, SIGNAL(clicked()), this, SLOT(nextItemInSearch()));
    connect(ui.previousItemButton, SIGNAL(clicked()), this, SLOT(previousItemInSearch()));
    connect(ui.resetButton, SIGNAL(clicked()), this, SLOT(resetButtonClicked()));


    ui.tableWidget->setColumnCount(ADV_TABLE_COLUMN_COUNT);
    ui.tableWidget->verticalHeader()->hide();
    ui.tableWidget->setColumnWidth(ADV_TABLE_COLUMN_PARAM,200);
    ui.tableWidget->setColumnWidth(ADV_TABLE_COLUMN_VALUE,100);
    ui.tableWidget->setColumnWidth(ADV_TABLE_COLUMN_UNIT,100);
    ui.tableWidget->setColumnWidth(ADV_TABLE_COLUMN_DESCRIPTION,800);
    ui.tableWidget->setHorizontalHeaderItem(ADV_TABLE_COLUMN_PARAM, new QTableWidgetItem("Param"));
    ui.tableWidget->setHorizontalHeaderItem(ADV_TABLE_COLUMN_VALUE, new QTableWidgetItem("Value"));
    ui.tableWidget->setHorizontalHeaderItem(ADV_TABLE_COLUMN_UNIT, new QTableWidgetItem("Unit"));
    ui.tableWidget->setHorizontalHeaderItem(ADV_TABLE_COLUMN_RANGE, new QTableWidgetItem("Range"));
    ui.tableWidget->setHorizontalHeaderItem(ADV_TABLE_COLUMN_DESCRIPTION,new QTableWidgetItem("Description"));
    ui.tableWidget->horizontalHeaderItem(ADV_TABLE_COLUMN_DESCRIPTION)->setTextAlignment(Qt::AlignLeft);

    ui.paramProgressBar->setRange(0,0);
    ui.paramProgressBar->hide();
    ui.progressLabel->hide();

    initConnections();
}
void AdvParameterList::tableWidgetItemChanged(QTableWidgetItem* item)
{
    if (!ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_VALUE)
            || !ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_PARAM))
    {
        //Invalid item, something has gone awry.
        return;
    }
    QLocale locallocale;
    bool ok = false;
    double number = locallocale.toDouble(item->text(),&ok);
    if (!ok)
    {
        //Failed to convert
        QMessageBox::warning(this,"Error","Failed to convert number, please verify your input and try again");
        disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),this, SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
        ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_VALUE)->setText(m_paramToOrigValueMap[ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_PARAM)->text()]);
        connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),this, SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
        return;
    }

    m_origBrushList.append(ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_PARAM)->text());
    QBrush brush = QBrush(QColor::fromRgb(132,181,132));
    ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_PARAM)->setBackground(brush);
    ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_VALUE)->setBackground(brush);
    ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_UNIT)->setBackground(brush);
    ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_RANGE)->setBackground(brush);
    ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_DESCRIPTION)->setBackground(brush);
    m_modifiedParamMap[ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_PARAM)->text()] = number;
    m_paramToOrigValueMap[ui.tableWidget->item(item->row(),ADV_TABLE_COLUMN_PARAM)->text()] = item->text();


    int itemsChanged = m_modifiedParamMap.size();

    QString str;
    str.sprintf("%d %s changed", itemsChanged, (itemsChanged == 1)? "param": "params");
    ui.progressLabel->setText(str);
    ui.paramProgressBar->setMaximum(itemsChanged);
    ui.progressLabel->show();
    ui.paramProgressBar->show();
}

void AdvParameterList::writeButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    m_waitingParamList.clear();
    for (QMap<QString,double>::const_iterator i = m_modifiedParamMap.constBegin();i!=m_modifiedParamMap.constEnd();i++)
    {
        QLOG_DEBUG() << "setParam:" << i.key() << "value:" << i.value();
        m_uas->getParamManager()->setParameter(1,i.key(),i.value());
        m_waitingParamList.append(i.key());
    }

    m_writingParams = true;
    m_paramsToWrite = m_modifiedParamMap.size();
    m_paramsWritten = 0;

    if(m_paramsToWrite == 0) {
        ui.paramProgressBar->setValue(0);
        ui.progressLabel->setText("No params to write");
        ui.progressLabel->show();
        QTimer::singleShot(700,ui.progressLabel, SLOT(hide()));
    }

    m_modifiedParamMap.clear();
}

AdvParameterList::~AdvParameterList()
{
    delete m_fileDialog;
    m_fileDialog = NULL;
}

void AdvParameterList::refreshButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    m_writingParams = false;
    m_paramsToWrite = 0;
    m_paramsWritten = 0;

    m_uas->getParamManager()->requestParameterList();
    m_paramDownloadState = starting;
}

void AdvParameterList::setParameterMetaData(const QString &name, const QString &humanname,
                                            const QString &description, const QString &unit,
                                            const QString &range)
{
    m_paramToNameMap[name] = humanname;
    m_paramToDescriptionMap[name] = description;
    m_paramToUnitMap[name] = unit;
    m_paramToRangeMap[name] = range;
}


void AdvParameterList::loadButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    QFileDialog *fileDialog = new QFileDialog(this,"Load",QGC::parameterDirectory());
    QLOG_DEBUG() << "CREATED:" << fileDialog;
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    fileDialog->setNameFilter("*.param *.txt");
    fileDialog->open(this, SLOT(loadDialogAccepted()));
    connect(fileDialog,SIGNAL(rejected()),SLOT(dialogRejected()));
}

void AdvParameterList::loadDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    if (dialog->selectedFiles().size() == 0)
    {
        return;
    }
    QString filename = dialog->selectedFiles().at(0);
    if(filename.length() == 0)
    {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(this,"Error","Unable to open the file.");
        return;
    }

    QString filestr = file.readAll();
    file.close();

    ParamCompareDialog::populateParamListFromString(filestr, &m_parameterList, this);

    foreach(UASParameter* param, m_parameterList){
        // Modify the elements in the table widget.
        if (param->isModified()){
            // Update the local table widget
            QTableWidgetItem* item = m_paramValueMap.value(param->name());
            if (item){
                if (param->value() != item->data(Qt::DisplayRole)){
                    item->setData(Qt::DisplayRole, param->value());
                    tableWidgetItemChanged(item);
                }
            }
        }
    }
}

void AdvParameterList::dialogRejected()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    QLOG_DEBUG() << "Dialog Rejected:" << dialog;
    if (dialog){
        dialog->deleteLater();
        dialog = NULL;
    }

}

void AdvParameterList::saveButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }

    QFileDialog *fileDialog = new QFileDialog(this,"Save",QGC::parameterDirectory());
    QLOG_DEBUG() << "CREATED:" << fileDialog;
    fileDialog->setAcceptMode(QFileDialog::AcceptSave);
    fileDialog->setNameFilter("*.param *.txt");
    fileDialog->selectFile("paramter.param");
    fileDialog->open(this, SLOT(saveDialogAccepted()));
    connect(fileDialog,SIGNAL(rejected()),SLOT(dialogRejected()));
}

void AdvParameterList::saveDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    if (dialog->selectedFiles().size() == 0)
    {
        return;
    }
    QString filename = dialog->selectedFiles().at(0);
    if(filename.length() == 0)
    {
        return;
    }

    QFile file(filename);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QMessageBox::information(this,"Error","Unable to save the file.");
        return;
    }

    QString fileheader = QInputDialog::getText(this,"Input file header","Header at beginning of file:");

    file.write(QString("#NOTE: " + QDateTime::currentDateTime().toString("M/d/yyyy h:m:s AP")
                       + ": " + fileheader + "\r\n").toLocal8Bit());

    QList<QString> paramnamelist = m_uas->getParamManager()->getParameterNames(1);
    for (int i=0;i<paramnamelist.size();i++)
    {
        QVariant value;
        m_uas->getParamManager()->getParameterValue(1,paramnamelist[i],value);
        if (value.type() == QVariant::Double || value.type() == QMetaType::Float)
        {
            file.write(paramnamelist[i].append(",").append(QString::number(value.toFloat(),'f',8)).append("\r\n").toLocal8Bit());
        }
        else
        {
            file.write(paramnamelist[i].append(",").append(QString::number(value.toInt())).append("\r\n").toLocal8Bit());
        }
    }
    file.flush();
    file.close();

    dialog->deleteLater(); // cleanup dialog instance
    dialog = NULL;
}

void AdvParameterList::parameterChanged(int /*uas*/, int /*component*/, QString parameterName, QVariant value)
{
    QLOG_DEBUG() << "Param:" << parameterName << ": " << value;

    disconnect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
    if (!m_paramValueMap.contains(parameterName))
    {
        ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);

        //Param name
        QTableWidgetItem *paramnameitem = new QTableWidgetItem(parameterName);
        paramnameitem->setFlags(paramnameitem->flags() ^ Qt::ItemIsEditable);
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,ADV_TABLE_COLUMN_PARAM,paramnameitem);

        // Param value
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
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,ADV_TABLE_COLUMN_VALUE,valitem);

        // Param unit
        if (m_paramToUnitMap.contains(parameterName))
        {
            QTableWidgetItem *item = new QTableWidgetItem(m_paramToUnitMap[parameterName]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1, ADV_TABLE_COLUMN_UNIT,item);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,ADV_TABLE_COLUMN_UNIT,item);
        }

        // Param range
        if (m_paramToRangeMap.contains(parameterName))
        {
            QTableWidgetItem *item = new QTableWidgetItem(m_paramToRangeMap[parameterName]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1, ADV_TABLE_COLUMN_RANGE,item);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem("");
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,ADV_TABLE_COLUMN_RANGE,item);
        }

        // Description
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
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,ADV_TABLE_COLUMN_DESCRIPTION,item);


        m_paramValueMap[parameterName] = ui.tableWidget->item(ui.tableWidget->rowCount()-1,ADV_TABLE_COLUMN_VALUE);
        ui.tableWidget->sortByColumn(0,Qt::AscendingOrder);
    }

    if (m_origBrushList.contains(parameterName))
    {
        m_paramValueMap[parameterName]->setBackground(QBrush());
        ui.tableWidget->item(m_paramValueMap[parameterName]->row(),ADV_TABLE_COLUMN_PARAM)->setBackground(QBrush());
        ui.tableWidget->item(m_paramValueMap[parameterName]->row(),ADV_TABLE_COLUMN_VALUE)->setBackground(QBrush());
        ui.tableWidget->item(m_paramValueMap[parameterName]->row(),ADV_TABLE_COLUMN_UNIT)->setBackground(QBrush());
        ui.tableWidget->item(m_paramValueMap[parameterName]->row(),ADV_TABLE_COLUMN_RANGE)->setBackground(QBrush());
        ui.tableWidget->item(m_paramValueMap[parameterName]->row(),ADV_TABLE_COLUMN_DESCRIPTION)->setBackground(QBrush());
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
    m_paramToOrigValueMap[parameterName] = valstr;
    m_paramValueMap[parameterName]->setText(valstr);
    connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));

    if(m_writingParams) {
        ++m_paramsWritten;

        QString str;

        if(m_paramsWritten >= m_paramsToWrite) {
            str.sprintf("%d params written", m_paramsWritten);
            m_writingParams = false;
            QTimer::singleShot(500,ui.progressLabel, SLOT(hide()));
            QTimer::singleShot(500,ui.paramProgressBar, SLOT(hide()));
        }
        else {
            str.sprintf("%d of %d", m_paramsWritten, m_paramsToWrite);
        }

        ui.progressLabel->setText(str);
        ui.paramProgressBar->setValue(m_paramsWritten);
    }
}

void AdvParameterList::parameterChanged(int uas, int component, int parameterCount, int parameterId, QString parameterName, QVariant value)
{
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

void AdvParameterList::downloadRemoteFiles()
{
    QLOG_DEBUG() << "DownloadRemoteFiles";

    DownloadRemoteParamsDialog* dialog = new DownloadRemoteParamsDialog(this, true);

    if(dialog->exec() == QDialog::Accepted) {
        // Pull the selected file and
        // modify the parameters on the adv param list.
        QLOG_DEBUG() << "Remote File Downloaded";
        QLOG_DEBUG() << "TODO: Trigger auto load or compare of the downloaded file";

        // Bring up the compare dialog
        m_paramFileToCompare = dialog->getDownloadedFileName();
        QTimer::singleShot(300, this, SLOT(compareButtonClicked()));
    }
    dialog->deleteLater();
    dialog = NULL;
}

void AdvParameterList::updateTableWidgetElements(QMap<QString, UASParameter *> &parameterList)
{
    foreach(UASParameter* param, parameterList){
        // Modify the elements in the table widget.
        if (param->isModified()){
            // Update the local table widget
            QTableWidgetItem* item = m_paramValueMap.value(param->name());
            if (item){
                if(param->value().toDouble() != item->data(Qt::DisplayRole).toDouble()){
                    item->setData(Qt::DisplayRole, param->value());
                    tableWidgetItemChanged(item);
                }
            }
        }
    }
}

void AdvParameterList::compareButtonClicked()
{
    QLOG_DEBUG() << "Compare Params to File";

    ParamCompareDialog* dialog = new ParamCompareDialog(m_parameterList, m_paramFileToCompare, this);

    if(dialog->exec() == QDialog::Accepted) {
        // Apply the selected parameters
        // [TODO] For now just scan the returned new list and update the advanced tableview
        updateTableWidgetElements(m_parameterList);
    }
    m_paramFileToCompare = ""; // clear any previous filenames
    dialog->deleteLater();
    dialog = NULL;
}

void AdvParameterList::findStringInTable(const QString &searchString)
{
    QLOG_DEBUG() << "Find String in table: " << searchString;

    // Don't want the items to be considered changed
    disconnect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(tableWidgetItemChanged(QTableWidgetItem*)));

    if (m_searchItemList.count() > 0){
       foreach(QTableWidgetItem *item, m_searchItemList){
            item->setBackground(QBrush());
            item->setSelected(false);
       }
    }

    m_searchItemList.clear();
    if (searchString.length() > 2){ //need at least three characters to search
        m_searchItemList = ui.tableWidget->findItems(searchString, Qt::MatchContains);
    }

    if (m_searchItemList.count() > 0){
        foreach(QTableWidgetItem *item, m_searchItemList){
            item->setBackgroundColor(QColor(255,255,160));
        }
        m_searchIndex = m_searchIndex < m_searchItemList.count()? m_searchIndex
                                                                : m_searchItemList.count() - 1;
        ui.tableWidget->scrollToItem(m_searchItemList[m_searchIndex],QAbstractItemView::PositionAtCenter);
        m_searchItemList[m_searchIndex]->setSelected(true);
    }

    // Reconnect changed signal
    connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)),
            this, SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
}

void AdvParameterList::nextItemInSearch()
{
    QLOG_DEBUG() << "Find Next Item in table: ";
    if (m_searchItemList.count()==0)
        return;

    m_searchItemList[m_searchIndex]->setSelected(false);
    m_searchIndex++;
    if(m_searchIndex < m_searchItemList.count()){
        ui.tableWidget->scrollToItem(m_searchItemList[m_searchIndex],QAbstractItemView::PositionAtCenter);
        m_searchItemList[m_searchIndex]->setSelected(true);
    } else {
        m_searchIndex = 0; // loop around
    }
}

void AdvParameterList::previousItemInSearch()
{
    QLOG_DEBUG() << "Find Previous Item in table: ";

    if (m_searchItemList.count()==0)
        return;

    m_searchItemList[m_searchIndex]->setSelected(false);
    m_searchIndex--;
    if(m_searchIndex >= 0){
        ui.tableWidget->scrollToItem(m_searchItemList[m_searchIndex],QAbstractItemView::PositionAtCenter);
        m_searchItemList[m_searchIndex]->setSelected(true);
    } else {
        m_searchIndex = m_searchItemList.count() - 1; // loops around
    }
}
void AdvParameterList::resetButtonClicked()
{
    if (!m_uas)
    {
        showNullMAVErrorMessageBox();
        return;
    }
    if (QMessageBox::question(this,"Warning","You are about to reset ALL EEPROM settings to their defaults and REBOOT the vehicle. Are you absolutely sure you want to do this?",QMessageBox::Yes,QMessageBox::No) == QMessageBox::Yes)
    {
        m_uas->setParameter(0,"FORMAT_VERSION",0); // Plane
        m_uas->setParameter(0,"SYSID_SW_MREV",0); // Copter
        QTimer::singleShot(1000,m_uas, SLOT(reboot()));
        QMessageBox::information(this,"Reboot","Please power cycle your autopilot");
    }
    else
    {
        QMessageBox::information(this,".","No Reset!!");
    }
}
