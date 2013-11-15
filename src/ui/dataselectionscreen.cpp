#include "dataselectionscreen.h"
#include <QCheckBox>
#include <QVBoxLayout>

DataSelectionScreen::DataSelectionScreen(QWidget *parent) : QWidget(parent)
{
	ui.setupUi(this);
	ui.scrollAreaWidgetContents->setLayout(new QVBoxLayout());
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
void DataSelectionScreen::addItem(QString name)
{
	QCheckBox *box = new QCheckBox(this);
	ui.scrollAreaWidgetContents->layout()->addWidget(box);
	box->setText(name);
	connect(box,SIGNAL(clicked(bool)),this,SLOT(checkBoxClicked(bool)));
	box->show();
    m_itemList.append(box);
}

void DataSelectionScreen::clear()
{
    for (int i=0;i<m_itemList.size();i++)
    {
        delete m_itemList[i];
    }
    m_itemList.clear();
}
