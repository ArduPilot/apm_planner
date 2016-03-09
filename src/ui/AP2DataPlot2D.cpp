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
#include <QSettings>

#define ROW_HEIGHT_PADDING 3 //Number of additional pixels over font height for each row for the table/excel view.

const double AP2DataPlot2D::c_timeDivisor = 1000000.0;

AP2DataPlot2D::AP2DataPlot2D(QWidget *parent,bool isIndependant) : QWidget(parent),
    m_updateTimer(NULL),
    m_showOnlyActive(false),
    m_graphCount(0),
    m_plot(NULL),
    m_wideAxisRect(NULL),
    m_logLoaderThread(NULL),
    m_logLoaded(false),
    m_currentIndex(0),
    m_startIndex(0),
    m_addGraphAction(NULL),
    m_uas(NULL),
    m_axisGroupingDialog(NULL),
    m_tlogReplayEnabled(false),
    m_logDownloadDialog(NULL),
    m_loadedLogMavType(MAV_TYPE_ENUM_END),
    m_statusTextPos(0),
    m_useTimeOnX(false)
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
    connect(ui.msgDisplayCheckBox,SIGNAL(clicked(bool)),this,SLOT(msgCheckBoxClicked(bool)));
    connect(ui.indexTypeCheckBox  ,SIGNAL(clicked(bool)),this,SLOT(indexTypeCheckBoxClicked(bool)));
    // Default invisible
    ui.errDisplayCheckBox->setVisible(false);
    ui.evDisplayCheckBox->setVisible(false);
    ui.msgDisplayCheckBox->setVisible(false);
    ui.indexTypeCheckBox->setVisible(false);
    ui.jumpToLocationCheckBox->setVisible(false);


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

    setExcelViewHidden(true);
    ui.tableWidget->verticalHeader()->setDefaultSectionSize(ui.tableWidget->fontMetrics().height() + ROW_HEIGHT_PADDING);
    ui.hideExcelView->setVisible(false);

    connect(ui.loadOfflineLogButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    connect(ui.autoScrollCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoScrollClicked(bool)));
    connect(ui.hideExcelView,SIGNAL(clicked(bool)),this,SLOT(setExcelViewHidden(bool)));
    connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCellChanged(int,int,int,int)));

    connect(ui.graphControlsPushButton,SIGNAL(clicked()),this,SLOT(graphControlsButtonClicked()));
    connect(ui.toKMLPushButton, SIGNAL(clicked()), this, SLOT(logToKmlClicked()));
    connect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));

    connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    connect(m_wideAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    m_plot->setPlottingHint(QCP::phFastPolylines,true);

    connect(ui.downloadPushButton, SIGNAL(clicked()), this, SLOT(showLogDownloadDialog()));
    ui.downloadPushButton->setEnabled(false);
    connect(ui.loadTLogButton,SIGNAL(clicked()),this,SLOT(replyTLogButtonClicked()));

    connect(ui.exportPushButton,SIGNAL(clicked()),this,SLOT(exportButtonClicked()));

    ui.horizontalSplitter->setStretchFactor(0,20);
    ui.horizontalSplitter->setStretchFactor(1,1);

    loadSettings();
}


void AP2DataPlot2D::loadSettings()
{
    QSettings settings;
    settings.beginGroup("DATAPLOT_SETTINGS");
    ui.jumpToLocationCheckBox->setChecked(settings.value("JUMP_TO_LOCATION", Qt::Unchecked).toBool());
    ui.showValuesCheckBox->setChecked(settings.value("SHOW_VALUES", Qt::Unchecked).toBool());
    ui.autoScrollCheckBox->setChecked(settings.value("AUTO_SCROLL", Qt::Unchecked).toBool());

    ui.evDisplayCheckBox->setChecked(settings.value("SHOW_EV", Qt::Checked).toBool());
    ui.errDisplayCheckBox->setChecked(settings.value("SHOW_ERR", Qt::Checked).toBool());
    ui.modeDisplayCheckBox->setChecked(settings.value("SHOW_MODE", Qt::Checked).toBool());
    settings.endGroup();
}

