#ifndef PARAMCOMPAREDIALOG_H
#define PARAMCOMPAREDIALOG_H

#include "UASParameter.h"
#include <QDialog>
#include <QFile>
#include <QPointer>

namespace Ui {
class ParamCompareDialog;
}

class ParamCompareDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ParamCompareDialog(QWidget *parent = 0);
    ~ParamCompareDialog();

    // const QMap<QString, UASParameter*>& aParamaterList should be a global
    // Parameter Manager
    void setCurrentList(QMap<QString, UASParameter*>& aParamaterList);
    static void populateParamListFromString(QString paramString, QMap<QString, UASParameter *> *list, QWidget *widget);
    void compareLists();

private slots:
    void loadParameterFile();
    void loadParameterFile(const QString& filename);
    void saveNewParameters();
    void checkAll();

private:
    void initConnections();

private:
    Ui::ParamCompareDialog *ui;

    const QMap<QString, UASParameter*>*  m_currentList; // The list to change
    QMap<QString, UASParameter*>* m_newList;
    QList<UASParameter*> m_paramsToChange;

};

#endif // PARAMCOMPAREDIALOG_H
