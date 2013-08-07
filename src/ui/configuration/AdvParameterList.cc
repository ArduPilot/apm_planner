#include "AdvParameterList.h"
#include <QTableWidgetItem>

AdvParameterList::AdvParameterList(QWidget *parent) : AP2ConfigWidget(parent)
{
    ui.setupUi(this);
    connect(ui.refreshPushButton,SIGNAL(clicked()),this,SLOT(refreshButtonClicked()));
    connect(ui.writePushButton,SIGNAL(clicked()),this,SLOT(writeButtonClicked()));
    connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
    ui.tableWidget->setColumnCount(4);
    ui.tableWidget->horizontalHeader()->hide();
    ui.tableWidget->verticalHeader()->hide();
    ui.tableWidget->setColumnWidth(0,200);
    ui.tableWidget->setColumnWidth(1,100);
    ui.tableWidget->setColumnWidth(2,200);
    ui.tableWidget->setColumnWidth(3,800);
    initConnections();
}
void AdvParameterList::tableWidgetItemChanged(QTableWidgetItem* item)
{
    if (!ui.tableWidget->item(item->row(),2))
    {
        //Invalid item, something has gone awry.
        return;
    }
    item->setBackgroundColor(QColor::fromRgb(255,100,100));
    m_modifiedParamMap[ui.tableWidget->item(item->row(),2)->text()] = item->text().toDouble();
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
        m_uas->getParamManager()->setParameter(1,i.key(),i.value());
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
}

void AdvParameterList::setParameterMetaData(QString name,QString humanname,QString description)
{
    m_paramToNameMap[name] = humanname;
    m_paramToDescriptionMap[name] = description;
}

void AdvParameterList::parameterChanged(int uas, int component, QString parameterName, QVariant value)
{
    disconnect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));
    if (!m_paramValueMap.contains(parameterName))
    {
        ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
        if (m_paramToNameMap.contains(parameterName))
        {
            QTableWidgetItem *item = new QTableWidgetItem(m_paramToNameMap[parameterName]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,item);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem("Unknown");
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,0,item);
        }
        QTableWidgetItem *valitem = new QTableWidgetItem(QString::number(value.toFloat(),'f',2));
        valitem->setFlags(valitem->flags() | Qt::ItemIsEditable);
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,1,valitem);

        QTableWidgetItem *paramnameitem = new QTableWidgetItem(parameterName);
        paramnameitem->setFlags(paramnameitem->flags() ^ Qt::ItemIsEditable);
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,2,paramnameitem);
        if (m_paramToDescriptionMap.contains(parameterName))
        {
            QTableWidgetItem *item = new QTableWidgetItem(m_paramToDescriptionMap[parameterName]);
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,3,item);
        }
        else
        {
            QTableWidgetItem *item = new QTableWidgetItem("Unknown");
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
            ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,3,item);
        }
        m_paramValueMap[parameterName] = ui.tableWidget->item(ui.tableWidget->rowCount()-1,1);
        ui.tableWidget->sortByColumn(2,Qt::AscendingOrder);
    }
    m_paramValueMap[parameterName]->setText(QString::number(value.toFloat(),'f',2));
    m_paramValueMap[parameterName]->setBackgroundColor(QColor::fromRgb(255,255,255));
    connect(ui.tableWidget,SIGNAL(itemChanged(QTableWidgetItem*)),this,SLOT(tableWidgetItemChanged(QTableWidgetItem*)));

}