void AP2DataPlot2D::saveSettings()
{
    QSettings settings;
    settings.beginGroup("DATAPLOT_SETTINGS");
    settings.setValue("JUMP_TO_LOCATION", ui.jumpToLocationCheckBox->isChecked());
    settings.setValue("SHOW_VALUES", ui.showValuesCheckBox->isChecked());
    settings.setValue("AUTO_SCROLL", ui.autoScrollCheckBox->isChecked());

    settings.setValue("SHOW_EV", ui.evDisplayCheckBox->isChecked());
    settings.setValue("SHOW_ERR", ui.errDisplayCheckBox->isChecked());
    settings.setValue("SHOW_MODE", ui.modeDisplayCheckBox->isChecked());

    settings.sync();
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
        m_graphClassMap[i.key()].graph->setPen(QPen(i.value()));
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
    double test = qAbs(m_wideAxisRect->axis(QCPAxis::atBottom)->range().center()-value);
    if (test  > 0.01) // if user is dragging plot, we don't want to replot twice
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
        m_updateTimer = NULL;
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
        m_updateTimer = NULL;
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

void AP2DataPlot2D::plotDoubleClick(QMouseEvent * evt)
{
    if (!ui.jumpToLocationCheckBox->isChecked())
    {
        return;
    }

    double key = 0.0;
    double timeStamp = 0.0;
    QCPGraph *graph = m_graphClassMap.value(m_graphClassMap.keys()[0]).graph;
    key = graph->keyAxis()->pixelToCoord(evt->x());

    if (m_useTimeOnX)
    {
        // We scaled the time by timeDivisor when plotting the graph
        // therefore we have to scale when searching for the original timestamp
        timeStamp = key;
        key = m_tableModel->getNearestIndexForTimestamp(timeStamp * c_timeDivisor);
    }

    quint64 position = floor(key);
    quint64 min = m_tableModel->getFirstIndex();
    quint64 max = m_tableModel->getLastIndex();

    if ( position > max )
    {
        ui.tableWidget->scrollToBottom();
        double plotPos = m_useTimeOnX ? m_tableModel->getMaxTime() : max;
        plotCurrentIndex(plotPos);
    }
    else if ( position < min )
    {
        ui.tableWidget->scrollToTop();
        double plotPos = m_useTimeOnX ? m_tableModel->getMinTime() : min;
        plotCurrentIndex(plotPos);
    }
    else
    {
        //search for previous event (remember the table may be filtered)
        QModelIndex sourceIndex = m_tableModel->index(position-min, 0);
        QModelIndex index = m_tableFilterProxyModel->mapFromSource(sourceIndex);
        while ( sourceIndex.row() >= static_cast<int>(min) && !index.isValid() )
        {
            sourceIndex = m_tableModel->index(sourceIndex.row() - 1, 0);
            index = m_tableFilterProxyModel->mapFromSource(sourceIndex);
        }

        if ( !index.isValid() )
        {
            //couldn't find filtered index by looking back, try forward...
            sourceIndex = m_tableModel->index(position-min, 0);
            index = m_tableFilterProxyModel->mapFromSource(sourceIndex);
            while ( sourceIndex.row() <= static_cast<int>(max) && !index.isValid() )
            {
                sourceIndex = m_tableModel->index(sourceIndex.row() + 1, 0);
                index = m_tableFilterProxyModel->mapFromSource(sourceIndex);
            }
        }
        ui.tableWidget->setCurrentIndex(index);
        ui.tableWidget->scrollTo(index);
    }
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
        QCPGraph *graph = m_graphClassMap.value(m_graphClassMap.keys()[i]).graph;
        key = graph->keyAxis()->pixelToCoord(evt->x());
        if (i == 0)
        {
            if (m_logLoaded)
            {
                if (m_useTimeOnX)
                {
                    newresult.append("Time s: " + QString::number(key,'f',3) + "\n");
                }
                else
                {
                    newresult.append("Log Line: " + QString::number(key,'f',0) + "\n");
                }
            }
            else
            {
                newresult.append("Time: " + QDateTime::fromMSecsSinceEpoch(key * 1000.0).toString("hh:mm:ss") + "\n");
            }
        }
        if (m_graphClassMap.keys()[i] == ModeMessage::TypeName)
        {
            if (m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.keys().size() > 1)
            {
                for (QMap<double,QString>::const_iterator messagemapiterator = m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.constBegin();messagemapiterator!=m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.constEnd();messagemapiterator++)
                {
                    if (messagemapiterator.key() < key)
                    {
                        if (messagemapiterator==m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.constEnd()-1)
                        {
                            //We're at the end, use the end
                            newresult.append(m_graphClassMap.keys()[i] + ": " + messagemapiterator.value() + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
                        }
                        else if ((messagemapiterator+1).key() > key)
                        {
                            //This only gets hit if we're not at the end, and we have the proper value
                            newresult.append(m_graphClassMap.keys()[i] + ": " + messagemapiterator.value() + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
                            break;
                        }
                    }
                }
            }
            else if (m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.keys().size() == 1)
            {
                newresult.append(m_graphClassMap.keys()[i] + ": " + m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.begin().value() + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
            }
            else
            {
                newresult.append(m_graphClassMap.keys()[i] + ": " + "Unknown" + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
            }
        }
        else if ((m_graphClassMap.keys()[i] == ErrorMessage::TypeName) ||
                 (m_graphClassMap.keys()[i] == EventMessage::TypeName) ||
                 (m_graphClassMap.keys()[i] == MsgMessage::TypeName))
        {
            //Ignore ERR / EV / MSG
        }
        else if (graph->data()->contains(key))
        {
            QString str = QString().sprintf( "%.9g", graph->data()->value(key).value);
            newresult.append(m_graphClassMap.keys()[i] + ": " + str + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
        }
        else if (graph->data()->lowerBound(key) != graph->data()->constEnd())
        {
        	QString str = QString().sprintf( "%.9g", graph->data()->lowerBound(key).value().value);
            newresult.append(m_graphClassMap.keys()[i] + ": " + str + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
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
            if (!i.key().contains(ModeMessage::TypeName) &&
                !i.key().contains(ErrorMessage::TypeName)  &&
                !i.key().contains(EventMessage::TypeName) &&
                !i.key().contains(MsgMessage::TypeName))
            {
                m_axisGroupingDialog->fullAxisUpdate(i.key(),i.value().axis->range().lower,i.value().axis->range().upper,i.value().isManualRange,i.value().isInGroup,i.value().groupName);
            }
        }
        return;
    }
    m_axisGroupingDialog = new AP2DataPlotAxisDialog();
    connect(m_axisGroupingDialog,SIGNAL(graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange>)),this,SLOT(graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange>)));
    connect(m_axisGroupingDialog,SIGNAL(graphColorsChanged(QMap<QString,QColor>)),this,SLOT(graphColorsChanged(QMap<QString,QColor>)));
    for (QMap<QString,Graph>::const_iterator i=m_graphClassMap.constBegin();i!=m_graphClassMap.constEnd();i++)
    {
        if (!i.key().contains(ModeMessage::TypeName) &&
            !i.key().contains(ErrorMessage::TypeName)  &&
            !i.key().contains(EventMessage::TypeName) &&
            !i.key().contains(MsgMessage::TypeName))
        {
            m_axisGroupingDialog->addAxis(i.key(),i.value().axis->range().lower,i.value().axis->range().upper,i.value().axis->labelColor());
        }
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
    if (!current.isValid())
    {
        return;
    }
    quint64 index =  m_tableFilterProxyModel->mapToSource(current).row() + m_tableModel->getFirstIndex();

    if (m_useTimeOnX)
    {
        // timestamp value of the current row is in colum 2
        double item = ui.tableWidget->model()->itemData(ui.tableWidget->model()->index(current.row(),2)).value(Qt::DisplayRole).toInt();
        plotCurrentIndex(item / c_timeDivisor);
    }
    else
    {
        plotCurrentIndex(index);
    }

    m_tableModel->selectedRowChanged(m_tableFilterProxyModel->mapToSource(current),m_tableFilterProxyModel->mapToSource(previous));

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
    else if (ui.tableWidget->selectionModel()->selectedIndexes().size() != 0)
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
    disableTableFilter();
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

    //textMessageReceived(uasId, message.compid, severity, text);

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
        QCPAxis *yAxis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
        yAxis->setVisible(false);
        yAxis->setLabel("MODE");
        yAxis->setRangeUpper(8.0);  // We have 7 different arrow lengths
        QCPGraph *mainGraph = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));

        if (m_graphCount > 0)
        {
            connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),yAxis,SLOT(setRange(QCPRange)));
        }

        Graph graph;
        graph.axis = yAxis;
        graph.graph = mainGraph;

        m_graphClassMap["MODE"] = graph;
        m_graphNameList.append("MODE");

        mainGraph->rescaleValueAxis();
        if (m_graphCount == 1)
        {
            mainGraph->rescaleKeyAxis();
        }
    }

    int index = newmsec / 1000.0;
    m_graphClassMap["MODE"].messageMap[index] = text;
    plotTextArrow(index, text, "MODE", QColor(50,125,0), ui.modeDisplayCheckBox);

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
    QMetaType::Type metaType(static_cast<QMetaType::Type>(value.type()));
    if (metaType == QMetaType::Double || metaType == QMetaType::Float)
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
    m_graphCount = 0;
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
    connect(m_logLoaderThread,SIGNAL(done(AP2DataPlotStatus,MAV_TYPE)),this,SLOT(threadDone(AP2DataPlotStatus,MAV_TYPE)));
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
    saveSettings();
    if (m_updateTimer)
    {
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
        m_updateTimer = NULL;
    }
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

    delete m_plot;
    m_plot = NULL;
}

