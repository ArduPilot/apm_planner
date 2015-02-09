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


#include "QsLog.h"
#include "AP2DataPlot2D.h"
#include "LogDownloadDialog.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>
#include <QStringList>
#include "UAS.h"
#include "UASManager.h"
#include <QToolTip>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QsLog.h>
#include <QStandardItemModel>
#include "MainWindow.h"
#include "AP2DataPlot2DModel.h"
#include "ArduPilotMegaMAV.h"

#define ROW_HEIGHT_PADDING 3 //Number of additional pixels over font height for each row for the table/excel view.

AP2DataPlot2D::AP2DataPlot2D(QWidget *parent,bool isIndependant) : QWidget(parent),
    m_updateTimer(NULL),
    m_showOnlyActive(false),
    m_graphCount(0),
    m_plot(NULL),
    m_wideAxisRect(NULL),
    m_logLoaderThread(NULL),
    m_model(NULL),
    m_logLoaded(false),
    m_currentIndex(0),
    m_startIndex(0),
    m_addGraphAction(NULL),
    m_uas(NULL),
    m_progressDialog(NULL),
    m_axisGroupingDialog(NULL),
    m_tlogReplayEnabled(false),
    m_logDownloadDialog(NULL),
    m_droneshareUploadDialog(NULL),
    m_loadedLogMavType(MAV_TYPE_ENUM_END),
    m_statusTextPos(0)
{
    ui.setupUi(this);

    connect(ui.sortSelectTreeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(sortItemChanged(QTreeWidgetItem*,int)));
    connect(ui.sortAcceptPushButton,SIGNAL(clicked()),this,SLOT(sortAcceptClicked()));
    connect(ui.sortCancelPushButton,SIGNAL(clicked()),this,SLOT(sortCancelClicked()));
    connect(ui.sortShowPushButton,SIGNAL(clicked()),this,SLOT(showSortButtonClicked()));
    connect(ui.sortSelectAllPushButton,SIGNAL(clicked()),this,SLOT(sortSelectAllClicked()));
    connect(ui.sortInvertSelectPushButton,SIGNAL(clicked()),this,SLOT(sortSelectInvertClicked()));
    ui.tableSortGroupBox->setVisible(false);
    ui.sortShowPushButton->setVisible(false);
    ui.exportPushButton->setVisible(false);

    QDateTime utc = QDateTime::currentDateTimeUtc();
    utc.setTimeSpec(Qt::LocalTime);
    //m_timeDiff = QDateTime::currentDateTime().msecsTo(utc);
    m_plot = new QCustomPlot(ui.widget);
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);

    connect(m_plot,SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));

    connect(m_plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(plotMouseMove(QMouseEvent*)));

    connect(ui.modeDisplayCheckBox,SIGNAL(clicked(bool)),this,SLOT(modeCheckBoxClicked(bool)));
    connect(ui.errDisplayCheckBox,SIGNAL(clicked(bool)),this,SLOT(errCheckBoxClicked(bool)));
    connect(ui.evDisplayCheckBox,SIGNAL(clicked(bool)),this,SLOT(evCheckBoxClicked(bool)));

    //ui.horizontalLayout_3->addWidget(m_plot);
    ui.verticalLayout_5->insertWidget(0,m_plot);

    m_plot->show();
    m_plot->plotLayout()->clear();

    m_wideAxisRect = new QCPAxisRect(m_plot);
    m_wideAxisRect->setupFullAxesBox(true);
    m_wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);
    m_wideAxisRect->removeAxis(m_wideAxisRect->axis(QCPAxis::atLeft,0));

    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelType(QCPAxis::ltDateTime);
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setDateTimeFormat("hh:mm:ss");
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setDateTimeSpec(Qt::UTC);
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(0,100); //Default range of 0-100 milliseconds?


    m_plot->plotLayout()->addElement(0, 0, m_wideAxisRect);

    QCPMarginGroup *marginGroup = new QCPMarginGroup(m_plot);
    m_wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

    //m_dataSelectionScreen = new DataSelectionScreen(this);
    connect(ui.dataSelectionScreen,SIGNAL(itemEnabled(QString)),this,SLOT(itemEnabled(QString)));
    connect( ui.dataSelectionScreen,SIGNAL(itemDisabled(QString)),this,SLOT(itemDisabled(QString)));

    ui.horizontalLayout_3->setStretch(0,5);
    ui.horizontalLayout_3->setStretch(1,1);

    if (!isIndependant)
    {
        connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
        activeUASSet(UASManager::instance()->getActiveUAS());
    }

    ui.tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_addGraphAction = new QAction("Add To Graph",0);
    ui.tableWidget->addAction(m_addGraphAction);
    connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft()));

    //ui.tableWidget->setVisible(false);
    setExcelViewHidden(true);
    ui.tableWidget->verticalHeader()->setDefaultSectionSize(ui.tableWidget->fontMetrics().height() + ROW_HEIGHT_PADDING);
    ui.hideExcelView->setVisible(false);

    connect(ui.loadOfflineLogButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    connect(ui.autoScrollCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoScrollClicked(bool)));
    //connect(ui.hideExcelView,SIGNAL(clicked(bool)),ui.tableWidget,SLOT(setHidden(bool)));
    connect(ui.hideExcelView,SIGNAL(clicked(bool)),this,SLOT(setExcelViewHidden(bool)));
    connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCellChanged(int,int,int,int)));

    connect(ui.graphControlsPushButton,SIGNAL(clicked()),this,SLOT(graphControlsButtonClicked()));
    m_model = new QStandardItemModel();
    connect(ui.toKMLPushButton, SIGNAL(clicked()), this, SLOT(logToKmlClicked()));
    connect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));

    connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    connect(m_wideAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    m_plot->setPlottingHint(QCP::phFastPolylines,true);

    connect(ui.downloadPushButton, SIGNAL(clicked()), this, SLOT(showLogDownloadDialog()));
    ui.downloadPushButton->setEnabled(false);
    connect(ui.loadTLogButton,SIGNAL(clicked()),this,SLOT(replyTLogButtonClicked()));

    connect(ui.droneshareButton, SIGNAL(clicked()), this, SLOT(droneshareButtonClicked()));
    connect(ui.exportPushButton,SIGNAL(clicked()),this,SLOT(exportButtonClicked()));

    ui.horizontalSplitter->setStretchFactor(0,20);
    ui.horizontalSplitter->setStretchFactor(1,1);

}
void AP2DataPlot2D::setExcelViewHidden(bool hidden)
{
    if (hidden)
    {
        ui.splitter->setSizes(QList<int>() << 1 << 0);
        ui.sortShowPushButton->setVisible(false);
    }
    else
    {
        ui.splitter->setSizes(QList<int>() << 1 << 1);
        ui.sortShowPushButton->setVisible(true);
    }
}
void AP2DataPlot2D::graphColorsChanged(QMap<QString,QColor> colormap)
{
    for (QMap<QString,QColor>::const_iterator i = colormap.constBegin();i!=colormap.constEnd();i++)
    {
        m_graphClassMap[i.key()].graph->setPen(i.value());
        m_graphClassMap[i.key()].axis->setLabelColor(i.value());
        m_graphClassMap[i.key()].axis->setTickLabelColor(i.value());
        m_graphClassMap[i.key()].axis->setTickLabelColor(i.value());
    }
}

