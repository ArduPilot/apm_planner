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
signals:
	void itemEnabled(QString name);
	void itemDisabled(QString name);
private slots:
	void checkBoxClicked(bool checked);
private:
    QList<QWidget*> m_itemList;
	Ui::DataSelectionScreen ui;
};

#endif // DATASELECTIONSCREEN_H