void AP2DataPlot2D::itemEnabled(QString name)
{
    if (m_logLoaded)
    {
        QString parent = name.split(".")[0];
        QString child = name.split(".")[1];
        bool isstr = false;
        QList<QPair<double,QString> > strlist;
        QVector<double> xlist;
        QVector<double> ylist;
        QMap<quint64,QVariant> values = m_tableModel->getValues(parent, child, m_useTimeOnX);
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
            if(m_useTimeOnX)
            {
                xlist.append(static_cast<double>(i.key())/c_timeDivisor);
            }
            else
            {
                xlist.append(i.key());
            }

        }
        QCPAxis *yAxis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
        yAxis->setLabel(name);
        yAxis->setNumberFormat("gb");

        if (m_graphCount > 0)
        {
            connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),yAxis,SLOT(setRange(QCPRange)));
        }
        QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
        yAxis->setLabelColor(color);
        yAxis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
        QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
        m_graphNameList.append(name);
        QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);
        mainGraph1->setPen(QPen(color, 1));

        Graph graph;
        graph.axis = yAxis;
        graph.graph=  mainGraph1;
        m_graphClassMap[name] = graph;
        if (isstr)
        {
            for (int i=0;i<strlist.size();i++)
            {
                QCPItemText *itemtext = new QCPItemText(m_plot);
                itemtext->setText(strlist.at(i).second);
                itemtext->position->setAxes(xAxis,yAxis);
                itemtext->position->setCoords(strlist.at(i).first,2.0);
                m_plot->addItem(itemtext);
                graph.itemList.append(itemtext);

                QCPItemLine *itemline = new QCPItemLine(m_plot);
                graph.itemList.append(itemline);
                itemline->start->setParentAnchor(itemtext->bottom);
                itemline->start->setAxes(xAxis, yAxis);
                itemline->start->setCoords(0.0, 0.0);
                itemline->end->setAxes(xAxis, yAxis);
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

        if (m_axisGroupingDialog)
        {
            if (!name.contains(ModeMessage::TypeName) &&
                !name.contains(ErrorMessage::TypeName)  &&
                !name.contains(EventMessage::TypeName) &&
                !name.contains(MsgMessage::TypeName))
            {
                m_axisGroupingDialog->addAxis(name,yAxis->range().lower,yAxis->range().upper,color);
            }
        }
        // Graph 1 is the text arrow graph Graph 2 is the first 'line graph'
        if (m_graphCount <= 2)
        {
            mainGraph1->rescaleKeyAxis();
            m_wideAxisRect->axis(QCPAxis::atBottom)->setRangeLower(xlist.at(0));
            m_wideAxisRect->axis(QCPAxis::atBottom)->setRangeUpper(xlist.back());
            yAxis->grid()->setVisible(true);
        }
        // as soon as 2 'line graphs' are visible remove the grid
        else if(m_graphCount == 3 )
        {
            // brute force
            foreach (Graph graph, m_graphClassMap)
            {
                if (graph.axis != NULL)
                {
                    graph.axis->grid()->setVisible(false);
                }
            }
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
                if (!name.contains(ModeMessage::TypeName) &&
                    !name.contains(ErrorMessage::TypeName)  &&
                    !name.contains(EventMessage::TypeName) &&
                    !name.contains(MsgMessage::TypeName))
                {
                    m_axisGroupingDialog->addAxis(name,axis->range().lower,axis->range().upper,color);
                }
            }
            Graph graph;
            graph.axis = axis;
            graph.graph=  mainGraph1;
            m_graphClassMap[name] = graph;

            mainGraph1->setPen(QPen(color, 1));
        }
    }
}

