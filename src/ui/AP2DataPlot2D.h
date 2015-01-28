/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2015 APM_PLANNER PROJECT <http://www.diydrones.com>

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
 *   @brief AP2DataPlot widget class
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 */


#ifndef AP2DATAPLOT2D_H
#define AP2DATAPLOT2D_H

#include "UASInterface.h"
#include "MAVLinkDecoder.h"
#include "kmlcreator.h"
#include "qcustomplot.h"
#include "DroneshareUploadDialog.h"

#include "AP2DataPlotThread.h"
#include "dataselectionscreen.h"
#include "AP2DataPlotAxisDialog.h"
#include "AP2DataPlot2DModel.h"
#include "ui_AP2DataPlot2D.h"

#include <QWidget>
#include <QProgressDialog>
#include <QSortFilterProxyModel>
#include <QTextBrowser>
#include <QSqlDatabase>
#include <QStandardItemModel>

class LogDownloadDialog;

class AP2DataPlot2D : public QWidget
{
    Q_OBJECT
    
public:
    explicit AP2DataPlot2D(QWidget *parent = 0,bool isIndependant = false);
    ~AP2DataPlot2D();
    void addSource(MAVLinkDecoder *decoder);
    void loadLog(QString filename);

public slots:
    void showLogDownloadDialog();
    void closeLogDownloadDialog();
    void clearGraph();

    void logToKmlClicked();

private slots:
    //New Active UAS set
    void activeUASSet(UASInterface* uas);

    void connected();
    void disconnected();

    //Load a graph from a file
    void loadButtonClicked();
    void loadDialogAccepted();

    //Graph loading thread started
    void loadStarted();
    //Progress of graph loading thread
    void loadProgress(qint64 pos,qint64 size);
    //Cancel clicked on the graph loading thread progress dialog
    void progressDialogCanceled();
    //Graph loading thread finished
    void threadDone(int errors,MAV_TYPE type);
    //Graph loading thread actually exited
    void threadTerminated();
    //Graph loading thread error
    void threadError(QString errorstr);

    //Payload decoded from the graph loading thread
    void payloadDecoded(int index,QString name,QVariantMap map);

    //Called to remove an item from the graph
    void itemDisabled(QString name);
    //Called to add an item to the graph
    void itemEnabled(QString name);

    //ValueChanged functions for getting mavlink values
    void valueChanged(const int uasid, const QString& name, const QString& unit, const QVariant& value,const quint64 msecs);
    //Called by every valueChanged function to actually save the value/graph it.
    void updateValue(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec,bool integer = true);

    void navModeChanged(int uasid, int mode, const QString& text);

    void autoScrollClicked(bool checked);
    void addGraphLeft();
    void removeGraphLeft();
    void axisDoubleClick(QCPAxis* axis,QCPAxis::SelectablePart part,QMouseEvent* evt);
    void showOnlyClicked();
    void showAllClicked();
    void graphControlsButtonClicked();
    void plotMouseMove(QMouseEvent *evt);
    void horizontalScrollMoved(int value);
    void verticalScrollMoved(int value);
    void xAxisChanged(QCPRange range);
    void replyTLogButtonClicked();

    void droneshareButtonClicked();

    void exportButtonClicked();
    void exportDialogAccepted();

    void graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange> graphRangeList);
    void selectedRowChanged(QModelIndex current,QModelIndex previous);

    void modeCheckBoxClicked(bool checked);
    void errCheckBoxClicked(bool checked);
    void evCheckBoxClicked(bool checked);
    void sortItemChanged(QTreeWidgetItem* item,int col);
    void sortAcceptClicked();
    void sortCancelClicked();
    void showSortButtonClicked();
    void sortSelectAllClicked();
    void sortSelectInvertClicked();

    void childGraphDestroyed(QObject *obj);

    void setExcelViewHidden(bool hidden);

private:
    void showEvent(QShowEvent *evt);
    void hideEvent(QHideEvent *evt);
    AP2DataPlot2DModel *m_tableModel;
    QSortFilterProxyModel *m_tableFilterProxyModel;
    QList<QString> m_tableFilterList;
    int getStatusTextPos();
    void plotTextArrow(int index, const QString& text, const QString& graph, QCheckBox *checkBox = NULL);

private:
    Ui::AP2DataPlot2D ui;

    QTimer *m_updateTimer;
    class Graph
    {
    public:
        bool isManualRange;
        QString groupName;
        bool isInGroup;
        double axisIndex;
        QCPAxis *axis;
        QCPGraph *graph;
        QList<QCPAbstractItem*> itemList;
        QMap<double,QString> modeMap;
    };

    QMap<QString,Graph> m_graphClassMap;

    bool m_showOnlyActive;
    //Map of group name to a list of graph titles
    QMap<QString,QList<QString> > m_graphGrouping;
    //Map from group titles to the value axis range.
    QMap<QString,QCPRange> m_graphGroupRanges;
    //Map from the spreadsheet view row name (ATT,GPS,etc), to the header names (roll,pitch,yaw or long,lat,alt)
    QMap<QString,QString> m_tableHeaderNameMap;
    //Graph name to list of values for "online" mode
    QMap<QString,QList<QPair<double,double> > > m_onlineValueMap;
    //Map from graph name to list of values for "offline" mode
    QMap<QString,QList<QPair<int,QVariantMap> > > m_dataList;
    QList<QString> loglines;
    QSqlDatabase m_sharedDb;
    int currentIndex;

    QList<QWidget*> m_childGraphList;

    QList<QPair<qint64,double> > m_onlineValueTimeoutList;

    //List of graph names, used in m_axisList, m_graphMap,m_graphToGroupMap and the like as the graph name
    QList<QString> m_graphNameList;
    int m_graphCount;
    QCustomPlot *m_plot;
    QCPAxisRect *m_wideAxisRect;
    AP2DataPlotThread *m_logLoaderThread;
    //DataSelectionScreen *m_dataSelectionScreen;
    QStandardItemModel *m_model;
    bool m_logLoaded;
    //Current "index", X axis on graph. Used to keep all the graphs lined up.
    qint64 m_currentIndex;
    qint64 m_startIndex; //epoch msecs since graphing started
    QAction *m_addGraphAction;
    UASInterface *m_uas;
    QProgressDialog *m_progressDialog;
    AP2DataPlotAxisDialog *m_axisGroupingDialog;
    //qint64 m_timeDiff;
    bool m_tlogReplayEnabled;


    qint64 m_scrollStartIndex; //Actual graph start
    qint64 m_scrollEndIndex; //Actual graph end

    LogDownloadDialog *m_logDownloadDialog;
    DroneshareUploadDialog *m_droneshareUploadDialog;

    MAV_TYPE m_loadedLogMavType;

    QString m_filename;
    int m_statusTextPos;
};

#endif // AP2DATAPLOT2D_H
