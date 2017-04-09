#include <QCloseEvent>
#include <QWidget>
#include <QMap>
#include <QComboBox>
#include <QCheckBox>
#include <QColorDialog>
#include "AP2DataPlotAxisDialog.h"
#include "ui_AP2DataPlotAxisDialog.h"

AP2DataPlotAxisDialog::AP2DataPlotAxisDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AP2DataPlotAxisDialog)
{
    ui->setupUi(this);
    //qRegisterMetaType<QList<GraphRange> >("QList<GraphRange>");
    qRegisterMetaType<QList<AP2DataPlotAxisDialog::GraphRange> >("QList<AP2DataPlotAxisDialog::GraphRange>");
    //connect(ui->graphTableWidget,SIGNAL(currentItemChanged(QTableWidgetItem*,QTableWidgetItem*)),this,SLOT(graphTableCurrentItemChanged(QTableWidgetItem*,QTableWidgetItem*)));
    //connect(ui->graphTableWidget,SIGNAL(itemSelectionChanged()),this,SLOT(graphTableItemSelectionChanged()));
    connect(ui->graphTableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(cellDoubleClicked(int,int)));
    connect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));

    ui->graphTableWidget->setColumnCount(6);
    ui->graphTableWidget->setColumnWidth(0,25);
    ui->graphTableWidget->setColumnWidth(1,100);
    ui->graphTableWidget->setColumnWidth(2,100);
    ui->graphTableWidget->setColumnWidth(3,60);
    ui->graphTableWidget->setColumnWidth(4,60);
    ui->graphTableWidget->setColumnWidth(5,60);
    ui->graphTableWidget->setHorizontalHeaderLabels(QStringList() << "C" << "Graph Name" << "Graph Group" << "Min" << "Max" << "Auto Axis");
    ui->graphTableWidget->verticalHeader()->hide();

    connect(ui->applyPushButton,SIGNAL(clicked()),this,SLOT(applyButtonClicked()));
    connect(ui->clearPushButton,SIGNAL(clicked()),this,SLOT(clearButtonClicked()));
    connect(ui->cancelPushButton,SIGNAL(clicked()),this,SLOT(cancelButtonClicked()));


    //ui->label_4->setVisible(false);
    //ui->graphScaleDoubleSpinBox->setVisible(false);

    ui->graphTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}
void AP2DataPlotAxisDialog::cellDoubleClicked(int row,int col)
{
    if (!ui->graphTableWidget->item(row,col))
    {
        //Error condition, double clicked on a bad cell somehow?
        return;
    }
    if (col == 0)
    {
        //Color, show a picker!
        QColorDialog *dialog = new QColorDialog(this);
        m_colorDialogRowId = row;
        dialog->open(this,SLOT(colorDialogAccepted()));
        return;
    }
    if (col != 3 && col != 4)
    {
        //Not a min/max cell
        return;
    }
    QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(row,5));
    if (checkbox)
    {
        checkbox->setChecked(false);
    }
}
void AP2DataPlotAxisDialog::autoCheckboxChecked(bool checked)
{
    if (!checked)
    {
        return;
    }
    QCheckBox *senderbox = qobject_cast<QCheckBox*>(sender());
    for (int i=0;i<ui->graphTableWidget->rowCount();i++)
    {
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(i,5));
        if (checkbox && checkbox == senderbox)
        {
            //Checkbox just got checked, reset ranges to auto
            QString name = ui->graphTableWidget->item(i,1)->text();
            if (m_rangeMap.contains(name))
            {
                //Disconnect cellchanged, otherwise the checkbox gets re-disabled automatically
                disconnect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
                ui->graphTableWidget->item(i,3)->setText(QString::number(m_rangeMap.value(name).first));
                ui->graphTableWidget->item(i,4)->setText(QString::number(m_rangeMap.value(name).second));
                connect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
            }
            return;
        }
    }
}

void AP2DataPlotAxisDialog::colorDialogAccepted()
{
    QColorDialog *dialog = qobject_cast<QColorDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    ui->graphTableWidget->item(m_colorDialogRowId,0)->setBackgroundColor(dialog->selectedColor());
}

void AP2DataPlotAxisDialog::cellChanged(int row,int col)
{
    if (col != 3 && col != 4)
    {
        //Not a min/max cell
        return;
    }
    if (!ui->graphTableWidget->item(row,col))
    {
        //Error condition, invalid cell changed
        return;
    }
    QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(row,5));
    if (checkbox)
    {
        checkbox->setChecked(false);
    }
}

void AP2DataPlotAxisDialog::closeEvent(QCloseEvent *evt)
{
    evt->ignore();
    this->hide();
}