void AP2DataPlot2D::itemDisabled(QString name)
{
    if (m_graphClassMap.contains(name)) // only enabled items can be disabled
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
        // first graph is the one for the text arrows, so the second
        // is the first 'line graph'
        if (m_graphCount == 2)
        {
            // Show grid if only one graph left
            QString lastGraph = m_graphNameList.back();
            if (m_graphClassMap.contains(lastGraph))
            {
                m_graphClassMap.value(lastGraph).axis->grid()->setVisible(true);
            }
        }
        if (m_axisGroupingDialog)
        {
            m_axisGroupingDialog->removeAxis(name);
        }
    }
}

void AP2DataPlot2D::progressDialogCanceled()
{
    m_logLoaderThread->stopLoad();
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
    m_graphCount = 0;
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
    m_progressDialog = QSharedPointer<QProgressDialog>(new QProgressDialog("Loading File","Cancel",0,100,this));
    m_progressDialog->setWindowModality(Qt::WindowModal);
    connect(m_progressDialog.data(),SIGNAL(canceled()),this,SLOT(progressDialogCanceled()));
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
    static const int numberOfPositions = 7;
    m_statusTextPos++;
    if(m_statusTextPos > numberOfPositions)
        m_statusTextPos = 1;
    return m_statusTextPos;
}

