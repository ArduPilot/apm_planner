#include <QCloseEvent>
#include <QWidget>
#include <QMap>

#include "AP2DataPlotAxisDialog.h"
#include "ui_AP2DataPlotAxisDialog.h"

AP2DataPlotAxisDialog::AP2DataPlotAxisDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AP2DataPlotAxisDialog)
{
    ui->setupUi(this);
    connect(ui->graphTableWidget,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this,SLOT(graphTableCurrentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));
    connect(ui->setMinMaxPushButton,SIGNAL(clicked()),this,SLOT(setMinMaxButtonClicked()));
    connect(ui->autoRadioButton,SIGNAL(clicked(bool)),this,SLOT(autoButtonClicked(bool)));
    connect(ui->groupARadioButton,SIGNAL(clicked(bool)),this,SLOT(groupAButtonClicked(bool)));
    connect(ui->groupBRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupBButtonClicked(bool)));
    connect(ui->groupCRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupCButtonClicked(bool)));
    connect(ui->groupDRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupDButtonClicked(bool)));
    ui->minMaxGroupBox->setVisible(false);
    ui->manualRadioButton->setVisible(false);

    ui->graphTableWidget->setColumnCount(3);
    ui->graphTableWidget->setColumnWidth(0,25);
    ui->graphTableWidget->setColumnWidth(1,100);
    ui->graphTableWidget->setColumnWidth(2,100);
    ui->graphTableWidget->setHorizontalHeaderLabels(QStringList() << "Graph Name" << "Graph Group" << "C");
    ui->graphTableWidget->verticalHeader()->hide();

    connect(ui->applyPushButton,SIGNAL(clicked()),this,SLOT(applyButtonClicked()));
    connect(ui->cancelPushButton,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));


    ui->label_4->setVisible(false);
    ui->graphScaleDoubleSpinBox->setVisible(false);

    ui->graphTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}
void AP2DataPlotAxisDialog::autoButtonClicked(bool checked)
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("NONE");
    m_graphToGroupNameMap[graphname] = "NONE";
//    emit graphRemovedFromGroup(graphname);
}
void AP2DataPlotAxisDialog::manualButtonClicked(bool checked)
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("GROUPA");
    if (m_graphToGroupNameMap[graphname] == "MANUAL")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "MANUAL";
    ui->minMaxGroupBox->setEnabled(true);
}

void AP2DataPlotAxisDialog::closeEvent(QCloseEvent *evt)
{
    evt->ignore();
    this->hide();
}

void AP2DataPlotAxisDialog::groupAButtonClicked(bool checked)
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("GROUPA");
    if (m_graphToGroupNameMap[graphname] == "GROUPA")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPA";
}

void AP2DataPlotAxisDialog::groupBButtonClicked(bool checked)
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("GROUPB");
    if (m_graphToGroupNameMap[graphname] == "GROUPB")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPB";
}

void AP2DataPlotAxisDialog::groupCButtonClicked(bool checked)
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("GROUPC");
    if (m_graphToGroupNameMap[graphname] == "GROUPC")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPC";
}

void AP2DataPlotAxisDialog::groupDButtonClicked(bool checked)
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("GROUPD");
    if (m_graphToGroupNameMap[graphname] == "GROUPD")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPD";
}
void AP2DataPlotAxisDialog::applyButtonClicked()
{
    for (QMap<QString,QString>::const_iterator i=m_graphToGroupNameMap.constBegin();i!=m_graphToGroupNameMap.constEnd();i++)
    {
        if (i.value() == "NONE")
        {
            emit graphRemovedFromGroup(i.key());
        }
        else
        {
            emit graphAddedToGroup(i.key(),i.value(),m_graphScaleMap.value(i.key()));
        }
    }
}

void AP2DataPlotAxisDialog::cancelButtonClicked()
{
    this->hide();
}