void AP2DataPlotAxisDialog::applyButtonClicked()
{
    /*for (QMap<QString,QString>::const_iterator i=m_graphToGroupNameMap.constBegin();i!=m_graphToGroupNameMap.constEnd();i++)
    {
        if (i.value() == "NONE")
        {
            emit graphRemovedFromGroup(i.key());
        }
        else if (i.value() == "MANUAL")
        {
            //emit graphManualRange(i.key(),m_graphRangeMap.value(i.key()).first,m_graphRangeMap.value(i.key()).second);
        }
        else
        {
            emit graphAddedToGroup(i.key(),i.value(),m_graphScaleMap.value(i.key()));
        }
    }*/
    QList<GraphRange> graphRangeList;
    QMap<QString,QColor> graphColorList;
    for (int i=0;i<ui->graphTableWidget->rowCount();i++)
    {
        GraphRange graph;
        QString name = ui->graphTableWidget->item(i,1)->text();
        QComboBox *combobox = qobject_cast<QComboBox*>(ui->graphTableWidget->cellWidget(i,2));
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(i,5));
        if (!checkbox || !combobox)
        {
            //error of some sort? Apply failed.
            return;
        }
        QString group = combobox->currentText();
        graph.graph = name;
        if (group == "NONE")
        {
            graph.isgrouped = false;
            emit graphRemovedFromGroup(name);
            if (!checkbox->isChecked())
            {
                //ui->graphTableWidget->item(i,1)->text(); // name
                //ui->graphTableWidget->item(i,3)->text(); // min
                //ui->graphTableWidget->item(i,4)->text(); // max
                graph.manual = true;
                graph.min = ui->graphTableWidget->item(i,3)->text().toDouble();
                graph.max = ui->graphTableWidget->item(i,4)->text().toDouble();
                emit graphManualRange(ui->graphTableWidget->item(i,1)->text(),ui->graphTableWidget->item(i,3)->text().toDouble(),ui->graphTableWidget->item(i,4)->text().toDouble());
            }
            else
            {
                graph.manual = false;
                emit graphAutoRange(ui->graphTableWidget->item(i,1)->text());
            }
        }
        else
        {
            graph.group = group;
            graph.isgrouped = true;
            graph.min = ui->graphTableWidget->item(i,3)->text().toDouble();
            graph.max = ui->graphTableWidget->item(i,4)->text().toDouble();
            checkbox->setChecked(true);
            emit graphAutoRange(name);
            emit graphAddedToGroup(name,group,m_graphScaleMap.value(name));
        }
        graphRangeList.append(graph);
        graphColorList[name] = ui->graphTableWidget->item(i,0)->backgroundColor();
    }
    emit graphGroupingChanged(graphRangeList);
    emit graphColorsChanged(graphColorList);
}

void AP2DataPlotAxisDialog::clearButtonClicked()
{
    disconnect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
    for (int i=0;i<ui->graphTableWidget->rowCount();i++)
    {
        QString graphname = ui->graphTableWidget->item(i,1)->text();
        m_graphToGroupNameMap[graphname] = "NONE";
        QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(i,5));
        checkbox->setChecked(true);
        QComboBox *combobox = qobject_cast<QComboBox*>(ui->graphTableWidget->cellWidget(i,2));
        combobox->setCurrentIndex(0);
        if (m_graphRangeMap.contains(graphname))
        {
            m_graphRangeMap.remove(graphname);
        }
        if (m_rangeMap.contains(graphname))
        {
            ui->graphTableWidget->item(i,3)->setText(QString::number(m_rangeMap.value(graphname).first));
            ui->graphTableWidget->item(i,4)->setText(QString::number(m_rangeMap.value(graphname).second));
        }

    }
    connect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
}

void AP2DataPlotAxisDialog::cancelButtonClicked()
{
    applyButtonClicked();
    this->hide();
}

AP2DataPlotAxisDialog::~AP2DataPlotAxisDialog()
{
    delete ui;
}

