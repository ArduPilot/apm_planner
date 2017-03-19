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


#include "logging.h"
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
#include <QStandardItemModel>
#include "MainWindow.h"
#include "ArduPilotMegaMAV.h"
#include <QSettings>

#include "Loghandling/LogExporter.h"
#include "Loghandling/LogAnalysis.h"

#define ROW_HEIGHT_PADDING 3 //Number of additional pixels over font height for each row for the table/excel view.

AP2DataPlot2D::AP2DataPlot2D(QWidget *parent) : QWidget(parent),
    m_updateTimer(NULL),
    m_graphCount(0),
    m_plot(NULL),
    m_wideAxisRect(NULL),
    m_currentIndex(0),
    m_startIndex(0),
    m_uas(NULL),
    m_axisGroupingDialog(NULL),
    m_tlogReplayEnabled(false),
    m_logDownloadDialog(NULL),
    m_statusTextPos(0),
    m_lastHorizontalScrollerVal(0)
{
    ui.setupUi(this);

    QDateTime utc = QDateTime::currentDateTimeUtc();
    utc.setTimeSpec(Qt::LocalTime);
    //m_timeDiff = QDateTime::currentDateTime().msecsTo(utc);
    m_plot = new QCustomPlot(ui.widget);
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);

    connect(m_plot,SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));
    connect(m_plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(plotMouseMove(QMouseEvent*)));
    connect(ui.modeDisplayCheckBox,SIGNAL(clicked(bool)),this,SLOT(modeCheckBoxClicked(bool)));

    ui.verticalLayout_5->insertWidget(0,m_plot);

    m_plot->show();
    m_plot->plotLayout()->clear();

    m_wideAxisRect = new QCPAxisRect(m_plot);
    m_wideAxisRect->setupFullAxesBox(true);
    m_wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);
    m_wideAxisRect->removeAxis(m_wideAxisRect->axis(QCPAxis::atLeft,0));

    // set time format of x-axis
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("hh:mm:ss");
    dateTicker->setDateTimeSpec(Qt::UTC);
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setTicker(dateTicker);
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(0,100); //Default range of 0-100 milliseconds?

    m_plot->plotLayout()->addElement(0, 0, m_wideAxisRect);

    QCPMarginGroup *marginGroup = new QCPMarginGroup(m_plot);
    m_wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

    connect(ui.dataSelectionScreen, SIGNAL(itemEnabled(QString)),  this, SLOT(itemEnabled(QString)));
    connect(ui.dataSelectionScreen, SIGNAL(itemDisabled(QString)), this, SLOT(itemDisabled(QString)));

    ui.horizontalLayout_3->setStretch(0,5);
    ui.horizontalLayout_3->setStretch(1,1);

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());

    connect(ui.loadOfflineLogButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    connect(ui.autoScrollCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoScrollClicked(bool)));
    //connect(ui.tableWidget,SIGNAL(currentCellChanged(int,int,int,int)),this,SLOT(tableCellChanged(int,int,int,int)));

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

    ui.horizontalSplitter->setStretchFactor(0,20);
    ui.horizontalSplitter->setStretchFactor(1,1);

    loadSettings();
}

void AP2DataPlot2D::loadSettings()
{
    QSettings settings;
    settings.beginGroup("DATAPLOT_SETTINGS");
    ui.showValuesCheckBox->setChecked(settings.value("SHOW_VALUES", Qt::Unchecked).toBool());
    ui.autoScrollCheckBox->setChecked(settings.value("AUTO_SCROLL", Qt::Unchecked).toBool());
    ui.modeDisplayCheckBox->setChecked(settings.value("SHOW_MODE", Qt::Checked).toBool());
    settings.endGroup();
}

void AP2DataPlot2D::saveSettings()
{
    QSettings settings;
    settings.beginGroup("DATAPLOT_SETTINGS");
    settings.setValue("SHOW_VALUES", ui.showValuesCheckBox->isChecked());
    settings.setValue("AUTO_SCROLL", ui.autoScrollCheckBox->isChecked());
    settings.setValue("SHOW_MODE", ui.modeDisplayCheckBox->isChecked());
    settings.sync();
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
    disconnect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));
    disconnect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    disconnect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));

    ui.horizontalScrollBar->setValue(qRound(range.center())); // adjust position of scroll bar slider
    ui.horizontalScrollBar->setPageStep(qRound(range.size())); // adjust size of scroll bar slider
    double totalrange = m_scrollEndIndex - m_scrollStartIndex;
    double currentrange = range.upper - range.lower;
    ui.verticalScrollBar->setValue(100.0 * (currentrange / totalrange));

    connect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));
    connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
}

