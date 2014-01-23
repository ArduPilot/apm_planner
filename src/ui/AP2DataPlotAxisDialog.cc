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
    //connect(ui->graphTableWidget,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this,SLOT(graphTableCurrentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));
    connect(ui->graphTableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(graphTableItemSelectionChanged()));
    connect(ui->setMinMaxPushButton,SIGNAL(clicked()),this,SLOT(setMinMaxButtonClicked()));
    connect(ui->autoRadioButton,SIGNAL(clicked(bool)),this,SLOT(autoButtonClicked(bool)));
    connect(ui->groupARadioButton,SIGNAL(clicked(bool)),this,SLOT(groupAButtonClicked(bool)));
    connect(ui->groupBRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupBButtonClicked(bool)));
    connect(ui->groupCRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupCButtonClicked(bool)));
    connect(ui->groupDRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupDButtonClicked(bool)));
    connect(ui->manualRadioButton,SIGNAL(clicked(bool)),this,SLOT(manualButtonClicked(bool)));

    ui->minMaxGroupBox->setEnabled(false);
    ui->axisRangeGroupBox->setEnabled(false);

    ui->graphTableWidget->setColumnCount(5);
    ui->graphTableWidget->setColumnWidth(0,25);
    ui->graphTableWidget->setColumnWidth(1,100);
    ui->graphTableWidget->setColumnWidth(2,100);
    ui->graphTableWidget->setColumnWidth(3,60);
    ui->graphTableWidget->setColumnWidth(4,60);
    ui->graphTableWidget->setHorizontalHeaderLabels(QStringList() << "C" << "Graph Name" << "Graph Group" << "Min" << "Max");
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
    if (m_graphRangeMap.contains(graphname))
    {
        m_graphRangeMap.remove(graphname);
    }
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
    ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("MANUAL");
    if (m_graphToGroupNameMap[graphname] == "MANUAL")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "MANUAL";
    ui->minMaxGroupBox->setEnabled(true);
    if (!m_graphRangeMap.contains(graphname))
    {
        //m_graphRangeMap.remove(graphname);
        m_graphRangeMap[graphname] = QPair<double,double>(m_rangeMap[graphname].first,m_rangeMap[graphname].second);
        ui->minDoubleSpinBox->setValue(m_rangeMap[graphname].first);
        ui->maxDoubleSpinBox->setValue(m_rangeMap[graphname].second);
    }
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
    if (m_graphRangeMap.contains(graphname))
    {
        m_graphRangeMap.remove(graphname);
    }
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
    if (m_graphRangeMap.contains(graphname))
    {
        m_graphRangeMap.remove(graphname);
    }
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
    if (m_graphRangeMap.contains(graphname))
    {
        m_graphRangeMap.remove(graphname);
    }
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
    if (m_graphRangeMap.contains(graphname))
    {
        m_graphRangeMap.remove(graphname);
    }
}
void AP2DataPlotAxisDialog::applyButtonClicked()
{
    for (QMap<QString,QString>::const_iterator i=m_graphToGroupNameMap.constBegin();i!=m_graphToGroupNameMap.constEnd();i++)
    {
        if (i.value() == "NONE")
        {
            emit graphRemovedFromGroup(i.key());
        }
        else if (i.value() == "MANUAL")
        {
            emit graphManualRange(i.key(),m_graphRangeMap.value(i.key()).first,m_graphRangeMap.value(i.key()).second);
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
void AP2DataPlotAxisDialog::graphTableItemSelectionChanged()
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        ui->axisRangeGroupBox->setEnabled(false);
        return;
    }
    ui->axisRangeGroupBox->setEnabled(true);
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
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
            if (m_graphRangeMap.contains(graphname))
            {
                ui->minDoubleSpinBox->setValue(m_graphRangeMap.value(graphname).first);
                ui->maxDoubleSpinBox->setValue(m_graphRangeMap.value(graphname).second);
            }
            else if (m_rangeMap.contains(graphname))
            {
                ui->minDoubleSpinBox->setValue(m_rangeMap.value(graphname).first);
                ui->maxDoubleSpinBox->setValue(m_rangeMap.value(graphname).second);
            }
        }
        else
        {
            ui->minMaxGroupBox->setEnabled(false);
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

void AP2DataPlotAxisDialog::graphTableCurrentItemChanged(QTableWidgetItem *current,QTableWidgetItem *previous)
{
    if (!current || ui->graphTableWidget->selectedItems().size() == 0)
    {
        ui->axisRangeGroupBox->setEnabled(false);
        return;
    }
    ui->axisRangeGroupBox->setEnabled(true);
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
            if (m_graphRangeMap.contains(graphname))
            {
                ui->minDoubleSpinBox->setValue(m_graphRangeMap.value(graphname).first);
                ui->maxDoubleSpinBox->setValue(m_graphRangeMap.value(graphname).second);
            }
            else if (m_rangeMap.contains(graphname))
            {
                ui->minDoubleSpinBox->setValue(m_rangeMap.value(graphname).first);
                ui->maxDoubleSpinBox->setValue(m_rangeMap.value(graphname).second);
            }
        }
        else
        {
            ui->minMaxGroupBox->setEnabled(false);
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
    QTableWidgetItem *minitem = new QTableWidgetItem(QString::number(lower));
    minitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QTableWidgetItem *maxitem = new QTableWidgetItem(QString::number(upper));
    maxitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,0,coloritem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,1,nameitem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,2,groupitem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,3,minitem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,4,maxitem);
    m_graphScaleMap[name] = 1.0;
    m_rangeMap[name] = QPair<double,double>(lower,upper);


    ui->graphTableWidget->item(ui->graphTableWidget->rowCount()-1,0)->setBackgroundColor(color);
}
void AP2DataPlotAxisDialog::removeAxis(QString name)
{
    m_rangeMap.remove(name);
    m_graphScaleMap.remove(name);
    m_graphRangeMap.remove(name);
    m_graphToGroupNameMap.remove(name);
    QList<QTableWidgetItem*> items = ui->graphTableWidget->findItems(name,Qt::MatchExactly);
    if (items.size() > 0)
    {
        ui->graphTableWidget->removeRow(items[0]->row());
    }
}

void AP2DataPlotAxisDialog::updateAxis(QString name,double lower, double upper)
{
    if (!m_rangeMap.contains(name))
    {
        m_rangeMap[name] = QPair<double,double>();
    }
    m_rangeMap[name].first = lower;
    m_rangeMap[name].second = upper;
    for (int i=0;i<ui->graphTableWidget->rowCount();i++)
    {
        if (ui->graphTableWidget->item(i,1))
        {
            if (ui->graphTableWidget->item(i,1)->text() == name && ui->graphTableWidget->item(i,2)->text() != "MANUAL")
            {
                QTableWidgetItem *minitem = ui->graphTableWidget->item(i,3);
                QTableWidgetItem *maxitem = ui->graphTableWidget->item(i,4);
                if (!minitem)
                {
                    minitem = new QTableWidgetItem();
                    ui->graphTableWidget->setItem(i,3,minitem);
                }
                if (!maxitem)
                {
                    maxitem = new QTableWidgetItem();
                    ui->graphTableWidget->setItem(i,3,maxitem);
                }
                minitem->setText(QString::number(lower));
                maxitem->setText(QString::number(upper));
                return;
            }
        }
    }
}
void AP2DataPlotAxisDialog::setMinMaxButtonClicked()
{
    /*ui->autoRadioButton->setAutoExclusive(false);
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
    ui->groupDRadioButton->setAutoExclusive(true);*/
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    m_graphRangeMap[graphname].first = ui->minDoubleSpinBox->value();
    m_graphRangeMap[graphname].second = ui->maxDoubleSpinBox->value();
    QTableWidgetItem *minitem = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),3);
    QTableWidgetItem *maxitem = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),4);
    if (!minitem)
    {
        minitem = new QTableWidgetItem();
        ui->graphTableWidget->setItem(ui->graphTableWidget->selectedItems()[0]->row(),3,minitem);
    }
    if (!maxitem)
    {
        maxitem = new QTableWidgetItem();
        ui->graphTableWidget->setItem(ui->graphTableWidget->selectedItems()[0]->row(),3,maxitem);
    }
    minitem->setText(QString::number(m_graphRangeMap[graphname].first));
    maxitem->setText(QString::number(m_graphRangeMap[graphname].second));

}
