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
    explicit ParamCompareDialog(QMap<QString, UASParameter*>& paramaterList,
                                const QString& filename, QWidget *parent = 0);
    ~ParamCompareDialog();

    void setAcceptButtonLabel(const QString& label);

    static void populateParamListFromString(QString paramString, QMap<QString, UASParameter *> *list, QWidget *widget);
    void compareLists();

private slots:
    void loadParameterFile();
    void loadParameterWithFile();
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
    const QString& m_fileToCompare;

};

#endif // PARAMCOMPAREDIALOG_H
