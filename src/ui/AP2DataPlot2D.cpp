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
AP2DataPlot2D::AP2DataPlot2D(QWidget *parent) : QWidget(parent),
    m_uas(NULL),
    m_logDownloadDialog(NULL),
    m_updateTimer(NULL),
    m_tlogReplayEnabled(false)
{
    m_startIndex = 0;
    m_axisGroupingDialog = 0;
    m_logLoaderThread= 0;
    m_logLoaded = false;
    m_progressDialog=0;
    m_currentIndex=0;
    m_graphCount=0;
    m_showOnlyActive = false;

    ui.setupUi(this);

    QDateTime utc = QDateTime::currentDateTimeUtc();
    utc.setTimeSpec(Qt::LocalTime);
    m_timeDiff = QDateTime::currentDateTime().msecsTo(utc);
    m_plot = new QCustomPlot(ui.widget);
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);

    connect(m_plot,SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));

    connect(m_plot,SIGNAL(mouseMove(QMouseEvent*)),this,SLOT(plotMouseMove(QMouseEvent*)));

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
    m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(m_timeDiff / 1000,(m_timeDiff / 1000) + 100); //Default range of 0-100 milliseconds?


    m_plot->plotLayout()->addElement(0, 0, m_wideAxisRect);

    QCPMarginGroup *marginGroup = new QCPMarginGroup(m_plot);
    m_wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

    m_dataSelectionScreen = new DataSelectionScreen(this);
    connect( m_dataSelectionScreen,SIGNAL(itemEnabled(QString)),this,SLOT(itemEnabled(QString)));
    connect( m_dataSelectionScreen,SIGNAL(itemDisabled(QString)),this,SLOT(itemDisabled(QString)));
    ui.horizontalLayout_3->addWidget(m_dataSelectionScreen);

    ui.horizontalLayout_3->setStretch(0,5);
    ui.horizontalLayout_3->setStretch(1,1);

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());

    ui.tableWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_addGraphAction = new QAction("Add To Graph",0);
    ui.tableWidget->addAction(m_addGraphAction);
    connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft()));

    ui.tableWidget->setVisible(false);
    ui.hideExcelView->setVisible(false);

    connect(ui.loadOfflineLogButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    connect(ui.autoScrollCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoScrollClicked(bool)));
    connect(ui.hideExcelView,SIGNAL(clicked(bool)),ui.tableWidget,SLOT(setHidden(bool)));
    connect(ui.tableWidget,SIGNAL(cellClicked(int,int)),this,SLOT(tableCellClicked(int,int)));

    ui.logTypeLabel->setText("<p align=\"center\"><span style=\" font-size:24pt; color:#0000ff;\">Live Data</span></p>");


    connect(ui.graphControlsPushButton,SIGNAL(clicked()),this,SLOT(graphControlsButtonClicked()));
    model = new QStandardItemModel();
    connect(ui.toKMLPushButton, SIGNAL(clicked()), this, SIGNAL(toKMLClicked()));
    connect(ui.horizontalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(horizontalScrollMoved(int)));
    connect(ui.verticalScrollBar,SIGNAL(sliderMoved(int)),this,SLOT(verticalScrollMoved(int)));

    connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    connect(m_wideAxisRect->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    m_plot->setPlottingHint(QCP::phFastPolylines,true);

    connect(ui.downloadPushButton, SIGNAL(clicked()), this, SLOT(showLogDownloadDialog()));
    ui.downloadPushButton->setEnabled(false);
    connect(ui.loadTLogButton,SIGNAL(clicked()),this,SLOT(replyTLogButtonClicked()));
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
}

void AP2DataPlot2D::hideEvent(QHideEvent *evt)
{
    if (m_updateTimer)
    {
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
        m_updateTimer = 0;
    }
}

void AP2DataPlot2D::verticalScrollMoved(int value)
{
    double percent = value / 100.0;
    double center = m_wideAxisRect->axis(QCPAxis::atBottom)->range().center();
    double requestedrange = ((m_scrollEndIndex - (m_timeDiff / 1000.0)) - m_scrollStartIndex) * percent;
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
    QToolTip::showText(QPoint(evt->pos().x() + m_plot->x(),evt->pos().y()+m_plot->y()),newresult);
}

