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
    connect(ui->listWidget,SIGNAL(currentRowChanged(int)),this,SLOT(listCurrentChanged(int)));
    connect(ui->setMinMaxPushButton,SIGNAL(clicked()),this,SLOT(setMinMaxButtonClicked()));
    connect(ui->autoRadioButton,SIGNAL(clicked(bool)),this,SLOT(autoButtonClicked(bool)));
    connect(ui->groupARadioButton,SIGNAL(clicked(bool)),this,SLOT(groupAButtonClicked(bool)));
    connect(ui->groupBRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupBButtonClicked(bool)));
    connect(ui->groupCRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupCButtonClicked(bool)));
    connect(ui->groupDRadioButton,SIGNAL(clicked(bool)),this,SLOT(groupDButtonClicked(bool)));
    ui->minMaxGroupBox->setVisible(false);
}
void AP2DataPlotAxisDialog::autoButtonClicked(bool checked)
{
    if (ui->listWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->listWidget->selectedItems()[0]->text();
    if (m_graphToGroupNameMap.contains(graphname))
    {
        m_graphToGroupNameMap.remove(graphname);
    }
    emit graphRemovedFromGroup(graphname);
}
void AP2DataPlotAxisDialog::closeEvent(QCloseEvent *evt)
{
    evt->ignore();
    this->hide();
}

void AP2DataPlotAxisDialog::groupAButtonClicked(bool checked)
{
    if (ui->listWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->listWidget->selectedItems()[0]->text();
    if (m_graphToGroupNameMap[graphname] == "GROUPA")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPA";
    emit graphAddedToGroup(graphname,"GROUPA");
}

void AP2DataPlotAxisDialog::groupBButtonClicked(bool checked)
{
    if (ui->listWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->listWidget->selectedItems()[0]->text();
    if (m_graphToGroupNameMap[graphname] == "GROUPB")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPB";
    emit graphAddedToGroup(graphname,"GROUPB");
}

void AP2DataPlotAxisDialog::groupCButtonClicked(bool checked)
{
    if (ui->listWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->listWidget->selectedItems()[0]->text();
    if (m_graphToGroupNameMap[graphname] == "GROUPC")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPC";
    emit graphAddedToGroup(graphname,"GROUPC");
}

void AP2DataPlotAxisDialog::groupDButtonClicked(bool checked)
{
    if (ui->listWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString graphname = ui->listWidget->selectedItems()[0]->text();
    if (m_graphToGroupNameMap[graphname] == "GROUPD")
    {
        return;
    }
    m_graphToGroupNameMap[graphname] = "GROUPD";
    emit graphAddedToGroup(graphname,"GROUPD");
}

AP2DataPlotAxisDialog::~AP2DataPlotAxisDialog()
{
    delete ui;
}
void AP2DataPlotAxisDialog::listCurrentChanged(int index)
{
    if (index < 0 || index > ui->listWidget->count())
    {
        //Invalid index
        return;
    }
    QString itemtext = ui->listWidget->item(index)->text();
    if (m_graphToGroupNameMap.contains(itemtext))
    {
        if (m_graphToGroupNameMap[itemtext] == "GROUPA")
        {
            ui->groupARadioButton->setChecked(true);
        }
        else if (m_graphToGroupNameMap[itemtext] == "GROUPB")
        {
            ui->groupBRadioButton->setChecked(true);
        }
        else if (m_graphToGroupNameMap[itemtext] == "GROUPC")
        {
            ui->groupCRadioButton->setChecked(true);
        }
        else if (m_graphToGroupNameMap[itemtext] == "GROUPD")
        {
            ui->groupDRadioButton->setChecked(true);
        }

    }
    else
    {
        ui->autoRadioButton->setChecked(true);
    }
    if (m_rangeMap.contains(itemtext))
    {
        ui->minDoubleSpinBox->setValue(m_rangeMap[itemtext].first);
        ui->maxDoubleSpinBox->setValue(m_rangeMap[itemtext].second);

    }
}

void AP2DataPlotAxisDialog::addAxis(QString name,double lower, double upper)
{
    m_rangeMap[name] = QPair<double,double>(lower,upper);
    ui->listWidget->addItem(name);
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
        addAxis(name,lower,upper);
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
