#include "dataselectionscreen.h"
#include <QCheckBox>
#include <QVBoxLayout>

DataSelectionScreen::DataSelectionScreen(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
    connect(ui.treeWidget,SIGNAL(itemClicked(QTreeWidgetItem*,int)),this,SLOT(treeDoubleClicked(QTreeWidgetItem*,int)));
    connect(ui.clearPushButton,SIGNAL(clicked()),this,SLOT(clearSelectionButtonClicked()));
}

DataSelectionScreen::~DataSelectionScreen()
{
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
                QString name = items[i]->parent()->text(0) + "." + items[i]->text(0);
                m_enabledList.removeOne(name);
                emit itemDisabled(name);
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
                m_enabledList.append(name);
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
            m_enabledList.removeOne(name);
            return;
        }
    }
}

void DataSelectionScreen::addItem(QString name)
{
    if (name.contains(":"))
    {
        QString sysid = name.mid(0,name.indexOf(":"));
        name = name.mid(name.indexOf(":")+1);
        m_nameToSysId[name] = sysid;
    }
    if (name.contains("."))
    {
        //It's a split name, "GCS Status.Roll" for instance.
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
}
void DataSelectionScreen::treeDoubleClicked(QTreeWidgetItem* item,int column)
{
    if (!item->parent())
    {
        return;
    }
    QString name = item->parent()->text(0) + "." + item->text(0);
    if (item->checkState(0) == Qt::Checked)
    {
        if (!m_enabledList.contains(name))
        {
            m_enabledList.append(name);
            emit itemEnabled(name);
        }
    }
    else
    {
        if (m_enabledList.contains(name))
        {
            m_enabledList.removeOne(name);
            emit itemDisabled(name);
        }
    }
}

void DataSelectionScreen::clear()
{
    ui.treeWidget->clear();
    m_enabledList.clear();
}