void AP2DataPlot2D::axisDoubleClick(QCPAxis* axis,QCPAxis::SelectablePart part,QMouseEvent* evt)
{
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
    connect(m_axisGroupingDialog,SIGNAL(graphAddedToGroup(QString,QString,double)),this,SLOT(graphAddedToGroup(QString,QString,double)));
    connect(m_axisGroupingDialog,SIGNAL(graphRemovedFromGroup(QString)),this,SLOT(graphRemovedFromGroup(QString)));
    connect(m_axisGroupingDialog,SIGNAL(graphManualRange(QString,double,double)),this,SLOT(graphManualRange(QString,double,double)));
    connect(m_axisGroupingDialog,SIGNAL(graphAutoRange(QString)),this,SLOT(graphAutoRange(QString)));
    for (QMap<QString,Graph>::const_iterator i=m_graphClassMap.constBegin();i!=m_graphClassMap.constEnd();i++)
    {
        m_axisGroupingDialog->addAxis(i.key(),i.value().axis->range().lower,i.value().axis->range().upper,i.value().axis->labelColor());
    }
    m_axisGroupingDialog->show();
    QApplication::postEvent(m_axisGroupingDialog, new QEvent(QEvent::Show));
    QApplication::postEvent(m_axisGroupingDialog, new QEvent(QEvent::WindowActivate));
}

void AP2DataPlot2D::addGraphRight()
{
    if (ui.tableWidget->selectedItems().size() == 0)
    {
        return;
    }
    ui.tableWidget->selectedItems()[0]->row();
    if (ui.tableWidget->horizontalHeaderItem(ui.tableWidget->selectedItems()[0]->column()))
    {
        QString headertext = ui.tableWidget->horizontalHeaderItem(ui.tableWidget->selectedItems()[0]->column())->text();
        QString itemtext = ui.tableWidget->item(ui.tableWidget->selectedItems()[0]->row(),0)->text();
        itemEnabled(itemtext + "." + headertext);
        m_dataSelectionScreen->enableItem(itemtext + "." + headertext);
    }
}

void AP2DataPlot2D::addGraphLeft()
{
    if (ui.tableWidget->selectedItems().size() == 0)
    {
        return;
    }
    if (ui.tableWidget->horizontalHeaderItem(ui.tableWidget->selectedItems()[0]->column()))
    {
        QString headertext = ui.tableWidget->horizontalHeaderItem(ui.tableWidget->selectedItems()[0]->column())->text();
        QString itemtext = ui.tableWidget->item(ui.tableWidget->selectedItems()[0]->row(),0)->text();
        itemEnabled(itemtext + "." + headertext);
        m_dataSelectionScreen->enableItem(itemtext + "." + headertext);
    }
}
void AP2DataPlot2D::removeGraphLeft()
{
    if (ui.tableWidget->selectedItems().size() == 0)
    {
        return;
    }
    ui.tableWidget->selectedItems()[0]->row();
    if (ui.tableWidget->horizontalHeaderItem(ui.tableWidget->selectedItems()[0]->column()))
    {
        QString headertext = ui.tableWidget->horizontalHeaderItem(ui.tableWidget->selectedItems()[0]->column())->text();
        QString itemtext = ui.tableWidget->item(ui.tableWidget->selectedItems()[0]->row(),0)->text();
        itemDisabled(itemtext + "." + headertext);
        m_dataSelectionScreen->disableItem(itemtext + "." + headertext);
    }
}
void AP2DataPlot2D::showOnlyClicked()
{
    if (ui.tableWidget->selectedItems().size() == 0)
    {
        return;
    }
    QString name = ui.tableWidget->item(ui.tableWidget->selectedItems()[0]->row(),0)->text();
    for (int i=0;i<ui.tableWidget->rowCount();i++)
    {
        if (ui.tableWidget->item(i,0))
        {
            if (ui.tableWidget->item(i,0)->text() != name)
            {
                ui.tableWidget->hideRow(i);
            }
        }
        else
        {
            ui.tableWidget->hideRow(i);
        }
    }
    m_showOnlyActive = true;
}
void AP2DataPlot2D::showAllClicked()
{
    for (int i=0;i<ui.tableWidget->rowCount();i++)
    {
        ui.tableWidget->showRow(i);
    }
    m_showOnlyActive = false;
}

