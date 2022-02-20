#ifndef DATASELECTIONSCREEN_H
#define DATASELECTIONSCREEN_H

#include <QWidget>
#include "ui_dataselectionscreen.h"

class DataSelectionScreen : public QWidget
{
	Q_OBJECT
	
public:
    explicit DataSelectionScreen(QWidget *parent = nullptr);
    ~DataSelectionScreen() override;
	void addItem(QString name);
    void addItems(const QMap<QString, QStringList> &fmtMap);
	void clear();
    void enableItem(const QString &name);
    void disableItem(const QString &name);
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

    void handleItem(const QString &name, Qt::CheckState checkState);
};

#endif // DATASELECTIONSCREEN_H