void AP2DataPlot2D::horizontalScrollMoved(int value)
{
    if (value != m_lastHorizontalScrollerVal)
    {
        m_lastHorizontalScrollerVal = value;
        disconnect(m_wideAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
        m_wideAxisRect->axis(QCPAxis::atBottom)->setRange(value,m_wideAxisRect->axis(QCPAxis::atBottom)->range().size(), Qt::AlignCenter);
        m_plot->replot();
        connect(m_wideAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    }
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

void AP2DataPlot2D::plotMouseMove(QMouseEvent *evt)
{
    if (!ui.showValuesCheckBox->isChecked())
    {
        return;
    }
    QString newresult = "";
    for (int i=0;i<m_graphClassMap.keys().size();i++)
    {
        QCPGraph *graph = m_graphClassMap.value(m_graphClassMap.keys()[i]).graph;
        double key   = graph->keyAxis()->pixelToCoord(evt->x());
        int keyIndex = graph->findBegin(key);
        if (i == 0)
        {
            newresult.append("Time: " + QDateTime::fromMSecsSinceEpoch(key * 1000.0).toString("hh:mm:ss") + "\n");
        }
        if (m_graphClassMap.keys()[i] == ModeMessage::TypeName)
        {
            if (m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.keys().size() > 1)
            {
                for (QMap<double,QString>::const_iterator messagemapiterator = m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.constBegin();messagemapiterator!=m_graphClassMap.value(m_graphClassMap.keys()[i]).messageMap.constEnd();messagemapiterator++)
                {
                    if ((messagemapiterator.key() < key) && ((messagemapiterator+1).key() > key))
                    {
                        //This only gets hit if we're not at the end, and we have the proper value
                        QStringList splitValue = messagemapiterator.value().split("\n");    // to remove the "by Radio" info
                        newresult.append(m_graphClassMap.keys()[i] + ": " + splitValue[0] + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
                        break;
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
        else if(keyIndex)
        {
            QString str = QString().sprintf( "%.9g", graph->dataMainValue(keyIndex));
            newresult.append(m_graphClassMap.keys()[i] + ": " + str + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
        }
        else
        {
            newresult.append(m_graphClassMap.keys()[i] + ": " + "NONE" + ((i == m_graphClassMap.keys().size()-1) ? "" : "\n"));
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

    qint64 msec_current = QDateTime::currentMSecsSinceEpoch();
    m_currentIndex = msec_current;
    qint64 newmsec = (msec_current - m_startIndex);// + m_timeDiff;
    if (m_graphCount > 0 && ui.autoScrollCheckBox->isChecked())
    {
        double diff = (newmsec / 1000.0) - m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().upper;
        m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower + diff);
        m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper((newmsec / 1000.0));
    }

    if(!m_graphClassMap.contains(ModeMessage::TypeName))
    {
        // create a graph with an invisible y-axis for the text arrows (mavlink has only mode)
        Graph arrowGraph;
        arrowGraph.axis = m_wideAxisRect->addAxis(QCPAxis::atLeft);
        arrowGraph.axis->setVisible(false);
        arrowGraph.axis->setRangeUpper(8.0);
        arrowGraph.graph = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft, m_graphCount++));
        arrowGraph.graph->rescaleValueAxis();
        m_graphClassMap.insert(ModeMessage::TypeName, arrowGraph);
        m_graphNameList.append(ModeMessage::TypeName);
    }

    int index = newmsec / 1000.0;
    m_graphClassMap[ModeMessage::TypeName].messageMap[index] = text;
    plotTextArrow(index, text, ModeMessage::TypeName, QColor(50,125,0), ui.modeDisplayCheckBox);
}

void AP2DataPlot2D::updateValue(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec,bool integer)
{
    Q_UNUSED(msec)
    Q_UNUSED(unit)
    if (m_uas->getUASID() != uasId)
    {
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
//                i.value().graph->removeData(0,m_onlineValueTimeoutList[0].second);
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
    QFileDialog *dialog = new QFileDialog(this,"Load File",QGC::logDirectory(),"Dataflash Log Files (*.log *.bin *.BIN *.tlog);;All Files (*.*)");
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->open(this, SLOT(loadDialogAccepted()));
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
    QString filename = dialog->selectedFiles().first();

    LogAnalysis *pAnalyze = new LogAnalysis(0);
    m_childGraphList.append(pAnalyze);
    connect(pAnalyze, SIGNAL(destroyed(QObject*)), this, SLOT(childGraphDestroyed(QObject*)));
    pAnalyze->setAttribute(Qt::WA_DeleteOnClose, true);
    pAnalyze->show();
    pAnalyze->activateWindow();
    pAnalyze->raise();
    pAnalyze->loadLog(filename);
}

AP2DataPlot2D::~AP2DataPlot2D()
{
    QLOG_DEBUG() << "AP2DataPlot2D::~AP2DataPlot2D()";
    saveSettings();
    if (m_updateTimer)
    {
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
        m_updateTimer = NULL;
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

void AP2DataPlot2D::itemDisabled(QString name)
{
    if (m_graphClassMap.contains(name)) // only enabled items can be disabled
    {
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
    m_graphNameList.clear();
    m_graphCount = 0;

    m_currentIndex = QDateTime::currentMSecsSinceEpoch();
    m_startIndex = m_currentIndex;
    m_onlineValueMap.clear();
    m_plot->replot();
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
    itemtext->position->setCoords(itemline->start->coords());

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

void AP2DataPlot2D::modeCheckBoxClicked(bool checked)
{
    hideShowTextArrows(checked, ModeMessage::TypeName);
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