void AP2DataPlot2D::graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange> graphRangeList)
{
    for (QMap<QString,Graph>::const_iterator i=m_graphClassMap.constBegin();i!=m_graphClassMap.constEnd();i++)
    {
        m_graphClassMap[i.key()].isManualRange = false;
        m_graphClassMap[i.key()].isInGroup = false;
        m_graphClassMap[i.key()].groupName = "";
        m_graphClassMap[i.key()].graph->rescaleValueAxis();
    }
    m_graphGrouping.clear();
    m_graphGroupRanges.clear();

    for (int i=0;i<graphRangeList.size();i++)
    {
        if (graphRangeList.at(i).isgrouped)
        {
            QString group = graphRangeList.at(i).group;
            m_graphClassMap[graphRangeList.at(i).graph].isInGroup = true;
            m_graphClassMap[graphRangeList.at(i).graph].groupName = group;
            if (!m_graphGroupRanges.contains(group))
            {
                m_graphGroupRanges[graphRangeList.at(i).group] = m_graphClassMap.value(graphRangeList.at(i).graph).axis->range();
                m_graphGrouping[group] = QList<QString>();
                m_graphGrouping[group].append(graphRangeList.at(i).graph);
            }
            else
            {
                m_graphGrouping[group].append(graphRangeList.at(i).graph);
                for (int j=0;j<m_graphGrouping[group].size();j++)
                {
                    if (m_graphClassMap.value(m_graphGrouping[group][j]).axis->range().upper > m_graphGroupRanges[group].upper)
                    {
                        m_graphGroupRanges[group].upper = m_graphClassMap.value(m_graphGrouping[group][j]).axis->range().upper;
                    }
                    if (m_graphClassMap.value(m_graphGrouping[group][j]).axis->range().lower < m_graphGroupRanges[group].lower)
                    {
                        m_graphGroupRanges[group].lower = m_graphClassMap.value(m_graphGrouping[group][j]).axis->range().lower;
                    }
                }
            }
        }
        else if (graphRangeList.at(i).manual)
        {
            m_graphClassMap[graphRangeList.at(i).graph].isManualRange = true;
            m_graphClassMap.value(graphRangeList.at(i).graph).axis->setRange(graphRangeList.at(i).min,graphRangeList.at(i).max);
        }
    }

    for (QMap<QString,QList<QString> >::const_iterator i = m_graphGrouping.constBegin();i!=m_graphGrouping.constEnd();i++)
    {
        QString group = i.key();
        for (int j=0;j<m_graphGrouping[group].size();j++)
        {
            m_graphClassMap.value(m_graphGrouping[group][j]).axis->setRange(m_graphGroupRanges[group]);
        }
    }
    m_plot->replot();
}

void AP2DataPlot2D::replyTLogButtonClicked()
{
    if (m_tlogReplayEnabled)
    {
        MainWindow::instance()->disableTLogReplayBar();
        m_tlogReplayEnabled = false;
        ui.loadTLogButton->setText("Enable Log Playback");
    }
    else
    {
        MainWindow::instance()->enableTLogReplayBar();
        m_tlogReplayEnabled = true;
        ui.loadTLogButton->setText("Disable Log Playback");
    }


}

void AP2DataPlot2D::xAxisChanged(QCPRange range)
{
    ui.horizontalScrollBar->setValue(qRound(range.center())); // adjust position of scroll bar slider
    ui.horizontalScrollBar->setPageStep(qRound(range.size())); // adjust size of scroll bar slider
    double totalrange = m_scrollEndIndex - m_scrollStartIndex;
    double currentrange = range.upper - range.lower;

    disconnect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    ui.verticalScrollBar->setValue(100 * (currentrange / totalrange));
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
}

void AP2DataPlot2D::horizontalScrollMoved(int value)
{
    if (qAbs(m_wideAxisRect->axis(QCPAxis::atBottom)->range().center()-value) > 0.01) // if user is dragging plot, we don't want to replot twice
    {
      m_wideAxisRect->axis(QCPAxis::atBottom)->setRange(value,m_wideAxisRect->axis(QCPAxis::atBottom)->range().size(), Qt::AlignCenter);
      m_plot->replot();
    }
    return;
}
void AP2DataPlot2D::showEvent(QShowEvent *evt)
{
    if (m_updateTimer)
    {
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
        m_updateTimer = 0;
    }
    m_updateTimer = new QTimer(this);
    connect(m_updateTimer,SIGNAL(timeout()),m_plot,SLOT(replot()));
    m_updateTimer->start(500);
    QWidget::showEvent(evt);
}

void AP2DataPlot2D::hideEvent(QHideEvent *evt)
{
    if (m_updateTimer)
    {
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
        m_updateTimer = 0;
    }
    QWidget::hideEvent(evt);
}

void AP2DataPlot2D::verticalScrollMoved(int value)
{
    double percent = value / 100.0;
    double center = m_wideAxisRect->axis(QCPAxis::atBottom)->range().center();
    double requestedrange = ((m_scrollEndIndex) - m_scrollStartIndex) * percent;
    m_wideAxisRect->axis(QCPAxis::atBottom)->setRangeUpper(center + (requestedrange/2.0));
    m_wideAxisRect->axis(QCPAxis::atBottom)->setRangeLower(center - (requestedrange/2.0));
    m_plot->replot();
}