void AP2DataPlot2D::plotTextArrow(double index, const QString &text, const QString& graph, const QColor &color, QCheckBox* checkBox)
{
    QLOG_DEBUG() << "plotTextArrow:" << index << " to " << graph;
    int pos = getStatusTextPos();
    QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);

    QCPItemText *itemtext = new QCPItemText(m_plot);
    itemtext->setText(text);
    itemtext->setColor(color);
    itemtext->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    itemtext->position->setAxes(xAxis,m_graphClassMap[graph].axis);

    m_graphClassMap[graph].itemList.append(itemtext);

    QCPItemLine *itemline = new QCPItemLine(m_plot);
    m_graphClassMap[graph].itemList.append(itemline);

    itemline->setPen(QPen(color));
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


void AP2DataPlot2D::removeTextArrows(const QString &graphName)
{
    if (m_graphClassMap.contains(graphName))
    {
        foreach (QCPAbstractItem *ptr, m_graphClassMap[graphName].itemList)
        {
            m_plot->removeItem(ptr);
        }
        m_graphClassMap[graphName].itemList.clear();
    }
}

void AP2DataPlot2D::threadDone(AP2DataPlotStatus state, MAV_TYPE type)
{
    m_loadedLogMavType = type;

    // Errorhandling
    if (state.getParsingState() != AP2DataPlotStatus::OK)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Log parsing ended with errors.");
        msgBox.addButton(QMessageBox::Ok);

        if (state.getParsingState() == AP2DataPlotStatus::FmtError)
        {
            msgBox.setInformativeText("There were errors only in format discription. Usually this is no problem.");
        }
        else if (state.getParsingState() == AP2DataPlotStatus::TruncationError)
        {
            msgBox.setInformativeText("The data was truncated!");
        }
        else
        {
            msgBox.setInformativeText("There were data errors / unreadable data in the log!"
                                      " The data is potentially corrupt and incorrect.");
        }
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setDetailedText(state.getErrorText());
        msgBox.exec();
    }

    // First setup X-axis and scroller
    setupXAxisAndScroller();

    // Insert data into tree view
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
        m_tableFilterList.append(name);
    }

    // Setup basic graph for all arrow plots -> MODE/ERR/EV/MSG
    // This is graph one!
    QCPAxis *yAxis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
    yAxis->setVisible(false);
    yAxis->setLabel("MODE/ERR/EV/MSG");
    yAxis->setRangeUpper(8.0);  // We have 7 different arrow lengths
    QCPGraph *mainGraph = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft, m_graphCount++));

    // Setup arrow plots. In a loaded log we always have MODE/ERR/EV/MSG
    Graph graph;
    graph.axis = yAxis;
    graph.graph = mainGraph;

    m_graphClassMap[ModeMessage::TypeName] = graph;
    m_graphNameList.append(ModeMessage::TypeName);
    m_graphClassMap[ErrorMessage::TypeName] = graph;
    m_graphNameList.append(ErrorMessage::TypeName);
    m_graphClassMap[EventMessage::TypeName] = graph;
    m_graphNameList.append(EventMessage::TypeName);
    m_graphClassMap[MsgMessage::TypeName] = graph;
    m_graphNameList.append(MsgMessage::TypeName);

    // Load MODE messages
    m_tableModel->getMessagesOfType(ModeMessage::TypeName, m_indexToMessageMap); //Must only be loaded once
    // Load ERR messages
    m_tableModel->getMessagesOfType(ErrorMessage::TypeName, m_indexToMessageMap); //Must only be loaded once
    // Load EV messages
    m_tableModel->getMessagesOfType(EventMessage::TypeName, m_indexToMessageMap); //Must only be loaded once
    // Load MSG messages
    m_tableModel->getMessagesOfType(MsgMessage::TypeName, m_indexToMessageMap);   //Must only be loaded once

    // Insert Text arrows for all messages in m_indexToMessageMap
    insertTextArrows();

    // insert time line
    insertCurrentIndex();

    // Rescale axis and remove zoom
    mainGraph->rescaleValueAxis();
    mainGraph->rescaleKeyAxis();
    double requestedrange = static_cast<double>(m_scrollEndIndex - m_scrollStartIndex);
    disconnect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    m_wideAxisRect->axis(QCPAxis::atBottom)->setRange(m_scrollStartIndex, requestedrange);
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    m_plot->replot();

    ui.verticalScrollBar->setValue(ui.verticalScrollBar->maximum());

    // Set up proxy for table filtering
    m_tableFilterProxyModel = new QSortFilterProxyModel(this);
    m_tableFilterProxyModel->setSourceModel(m_tableModel);
    ui.tableWidget->setModel(m_tableFilterProxyModel);
    connect(ui.tableWidget->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(selectedRowChanged(QModelIndex,QModelIndex)));

    m_progressDialog->hide();
    m_progressDialog.reset();
    setExcelViewHidden(false);
    ui.hideExcelView->setVisible(true);
    ui.sortShowPushButton->setVisible(true);
    // the switch x axis check box shall only be active if model can handle timestamps
    ui.indexTypeCheckBox->setVisible(m_tableModel->canUseTimeOnX());

    // All these functions are supported when log is loaded
    ui.errDisplayCheckBox->setVisible(true);
    ui.evDisplayCheckBox->setVisible(true);
    ui.msgDisplayCheckBox->setVisible(true);
    ui.jumpToLocationCheckBox->setVisible(true);

    // Enable double clicking of graph
    connect(m_plot,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(plotDoubleClick(QMouseEvent*)));

}


