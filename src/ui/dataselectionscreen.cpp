#include "dataselectionscreen.h"
#include <QCheckBox>
#include <QVBoxLayout>

DataSelectionScreen::DataSelectionScreen(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
    //ui.scrollAreaWidgetContents->setLayout(new QVBoxLayout());
    //connect(ui.treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),this,SLOT(treeDoubleClicked(QTreeWidgetItem*,int)));
    connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(treeDoubleClicked(QTreeWidgetItem*,int)));
    connect(ui.clearPushButton,SIGNAL(clicked()),this,SLOT(clearSelectionButtonClicked()));
}

DataSelectionScreen::~DataSelectionScreen()
{
}
void DataSelectionScreen::checkBoxClicked(bool checked)
{
	QCheckBox *check = qobject_cast<QCheckBox*>(sender());
	if (!check)
	{
		return;
	}
	if (checked)
	{
		emit itemEnabled(check->text());
	}
	else
	{
		emit itemDisabled(check->text());
	}
}
void DataSelectionScreen::clearSelectionButtonClicked()
{
    QList<QTreeWidgetItem*> items = ui.treeWidget->findItems("",Qt::MatchContains | Qt::MatchRecursive);
    for (int i=0;i<items.size();i++)
    {
        if (items[i]->parent())
        {
            if (items[i]->checkState(0) == Qt::Checked)
            {
                items[i]->setCheckState(0,Qt::Unchecked);
                emit itemDisabled("M1:" + items[i]->parent()->text(0) + "." + items[i]->text(0));
            }
        }
    }

}

void DataSelectionScreen::enableItem(QString name)
{
    QString first = name.split(".")[0];
    QString second = name.split(".")[1];
    QList<QTreeWidgetItem*> items = ui.treeWidget->findItems(second,Qt::MatchContains | Qt::MatchRecursive);
    if (items.size() == 0)
    {
        return;
    }
    for (int i=0;i<items.size();i++)
    {
        if (items[i]->parent())
        {
            if (items[i]->parent()->text(0).contains(first))
            {
                items[i]->setCheckState(0,Qt::Checked);
                ui.treeWidget->scrollToItem(items[i]);
                return;
            }
        }
    }

}

void DataSelectionScreen::disableItem(QString name)
{
    QString first = name.split(".")[0];
    QString second = name.split(".")[1];
    QList<QTreeWidgetItem*> items = ui.treeWidget->findItems(second,Qt::MatchContains | Qt::MatchRecursive);
    if (items.size() == 0)
    {
        return;
    }
    for (int i=0;i<items.size();i++)
    {
        if (items[i]->parent()->text(0).contains(first))
        {
            items[0]->setCheckState(0,Qt::Unchecked);
            //ui.treeWidget->scrollToItem(items[0]);
            return;
        }
    }
}

void DataSelectionScreen::addItem(QString name)
{
    if (name.contains(":"))
    {
        name = name.mid(name.indexOf(":")+1);
    }
    if (name.contains("."))
    {
        //It's a split name
        QString shortname = name.split(".")[1];
        QString groupname = name.split(".")[0];
        QList<QTreeWidgetItem*> findlist = ui.treeWidget->findItems(groupname,Qt::MatchContains);
        if (findlist.size() > 0)
        {
            QTreeWidgetItem *child = new QTreeWidgetItem(QStringList() << shortname);
            child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
            child->setCheckState(0,Qt::Unchecked);
            findlist[0]->addChild(child);

        }
        else
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(QStringList() << groupname);
            ui.treeWidget->addTopLevelItem(item);
            QTreeWidgetItem *child = new QTreeWidgetItem(QStringList() << shortname);
            child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
            child->setCheckState(0,Qt::Unchecked);
            item->addChild(child);
        }

    }
    //QCheckBox *box = new QCheckBox(this);
    //ui.scrollAreaWidgetContents->layout()->addWidget(box);
    //box->setText(name);
    //connect(box,SIGNAL(clicked(bool)),this,SLOT(checkBoxClicked(bool)));
    //box->show();
    //m_itemList.append(box);
}
void DataSelectionScreen::treeDoubleClicked(QTreeWidgetItem* item,int column)
{
    if (!item->parent())
    {
        return;
    }
    QString name = "M1:" + item->parent()->text(0) + "." + item->text(0);
    if (item->checkState(0) == Qt::Checked)
    {
        if (!m_enabedList.contains(name))
        {
            m_enabedList.append(name);
            emit itemEnabled(name);
        }
    }
    else
    {
        if (m_enabedList.contains(name))
        {
            m_enabedList.removeOne(name);
            emit itemDisabled(name);
        }
    }
}

void DataSelectionScreen::clear()
{
    ui.treeWidget->clear();
    m_enabedList.clear();
    //for (int i=0;i<m_itemList.size();i++)
    //{
    //    delete m_itemList[i];
    //}
    //m_itemList.clear();
}