void AP2DataPlot2D::tableCellClicked(int row,int column)
{

    if (ui.tableWidget->item(row,0))
    {
        if (m_tableHeaderNameMap.contains(ui.tableWidget->item(row,0)->text()))
        {
            QString formatstr = m_tableHeaderNameMap.value(ui.tableWidget->item(row,0)->text());
            QStringList split = formatstr.split(",");
            for (int i=0;i<split.size();i++)
            {
                QTableWidgetItem *item = new QTableWidgetItem(split[i]);
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                ui.tableWidget->setHorizontalHeaderItem(i+1,item);
            }
            for (int i=split.size();i<ui.tableWidget->columnCount()-1;i++)
            {
                QTableWidgetItem *item = new QTableWidgetItem();
                item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                ui.tableWidget->setHorizontalHeaderItem(i+1,item);
            }
            if (ui.tableWidget->horizontalHeaderItem(column) && column != 0)
            {
                QString label = ui.tableWidget->item(row,0)->text() + "." + ui.tableWidget->horizontalHeaderItem(column)->text();
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
            else
            {
                //This is column 0
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
        }
    }

}

void AP2DataPlot2D::autoScrollClicked(bool checked)
{
    if (checked)
    {
        if (m_graphCount > 0)
        {
            double msec_current = ((QDateTime::currentMSecsSinceEpoch()- m_startIndex) + m_timeDiff) / 1000.0;
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
    ui.horizontalScrollBar->setMinimum(m_scrollStartIndex + m_timeDiff);
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

void AP2DataPlot2D::addSource(MAVLinkDecoder *decoder)
{
    //connect(decoder,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));
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
    qint64 newmsec = (msec_current - m_startIndex) + m_timeDiff;
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
    QCPAxis *xAxis = m_wideAxisRect->axis(QCPAxis::atBottom);
    QCPItemText *itemtext = new QCPItemText(m_plot);
    itemtext->setText(text);
    itemtext->position->setAxes(xAxis,m_graphClassMap["MODE"].axis);
    itemtext->position->setCoords((newmsec / 1000.0),2.0);
    m_plot->addItem(itemtext);
    m_graphClassMap["MODE"].itemList.append(itemtext);
    m_graphClassMap["MODE"].modeMap[newmsec / 1000.0] = text;


    QCPItemLine *itemline = new QCPItemLine(m_plot);
    m_graphClassMap["MODE"].itemList.append(itemline);
    itemline->start->setParentAnchor(itemtext->bottom);
    itemline->start->setAxes(xAxis, m_graphClassMap["MODE"].axis);
    itemline->start->setCoords(0.0, 0.0);
    itemline->end->setAxes(xAxis, m_graphClassMap["MODE"].axis);
    itemline->end->setCoords((newmsec / 1000.0), 0.0);
    itemline->setTail(QCPLineEnding::esDisc);
    itemline->setHead(QCPLineEnding::esSpikeArrow);
    m_plot->addItem(itemline);
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
        m_dataSelectionScreen->addItem(propername);
    }


    qint64 msec_current = QDateTime::currentMSecsSinceEpoch();
    m_currentIndex = msec_current;
    qint64 newmsec = (msec_current - m_startIndex) + m_timeDiff;
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
        m_onlineValueTimeoutList.append(QPair<qint64,double>(current + m_timeDiff,msec));
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
    if (value.type() == QVariant::Double)
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
    QString filename = "";
    if (!m_logLoaded)
    {
        filename = QFileDialog::getOpenFileName(this,"Select log file to open",QGC::logDirectory());
        if (filename == "")
        {
            return;
        }
    }
    QApplication::processEvents();
    //Clear the graph
    for (int i=0;i<m_graphNameList.size();i++)
    {

        m_wideAxisRect->removeAxis(m_graphClassMap.value(m_graphNameList[i]).axis);
        m_plot->removeGraph(m_graphClassMap.value(m_graphNameList[i]).graph);
    }
    m_dataSelectionScreen->clear();
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->clear();
    }
    m_plot->replot();
    m_graphClassMap.clear();
    m_graphCount=0;
    m_dataList.clear();

    if (m_logLoaded)
    {
        //Unload the log.
        m_logLoaded = false;
        ui.loadOfflineLogButton->setText("Load Log");
        ui.tableWidget->setVisible(false);
        ui.hideExcelView->setVisible(false);
        ui.logTypeLabel->setText("<p align=\"center\"><span style=\" font-size:24pt; color:#0000ff;\">Live Data</span></p>");
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelType(QCPAxis::ltDateTime);
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setDateTimeFormat("hh:mm:ss");
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(m_timeDiff / 1000,(m_timeDiff / 1000) + 100); //Default range of 0-100 milliseconds?
        m_currentIndex = QDateTime::currentMSecsSinceEpoch();
        m_startIndex = m_currentIndex;
        return;
    }
    else
    {
        ui.logTypeLabel->setText("<p align=\"center\"><span style=\" font-size:24pt; color:#ff0000;\">Offline Log Loaded</span></p>");
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setTickLabelType(QCPAxis::ltNumber);
        m_wideAxisRect->axis(QCPAxis::atBottom, 0)->setRange(0,100);
    }
    ui.autoScrollCheckBox->setChecked(false);
    ui.loadOfflineLogButton->setText("Unload Log");

    m_logLoaded = true;
    //Create the in-memory database
    m_sharedDb = QSqlDatabase::addDatabase("QSQLITE");
    m_sharedDb.setDatabaseName(":memory:");
    if (!m_sharedDb.open())
    {
        QMessageBox::information(0,"error","Error opening shared database " + m_sharedDb.lastError().text());
        return;
    }

    m_logLoaderThread = new AP2DataPlotThread();
    connect(m_logLoaderThread,SIGNAL(startLoad()),this,SLOT(loadStarted()));
    connect(m_logLoaderThread,SIGNAL(loadProgress(qint64,qint64)),this,SLOT(loadProgress(qint64,qint64)));
    connect(m_logLoaderThread,SIGNAL(error(QString)),this,SLOT(threadError(QString)));
    connect(m_logLoaderThread,SIGNAL(done(int)),this,SLOT(threadDone(int)));
    connect(m_logLoaderThread,SIGNAL(terminated()),this,SLOT(threadTerminated()));
    connect(m_logLoaderThread,SIGNAL(payloadDecoded(int,QString,QVariantMap)),this,SLOT(payloadDecoded(int,QString,QVariantMap)));
    connect(m_logLoaderThread,SIGNAL(lineRead(QString)),this,SLOT(logLine(QString)));
    currentIndex=0;
    m_logLoaderThread->loadFile(filename,&m_sharedDb);
}
void AP2DataPlot2D::logLine(QString line)
{
    if (ui.tableWidget->rowCount() <= currentIndex)
    {
        ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1000);
    }
        QStringList linesplit = line.split(",");
        if (ui.tableWidget->columnCount() < linesplit.size())
        {
            ui.tableWidget->setColumnCount(linesplit.size());
        }
        for (int j=0;j<linesplit.size();j++)
        {
            ui.tableWidget->setItem(currentIndex,j,new QTableWidgetItem(linesplit[j].trimmed()));
        }
        for (int j=linesplit.size();j<ui.tableWidget->columnCount();j++)
        {
            if (ui.tableWidget->item(currentIndex,j))
            {
                ui.tableWidget->item(currentIndex,j)->setText("");
            }
        }
        if (line.startsWith("FMT"))
        {
            //Format line
            QString linename = linesplit[3].trimmed();
            QString lastformat = line.mid(linesplit[0].size() + linesplit[1].size() + linesplit[2].size() + linesplit[3].size() + linesplit[4].size() + 5);
            m_tableHeaderNameMap[linename] = lastformat.trimmed();
        }
        currentIndex++;
}

void AP2DataPlot2D::threadTerminated()
{
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
}
void AP2DataPlot2D::itemEnabled(QString name)
{
    if (m_logLoaded)
    {
        QString parent = name.split(".")[0];
        QString child = name.split(".")[1];
        if (!m_sharedDb.isOpen())
        {
            if (!m_sharedDb.open())
            {
                //emit error("Unable to open database: " + m_sharedDb.lastError().text());
                QMessageBox::information(0,"Error","Error opening DB");
                return;
            }
        }
        QSqlQuery tablequery(m_sharedDb);
        //tablequery.prepare("SELECT * FROM '" + parent + "';");
        tablequery.prepare("SELECT * FROM 'FMT' WHERE name == '" + parent + "';");
        tablequery.exec();
        if (!tablequery.next())
        {
            return;
        }
        QSqlRecord record = tablequery.record();
        QStringList valuessplit = record.value(4).toString().split(","); //comma delimited list of names
        bool found = false;
        int index = 0;
        for (int i=0;i<valuessplit.size();i++)
        {
            if (valuessplit.at(i) == child)
            {
                found = true;
                index = i;
                i = valuessplit.size();
            }
        }
        if (!found)
        {
            return;
        }
        QSqlQuery itemquery(m_sharedDb);
        itemquery.prepare("SELECT * FROM '" + parent + "';");
        itemquery.exec();
        QVector<double> xlist;
        QVector<double> ylist;
        QList<QPair<double,QString> > strlist;
        bool isstr = false;
        while (itemquery.next())
        {
            QSqlRecord record = itemquery.record();
            int graphindex = record.value(0).toInt();
            if (record.value(index+1).type() == QVariant::String)
            {
                QString graphvaluestr = record.value(index+1).toString();
                strlist.append(QPair<double,QString>(graphindex,graphvaluestr));
                isstr = true;
            }
            else
            {
                double graphvalue = record.value(index+1).toDouble();
                ylist.append(graphvalue);
            }
            xlist.append(graphindex);
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
        if (m_graphCount == 1)
        {
            mainGraph1->rescaleKeyAxis();
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
void AP2DataPlot2D::graphAddedToGroup(QString name,QString group,double scale)
{
    m_graphClassMap[name].isManualRange = false;
    if (!m_graphGrouping.contains(group))
    {
        m_graphGrouping[group] = QList<QString>();
        m_graphGroupRanges[group] = m_graphClassMap.value(name).axis->range();
    }
    if (m_graphClassMap.value(name).groupName == group)
    {
        return;
    }
    else if (m_graphGrouping[group].contains(name))
    {
        return;
    }
    else
    {
        graphRemovedFromGroup(name);
    }
    m_graphClassMap[name].groupName = group;
    m_graphClassMap[name].isInGroup = true;
    m_graphGrouping[group].append(name);
    if (m_graphClassMap.value(name).axis->range().upper > m_graphGroupRanges[group].upper)
    {
        m_graphGroupRanges[group].upper = m_graphClassMap.value(name).axis->range().upper;
    }
    if (m_graphClassMap.value(name).axis->range().lower < m_graphGroupRanges[group].lower)
    {
        m_graphGroupRanges[group].lower = m_graphClassMap.value(name).axis->range().lower;
    }
    for (int i=0;i<m_graphGrouping[group].size();i++)
    {
        m_graphClassMap.value(m_graphGrouping[group][i]).axis->setRange(m_graphGroupRanges[group]);
    }
    m_plot->replot();
}
void AP2DataPlot2D::graphManualRange(QString name, double min, double max)
{
    graphRemovedFromGroup(name);
    m_graphClassMap[name].isManualRange = true;
    m_graphClassMap.value(name).axis->setRange(min,max);
}
void AP2DataPlot2D::graphAutoRange(QString name)
{
    m_graphClassMap[name].isManualRange = true;
    m_graphClassMap.value(name).graph->rescaleValueAxis();
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->updateAxis(name,m_graphClassMap.value(name).axis->range().lower,m_graphClassMap.value(name).axis->range().upper);
    }
}

void AP2DataPlot2D::graphRemovedFromGroup(QString name)
{
    //Always remove it from manual range
    if (!m_graphClassMap.contains(name))
    {
        return;
    }
    m_graphClassMap[name].isManualRange = false;
    if (!m_graphClassMap.value(name).isInGroup)
    {
        //Not in a group
        return;
    }
    QString group = m_graphClassMap.value(name).groupName;
    m_graphGrouping[group].removeOne(name);
    m_graphClassMap[name].isInGroup = false;
    //m_graphClassMap.value(name).graph->valueAxis()->setRange;
    m_graphClassMap.value(name).graph->rescaleValueAxis();
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->updateAxis(name,m_graphClassMap.value(name).axis->range().lower,m_graphClassMap.value(name).axis->range().upper);
    }
    if (m_graphGrouping[group].size() > 0)
    {
        m_graphClassMap.value(m_graphGrouping[group][0]).graph->rescaleValueAxis();
        if (m_axisGroupingDialog)
        {
            m_axisGroupingDialog->updateAxis(name,m_graphClassMap.value(name).axis->range().lower,m_graphClassMap.value(name).axis->range().upper);
        }
        m_graphGroupRanges[group] = m_graphClassMap.value(m_graphGrouping[group][0]).axis->range();
    }
    for (int i=0;i<m_graphGrouping[group].size();i++)
    {
        if (m_graphClassMap.value(m_graphGrouping[group][i]).axis->range().upper > m_graphGroupRanges[group].upper)
        {
            m_graphGroupRanges[group].upper = m_graphClassMap.value(m_graphGrouping[group][i]).axis->range().upper;
        }
        if (m_graphClassMap.value(m_graphGrouping[group][i]).axis->range().lower < m_graphGroupRanges[group].lower)
        {
            m_graphGroupRanges[group].lower = m_graphClassMap.value(m_graphGrouping[group][i]).axis->range().lower;
        }
    }
    for (int i=0;i<m_graphGrouping[group].size();i++)
    {
        m_graphClassMap.value(m_graphGrouping[group][i]).axis->setRange(m_graphGroupRanges[group]);
    }
    m_plot->replot();
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
}

void AP2DataPlot2D::loadStarted()
{
    m_progressDialog = new QProgressDialog("Loading File","Cancel",0,100);
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

void AP2DataPlot2D::threadDone(int errors)
{
    if (!m_sharedDb.isOpen())
    {
        if (!m_sharedDb.open())
        {
            //emit error("Unable to open database: " + m_sharedDb.lastError().text());
            QMessageBox::information(0,"Error","Error opening DB");
            return;
        }
    }
    if (errors != 0)
    {
        QMessageBox::information(this,"Warning","There were errors countered with " + QString::number(errors) + " lines in the log file. The data is potentially corrupt and incorrect");
    }
    //fmttablecreate.prepare("CREATE TABLE 'FMT' (typeID integer PRIMARY KEY,length integer,name varchar(200),format varchar(6000));");
    QSqlQuery fmtquery(m_sharedDb);
    fmtquery.prepare("SELECT * FROM 'FMT';");
    if (!fmtquery.exec())
    {
        QMessageBox::information(0,"Error","Error selecting from table 'FMT' " + m_sharedDb.lastError().text());
        return;

    }
    //QSqlQuery itemquery(m_sharedDb);

    while (fmtquery.next())
    {
        QSqlRecord record = fmtquery.record();
        QString name = record.value(2).toString();
        QString vars = record.value(4).toString();
        QStringList varssplit = vars.split(",");
        for (int i=0;i<varssplit.size();i++)
        {
            m_dataSelectionScreen->addItem(name + "." + varssplit.at(i));
        }
        QLOG_DEBUG() << record.value(0) << record.value(1) << record.value(2) << record.value(3) << record.value(4);
        //rowlist.clear();
        //itemquery.prepare("SELECT * FROM '" + name + "';");
        //itemquery.exec();
        //while (itemquery.next())
        //{

        //}
        QString linename = name;
        QString lastformat = vars;
        m_tableHeaderNameMap[linename] = lastformat.trimmed();
    }
    m_scrollStartIndex = 0;
    m_scrollEndIndex = currentIndex;
    ui.horizontalScrollBar->setMinimum(m_scrollStartIndex);
    ui.horizontalScrollBar->setMaximum(m_scrollEndIndex);
    ui.tableWidget->setRowCount(currentIndex);

    m_progressDialog->hide();
    delete m_progressDialog;
    m_progressDialog=0;
    ui.tableWidget->setVisible(true);
    ui.hideExcelView->setVisible(true);
}
void AP2DataPlot2D::threadError(QString errorstr)
{
    QMessageBox::information(0,"Error",errorstr);
    m_progressDialog->hide();
    delete m_progressDialog;
    m_progressDialog=0;
    m_dataSelectionScreen->clear();
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