void AP2DataPlot2D::threadError(QString errorstr)
{
    QMessageBox::information(0,"Error",errorstr);
    if (m_progressDialog)
    {
        m_progressDialog->hide();
        m_progressDialog.reset();
    }
    ui.dataSelectionScreen->clear();
    m_dataList.clear();
    this->close();
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
    QElapsedTimer timer1;
    timer1.start();
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

    QString formatheader = "FMT, 128, 89, FMT, BBnNZ, Type,Length,Name,Format,Columns\r\n";
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

    QLOG_DEBUG() << "Log export took " << timer1.elapsed() << "ms";

}

void AP2DataPlot2D::modeCheckBoxClicked(bool checked)
{
    hideShowTextArrows(checked, ModeMessage::TypeName);
}

void AP2DataPlot2D::errCheckBoxClicked(bool checked)
{
    hideShowTextArrows(checked, ErrorMessage::TypeName);
}

void AP2DataPlot2D::evCheckBoxClicked(bool checked)
{
    hideShowTextArrows(checked, EventMessage::TypeName);
}

void AP2DataPlot2D::msgCheckBoxClicked(bool checked)
{
    hideShowTextArrows(checked, MsgMessage::TypeName);
}

void AP2DataPlot2D::hideShowTextArrows(bool checked, const QString &type)
{
    if (!m_graphClassMap.contains(type))
    {
        return;
    }
    for (int i=0;i<m_graphClassMap[type].itemList.size();i++)
    {
        m_graphClassMap[type].itemList.at(i)->setVisible(checked);
    }
}