void AP2DataPlot2D::plotMouseMove(QMouseEvent *evt)
{
    if (!ui.showValuesCheckBox->isChecked())
    {
        return;
    }
    QString newresult = "";
    for (int i=0;i<m_graphClassMap.keys().size();i++)
    {

        double key=0;
        double val=0;
        QCPGraph *graph = m_graphClassMap.value(m_graphClassMap.keys()[i]).graph;
        graph->pixelsToCoords(evt->x(),evt->y(),key,val);
        if (i == 0)
        {
            if (m_logLoaded)
            {
                newresult.append("Log Line: " + QString::number(key,'f',0) + "\n");
            }
            else
            {
                newresult.append("Time: " + QDateTime::fromMSecsSinceEpoch(key * 1000.0).toString("hh:mm:ss") + "\n");
            }
        }
        if (m_graphClassMap.keys()[i] == "MODE")
        {
            if (m_graphClassMap.value(m_graphClassMap.keys()[i]).modeMap.keys().size() > 1)
            {
                for (QMap<double,QString>::const_iterator modemapiterator = m_graphClassMap.value(m_graphClassMap.keys()[i]).modeMap.constBegin();modemapiterator!=m_graphClassMap.value(m_graphClassMap.keys()[i]).modeMap.constEnd();modemapiterator++)
                {
                    if (modemapiterator.key() < key)
                    {
                        if (modemapiterator==m_graphClassMap.value(m_graphClassMap.keys()[i]).modeMap.constEnd()-1)
                        {
                            //We're at the end, use the end
                            newresult.append(m_graphClassMap.keys()[i] + ": " + modemapiterator.value() + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
                        }
                        else if ((modemapiterator+1).key() > key)
                        {
                            //This only gets hit if we're not at the end, and we have the proper value
                            newresult.append(m_graphClassMap.keys()[i] + ": " + modemapiterator.value() + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
                            break;
                        }
                    }
                }
            }
            else if (m_graphClassMap.value(m_graphClassMap.keys()[i]).modeMap.keys().size() == 1)
            {
                newresult.append(m_graphClassMap.keys()[i] + ": " + m_graphClassMap.value(m_graphClassMap.keys()[i]).modeMap.begin().value() + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
            }
            else
            {
                newresult.append(m_graphClassMap.keys()[i] + ": " + "Unknown" + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
            }
        }
        else if (m_graphClassMap.keys()[i] == "ERR")
        {
            //Ignore ERR
        }
        else if (graph->data()->contains(key))
        {
            newresult.append(m_graphClassMap.keys()[i] + ": " + QString::number(graph->data()->value(key).value,'f',4) + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
        }
        else if (graph->data()->lowerBound(key) != graph->data()->constEnd())
        {
            newresult.append(m_graphClassMap.keys()[i] + ": " + QString::number((graph->data()->lowerBound(key).value().value),'f',4) + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
        }
        else
        {
            newresult.append(m_graphClassMap.keys()[i] + ": " + "ERR" + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
        }
    }
    QToolTip::showText(QPoint(evt->globalPos().x() + m_plot->x(),evt->globalPos().y()+m_plot->y()),newresult);
}

void AP2DataPlot2D::axisDoubleClick(QCPAxis* axis,QCPAxis::SelectablePart part,QMouseEvent* evt)
{
    Q_UNUSED(axis)
    Q_UNUSED(part)
    Q_UNUSED(evt)
    graphControlsButtonClicked();
}
void AP2DataPlot2D::graphControlsButtonClicked()
{
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->show();
        m_axisGroupingDialog->activateWindow();
        m_axisGroupingDialog->raise();
        //QApplication::postEvent(m_axisGroupingDialog, new QEvent(QEvent::Show));
        //QApplication::postEvent(m_axisGroupingDialog, new QEvent(QEvent::WindowActivate));
        for (QMap<QString,Graph>::const_iterator i=m_graphClassMap.constBegin();i!=m_graphClassMap.constEnd();i++)
        {
            //m_axisGroupingDialog->addAxis(i.key(),i.value().axis->range().lower,i.value().axis->range().upper,i.value().axis->labelColor());
            m_axisGroupingDialog->fullAxisUpdate(i.key(),i.value().axis->range().lower,i.value().axis->range().upper,i.value().isManualRange,i.value().isInGroup,i.value().groupName);
        }
        return;
    }
    m_axisGroupingDialog = new AP2DataPlotAxisDialog();
    connect(m_axisGroupingDialog,SIGNAL(graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange>)),this,SLOT(graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange>)));
    connect(m_axisGroupingDialog,SIGNAL(graphColorsChanged(QMap<QString,QColor>)),this,SLOT(graphColorsChanged(QMap<QString,QColor>)));
    for (QMap<QString,Graph>::const_iterator i=m_graphClassMap.constBegin();i!=m_graphClassMap.constEnd();i++)
    {
        m_axisGroupingDialog->addAxis(i.key(),i.value().axis->range().lower,i.value().axis->range().upper,i.value().axis->labelColor());
    }
    m_axisGroupingDialog->show();
    QApplication::postEvent(m_axisGroupingDialog, new QEvent(QEvent::Show));
    QApplication::postEvent(m_axisGroupingDialog, new QEvent(QEvent::WindowActivate));
}



void AP2DataPlot2D::addGraphLeft()
{
    if (ui.tableWidget->selectionModel()->selectedIndexes().size() == 0)
    {
        return;
    }

    QString itemtext = ui.tableWidget->model()->itemData(ui.tableWidget->model()->index(ui.tableWidget->selectionModel()->selectedIndexes().at(0).row(),1)).value(Qt::DisplayRole).toString();
    QString headertext = ui.tableWidget->model()->headerData(ui.tableWidget->selectionModel()->selectedIndexes().at(0).column(),Qt::Horizontal,Qt::DisplayRole).toString();
    ui.dataSelectionScreen->enableItem(itemtext + "." + headertext);
    //Whenever we add a graph, make the button a remove, since there is no selectedItemChanged signal if they re-click it
    //It's an enabled
    m_addGraphAction->setText("Remove From Graph");
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
    connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft()));

}
void AP2DataPlot2D::selectedRowChanged(QModelIndex current,QModelIndex previous)
{

    m_tableModel->selectedRowChanged(m_tableFilterProxyModel->mapToSource(current),m_tableFilterProxyModel->mapToSource(previous));
    if (ui.tableWidget->selectionModel()->selectedIndexes().size() == 0)
    {
        return;
    }
    if (current.column() == 0 || current.column() == 1)
    {
        //This is column 0 or 1, index and name
        if (m_showOnlyActive)
        {
            m_addGraphAction->setText("Show All");
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
            connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
        }
        else
        {
            m_addGraphAction->setText("Show only these rows");
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
            connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
        }
    }
    else
    {
        QString itemtext = ui.tableWidget->model()->itemData(ui.tableWidget->model()->index(current.row(),1)).value(Qt::DisplayRole).toString();
        QString headertext = ui.tableWidget->model()->headerData(current.column(),Qt::Horizontal,Qt::DisplayRole).toString();
        QString label = itemtext + "." + headertext;
        if (m_graphClassMap.contains(label))
        {
            //It's an enabled
            m_addGraphAction->setText("Remove From Graph");
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
            connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft()));
        }
        else
        {
            m_addGraphAction->setText("Add To Graph");
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
            disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
            connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Add addgraphleft
        }
    }
}
void AP2DataPlot2D::removeGraphLeft()
{
    if (ui.tableWidget->selectionModel()->selectedIndexes().size() == 0)
    {
        return;
    }
    QString itemtext = ui.tableWidget->model()->itemData(ui.tableWidget->model()->index(ui.tableWidget->selectionModel()->selectedIndexes().at(0).row(),1)).value(Qt::DisplayRole).toString();
    QString headertext = ui.tableWidget->model()->headerData(ui.tableWidget->selectionModel()->selectedIndexes().at(0).column(),Qt::Horizontal,Qt::DisplayRole).toString();
    QString label = itemtext + "." + headertext;
    ui.dataSelectionScreen->disableItem(itemtext + "." + headertext);
    m_addGraphAction->setText("Add To Graph");
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showOnlyClicked()));
    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(showAllClicked()));
    connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Add addgraphleft
}
void AP2DataPlot2D::showOnlyClicked()
{
    if (ui.tableWidget->selectionModel()->selectedIndexes().size() == 0)
    {
        return;
    }
    QString itemtext = ui.tableWidget->model()->itemData(ui.tableWidget->model()->index(ui.tableWidget->selectionModel()->selectedIndexes().at(0).row(),1)).value(Qt::DisplayRole).toString();
    m_tableFilterProxyModel->setFilterFixedString(itemtext);
    m_tableFilterProxyModel->setFilterRole(Qt::DisplayRole);
    m_tableFilterProxyModel->setFilterKeyColumn(1);
    m_showOnlyActive = true;
}
void AP2DataPlot2D::showAllClicked()
{
    m_tableFilterProxyModel->setFilterRegExp("");
    m_showOnlyActive = false;
}


