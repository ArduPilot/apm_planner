#ifndef ADVPARAMETERLIST_H
#define ADVPARAMETERLIST_H

#include <QWidget>
#include "ui_AdvParameterList.h"
#include "AP2ConfigWidget.h"

class AdvParameterList : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit AdvParameterList(QWidget *parent = 0);
    void setParameterMetaData(QString name,QString humanname,QString description,QString unit);
    ~AdvParameterList();
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void refreshButtonClicked();
    void writeButtonClicked();
    void tableWidgetItemChanged(QTableWidgetItem* item);
    void loadButtonClicked();
    void saveButtonClicked();
private:
    QMap<QString,QTableWidgetItem*> m_paramValueMap;
    QList<QString> m_origBrushList;
    QMap<QString,QString> m_paramToNameMap;
    QMap<QString,QString> m_paramToDescriptionMap;
    QMap<QString,double> m_modifiedParamMap;
    QMap<QString,QString> m_paramToUnitMap;
    Ui::AdvParameterList ui;
};

#endif // ADVPARAMETERLIST_H