void AP2DataPlot2D::indexTypeCheckBoxClicked(bool checked)
{
    if (m_useTimeOnX != checked)
    {
        m_useTimeOnX = checked;
        // We have to remove all graphs when changing x-axis storing the active selection
        QList<QString> reEnableList = ui.dataSelectionScreen->disableAllItems();

        // And all arrows too
        removeTextArrows(ModeMessage::TypeName);
        removeTextArrows(ErrorMessage::TypeName);
        removeTextArrows(EventMessage::TypeName);
        removeTextArrows(MsgMessage::TypeName);

        // arrows can be inserted instantly again
        m_statusTextPos = 0;    // reset text arrow length
        insertTextArrows();

        // Graphs can be reenabled using previous stored selection
        ui.dataSelectionScreen->enableItemList(reEnableList);

        // Re -set x axis, scroller and zoom
        disconnect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
        disconnect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));
        disconnect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));

        setupXAxisAndScroller();
        m_wideAxisRect->axis(QCPAxis::atBottom)->setRange(m_scrollStartIndex, m_scrollEndIndex);
        m_plot->replot();
        ui.verticalScrollBar->setValue(ui.verticalScrollBar->maximum());

        connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
        connect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));
        connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
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
    // All elements selected -> filter is disabled
    if (ui.sortSelectTreeWidget->topLevelItemCount() == m_tableFilterList.size())
    {
        disableTableFilter();
        m_showOnlyActive = false;
    }
    // one or more elements selected -> RegEx must be used
    else
    {
        // It is VERY important to disable the filtering prior to set up a new one
        // If this is not done the regex filter gets terribly slow!!!
        disableTableFilter();
        for (int i=0;i<m_tableFilterList.size();i++)
        {
            sortstring += m_tableFilterList.at(i) + ((i == m_tableFilterList.size()-1) ? "" : "|");
        }
        m_tableFilterProxyModel->setFilterRegExp(sortstring);
        m_tableFilterProxyModel->setFilterRole(Qt::DisplayRole);
        m_tableFilterProxyModel->setFilterKeyColumn(1);
    }

    ui.tableSortGroupBox->setVisible(false);
    ui.sortShowPushButton->setText("Show Sort");
    m_showOnlyActive = true;
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