void AP2DataPlot2D::autoScrollClicked(bool checked)
{
    if (checked)
    {
        if (m_graphCount > 0)
        {
            double msec_current = ((QDateTime::currentMSecsSinceEpoch()- m_startIndex)) / 1000.0;
            double difference = m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().upper - m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower;
            m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(msec_current - difference);
            m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper(msec_current);
        }
    }
}

void AP2DataPlot2D::activeUASSet(UASInterface* uas)
{
    if (!uas)
    {
        return;
    }
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));
        disconnect(m_uas,SIGNAL(navModeChanged(int,int,QString)),this,SLOT(navModeChanged(int,int,QString)));
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(connected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(disconnected()));
    }
    m_currentIndex = QDateTime::currentMSecsSinceEpoch();
    m_startIndex = m_currentIndex;
    m_scrollStartIndex = 0;
    ui.horizontalScrollBar->blockSignals(true);
    ui.horizontalScrollBar->setMinimum(m_scrollStartIndex);
    ui.horizontalScrollBar->blockSignals(false);
    m_uas = uas;

    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));
    connect(m_uas,SIGNAL(navModeChanged(int,int,QString)),this,SLOT(navModeChanged(int,int,QString)));

    connect(m_uas,SIGNAL(connected()),this,SLOT(connected()));
    connect(m_uas,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connected();

}

void AP2DataPlot2D::connected()
{
    ui.downloadPushButton->setEnabled(true);
}

void AP2DataPlot2D::disconnected()
{
     ui.downloadPushButton->setEnabled(false);
}

void AP2DataPlot2D::navModeChanged(int uasid, int mode, const QString& text)
{
    Q_UNUSED(mode);
    if (m_uas->getUASID() != uasid)
    {
        return;
    }
    if (m_logLoaded)
    {
        //If a log is currently loaded, we don't care about incoming data.
        return;
    }
    qint64 msec_current = QDateTime::currentMSecsSinceEpoch();
    m_currentIndex = msec_current;
    qint64 newmsec = (msec_current - m_startIndex);// + m_timeDiff;
    if (m_graphCount > 0 && ui.autoScrollCheckBox->isChecked())
    {
        double diff = (newmsec / 1000.0) - m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().upper;
        m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower + diff);
        m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper((newmsec / 1000.0));
    }
    if (!m_graphClassMap.contains("MODE"))
    {
        QCPAxis *axis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
        axis->setLabel("MODE");

        if (m_graphCount > 0)
        {
            connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
        }
        QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
        axis->setLabelColor(color);
        axis->setTickLabelColor(color);
        axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
        QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
        m_graphNameList.append("MODE");

        mainGraph1->setPen(QPen(color, 2));
        Graph graph;
        graph.axis = axis;
        graph.groupName = "";
        graph.graph=  mainGraph1;
        graph.isInGroup = false;
        graph.isManualRange = false;
        m_graphClassMap["MODE"] = graph;

        mainGraph1->rescaleValueAxis();
        if (m_graphCount == 1)
        {
            mainGraph1->rescaleKeyAxis();
        }
    }

    int index = newmsec / 1000.0;
    m_graphClassMap["MODE"].modeMap[index] = text;
    plotTextArrow(index, text, "MODE");

}

void AP2DataPlot2D::updateValue(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec,bool integer)
{
    Q_UNUSED(msec)
    Q_UNUSED(unit)
    if (m_uas->getUASID() != uasId)
    {
        return;
    }
    if (m_logLoaded)
    {
        //If a log is currently loaded, we don't care about incoming data.
        return;
    }
    QString propername  = name.mid(name.indexOf(":")+1);
    if (!m_onlineValueMap.contains(propername))
    {
        ui.dataSelectionScreen->addItem(propername);
    }


    qint64 msec_current = QDateTime::currentMSecsSinceEpoch();
    m_currentIndex = msec_current;
    qint64 newmsec = (msec_current - m_startIndex);// + m_timeDiff;
    if (m_graphCount > 0 && ui.autoScrollCheckBox->isChecked())
    {
        double diff = (newmsec / 1000.0) - m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().upper;
        m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower + diff);
        m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper((newmsec / 1000.0));
    }

    if (m_graphClassMap.contains(propername))
    {
        m_graphClassMap[propername].axisIndex = newmsec / 1000.0;// + 18000000;
        m_graphClassMap.value(propername).graph->addData(m_graphClassMap.value(propername).axisIndex,value);
        m_scrollEndIndex = newmsec /  1000.0;
        //ui.horizontalScrollBar->setMinimum(m_startIndex);
        ui.horizontalScrollBar->setMaximum(m_scrollEndIndex);
        //Set a timeout for 30 minutes from now, 1800 seconds.
        qint64 current = QDateTime::currentMSecsSinceEpoch();
        //This is 30 minutes
        m_onlineValueTimeoutList.append(QPair<qint64,double>(current + 1800000,msec));
        //This is 1 minute
        //m_onlineValueTimeoutList.append(QPair<qint64,double>(current + 60000,m_currentIndex));
        if (m_onlineValueTimeoutList[0].first <= current)
        {
            for (QMap<QString,Graph>::const_iterator i = m_graphClassMap.constBegin();i!=m_graphClassMap.constEnd();i++)
            {
                i.value().graph->removeData(0,m_onlineValueTimeoutList[0].second);
            }
            m_onlineValueTimeoutList.removeAt(0);
        }
        if (m_graphClassMap.value(propername).groupName != "" && m_graphClassMap.value(propername).groupName != "MANUAL")
        {
            //Current graph is in a group
            if (!m_graphGroupRanges[m_graphClassMap.value(propername).groupName].contains(value))
            {
                //It's out of scale for the group, expand it.
                if (m_graphGroupRanges[m_graphClassMap.value(propername).groupName].lower > value)
                {
                    m_graphGroupRanges[m_graphClassMap.value(propername).groupName].lower = value;
                }
                else if (m_graphGroupRanges[m_graphClassMap.value(propername).groupName].upper < value)
                {
                    m_graphGroupRanges[m_graphClassMap.value(propername).groupName].upper = value;
                }
                for (int i=0;i<m_graphGrouping[m_graphClassMap.value(propername).groupName].size();i++)
                {
                    m_graphClassMap.value(m_graphGrouping[m_graphClassMap.value(propername).groupName][i]).axis->setRange(m_graphGroupRanges[m_graphClassMap.value(propername).groupName]);
                }
                if (m_axisGroupingDialog)
                {
                    m_axisGroupingDialog->updateAxis(propername,m_graphClassMap.value(propername).axis->range().lower,m_graphClassMap.value(propername).axis->range().upper);
                }
            }
        }
        else if (!m_graphClassMap.value(propername).graph->keyAxis()->range().contains(value) && !m_graphClassMap.value(propername).isManualRange)
        {
            m_graphClassMap.value(propername).graph->rescaleValueAxis();
            if (m_axisGroupingDialog)
            {
                m_axisGroupingDialog->updateAxis(propername,m_graphClassMap.value(propername).axis->range().lower,m_graphClassMap.value(propername).axis->range().upper);
            }
        }
        if (integer)
        {
            m_graphClassMap.value(propername).axis->setNumberPrecision(0);
        }
    }
    m_onlineValueMap[propername].append(QPair<double,double>(newmsec / 1000.0,value));
}

