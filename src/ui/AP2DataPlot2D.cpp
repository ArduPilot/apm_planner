#include "AP2DataPlot2D.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>
#include <QStringList>

#include "UAS.h"
#include "UASManager.h"

AP2DataPlot2D::AP2DataPlot2D(QWidget *parent) : QWidget(parent)
{
    m_uas = 0;
    m_axisGroupingDialog = 0;
    m_logLoaderThread= 0;
    m_logLoaded = false;
    m_progressDialog=0;
    m_currentIndex=0;
    m_graphCount=0;

    ui.setupUi(this);

    m_plot = new QCustomPlot(ui.widget);
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);

    connect(m_plot,SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)),this,SLOT(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)));

    ui.horizontalLayout_3->addWidget(m_plot);

    m_plot->show();
    m_plot->plotLayout()->clear();

    m_wideAxisRect = new QCPAxisRect(m_plot);
    m_wideAxisRect->setupFullAxesBox(true);
    m_wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);
    m_wideAxisRect->removeAxis(m_wideAxisRect->axis(QCPAxis::atLeft,0));

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

    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),m_plot,SLOT(replot()));
    timer->start(500);
}
void AP2DataPlot2D::axisDoubleClick(QCPAxis* axis,QCPAxis::SelectablePart part,QMouseEvent* evt)
{
    if (m_axisGroupingDialog)
    {
        m_axisGroupingDialog->show();
        return;
    }
    m_axisGroupingDialog = new AP2DataPlotAxisDialog();
    connect(m_axisGroupingDialog,SIGNAL(graphAddedToGroup(QString,QString)),this,SLOT(graphAddedToGroup(QString,QString)));
    connect(m_axisGroupingDialog,SIGNAL(graphRemovedFromGroup(QString)),this,SLOT(graphRemovedFromGroup(QString)));
    //QMap<QString,QCPAxis*> m_axisList;
    for(QMap<QString,QCPAxis*>::const_iterator i=m_axisList.constBegin();i!=m_axisList.constEnd();i++)
    {
        m_axisGroupingDialog->addAxis(i.key(),i.value()->range().lower,i.value()->range().upper);
    }
    m_axisGroupingDialog->show();
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
                ui.tableWidget->setHorizontalHeaderItem(i+1,new QTableWidgetItem(split[i]));
            }
            for (int i=split.size();i<ui.tableWidget->columnCount()-1;i++)
            {
                ui.tableWidget->setHorizontalHeaderItem(i+1,new QTableWidgetItem(""));
            }
            if (ui.tableWidget->horizontalHeaderItem(column))
            {
                if (m_axisList.contains(ui.tableWidget->item(row,0)->text() + "." + ui.tableWidget->horizontalHeaderItem(column)->text()))
                {
                    //It's an enabled
                    m_addGraphAction->setText("Remove From Graph");
                    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
                    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
                    connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft()));
                }
                else
                {
                    m_addGraphAction->setText("Add To Graph");
                    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Disconnect from everything
                    disconnect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(removeGraphLeft())); //Disconnect from everything
                    connect(m_addGraphAction,SIGNAL(triggered()),this,SLOT(addGraphLeft())); //Add addgraphleft
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
            double difference = m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().upper - m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower;
            m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(m_currentIndex - difference);
            m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper(m_currentIndex);
            m_plot->replot();
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
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,double,quint64)),this,SLOT(valueChanged(int,QString,QString,double,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint8,quint64)),this,SLOT(valueChanged(int,QString,QString,qint8,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint16,quint64)),this,SLOT(valueChanged(int,QString,QString,qint16,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint32,quint64)),this,SLOT(valueChanged(int,QString,QString,qint32,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint64,quint64)),this,SLOT(valueChanged(int,QString,QString,qint64,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint8,quint64)),this,SLOT(valueChanged(int,QString,QString,quint8,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint16,quint64)),this,SLOT(valueChanged(int,QString,QString,quint16,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint32,quint64)),this,SLOT(valueChanged(int,QString,QString,quint32,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint64,quint64)),this,SLOT(valueChanged(int,QString,QString,quint64,quint64)));
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));
    }
    m_uas = uas;
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,double,quint64)),this,SLOT(valueChanged(int,QString,QString,double,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint8,quint64)),this,SLOT(valueChanged(int,QString,QString,qint8,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint16,quint64)),this,SLOT(valueChanged(int,QString,QString,qint16,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint32,quint64)),this,SLOT(valueChanged(int,QString,QString,qint32,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,qint64,quint64)),this,SLOT(valueChanged(int,QString,QString,qint64,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint8,quint64)),this,SLOT(valueChanged(int,QString,QString,quint8,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint16,quint64)),this,SLOT(valueChanged(int,QString,QString,quint16,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint32,quint64)),this,SLOT(valueChanged(int,QString,QString,quint32,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,quint64,quint64)),this,SLOT(valueChanged(int,QString,QString,quint64,quint64)));
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));

}
void AP2DataPlot2D::addSource(MAVLinkDecoder *decoder)
{
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,double,quint64)),this,SLOT(valueChanged(int,QString,QString,double,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,qint8,quint64)),this,SLOT(valueChanged(int,QString,QString,qint8,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,qint16,quint64)),this,SLOT(valueChanged(int,QString,QString,qint16,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,qint32,quint64)),this,SLOT(valueChanged(int,QString,QString,qint32,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,qint64,quint64)),this,SLOT(valueChanged(int,QString,QString,qint64,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,quint8,quint64)),this,SLOT(valueChanged(int,QString,QString,quint8,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,quint16,quint64)),this,SLOT(valueChanged(int,QString,QString,quint16,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,quint32,quint64)),this,SLOT(valueChanged(int,QString,QString,quint32,quint64)));
    connect(decoder,SIGNAL(valueChanged(int,QString,QString,quint64,quint64)),this,SLOT(valueChanged(int,QString,QString,quint64,quint64)));
}
void AP2DataPlot2D::updateValue(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec)
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
    if (!m_nameToAxisIndex.contains(propername))
    {
        //Also doesn't exist on the data select screen
        m_dataSelectionScreen->addItem(propername);
        m_nameToAxisIndex[propername] = m_currentIndex;
    }
    else
    {
        if (m_nameToAxisIndex[propername] == m_currentIndex)
        {
            m_currentIndex++;
            if (m_graphCount > 0 && ui.autoScrollCheckBox->isChecked())
            {
                m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower+1);
                m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper(m_currentIndex);
            }
        }
    }
    m_nameToAxisIndex[propername] = m_currentIndex;
    if (m_graphMap.contains(propername))
    {
        m_graphMap[propername]->addData(m_nameToAxisIndex[propername],value);
        if (m_graphToGroupMap.contains(propername))
        {
            //It's in a group
            if (!m_graphGroupRanges[m_graphToGroupMap[propername]].contains(value))
            {
                //It's out of scale for the group, expand it.
                if (m_graphGroupRanges[m_graphToGroupMap[propername]].lower > value)
                {
                    m_graphGroupRanges[m_graphToGroupMap[propername]].lower = value;
                }
                else if (m_graphGroupRanges[m_graphToGroupMap[propername]].upper < value)
                {
                    m_graphGroupRanges[m_graphToGroupMap[propername]].upper = value;
                }
                for (int i=0;i<m_graphGrouping[m_graphToGroupMap[propername]].size();i++)
                {
                    //m_graphMap[m_graphGrouping[m_graphToGroupName[name]][i]]->getA
                    //m_axisList[
                   m_axisList[m_graphGrouping[m_graphToGroupMap[propername]][i]]->setRange(m_graphGroupRanges[m_graphToGroupMap[propername]]);
                }

            }
        }
        else if (!m_graphMap[propername]->keyAxis()->range().contains(value))
        {
            m_graphMap[propername]->rescaleValueAxis();
            if (m_axisGroupingDialog)
            {
                m_axisGroupingDialog->updateAxis(propername,m_axisList[propername]->range().lower,m_axisList[propername]->range().upper);
            }
        }
    }
    m_onlineValueMap[propername].append(QPair<double,double>(m_nameToAxisIndex[propername],value));
}