void AP2DataPlot2D::disableTableFilter()
{
    // The order of the statements is important to be fast on huge logs (15MB)
    // It does not really disable the filter but sets the rules to get a fast
    // result without any filtering
    m_tableFilterProxyModel->setFilterKeyColumn(0);
    m_tableFilterProxyModel->setFilterRole(Qt::DisplayRole);
    m_tableFilterProxyModel->setFilterFixedString("");
}


void AP2DataPlot2D::setupXAxisAndScroller()
{
    // Setup X-Axis for time or index formatting
    QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);
    xAxis->setNumberFormat("f");

    if (m_tableModel->canUseTimeOnX() && m_useTimeOnX)
    {
        m_scrollStartIndex = m_tableModel->getMinTime() / c_timeDivisor;
        m_scrollEndIndex = m_tableModel->getMaxTime() / c_timeDivisor;
        xAxis->setNumberPrecision(2);
        xAxis->setLabel("Time s");
    }
    else
    {
        m_scrollStartIndex = m_tableModel->getFirstIndex();
        m_scrollEndIndex = m_tableModel->getLastIndex();
        xAxis->setNumberPrecision(0);
        xAxis->setLabel("Index");
    }
    ui.horizontalScrollBar->setMinimum(m_scrollStartIndex);
    ui.horizontalScrollBar->setMaximum(m_scrollEndIndex);
}

void AP2DataPlot2D::plotCurrentIndex(double index)
{
    QLOG_DEBUG() << index;
    m_timeLine->start->setCoords(index, 999999);
    m_timeLine->end->setCoords(index, -999999);
    //m_plot->replot();
}

void AP2DataPlot2D::insertCurrentIndex()
{
    QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);
    QCPAxis *yAxis = m_wideAxisRect->axis(QCPAxis::atLeft);

    m_timeLine = new QCPItemLine(m_plot);
    m_timeLine->start->setAxes(xAxis, yAxis);
    m_timeLine->start->setCoords(0, 5);
    m_timeLine->end->setAxes(xAxis, yAxis);
    m_timeLine->end->setCoords(0, 0.0);
    m_timeLine->setPen(QPen(QColor::fromRgb(255, 0, 0), 1));

    m_plot->addItem(m_timeLine);
}

void AP2DataPlot2D::insertTextArrows()
{

    // Iterate all elements and call their formatter to create output string
    foreach (MessageBase::Ptr p_msg, m_indexToMessageMap)
    {
        double index = m_useTimeOnX ? p_msg->getTimeStamp() / c_timeDivisor : p_msg->getIndex();
        QString string;
        switch (m_loadedLogMavType)
        {
            case MAV_TYPE_QUADROTOR:
            case MAV_TYPE_HEXAROTOR:
            case MAV_TYPE_OCTOROTOR:
            case MAV_TYPE_HELICOPTER:
            case MAV_TYPE_TRICOPTER:
                string = Copter::MessageFormatter::format(p_msg);
                break;

            case MAV_TYPE_FIXED_WING:
                string = Plane::MessageFormatter::format(p_msg);
                break;

            case MAV_TYPE_GROUND_ROVER:
                string = Rover::MessageFormatter::format(p_msg);
                break;

            default:
                string = p_msg->toString();
                break;
        }
        QLOG_DEBUG() << p_msg->typeName() << " change at index" << index << "to" << string;

        // select ui checkbox associated with the message type
        QCheckBox *p_Check = NULL;
        if (p_msg->typeName() == ModeMessage::TypeName)
        {
            p_Check = ui.modeDisplayCheckBox;
        }
        else if (p_msg->typeName() == ErrorMessage::TypeName)
        {
            p_Check = ui.errDisplayCheckBox;
        }
        else if (p_msg->typeName() == EventMessage::TypeName)
        {
            p_Check = ui.evDisplayCheckBox;
        }
        else if (p_msg->typeName() == MsgMessage::TypeName)
        {
            p_Check = ui.msgDisplayCheckBox;
        }

        // plot the text arrow
        plotTextArrow(index, string, p_msg->typeName(), p_msg->typeColor(), p_Check);
        m_graphClassMap[p_msg->typeName()].messageMap[index] = string;
    }
}