void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant& value,const quint64 msec)
{
    if (value.type() == QVariant::Double || value.type() == QMetaType::Float)
    {
        updateValue(uasId,name,unit,value.toDouble(),msec,false);
    }
    else
    {
        updateValue(uasId,name,unit,static_cast<double>(value.toInt()),msec);
    }
}

void AP2DataPlot2D::loadButtonClicked()
{
    if (m_logLoaded){
        if (this->parent() == NULL){
            //Good to close out
            this->close();
        }
    } else {
        QFileDialog *dialog = new QFileDialog(this,"Load File",QGC::logDirectory(),"Dataflash Log Files (*.log *.bin *.tlog);;All Files (*.*)");
        dialog->setFileMode(QFileDialog::ExistingFile);
        dialog->open(this, SLOT(loadDialogAccepted()));
    }
}

void AP2DataPlot2D::loadDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    if (dialog->selectedFiles().size() == 0)
    {
        return;
    }
    m_filename = dialog->selectedFiles().first();

    AP2DataPlot2D *plot = new AP2DataPlot2D(0,true);
    connect(plot,SIGNAL(destroyed(QObject*)),this,SLOT(childGraphDestroyed(QObject*)));
    m_childGraphList.append(plot);
    plot->setAttribute(Qt::WA_DeleteOnClose,true);
    plot->show();
    plot->loadLog(m_filename);

    //loadLog(m_filename);

}
void AP2DataPlot2D::loadLog(QString filename)
{
    m_logLoaded = true;
    for (int i=0;i<m_graphNameList.size();i++)
    {
        m_wideAxisRect->removeAxis(m_graphClassMap.value(m_graphNameList[i]).axis);
        m_plot->removeGraph(m_graphClassMap.value(m_graphNameList[i]).graph);
    }
    ui.dataSelectionScreen->clear();
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->clear();
    }
    m_plot->replot();
    m_graphClassMap.clear();
    m_graphCount=0;
    m_dataList.clear();

    QString shortfilename =filename.mid(filename.lastIndexOf("/")+1);
    setWindowTitle(tr("Graph: %1").arg(shortfilename));
    ui.toKMLPushButton->setDisabled(true);
    ui.exportPushButton->setVisible(true);

    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelType(QCPAxis::ltNumber);
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(0,100);
    ui.autoScrollCheckBox->setChecked(false);
    ui.loadOfflineLogButton->setText("Close Log");
    ui.loadTLogButton->setVisible(false);
    ui.downloadPushButton->setVisible(false);
    ui.autoScrollCheckBox->setVisible(false);

    m_tableModel = new AP2DataPlot2DModel(this);
    m_logLoaderThread = new AP2DataPlotThread(m_tableModel);
    connect(m_logLoaderThread,SIGNAL(startLoad()),this,SLOT(loadStarted()));
    connect(m_logLoaderThread,SIGNAL(loadProgress(qint64,qint64)),this,SLOT(loadProgress(qint64,qint64)));
    connect(m_logLoaderThread,SIGNAL(error(QString)),this,SLOT(threadError(QString)));
    connect(m_logLoaderThread,SIGNAL(done(int,MAV_TYPE)),this,SLOT(threadDone(int,MAV_TYPE)));
    connect(m_logLoaderThread,SIGNAL(finished()),this,SLOT(threadTerminated()));
    connect(m_logLoaderThread,SIGNAL(payloadDecoded(int,QString,QVariantMap)),this,SLOT(payloadDecoded(int,QString,QVariantMap)));
    m_logLoaderThread->loadFile(filename);
}

void AP2DataPlot2D::threadTerminated()
{
    QLOG_DEBUG() << "AP2DataPlot2D::threadTerminated = " << m_logLoaderThread;
    m_logLoaderThread->deleteLater();
    m_logLoaderThread = NULL;
}