void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const quint8 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const qint8 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const quint16 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const qint16 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const quint32 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const qint32 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const quint64 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const qint64 value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec)
{
    updateValue(uasId,name,unit,value,msec);
}
void AP2DataPlot2D::valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant value,const quint64 msec)
{
    updateValue(uasId,name,unit,value.toDouble(),msec);
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
    //Clear the graph
    for (int i=0;i<m_graphNameList.size();i++)
    {
        m_wideAxisRect->removeAxis(m_axisList[m_graphNameList[i]]);
        m_plot->removeGraph(m_graphMap[m_graphNameList[i]]);
        m_graphMap.remove(m_graphNameList[i]);
        m_axisList.remove(m_graphNameList[i]);
    }
    m_dataSelectionScreen->clear();
    m_nameToAxisIndex.clear();
    m_dataList.clear();
    m_onlineValueMap.clear();
    m_plot->replot();
    m_graphCount=0;

    if (m_logLoaded)
    {
        //Unload the log.
        m_logLoaded = false;
        ui.loadOfflineLogButton->setText("Load Log");
        ui.tableWidget->setVisible(false);
        ui.hideExcelView->setVisible(false);
        //<html><head/><body><p align="center">asdf</p></body></html>
        ui.logTypeLabel->setText("<p align=\"center\"><span style=\" font-size:24pt; color:#0000ff;\">Live Data</span></p>");
        return;
    }
    else
    {
        ui.logTypeLabel->setText("<p align=\"center\"><span style=\" font-size:24pt; color:#ff0000;\">Offline Log Loaded</span></p>");
    }
    ui.tableWidget->setVisible(true);
    ui.hideExcelView->setVisible(true);
    ui.autoScrollCheckBox->setChecked(false);
    ui.loadOfflineLogButton->setText("Unload Log");

    m_logLoaded = true;
    m_logLoaderThread = new AP2DataPlotThread();
    connect(m_logLoaderThread,SIGNAL(startLoad()),this,SLOT(loadStarted()));
    connect(m_logLoaderThread,SIGNAL(loadProgress(qint64,qint64)),this,SLOT(loadProgress(qint64,qint64)));
    connect(m_logLoaderThread,SIGNAL(error(QString)),this,SLOT(threadError(QString)));
    connect(m_logLoaderThread,SIGNAL(done()),this,SLOT(threadDone()));
    connect(m_logLoaderThread,SIGNAL(terminated()),this,SLOT(threadTerminated()));
    connect(m_logLoaderThread,SIGNAL(payloadDecoded(int,QString,QVariantMap)),this,SLOT(payloadDecoded(int,QString,QVariantMap)));
    connect(m_logLoaderThread,SIGNAL(lineRead(QString)),this,SLOT(logLine(QString)));
    m_logLoaderThread->loadFile(filename);
}
void AP2DataPlot2D::logLine(QString line)
{
    QStringList linesplit = line.split(",");
    if (ui.tableWidget->columnCount() < linesplit.size())
    {
        ui.tableWidget->setColumnCount(linesplit.size());
    }
    ui.tableWidget->setRowCount(ui.tableWidget->rowCount()+1);
    for (int i=0;i<linesplit.size();i++)
    {
        ui.tableWidget->setItem(ui.tableWidget->rowCount()-1,i,new QTableWidgetItem(linesplit[i].trimmed()));
    }
    if (line.startsWith("FMT"))
    {
        //Format line
        QString linename = linesplit[3].trimmed();
        QString lastformat = line.mid(linesplit[0].size() + linesplit[1].size() + linesplit[2].size() + linesplit[3].size() + linesplit[4].size() + 5);
        m_tableHeaderNameMap[linename] = lastformat.trimmed();

    }
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
        name = name.mid(name.indexOf(":")+1);
        for (QMap<QString,QList<QPair<int,QVariantMap> > >::const_iterator i=m_dataList.constBegin();i!=m_dataList.constEnd();i++)
        {
            if (i.value().size() > 0)
            {
                if (i.value()[0].second.contains(name))
                {
                    //This is the one we want.
                    QVector<double> xlist;
                    QVector<double> ylist;
                    float min = i.value()[0].second[name].toDouble();
                    float max = i.value()[0].second[name].toDouble();
                    for (int j=0;j<i.value().size();j++)
                    {
                        xlist.append(i.value()[j].first);
                        double val = i.value()[j].second[name].toDouble();
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

                    if (m_graphCount > 0)
                    {
                        connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
                    }
                    QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
                    axis->setLabelColor(color);
                    axis->setTickLabelColor(color);
                    axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
                    m_axisList[name] = axis;
                    QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
                    m_graphMap[name] = mainGraph1;
                    m_graphNameList.append(name);
                    mainGraph1->setData(xlist, ylist);
                    mainGraph1->rescaleValueAxis();
                    if (m_graphCount == 1)
                    {
                        mainGraph1->rescaleKeyAxis();
                    }

                   // mainGraph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
                    mainGraph1->setPen(QPen(color, 2));
                    m_plot->replot();
                    return;
                }
            }

        }
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

            if (m_graphCount > 0)
            {
                //connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
            }
            QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
            axis->setLabelColor(color);
            axis->setTickLabelColor(color);
            axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
            m_axisList[name] = axis;
            QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,m_graphCount++));
            m_graphMap[name] = mainGraph1;
            m_graphNameList.append(name);
            mainGraph1->setData(xlist, ylist);
            mainGraph1->rescaleValueAxis();
            if (m_graphCount == 1)
            {
                mainGraph1->rescaleKeyAxis();
            }

           // mainGraph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
            mainGraph1->setPen(QPen(color, 2));
            m_plot->replot();

        }

    }
}
void AP2DataPlot2D::graphAddedToGroup(QString name,QString group)
{
    if (!m_graphGrouping.contains(group))
    {
        m_graphGrouping[group] = QList<QString>();
        m_graphGroupRanges[group] = QCPRange();
    }
    m_graphToGroupMap[name] = group;
    m_graphGrouping[group].append(name);
    QCPAxis *firstaxis = m_axisList[m_graphGrouping[group][0]];
    //firstaxis->range()
    for (int i=0;i<m_graphGrouping[group].size();i++)
    {
        QCPAxis *axis = m_axisList[m_graphGrouping[group][i]];
    }
}

