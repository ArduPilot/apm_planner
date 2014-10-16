/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>
(c) author: Bill Bonney <billbonney@communistech.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

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
    void showLoadFileDialog();
    void loadParameterFile();
    void loadParameterWithFile();
    void loadParameterFile(const QString& filename);
    void saveNewParameters();
    void checkAll();
    bool paramCompareEqual(const QVariant& leftValue, const QVariant& rightValue);
    void dialogRejected();

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