AP2DataPlot2D::~AP2DataPlot2D()
{
    if (m_logLoaderThread)
    {
        m_logLoaderThread->stopLoad();
        m_logLoaderThread->deleteLater();
        m_logLoaderThread = NULL;
    }
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->close();
        delete m_axisGroupingDialog;
        m_axisGroupingDialog = NULL;
    }

    for (int i=0;i<m_childGraphList.size();i++)
    {
        m_childGraphList.at(i)->close();
    }

    delete m_model;
    m_model = NULL;
}
void AP2DataPlot2D::itemEnabled(QString name)
{
    if (m_logLoaded)
    {
        QString parent = name.split(".")[0];
        QString child = name.split(".")[1];
        /*if (!m_sharedDb.isOpen())
        {
            if (!m_sharedDb.open())
            {
                //emit error("Unable to open database: " + m_sharedDb.lastError().text());
                QMessageBox::information(0,"Error","Error opening DB");
                return;
            }
        }*/

        bool isstr = false;
        QList<QPair<double,QString> > strlist;
        QVector<double> xlist;
        QVector<double> ylist;
        QMap<quint64,QVariant> values = m_tableModel->getValues(parent,child);
        if (values.size() == 0)
        {
            //No values!
            m_graphCount++; //Prevent crash when it tries to disable
            ui.dataSelectionScreen->disableItem(name);
            return;
        }
        for (QMap<quint64,QVariant>::const_iterator i = values.constBegin();i!=values.constEnd();i++)
        {
            if (i.value().type() == QVariant::String)
            {
                QString graphvaluestr = i.value().toString();
                strlist.append(QPair<double,QString>(i.key(),graphvaluestr));
                isstr = true;
            }
            else
            {
                double graphvalue = i.value().toDouble();
                ylist.append(graphvalue);
            }
            xlist.append(i.key());

        }
        QCPAxis *axis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
        axis->setLabel(name);

        if (m_graphCount > 0)
        {
            connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
        }
        QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
        axis->setLabelColor(color);
        axis->setTickLabelColor(color);
        axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
        QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
        m_graphNameList.append(name);
        QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);


        if (m_axisGroupingDialog)
        {
            m_axisGroupingDialog->addAxis(name,axis->range().lower,axis->range().upper,color);
        }
        mainGraph1->setPen(QPen(color, 2));
        Graph graph;
        graph.axis = axis;
        graph.groupName = "";
        graph.graph=  mainGraph1;
        graph.isInGroup = false;
        graph.isManualRange = false;
        m_graphClassMap[name] = graph;
        if (isstr)
        {
            for (int i=0;i<strlist.size();i++)
            {
                QCPItemText *itemtext = new QCPItemText(m_plot);
                itemtext->setText(strlist.at(i).second);
                itemtext->position->setAxes(xAxis,axis);
                itemtext->position->setCoords(strlist.at(i).first,2.0);
                m_plot->addItem(itemtext);
                graph.itemList.append(itemtext);

                QCPItemLine *itemline = new QCPItemLine(m_plot);
                graph.itemList.append(itemline);
                itemline->start->setParentAnchor(itemtext->bottom);
                itemline->start->setAxes(xAxis, axis);
                itemline->start->setCoords(0.0, 0.0);
                itemline->end->setAxes(xAxis, axis);
                itemline->end->setCoords(strlist.at(i).first, 0.0);
                itemline->setTail(QCPLineEnding::esDisc);
                itemline->setHead(QCPLineEnding::esSpikeArrow);
                m_plot->addItem(itemline);

            }
        }
        else
        {
            mainGraph1->setData(xlist, ylist);
        }
        mainGraph1->rescaleValueAxis();
        if (m_graphCount <= 2)
        {
            mainGraph1->rescaleKeyAxis();
            m_wideAxisRect->axis(QCPAxis::atBottom)->setRangeLower(xlist.at(0));
        }

        return;
    } //if (m_logLoaded)
    else
    {
        if (m_onlineValueMap.contains(name))
        {
            QVector<double> xlist;
            QVector<double> ylist;

            float min = m_onlineValueMap[name][0].second;
            float max = m_onlineValueMap[name][0].second;
            for (int j=0;j<m_onlineValueMap[name].size();j++)
            {
                xlist.append(m_onlineValueMap[name][j].first);
                double val = m_onlineValueMap[name][j].second;
                if (val > max)
                {
                    max = val;
                }
                if (val < min)
                {
                    min = val;
                }
                ylist.append(val);
            }
            QCPAxis *axis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
            axis->setLabel(name);
            QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
            axis->setLabelColor(color);
            axis->setTickLabelColor(color);
            axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
            QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
            m_graphNameList.append(name);
            mainGraph1->setData(xlist, ylist);
            mainGraph1->rescaleValueAxis();
            if (m_graphCount == 1)
            {
                mainGraph1->rescaleKeyAxis();
                //m_scrollStartIndex = m_currentIndex;
                //ui.horizontalScrollBar->setMinimum(m_timeDiff);
            }
            if (m_axisGroupingDialog)
            {
                m_axisGroupingDialog->addAxis(name,axis->range().lower,axis->range().upper,color);
            }
            Graph graph;
            graph.axis = axis;
            graph.groupName = "";
            graph.graph=  mainGraph1;
            graph.isInGroup = false;
            graph.isManualRange = false;
            m_graphClassMap[name] = graph;

            mainGraph1->setPen(QPen(color, 2));
        }
    }
}

void AP2DataPlot2D::itemDisabled(QString name)
{
    if (m_logLoaded)
    {
        name = name.mid(name.indexOf(":")+1);
    }
    for (int i=0;i<m_graphClassMap.value(name).itemList.size();i++)
    {
        m_plot->removeItem(m_graphClassMap.value(name).itemList.at(i));
    }
    m_wideAxisRect->removeAxis(m_graphClassMap.value(name).axis);
    m_plot->removeGraph(m_graphClassMap.value(name).graph);
    m_plot->replot();
    m_graphClassMap.remove(name);
    m_graphNameList.removeOne(name);
    m_graphCount--;
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->removeAxis(name);
    }

}
void AP2DataPlot2D::progressDialogCanceled()
{
    m_logLoaderThread->stopLoad();
    this->close();
}
void AP2DataPlot2D::clearGraph()
{
    //Clear the graph
    for (int i=0;i<m_graphNameList.size();i++)
    {
        m_wideAxisRect->removeAxis(m_graphClassMap.value(m_graphNameList[i]).axis);
        m_plot->removeGraph(m_graphClassMap.value(m_graphNameList[i]).graph);
    }
    ui.dataSelectionScreen->clear();
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->clear();
        m_axisGroupingDialog->hide();
    }
    m_graphClassMap.clear();
    m_graphCount=0;
    m_dataList.clear();

    if (m_logLoaded)
    {
        //Unload the log.
        m_logLoaded = false;
        ui.loadOfflineLogButton->setText("Open Log");
        ui.hideExcelView->setVisible(false);
        ui.hideExcelView->setChecked(false);
        //ui.tableWidget->setVisible(false);
        setExcelViewHidden(true);
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelType(QCPAxis::ltDateTime);
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setDateTimeFormat("hh:mm:ss");
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(0,100); //Default range of 0-100 milliseconds?
    }
    m_currentIndex = QDateTime::currentMSecsSinceEpoch();
    m_startIndex = m_currentIndex;
    m_onlineValueMap.clear();
    m_plot->replot();
}

void AP2DataPlot2D::loadStarted()
{
    m_progressDialog = new QProgressDialog("Loading File","Cancel",0,100,this);
    m_progressDialog->setWindowModality(Qt::WindowModal);
    connect(m_progressDialog,SIGNAL(canceled()),this,SLOT(progressDialogCanceled()));
    m_progressDialog->show();
    QApplication::processEvents();
    //ui.tableWidget->clear();
    //ui.tableWidget->setRowCount(0);
}

void AP2DataPlot2D::loadProgress(qint64 pos,qint64 size)
{
    m_progressDialog->setValue(((double)pos / (double)size) * 100.0);
}

int AP2DataPlot2D::getStatusTextPos()
{
    static const int numberOfPositions = 4;
    m_statusTextPos++;
    if(m_statusTextPos > numberOfPositions)
        m_statusTextPos = 1;
    return m_statusTextPos;
}

void AP2DataPlot2D::plotTextArrow(int index, const QString &text, const QString& graph, QCheckBox* checkBox)
{
    QLOG_DEBUG() << "plotTextArrow:" << index << " to " << graph;
    int pos = getStatusTextPos();
    QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);

    QCPItemText *itemtext = new QCPItemText(m_plot);
    itemtext->setText(text);
    itemtext->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    itemtext->position->setAxes(xAxis,m_graphClassMap[graph].axis);

    m_graphClassMap[graph].itemList.append(itemtext);

    QCPItemLine *itemline = new QCPItemLine(m_plot);
    m_graphClassMap[graph].itemList.append(itemline);

    itemline->start->setAxes(xAxis, m_graphClassMap[graph].axis);
    itemline->start->setCoords(index, pos);
    itemline->end->setAxes(xAxis, m_graphClassMap[graph].axis);
    itemline->end->setCoords(index, 0.0);
    itemline->setTail(QCPLineEnding::esDisc);
    itemline->setHead(QCPLineEnding::esSpikeArrow);

    m_plot->addItem(itemline);
    itemtext->position->setCoords(itemline->start->coords());
    m_plot->addItem(itemtext);

    if (checkBox && !checkBox->isChecked())
    {
        itemtext->setVisible(false);
        itemline->setVisible(false);
    }
}

