#ifndef DATASELECTIONSCREEN_H
#define DATASELECTIONSCREEN_H

#include <QWidget>
#include "ui_dataselectionscreen.h"

class DataSelectionScreen : public QWidget
{
	Q_OBJECT
	
public:
	explicit DataSelectionScreen(QWidget *parent = 0);
	~DataSelectionScreen();
	void addItem(QString name);
	void clear();
    void enableItem(QString name);
    void disableItem(QString name);
    QList<QString> disableAllItems();
    void enableItemList(QList<QString> &itemList);

signals:
	void itemEnabled(QString name);
	void itemDisabled(QString name);
private slots:
    void clearSelectionButtonClicked();
    void onItemChanged(QTreeWidgetItem* item,int column);

private:
    QMap<QString,QString> m_nameToSysId;
    QList<QWidget*> m_itemList;
	Ui::DataSelectionScreen ui;
    QList<QString> m_enabledList;
};

#endif // DATASELECTIONSCREEN_H