void AP2DataPlot2D::graphRemovedFromGroup(QString name)
{

}

void AP2DataPlot2D::itemDisabled(QString name)
{
    if (m_logLoaded)
    {
        name = name.mid(name.indexOf(":")+1);
    }
    m_wideAxisRect->removeAxis(m_axisList[name]);
    m_plot->removeGraph(m_graphMap[name]);
    m_plot->replot();
    m_graphMap.remove(name);
    m_axisList.remove(name);
    m_graphNameList.removeOne(name);
    m_graphCount--;

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
}

void AP2DataPlot2D::loadProgress(qint64 pos,qint64 size)
{
    m_progressDialog->setValue(((double)pos / (double)size) * 100.0);
}

void AP2DataPlot2D::threadDone()
{
    for (QMap<QString,QList<QPair<int,QVariantMap> > >::const_iterator i=m_dataList.constBegin();i!=m_dataList.constEnd();i++)
    {
        if (i.value().size() > 0)
        {
            for (QVariantMap::const_iterator j=i.value()[0].second.constBegin();j!=i.value()[0].second.constEnd();j++)
            {
                m_dataSelectionScreen->addItem(j.key());
            }
        }
    }
    m_progressDialog->hide();
    delete m_progressDialog;
    m_progressDialog=0;
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