void AP2DataPlot2D::threadDone(int errors,MAV_TYPE type)
{
    m_loadedLogMavType = type;

    if (errors != 0)
    {
        QMessageBox::information(this,"Warning","There were errors countered with " + QString::number(errors) + " lines in the log file. The data is potentially corrupt and incorrect");
    }


    m_scrollStartIndex = m_tableModel->getFirstIndex();
    m_scrollEndIndex = m_tableModel->getLastIndex();
    ui.horizontalScrollBar->setMinimum(m_scrollStartIndex);
    ui.horizontalScrollBar->setMaximum(m_scrollEndIndex);

    QMap<QString,QList<QString> > fmtlist = m_tableModel->getFmtValues();
    for (QMap<QString,QList<QString> >::const_iterator i=fmtlist.constBegin();i!=fmtlist.constEnd();i++)
    {
        QString name = i.key();
        for (int j=0;j<i.value().size();j++)
        {
            ui.dataSelectionScreen->addItem(name + "." + i.value().at(j));
        }
        QTreeWidgetItem *child = new QTreeWidgetItem(QStringList() << name);
        child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
        child->setCheckState(0,Qt::Checked); // Set it checked, since all items are enabled by default
        ui.sortSelectTreeWidget->addTopLevelItem(child);
    }

    QMap<quint64,QString> modes = m_tableModel->getModeValues();
    if (modes.size() == 0)
    {
        QLOG_DEBUG() << "Graph loaded with no mode table. Running anyway, but text modes will not be available";
    }
    else
    {
        if (!m_graphClassMap.contains("MODE"))
        {
            QCPAxis *axis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
            axis->setVisible(false);
            axis->setLabel("MODE");

            if (m_graphCount > 0)
            {
                connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
            }
            QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
            axis->setLabelColor(color);
            axis->setTickLabelColor(color);
            axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
            QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
            m_graphNameList.append("MODE");

            mainGraph1->setPen(QPen(color, 2));
            Graph graph;
            graph.axis = axis;
            graph.groupName = "";
            graph.graph=  mainGraph1;
            graph.isInGroup = false;
            graph.isManualRange = false;
            m_graphClassMap["MODE"] = graph;

            mainGraph1->rescaleValueAxis();
            if (m_graphCount == 1)
            {
                mainGraph1->rescaleKeyAxis();
            }
        }
        for (QMap<quint64,QString>::const_iterator i = modes.constBegin(); i != modes.constEnd(); i++)
        {
            quint64 index = i.key();
            QString mode = i.value();
            bool ok = false;
            int modeint = mode.toInt(&ok);
            if (!ok)
            {
                QLOG_DEBUG() << "Unable to determine Mode number in log" << mode;
            }
            else
            {
                //It's an integer!
                switch (type)
                {
                    case MAV_TYPE_QUADROTOR:
                    {
                        mode = ApmCopter::stringForMode(modeint);
                    }
                    break;
                    case MAV_TYPE_FIXED_WING:
                    {
                        mode = ApmPlane::stringForMode(modeint);
                    }
                    break;
                    case MAV_TYPE_GROUND_ROVER:
                    {
                        mode = ApmRover::stringForMode(modeint);
                    }
                    break;
                }
            }
            QLOG_DEBUG() << "Mode change at index" << index << "to" << mode;
            plotTextArrow(index, mode, "MODE",ui.modeDisplayCheckBox);
            m_graphClassMap["MODE"].modeMap[index] = mode;
        }
    }
    ui.verticalScrollBar->setValue(ui.verticalScrollBar->maximum());

    //m_tableModel = new AP2DataPlot2DModel(&m_sharedDb,this);
    m_tableFilterProxyModel = new QSortFilterProxyModel(this);
    m_tableFilterProxyModel->setSourceModel(m_tableModel);
    ui.tableWidget->setModel(m_tableFilterProxyModel);
    connect(ui.tableWidget->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectedRowChanged(QModelIndex,QModelIndex)));



    m_progressDialog->hide();
    delete m_progressDialog;
    m_progressDialog=0;
    //ui.tableWidget->setVisible(true);
    setExcelViewHidden(false);
    ui.hideExcelView->setVisible(true);
    ui.sortShowPushButton->setVisible(true);
}


void AP2DataPlot2D::threadError(QString errorstr)
{
    QMessageBox::information(0,"Error",errorstr);
    m_progressDialog->hide();
    delete m_progressDialog;
    m_progressDialog=0;
    ui.dataSelectionScreen->clear();
    m_dataList.clear();
}

void AP2DataPlot2D::payloadDecoded(int index,QString name,QVariantMap map)
{
    if (!m_dataList.contains(name))
    {
        m_dataList[name] = QList<QPair<int,QVariantMap> >();
    }
    m_dataList[name].append(QPair<int,QVariantMap>(index,map));
}

void AP2DataPlot2D::showLogDownloadDialog()
{
    QLOG_DEBUG() << "showLogDownloadDialog";
    if (m_logDownloadDialog == NULL){
        m_logDownloadDialog = new LogDownloadDialog(this);
        connect(m_logDownloadDialog, SIGNAL(accepted()), this, SLOT(closeLogDownloadDialog()));
    }
    m_logDownloadDialog->show();
    m_logDownloadDialog->raise();
}

void AP2DataPlot2D::closeLogDownloadDialog()
{
    if (m_logDownloadDialog){
        m_logDownloadDialog->hide();
        m_logDownloadDialog->deleteLater();
        m_logDownloadDialog = NULL;
    }
}

void AP2DataPlot2D::droneshareButtonClicked()
{
    if(!m_droneshareUploadDialog){
        m_droneshareUploadDialog = new DroneshareUploadDialog();
    }
    m_droneshareUploadDialog->show();
    m_droneshareUploadDialog->raise();
}