AP2DataPlotAxisDialog::~AP2DataPlotAxisDialog()
{
    delete ui;
}
void AP2DataPlotAxisDialog::graphTableCurrentItemChanged(QTableWidgetItem *current,QTableWidgetItem *previous)
{
    if (!current)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(current->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(current->row(),2)->text();
    if (m_graphToGroupNameMap.contains(graphname))
    {
        if (m_graphToGroupNameMap[graphname] == "GROUPA")
        {
            ui->groupARadioButton->setChecked(true);
            ui->minMaxGroupBox->setEnabled(false);
        }
        else if (m_graphToGroupNameMap[graphname] == "GROUPB")
        {
            ui->groupBRadioButton->setChecked(true);
            ui->minMaxGroupBox->setEnabled(false);
        }
        else if (m_graphToGroupNameMap[graphname] == "GROUPC")
        {
            ui->groupCRadioButton->setChecked(true);
            ui->minMaxGroupBox->setEnabled(false);
        }
        else if (m_graphToGroupNameMap[graphname] == "GROUPD")
        {
            ui->groupDRadioButton->setChecked(true);
            ui->minMaxGroupBox->setEnabled(false);
        }
        else if (m_graphToGroupNameMap[graphname] == "NONE")
        {
            ui->autoRadioButton->setChecked(true);
            ui->minMaxGroupBox->setEnabled(false);
        }
        else if (m_graphToGroupNameMap[graphname] == "MANUAL")
        {
            ui->minMaxGroupBox->setEnabled(true);
        }
    }
    else
    {
        ui->autoRadioButton->setChecked(true);
    }
    if (m_graphScaleMap.contains(graphname))
    {
        ui->graphScaleDoubleSpinBox->setValue(m_graphScaleMap.value(graphname));
    }

}

void AP2DataPlotAxisDialog::addAxis(QString name,double lower, double upper,QColor color)
{
    m_rangeMap[name] = QPair<double,double>(lower,upper);
    ui->graphTableWidget->setRowCount(ui->graphTableWidget->rowCount()+1);
    QTableWidgetItem *nameitem = new QTableWidgetItem(name);
    nameitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QTableWidgetItem *groupitem = new QTableWidgetItem("");
    groupitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QTableWidgetItem *coloritem = new QTableWidgetItem("");
    coloritem->setFlags(Qt::ItemIsEnabled);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,0,coloritem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,1,nameitem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,2,groupitem);
    m_graphScaleMap[name] = 1.0;

    ui->graphTableWidget->item(ui->graphTableWidget->rowCount()-1,0)->setBackgroundColor(color);
}
void AP2DataPlotAxisDialog::removeAxis(QString name)
{
    m_rangeMap.remove(name);
    m_graphScaleMap.remove(name);
    QList<QTableWidgetItem*> items = ui->graphTableWidget->findItems(name,Qt::MatchExactly);
    if (items.size() > 0)
    {
        ui->graphTableWidget->removeRow(items[0]->row());
    }
}

void AP2DataPlotAxisDialog::updateAxis(QString name,double lower, double upper)
{
    if (m_rangeMap.contains(name))
    {
        m_rangeMap[name].first = lower;
        m_rangeMap[name].second = upper;
    }
    else
    {
        //addAxis(name,lower,upper);
    }
}
void AP2DataPlotAxisDialog::setMinMaxButtonClicked()
{
    ui->autoRadioButton->setAutoExclusive(false);
    ui->autoRadioButton->setChecked(false);
    ui->autoRadioButton->setAutoExclusive(true);

    ui->groupARadioButton->setAutoExclusive(false);
    ui->groupARadioButton->setChecked(false);
    ui->groupARadioButton->setAutoExclusive(true);

    ui->groupBRadioButton->setAutoExclusive(false);
    ui->groupBRadioButton->setChecked(false);
    ui->groupBRadioButton->setAutoExclusive(true);

    ui->groupCRadioButton->setAutoExclusive(false);
    ui->groupCRadioButton->setChecked(false);
    ui->groupCRadioButton->setAutoExclusive(true);

    ui->groupDRadioButton->setAutoExclusive(false);
    ui->groupDRadioButton->setChecked(false);
    ui->groupDRadioButton->setAutoExclusive(true);

}
