/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

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

/**
 * @file
 *   @brief Advanced Parameter list, a list of all parameters on vehicle, with save/load capabiltiy
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */

#ifndef ADVPARAMETERLIST_H
#define ADVPARAMETERLIST_H

#include "UASParameter.h"
// #include "UASParameterManager.h"
#include <QWidget>
#include "ui_AdvParameterList.h"
#include "AP2ConfigWidget.h"

class QFileDialog;

class AdvParameterList : public AP2ConfigWidget
{
    Q_OBJECT

    // Download state machine for parameters
    enum ParamDownloadState { none, starting, refreshing, completed };

public:
    explicit AdvParameterList(QWidget *parent = 0);
    void setParameterMetaData(const QString& name, const QString& humanname, const QString& description,
                              const QString& unit, const QString& range);
    ~AdvParameterList();
    void updateTableWidgetElements(QMap<QString, UASParameter*> &parameterList);
private slots:
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void parameterChanged(int uas, int component, int parameterCount, int parameterId,
                          QString parameterName, QVariant value);
    void refreshButtonClicked();
    void writeButtonClicked();
    void tableWidgetItemChanged(QTableWidgetItem* item);
    void loadButtonClicked();
    void saveButtonClicked();
    void downloadRemoteFiles();
    void compareButtonClicked();
    void findStringInTable(const QString& searchString);
    void nextItemInSearch();
    void previousItemInSearch();
    void resetButtonClicked();
    void loadDialogAccepted();
    void saveDialogAccepted();
    void dialogRejected();

private:
    Ui::AdvParameterList ui;
    QMap<QString, UASParameter*> m_parameterList;

    QMap<QString,QTableWidgetItem*> m_paramValueMap;
    QList<QString> m_origBrushList;
    QList<QString> m_waitingParamList;
    QMap<QString,QString> m_paramToNameMap;
    QMap<QString,QString> m_paramToDescriptionMap;
    QMap<QString,double> m_modifiedParamMap;
    QMap<QString,QString> m_paramToUnitMap;
    QMap<QString,QString> m_paramToRangeMap;
    QMap<QString,QString> m_paramToOrigValueMap;

    QList<QTableWidgetItem *> m_searchItemList;
    int m_searchIndex;

    ParamDownloadState m_paramDownloadState;
    int m_paramDownloadCount;
    int m_paramTotalCount;

    bool m_writingParams;
    int m_paramsWritten;
    int m_paramsToWrite;

    QString m_paramFileToCompare;
    QFileDialog *m_fileDialog;
};

#endif // ADVPARAMETERLIST_H