void AP2DataPlot2D::exportButtonClicked()
{
    if (!m_logLoaded)
    {
        QMessageBox::information(this,"Error","You must have a log loaded before attempting to export");
        return;
    }

    if (m_filename.endsWith(".log")){
        QMessageBox::information(this,"Error","Cannot export a dataflash log!");
        return;
    }

    /*if (!m_sharedDb.isOpen())
    {
        if (!m_sharedDb.open())
        {
            //emit error("Unable to open database: " + m_sharedDb.lastError().text());
            QMessageBox::information(0,"Error","Error opening DB");
            return;
        }
    }*/

    //remove current extension
    QString exportFilename = m_filename.replace(".bin",".log", Qt::CaseInsensitive); // remove extension
    QFileDialog *dialog = new QFileDialog(this,"Save Log File",QGC::logDirectory());
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setNameFilter("*.log");
    dialog->selectFile(exportFilename);
    QLOG_DEBUG() << " Suggested Export Filename: " << exportFilename;
    dialog->open(this,SLOT(exportDialogAccepted()));
}
void AP2DataPlot2D::exportDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    if (dialog->selectedFiles().size() == 0)
    {
        return;
    }
    QString outputFileName = dialog->selectedFiles().at(0);
    dialog->close();

    QFile outputfile(outputFileName);
    if (!outputfile.open(QIODevice::ReadWrite | QIODevice::Truncate))
    {
        QMessageBox::information(this,"Error","Unable to open output file: " + outputfile.errorString());
        return;
    }
    QProgressDialog *progressDialog = new QProgressDialog("Exporting File","Cancel",0,100,this);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->show();
    QApplication::processEvents();

    QString formatheader = "FMT, 128, 89, FMT, BBnNZ, Type,Length,Name,Format\r\n";
    QMap<QString,QList<QString> > fmtlist = m_tableModel->getFmtValues();
    for (QMap<QString,QList<QString> >::const_iterator i = fmtlist.constBegin();i!=fmtlist.constEnd();i++)
    {
        QString fmtname = i.key();
        QString line = m_tableModel->getFmtLine(fmtname);
        if (line != "")
        {
            formatheader += line + "\r\n";
        }
    }

    outputfile.write(formatheader.toLatin1());

    for (int i=0;i<m_tableModel->rowCount();i++)
    {
        int j=1;
        QVariant val = m_tableModel->data(m_tableModel->index(i,j++));
        QString line = val.toString();
        val = m_tableModel->data(m_tableModel->index(i,j++));
        while (!val.isNull())
        {
            line += ", " + val.toString();
            val = m_tableModel->data(m_tableModel->index(i,j++));
        }
        outputfile.write(line.append("\r\n").toLatin1());
        if (i % 5)
        {
            progressDialog->setValue(100.0 * ((double)i / (double)m_tableModel->rowCount()));
        }
        QApplication::processEvents();
        if (progressDialog->wasCanceled())
        {
            outputfile.close();
            progressDialog->hide();
            progressDialog->deleteLater();
            progressDialog=NULL;
            QMessageBox::information(0,"Warning","Export was canceled");
            return;
        }
    }

    outputfile.close();
    progressDialog->hide();
    progressDialog->deleteLater();
    progressDialog=NULL;

}

void AP2DataPlot2D::modeCheckBoxClicked(bool checked)
{
    if (!m_graphClassMap.contains("MODE"))
    {
        return;
    }
    for (int i=0;i<m_graphClassMap["MODE"].itemList.size();i++)
    {
        m_graphClassMap["MODE"].itemList.at(i)->setVisible(checked);
    }
}

void AP2DataPlot2D::errCheckBoxClicked(bool checked)
{
    if (!m_graphClassMap.contains("ERR"))
    {
        return;
    }
    for (int i=0;i<m_graphClassMap["ERR"].itemList.size();i++)
    {
        m_graphClassMap["ERR"].itemList.at(i)->setVisible(checked);
    }
}

void AP2DataPlot2D::evCheckBoxClicked(bool checked)
{
    if (!m_graphClassMap.contains("EV"))
    {
        return;
    }
    m_graphClassMap.value("EV").graph->setVisible(checked);
    for (int i=0;i<m_graphClassMap["EV"].itemList.size();i++)
    {
        m_graphClassMap["EV"].itemList.at(i)->setVisible(checked);
    }
}
void AP2DataPlot2D::sortItemChanged(QTreeWidgetItem* item,int col)
{
    Q_UNUSED(col)
	//Sorting item changed.
	if (!item)
	{
		return;
	}
	QString msgname = item->text(0);
	if (item->checkState(0) == Qt::Checked)
	{
		if (!m_tableFilterList.contains(msgname))
		{
			m_tableFilterList.append(msgname);
		}
	}
	else
	{
		if (m_tableFilterList.contains(msgname))
		{
			m_tableFilterList.removeOne(msgname);
		}
	}
}
void AP2DataPlot2D::sortAcceptClicked()
{
    QString sortstring = "";
    for (int i=0;i<m_tableFilterList.size();i++)
    {
        sortstring += m_tableFilterList.at(i) + ((i == m_tableFilterList.size()-1) ? "" : "|");
    }
    m_tableFilterProxyModel->setFilterRegExp(sortstring);
    m_tableFilterProxyModel->setFilterRole(Qt::DisplayRole);
    m_tableFilterProxyModel->setFilterKeyColumn(1);
    ui.tableSortGroupBox->setVisible(false);
    ui.sortShowPushButton->setText("Show Sort");
}

void AP2DataPlot2D::sortCancelClicked()
{
    ui.tableSortGroupBox->setVisible(false);
    ui.sortShowPushButton->setText("Show Sort");
}
void AP2DataPlot2D::showSortButtonClicked()
{
    if (ui.tableSortGroupBox->isVisible())
    {
        ui.tableSortGroupBox->setVisible(false);
        ui.sortShowPushButton->setText("Show Sort");
    }
    else
    {
        ui.tableSortGroupBox->setVisible(true);
        ui.sortShowPushButton->setText("Hide Sort");
    }
}
void AP2DataPlot2D::sortSelectAllClicked()
{
    for (int i=0;i<ui.sortSelectTreeWidget->topLevelItemCount();i++)
    {
        if (ui.sortSelectTreeWidget->topLevelItem(i)->checkState(0) != Qt::Checked)
        {
            ui.sortSelectTreeWidget->topLevelItem(i)->setCheckState(0,Qt::Checked);
        }
    }
}

void AP2DataPlot2D::sortSelectInvertClicked()
{
    for (int i=0;i<ui.sortSelectTreeWidget->topLevelItemCount();i++)
    {
        if (ui.sortSelectTreeWidget->topLevelItem(i)->checkState(0) == Qt::Checked)
        {
            ui.sortSelectTreeWidget->topLevelItem(i)->setCheckState(0,Qt::Unchecked);
        }
        else
        {
            ui.sortSelectTreeWidget->topLevelItem(i)->setCheckState(0,Qt::Checked);
        }
    }
}
void AP2DataPlot2D::childGraphDestroyed(QObject *obj)
{
    QWidget *plot = qobject_cast<QWidget*>(obj);
    if (!plot)
    {
        return;
    }
    if (m_childGraphList.contains(plot))
    {
        m_childGraphList.removeOne(plot);
    }
}

void AP2DataPlot2D::logToKmlClicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Log File", QGC::logDirectory(), tr("Log Files (*.log)"));
    QApplication::processEvents(); // Helps clear dialog from screen

    if(filename.length() > 0) {
        QFile file(filename);
        if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString kmlFile(filename);
            kmlFile.replace(".log", ".kml");
            kml::KMLCreator kml;

            kml.start(kmlFile);
            QTextStream in(&file);
            while(!in.atEnd()) {
                QString line = in.readLine();
                kml.processLine(line);
            }

            QString generated = kml.finish(true);
            file.close();

            QString msg = QString("Generated %1.").arg(generated);
            QMessageBox::information(this, "Log to KML", msg);
        }
        else {
            QString msg = QString("Unable to open %1.").arg(filename);
            QMessageBox::warning(this, "Log to KML", msg, QMessageBox::Ok);
        }
    }
}