void AP2DataPlotAxisDialog::addAxis(QString name,double lower, double upper,QColor color)
{
    disconnect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
    m_rangeMap[name] = QPair<double,double>(lower,upper);
    ui->graphTableWidget->setRowCount(ui->graphTableWidget->rowCount()+1);
    QTableWidgetItem *nameitem = new QTableWidgetItem(name);
    nameitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QTableWidgetItem *groupitem = new QTableWidgetItem("");
    QTableWidgetItem *coloritem = new QTableWidgetItem("");
    coloritem->setFlags(Qt::ItemIsEnabled);
    QTableWidgetItem *minitem = new QTableWidgetItem(QString::number(lower));
    //minitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    QTableWidgetItem *maxitem = new QTableWidgetItem(QString::number(upper));
    //maxitem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,0,coloritem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,1,nameitem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,2,groupitem);
    QComboBox *groupcombobox = new QComboBox(this);
    groupcombobox->addItem("NONE",QVariant(name));
    groupcombobox->addItem("GROUPA",QVariant(name));
    groupcombobox->addItem("GROUPB",QVariant(name));
    groupcombobox->addItem("GROUPC",QVariant(name));
    groupcombobox->addItem("GROUPD",QVariant(name));
    connect(groupcombobox,SIGNAL(currentIndexChanged(int)),this,SLOT(groupComboChanged(int)));
    ui->graphTableWidget->setCellWidget(ui->graphTableWidget->rowCount()-1,2,groupcombobox);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,3,minitem);
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,4,maxitem);
    QTableWidgetItem *autoitem = new QTableWidgetItem();
    ui->graphTableWidget->setItem(ui->graphTableWidget->rowCount()-1,5,autoitem);
    QCheckBox *checkbox = new QCheckBox(this);
    connect(checkbox,SIGNAL(clicked(bool)),this,SLOT(autoCheckboxChecked(bool)));
    checkbox->setText("");
    checkbox->setChecked(true);
    ui->graphTableWidget->setCellWidget(ui->graphTableWidget->rowCount()-1,5,checkbox);
    m_graphScaleMap[name] = 1.0;
    m_rangeMap[name] = QPair<double,double>(lower,upper);


    ui->graphTableWidget->item(ui->graphTableWidget->rowCount()-1,0)->setBackgroundColor(color);
    connect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
}

void AP2DataPlotAxisDialog::groupComboChanged(int index)
{
    QComboBox *combo = qobject_cast<QComboBox*>(sender());
    QString graphname = combo->itemData(index).toString();
    QString name = combo->itemText(index);
    //QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    //QString name = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    //ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->setText("GROUPC");
    //if (m_graphToGroupNameMap[graphname] == "GROUPC")
    //{
    //    return;
    //}
    m_graphToGroupNameMap[graphname] = name;
    if (m_graphRangeMap.contains(graphname))
    {
        m_graphRangeMap.remove(graphname);
    }
}
void AP2DataPlotAxisDialog::clear()
{
    m_rangeMap.clear();
    m_graphScaleMap.clear();
    m_graphRangeMap.clear();
    m_graphToGroupNameMap.clear();
    ui->graphTableWidget->setRowCount(0);
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
            if (ui->graphTableWidget->item(i,1)->text() == name)
            {
                QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(i,5));
                if (checkbox)
                {
                    if (!checkbox->isChecked())
                    {
                        return;
                    }
                }

                disconnect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
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
                connect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
                return;
            }
        }
    }
}

void AP2DataPlotAxisDialog::fullAxisUpdate(QString name,double lower, double upper,bool ismanual, bool isingroup, QString groupname)
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
            if (ui->graphTableWidget->item(i,1)->text() == name)
            {
                QCheckBox *checkbox = qobject_cast<QCheckBox*>(ui->graphTableWidget->cellWidget(i,5));
                if (checkbox)
                {
                    if (ismanual)
                    {
                        checkbox->setChecked(false);
                        return;
                    }
                    else
                    {
                        checkbox->setChecked(true);
                    }
                }
                QComboBox *combobox = qobject_cast<QComboBox*>(ui->graphTableWidget->cellWidget(i,2));
                if (combobox)
                {
                    disconnect(combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(groupComboChanged(int)));
                    if (isingroup)
                    {
                        if (combobox->currentText() != groupname)
                        {
                            combobox->setCurrentIndex(combobox->findText(groupname));

                        }
                    }
                    else
                    {
                        combobox->setCurrentIndex(combobox->findText("NONE"));
                    }
                    connect(combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(groupComboChanged(int)));
                }
                disconnect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
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
                connect(ui->graphTableWidget,SIGNAL(cellChanged(int,int)),this,SLOT(cellChanged(int,int)));
                return;
            }
        }
    }
}
void AP2DataPlotAxisDialog::setMinMaxButtonClicked()
{
    if (ui->graphTableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),1)->text();
    QString groupname = ui->graphTableWidget->item(ui->graphTableWidget->selectedItems()[0]->row(),2)->text();
    //m_graphRangeMap[graphname].first = ui->minDoubleSpinBox->value();
    //m_graphRangeMap[graphname].second = ui->maxDoubleSpinBox->value();
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
