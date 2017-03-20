/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2017 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file LogAnalysis.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @author Michael Carpenter <malcom2073@gmail.com>
 * @date 17 Mrz 2017
 * @brief File providing implementation for the log analysing classes
 */

#include "LogAnalysis.h"
#include "logging.h"

#include "ArduPilotMegaMAV.h"
#include "Loghandling/LogExporter.h"



LogAnalysisCursor::LogAnalysisCursor(QCustomPlot *parentPlot, double xPosition, CursorType type) :
    QCPItemStraightLine(parentPlot),
    m_currentPos(xPosition),
    m_otherCursorPos(0.0),
    m_type(type),
    mp_otherCursor(0)
{
    QLOG_DEBUG() << "LogAnalysisCursor::LogAnalysisCursor - CTOR - type " << type;
    point1->setCoords(m_currentPos, 1);
    point2->setCoords(m_currentPos, 0);
    setSelectable(true);
    setClipToAxisRect(false);
}

LogAnalysisCursor::~LogAnalysisCursor()
{
    QLOG_DEBUG() << "LogAnalysisCursor::~LogAnalysisCursor - DTOR - type " << m_type;
}

void LogAnalysisCursor::setOtherCursor(LogAnalysisCursor *pCursor)
{
    mp_otherCursor = pCursor;
}

double LogAnalysisCursor::getCurrentXPos() const
{
    return m_currentPos;
}

void LogAnalysisCursor::setCurrentXPos(double xPosition)
{
    if(m_type == simple)
    {
        m_currentPos = xPosition;
        point1->setCoords(m_currentPos, 1);
        point2->setCoords(m_currentPos, 0);
        mParentPlot->replot();
    }
}

void LogAnalysisCursor::mousePressEvent(QMouseEvent *event, const QVariant &details)
{
    Q_UNUSED(details);
    QLOG_TRACE() << "LogAnalysisCursor::mousePressEvent";
    if(mSelected)
    {
        event->accept();
        if(mp_otherCursor)
        {
            m_otherCursorPos = dynamic_cast<LogAnalysisCursor*>(mp_otherCursor)->getCurrentXPos();
        }
    }
    else
    {
        event->ignore();
    }
}

void LogAnalysisCursor::mouseMoveEvent(QMouseEvent *event, const QPointF &startPos)
{
    QLOG_TRACE() << "LogAnalysisCursor::mouseMoveEvent " << event << " " << startPos;
    // allow dragging only if selected
    if(mSelected)
    {
        double movePos = point1->keyAxis()->pixelToCoord(event->x());
        if(m_type == simple)
        {
            m_currentPos = movePos;
        }
        else if(m_type == left && movePos < m_otherCursorPos)   // avoid left cursor overtaking the right one
        {
            m_currentPos = movePos;
            emit rangeCursorMoving();
        }
        else if(m_type == right && movePos > m_otherCursorPos)  // avoid right cursor overtaking the left one
        {
            m_currentPos = movePos;
            emit rangeCursorMoving();
        }

        point1->setCoords(m_currentPos, 1);
        point2->setCoords(m_currentPos, 0);
        mParentPlot->replot();
    }
}

void LogAnalysisCursor::mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos)
{
    QLOG_TRACE() << "LogAnalysisCursor::mouseReleaseEvent " << event << " " << startPos;
    m_currentPos = point1->keyAxis()->pixelToCoord(event->x());
    if(m_type == simple)
    {
        emit newCursorPos(m_currentPos); // Simple cursor emits signal with new position
    }
    else
    {
        emit cursorRangeChange();   // left and right emit signal that position has changed
    }
}

void LogAnalysisCursor::mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details)
{
    Q_UNUSED(event);
    Q_UNUSED(details);
    QLOG_DEBUG() << "LogAnalysisCursor::mouseDoubleClickEvent";
}

void LogAnalysisCursor::wheelEvent(QWheelEvent *event)
{
    QLOG_TRACE() << "LogAnalysisCursor::wheelEvent";
    if(mSelected)
    {
        event->accept();
    }
    else
    {
        event->ignore();
    }
}

//************************************************************************************

LogAnalysisAxis::LogAnalysisAxis(QCPAxisRect *axisRect, AxisType type) : QCPAxis(axisRect, type)
{
    QLOG_DEBUG() << "LogAnalysisAxis::LogAnalysisAxis - CTOR";
}

LogAnalysisAxis::~LogAnalysisAxis()
{
    QLOG_DEBUG() << "LogAnalysisAxis::~LogAnalysisAxis - DTOR";
}

void LogAnalysisAxis::wheelEvent(QWheelEvent *event)
{
    QLOG_TRACE() << "LogAnalysisCursor::wheelEvent";
    if(mSelectedParts == QCPAxis::spAxis)
    {
        event->accept();
        double axisPos = pixelToCoord(event->y());

        if(event->delta() < 0)
        {
            scaleRange(1.1, axisPos);
        }
        else if(event->delta() > 0)
        {
            scaleRange(0.9, axisPos);
        }
        mParentPlot->replot();
    }
    else
    {
        event->ignore();
    }
}

//************************************************************************************

const QString LogAnalysis::s_CursorLayerName("cursors");

LogAnalysis::LogAnalysis(QWidget *parent) :
    QWidget(parent),
    m_loadedLogMavType(MAV_TYPE_ENUM_END),
    m_useTimeOnXAxis(false),
    m_scrollStartIndex(0),
    m_scrollEndIndex(0),
    m_statusTextPos(0),
    m_lastHorizontalScrollVal(0),
    m_kmlExport(false),
    m_cursorXAxisRange(0.0),
    mp_cursorSimple(0),
    mp_cursorLeft(0),
    mp_cursorRight(0)
{
    QLOG_DEBUG() << "LogAnalysis::LogAnalysis - CTOR";
    ui.setupUi(this);

    // create QCustomPlot
    m_plotPtr.reset(new QCustomPlot(ui.widget));

    // Setup UI
    ui.verticalLayout_5->insertWidget(0, m_plotPtr.data());
    ui.horizontalLayout_3->setStretch(0,5);
    ui.horizontalLayout_3->setStretch(1,1);
    ui.horizontalSplitter->setStretchFactor(0,20);
    ui.horizontalSplitter->setStretchFactor(1,1);
    ui.tableWidget->verticalHeader()->setDefaultSectionSize(ui.tableWidget->fontMetrics().height() + s_ROW_HEIGHT_PADDING);
    ui.tableFilterGroupBox->setVisible(false);

    // on window open table view is hidden
    hideTableView(true);

    // setup QCustomPlot
    m_plotPtr->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectItems | QCP::iSelectAxes);  // set allowed intercations
    m_plotPtr->plotLayout()->clear();                               // clear default axis rectangle so we can start from scratch

    QCPAxisRect *axisRect = new QCPAxisRect(m_plotPtr.data());      // Create new axis rectangle. QCustomPlot takes ownership
    axisRect->setupFullAxesBox(true);                               // Setup axis rect
    axisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);      // remove tick labels on right axis
    axisRect->axis(QCPAxis::atRight, 0)->setTicks(false);           // remove ticks on right axis
    axisRect->removeAxis(axisRect->axis(QCPAxis::atLeft, 0));       // remove left axis
    // setup x axis ticker
    QSharedPointer<QCPAxisTickerFixed> fixedTicker(new QCPAxisTickerFixed);
    fixedTicker->setTickStep(1.0);
    fixedTicker->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    axisRect->axis(QCPAxis::atBottom, 0)->setTicker(fixedTicker);
    axisRect->axis(QCPAxis::atBottom, 0)->setRange(0, 100);
    axisRect->axis(QCPAxis::atBottom, 0)->setSelectableParts(QCPAxis::spNone);
    // setup margins
    QCPMarginGroup *marginGroup = new QCPMarginGroup(m_plotPtr.data()); //QCustomPlot takes ownership
    axisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

    m_plotPtr->plotLayout()->addElement(0, 0, axisRect);    // Add the configured axis rect to layout
    m_plotPtr->setPlottingHint(QCP::phFastPolylines, true);  // TODO perhaps use OpenGL?!
    m_plotPtr->show();

    // Add layers and make them invisible. All arrow plots and the cursor have an own layer above main
    QCPLayer *pLayerBelow = m_plotPtr->layer("main");
    if(pLayerBelow)
    {
        m_plotPtr->addLayer(ModeMessage::TypeName, pLayerBelow);
        pLayerBelow = m_plotPtr->layer(ModeMessage::TypeName);
        pLayerBelow->setVisible(false);

        m_plotPtr->addLayer(ErrorMessage::TypeName, pLayerBelow);
        pLayerBelow = m_plotPtr->layer(ErrorMessage::TypeName);
        pLayerBelow->setVisible(false);

        m_plotPtr->addLayer(EventMessage::TypeName, pLayerBelow);
        pLayerBelow = m_plotPtr->layer(EventMessage::TypeName);
        pLayerBelow->setVisible(false);

        m_plotPtr->addLayer(MsgMessage::TypeName, pLayerBelow);
        pLayerBelow = m_plotPtr->layer(MsgMessage::TypeName);
        pLayerBelow->setVisible(false);

        m_plotPtr->addLayer(s_CursorLayerName, pLayerBelow);
        pLayerBelow = m_plotPtr->layer(s_CursorLayerName);
        pLayerBelow->setVisible(false);
    }

    // Create axis grouping dialog
    m_axisGroupingDialog.reset(new AP2DataPlotAxisDialog());
    m_axisGroupingDialog->hide();
    connect(m_axisGroupingDialog.data(), SIGNAL(graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange>)), this, SLOT(graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange>)));

    // setup policy and connect slot for context menu popup
    m_plotPtr->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_plotPtr.data(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));
    m_plotPtr->replot();

    // connect to default signals
    connect(ui.hideTableWidgetCheckBox, SIGNAL(clicked(bool)), this, SLOT(hideTableView(bool)));
    connect(ui.showValuesCheckBox,      SIGNAL(clicked(bool)), this, SLOT(showValueUnderMouseClicked(bool)));
    connect(ui.modeDisplayCheckBox,     SIGNAL(clicked(bool)), this, SLOT(modeCheckboxClicked(bool)));
    connect(ui.evDisplayCheckBox,       SIGNAL(clicked(bool)), this, SLOT(eventCheckboxClicked(bool)));
    connect(ui.errDisplayCheckBox,      SIGNAL(clicked(bool)), this, SLOT(errorCheckboxClicked(bool)));
    connect(ui.msgDisplayCheckBox,      SIGNAL(clicked(bool)), this, SLOT(msgCheckboxClicked(bool)));
    connect(ui.indexTypeCheckBox,       SIGNAL(clicked(bool)), this, SLOT(indexTypeCheckBoxClicked(bool)));
    connect(ui.tableCursorCheckBox,     SIGNAL(clicked(bool)), this, SLOT(enableTableCursor(bool)));
    connect(ui.rangeCursorCheckBox,     SIGNAL(clicked(bool)), this, SLOT(enableRangeCursor(bool)));

    connect(ui.filterSelectTreeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(filterItemChanged(QTreeWidgetItem*,int)));
    connect(ui.filterAcceptPushButton,SIGNAL(clicked()),this,SLOT(filterAcceptClicked()));
    connect(ui.filterCancelPushButton,SIGNAL(clicked()),this,SLOT(filterCancelClicked()));
    connect(ui.filterShowPushButton,SIGNAL(clicked()),this,SLOT(showFilterButtonClicked()));
    connect(ui.filterSelectAllPushButton,SIGNAL(clicked()),this,SLOT(filterSelectAllClicked()));
    connect(ui.filterInvertSelectPushButton,SIGNAL(clicked()),this,SLOT(filterSelectInvertClicked()));

    connect(ui.exportLogButton, SIGNAL(clicked()), this, SLOT(exportAsciiLogClicked()));
    connect(ui.exportKmlButton, SIGNAL(clicked()), this, SLOT(exportKmlClicked()));
    connect(ui.graphControlsPushButton, SIGNAL(clicked()), this, SLOT(graphControlsButtonClicked()));
    connect(ui.resetScalingPushButton, SIGNAL(clicked()), this, SLOT(resetValueScaling()));

    loadSettings();
}

LogAnalysis::~LogAnalysis()
{
    QLOG_DEBUG() << "LogAnalysis::~LogAnalysis - DTOR";
    saveSettings();
}

void LogAnalysis::loadLog(QString filename)
{
    QLOG_DEBUG() << "LogAnalysis::loadLog - Filename:" << filename;
    m_filename = filename;
    // setup window name
    QString shortfilename = filename.mid(filename.lastIndexOf("/")+1);
    setWindowTitle(tr("Graph: %1").arg(shortfilename));

    // create datastorage, loader thread and connect the signals
    m_dataStoragePtr = LogdataStorage::Ptr(new LogdataStorage());
    m_loaderThreadPtr.reset(new AP2DataPlotThread(m_dataStoragePtr));
    connect(m_loaderThreadPtr.data(), SIGNAL(startLoad()), this, SLOT(logLoadingStarted()));
    connect(m_loaderThreadPtr.data(), SIGNAL(loadProgress(qint64, qint64)), this, SLOT(logLoadingProgress(qint64, qint64)));
    connect(m_loaderThreadPtr.data(), SIGNAL(error(QString)), this, SLOT(logLoadingError(QString)));
    connect(m_loaderThreadPtr.data(), SIGNAL(done(AP2DataPlotStatus)), this, SLOT(logLoadingDone(AP2DataPlotStatus)));
    connect(m_loaderThreadPtr.data(), SIGNAL(finished()), this, SLOT(logLoadingThreadTerminated()));

    // let the loader start the log loading
    m_loaderThreadPtr->loadFile(filename);
}

void LogAnalysis::setTablePos(double xPosition)
{
    if(mp_cursorSimple)  // only if simple cursor is active
    {
        double timeStamp = 0.0;

        if (m_useTimeOnXAxis)
        {
            timeStamp = xPosition;
            xPosition = m_dataStoragePtr->getNearestIndexForTimestamp(timeStamp);
        }

        qint64 position = static_cast<qint64>(floor(xPosition));
        qint64 min = 0;
        qint64 max = static_cast<qint64>(m_dataStoragePtr->rowCount());

        if ( position >= max )
        {
            ui.tableWidget->scrollToBottom();
            double plotPos = m_useTimeOnXAxis ? m_dataStoragePtr->getMaxTimeStamp() : max;
            mp_cursorSimple->setCurrentXPos(plotPos);

        }
        else if ( position <= min )
        {
            ui.tableWidget->scrollToTop();
            double plotPos = m_useTimeOnXAxis ? m_dataStoragePtr->getMinTimeStamp() : min;
            mp_cursorSimple->setCurrentXPos(plotPos);
        }
        else
        {
            //search for previous event (remember the table may be filtered)
            QModelIndex sourceIndex = m_dataStoragePtr->index(static_cast<int>(position - min), 0);
            QModelIndex index = mp_tableFilterProxyModel->mapFromSource(sourceIndex);
            while ( sourceIndex.row() >= static_cast<int>(min) && !index.isValid() )
            {
                sourceIndex = m_dataStoragePtr->index(sourceIndex.row() - 1, 0);
                index = mp_tableFilterProxyModel->mapFromSource(sourceIndex);
            }

            if ( !index.isValid() )
            {
                //couldn't find filtered index by looking back, try forward...
                sourceIndex = m_dataStoragePtr->index(static_cast<int>(position - min), 0);
                index = mp_tableFilterProxyModel->mapFromSource(sourceIndex);
                while ( sourceIndex.row() <= static_cast<int>(max) && !index.isValid() )
                {
                    sourceIndex = m_dataStoragePtr->index(sourceIndex.row() + 1, 0);
                    index = mp_tableFilterProxyModel->mapFromSource(sourceIndex);
                }
            }
            ui.tableWidget->setCurrentIndex(index);
            ui.tableWidget->scrollTo(index);
        }
    }
}

void LogAnalysis::rangeCursorsMoving()
{
    if(mp_cursorLeft && mp_cursorRight)
    {
        double leftPos  = mp_cursorLeft->getCurrentXPos();
        double rightPos = mp_cursorRight->getCurrentXPos();

        m_cursorXAxisRange = rightPos - leftPos;
    }
}


void LogAnalysis::cursorRangeChange()
{
    if(mp_cursorLeft && mp_cursorRight)
    {
        m_rangeValuesStorage.clear();
        double leftPos  = mp_cursorLeft->getCurrentXPos();
        double rightPos = mp_cursorRight->getCurrentXPos();

        m_cursorXAxisRange = rightPos - leftPos;

        QHash<QString, GraphElements>::Iterator iter;
        for(iter = m_activeGraphs.begin(); iter != m_activeGraphs.end(); ++iter)
        {
            QSharedPointer<QCPGraphDataContainer> dataPtr = iter->p_graph->data();
            RangeValues rangeVals;

            int rangeStartIndex = iter->p_graph->findBegin(leftPos);
            int rangeEndIndex   = iter->p_graph->findBegin(rightPos);
            rangeVals.m_measurements = rangeEndIndex - rangeStartIndex;

            for(int i = rangeStartIndex; i < rangeEndIndex; ++i)
            {
                double value = dataPtr->at(i)->mainValue();
                rangeVals.m_average += value;
                rangeVals.m_min = rangeVals.m_min > value ? value : rangeVals.m_min;
                rangeVals.m_max = rangeVals.m_max < value ? value : rangeVals.m_max;
            }
            rangeVals.m_average /= rangeVals.m_measurements;
            m_rangeValuesStorage.insert(iter.key(), rangeVals);
        }
    }
}

void LogAnalysis::setupXAxisAndScroller()
{
    // Setup X-Axis for time or index formatting
    QCPAxis *xAxis = m_plotPtr->axisRect()->axis(QCPAxis::atBottom);
    xAxis->setNumberFormat("f");

    if (m_useTimeOnXAxis)
    {
        m_scrollStartIndex = static_cast<qint64>(m_dataStoragePtr->getMinTimeStamp());
        m_scrollEndIndex = static_cast<qint64>(m_dataStoragePtr->getMaxTimeStamp());
        xAxis->setNumberPrecision(2);
        xAxis->setLabel("Time s");
    }
    else
    {
        m_scrollStartIndex = 0;
        m_scrollEndIndex = m_dataStoragePtr->rowCount();
        xAxis->setNumberPrecision(0);
        xAxis->setLabel("Index");
    }
    ui.horizontalScrollBar->setMinimum(static_cast<int>(m_scrollStartIndex));
    ui.horizontalScrollBar->setMaximum(static_cast<int>(m_scrollEndIndex));
}

void LogAnalysis::insertTextArrows()
{
    // Iterate all elements and call their formatter to create output string
    foreach (MessageBase::Ptr p_msg, m_indexToMessageMap)
    {
        double index = m_useTimeOnXAxis ? p_msg->getTimeStamp() : p_msg->getIndex();
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

        // plot the text arrow
        plotTextArrow(index, string, p_msg->typeName(), p_msg->typeColor());
    }
}

void LogAnalysis::plotTextArrow(double index, const QString &text, const QString &layerName, const QColor &color)
{
    QLOG_DEBUG() << "plotTextArrow:" << index << " to " << layerName << " layer.";
    m_statusTextPos++;
    m_statusTextPos = m_statusTextPos > s_TextArrowPositions ? 1 : m_statusTextPos;

    if(!m_plotPtr->setCurrentLayer(layerName))  // text arrows all have their own layer
    {
        QLOG_WARN() << "LogAnalysis::plotTextArrow - no layer with name " << layerName;
    }

    QCPAxis *xAxis = m_plotPtr->axisRect()->axis(QCPAxis::atBottom);
    QCPItemText *itemtext = new QCPItemText(m_plotPtr.data()); // QCustomPlot takes ownership
    itemtext->setText(text);
    itemtext->setColor(color);
    itemtext->setPositionAlignment(Qt::AlignLeft | Qt::AlignBottom);
    itemtext->position->setAxes(xAxis, m_arrowGraph.p_yAxis);
    itemtext->setSelectable(false);

    QCPItemLine *itemline = new QCPItemLine(m_plotPtr.data()); // QCustomPlot takes ownership
    itemline->setPen(QPen(color));
    itemline->start->setAxes(xAxis, m_arrowGraph.p_yAxis);
    itemline->start->setCoords(index, m_statusTextPos);
    itemline->end->setAxes(xAxis, m_arrowGraph.p_yAxis);
    itemline->end->setCoords(index, 0.0);
    itemline->setTail(QCPLineEnding::esDisc);
    itemline->setHead(QCPLineEnding::esSpikeArrow);
    itemtext->position->setCoords(itemline->start->coords());
    itemline->setSelectable(false);
}

void LogAnalysis::disableTableFilter()
{
    // The order of the statements is important to be fast on huge logs (15MB)
    // It does not really disable the filter but sets the rules to get a fast
    // result without any filtering
    mp_tableFilterProxyModel->setFilterKeyColumn(0);
    mp_tableFilterProxyModel->setFilterRole(Qt::DisplayRole);
    mp_tableFilterProxyModel->setFilterFixedString("");
}

void LogAnalysis::doExport(bool kmlExport)
{
    m_kmlExport = kmlExport;
    QString exportExtension = kmlExport ? "kml" : "log";

    // replace any extension by the export extension
    QString exportFilename = m_filename.replace(QRegularExpression("\\w+$"), exportExtension);
    QFileDialog *dialog = new QFileDialog(this, "Save Log File", QGC::logDirectory());
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setAcceptMode(QFileDialog::AcceptSave);
    dialog->setNameFilter("*." + exportExtension);
    dialog->selectFile(exportFilename);
    QLOG_DEBUG() << "Suggested Export Filename: " << exportFilename;
    dialog->open(this, SLOT(exportDialogAccepted()));
}

void LogAnalysis::loadSettings()
{
    QSettings settings;
    settings.beginGroup("LOGANALYSIS_SETTINGS");
    ui.modeDisplayCheckBox->setChecked(settings.value("SHOW_MODE", Qt::Checked).toBool());
    ui.errDisplayCheckBox->setChecked(settings.value("SHOW_ERR", Qt::Checked).toBool());
    ui.evDisplayCheckBox->setChecked(settings.value("SHOW_EV", Qt::Checked).toBool());
    ui.msgDisplayCheckBox->setChecked(settings.value("SHOW_MSG", Qt::Checked).toBool());
    ui.hideTableWidgetCheckBox->setChecked(settings.value("HIDE_TABLE_VIEW", Qt::Checked).toBool());
    ui.showValuesCheckBox->setChecked(settings.value("SHOW_VALUES", Qt::Unchecked).toBool());
    settings.endGroup();
}

void LogAnalysis::saveSettings()
{
    QSettings settings;
    settings.beginGroup("LOGANALYSIS_SETTINGS");
    settings.setValue("SHOW_MODE", ui.modeDisplayCheckBox->isChecked());
    settings.setValue("SHOW_ERR", ui.errDisplayCheckBox->isChecked());
    settings.setValue("SHOW_EV", ui.evDisplayCheckBox->isChecked());
    settings.setValue("SHOW_MSG", ui.msgDisplayCheckBox->isChecked());
    settings.setValue("HIDE_TABLE_VIEW", ui.hideTableWidgetCheckBox->isChecked());
    settings.setValue("SHOW_VALUES", ui.showValuesCheckBox->isChecked());
    settings.endGroup();
}

void LogAnalysis::hideTableView(bool hide)
{
    if (hide)
    {
        ui.splitter->setSizes(QList<int>() << 1 << 0);
        ui.filterShowPushButton->setVisible(false);
    }
    else
    {
        ui.splitter->setSizes(QList<int>() << 1 << 1);
        ui.filterShowPushButton->setVisible(true);
    }
}

void LogAnalysis::showValueUnderMouseClicked(bool checked)
{
    if(checked)
    {
        connect(m_plotPtr.data(), SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(plotMouseMove(QMouseEvent*)));
    }
    else
    {
        disconnect(m_plotPtr.data(), SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(plotMouseMove(QMouseEvent*)));
    }
}


void LogAnalysis::logLoadingStarted()
{
    m_loadProgressDialog.reset(new QProgressDialog("Loading File", "Cancel", 0, 100, 0));
    m_loadProgressDialog->setWindowModality(Qt::WindowModal);
    connect(m_loadProgressDialog.data(), SIGNAL(canceled()), this, SLOT(logLoadingProgressDialogCanceled()));
    m_loadProgressDialog->show();
}

void LogAnalysis::logLoadingProgress(qint64 pos, qint64 size)
{
    if (m_loadProgressDialog)
    {
        double tempProgress = (static_cast<double>(pos) / static_cast<double>(size)) * 100.0;
        m_loadProgressDialog->setValue(static_cast<int>(tempProgress));
    }
}

void LogAnalysis::logLoadingError(QString errorstr)
{
    QLOG_ERROR() << "LogAnalysis::logLoadingError - Log loading stopped with error:" << errorstr;
    QMessageBox::warning(this ,"Error", errorstr);
    this->close();
}

void LogAnalysis::logLoadingDone(AP2DataPlotStatus status)
{
    QLOG_DEBUG() << "LogAnalysis::logLoadingDone - Log loading is done.";
    m_loadedLogMavType = status.getMavType();

    // close progress window
    m_loadProgressDialog->close();
    m_loadProgressDialog.reset();

    // status handling
    if (status.getParsingState() != AP2DataPlotStatus::OK)
    {
        QString infoString;
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setWindowTitle("Warning");
        msgBox.setText("Log parsing ended with errors.");
        msgBox.addButton(QMessageBox::Ok);

        if (status.getParsingState() == AP2DataPlotStatus::FmtError)
        {
            infoString = "There were errors only in format discription. Usually this is no problem.\n\r";
        }
        else if (status.getParsingState() == AP2DataPlotStatus::TruncationError)
        {
            infoString = "The data was truncated!\n\r";
        }
        else if (status.getParsingState() == AP2DataPlotStatus::TimeError)
        {
            infoString = "The time data was currupted. Maybe plotting by time may not work.\n\r";
        }
        else
        {
            infoString = "There were data errors / unreadable data in the log! The data is potentially corrupt and incorrect.\r";
        }

        QLOG_WARN() << infoString << status.getErrorOverview();
        msgBox.setInformativeText(infoString + "Detected:\r" + status.getErrorOverview());
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.setDetailedText(status.getDetailedErrorText());
        msgBox.exec();
    }

    // First setup X-axis and scroller
    setupXAxisAndScroller();

    // Insert data into tree view suppressing all measurements containing strings as values
    fmtMapType fmtMap = m_dataStoragePtr->getFmtValues(true);
    for (fmtMapType::const_iterator iter = fmtMap.constBegin(); iter != fmtMap.constEnd(); ++iter)
    {
        QString name = iter.key();
        for (int i = 0; i < iter.value().size(); ++i)
        {
            ui.dataSelectionScreen->addItem(name + "." + iter.value().at(i));
        }
    }
    // and connect the signals for enabling and disabling
    connect(ui.dataSelectionScreen, SIGNAL(itemEnabled(QString)), this, SLOT(itemEnabled(QString)));
    connect(ui.dataSelectionScreen, SIGNAL(itemDisabled(QString)), this, SLOT(itemDisabled(QString)));

    // Insert data into filter window including all measurements with string data
    fmtMap.clear();
    fmtMap = m_dataStoragePtr->getFmtValues(false);
    for (fmtMapType::const_iterator iter = fmtMap.constBegin(); iter != fmtMap.constEnd(); ++iter)
    {
        QTreeWidgetItem *child = new QTreeWidgetItem(QStringList() << iter.key());
        child->setFlags(child->flags() | Qt::ItemIsUserCheckable);
        child->setCheckState(0, Qt::Checked); // Set it checked, since all items are enabled by default
        ui.filterSelectTreeWidget->addTopLevelItem(child);
        m_tableFilterList.append(iter.key());
    }

    // create an invisible y-axis for the text arrows
    m_arrowGraph.p_yAxis = m_plotPtr->axisRect()->addAxis(QCPAxis::atLeft);
    m_arrowGraph.p_yAxis->setVisible(false);
    m_arrowGraph.p_yAxis->setRangeUpper(8.0);
    // Load MODE, ERR, EV, MSG messages from datamodel
    m_dataStoragePtr->getMessagesOfType(ModeMessage::TypeName, m_indexToMessageMap);
    m_dataStoragePtr->getMessagesOfType(ErrorMessage::TypeName, m_indexToMessageMap);
    m_dataStoragePtr->getMessagesOfType(EventMessage::TypeName, m_indexToMessageMap);
    m_dataStoragePtr->getMessagesOfType(MsgMessage::TypeName, m_indexToMessageMap);
    // and insert them into the graph
    insertTextArrows();

    // connect scroll bars
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
    connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    connect(m_plotPtr->axisRect()->axis(QCPAxis::atBottom), SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    // set range -> whole graph should be viewable therefore 20 percent offset on both sides
    qint64 offset = (m_scrollEndIndex - m_scrollStartIndex) / 20;
    m_plotPtr->axisRect()->axis(QCPAxis::atBottom)->setRange(m_scrollStartIndex - offset, m_scrollEndIndex + offset);
    ui.verticalScrollBar->setValue(ui.verticalScrollBar->maximum());

    // Set up proxy for table filtering
    mp_tableFilterProxyModel = new QSortFilterProxyModel(this);     // will be deleted upon destruction of "this"
    mp_tableFilterProxyModel->setSourceModel(m_dataStoragePtr.data());
    ui.tableWidget->setModel(mp_tableFilterProxyModel);
    connect(ui.tableWidget->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(selectedRowChanged(QModelIndex, QModelIndex)));

    // Graph is loaded - Setup ui. Connect mouseMove signal according to checkbox state
    if(ui.showValuesCheckBox->isChecked())
    {
        connect(m_plotPtr.data(), SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(plotMouseMove(QMouseEvent*)));
    }

    // Show the table according to checkbox state
    if(!ui.hideTableWidgetCheckBox->isChecked())  hideTableView(false);

    // Enable only the layers that are enabled by their checkbox
    if(ui.modeDisplayCheckBox->isChecked()) m_plotPtr->layer(ModeMessage::TypeName)->setVisible(true);
    if(ui.evDisplayCheckBox->isChecked())   m_plotPtr->layer(EventMessage::TypeName)->setVisible(true);
    if(ui.errDisplayCheckBox->isChecked())  m_plotPtr->layer(ErrorMessage::TypeName)->setVisible(true);
    if(ui.msgDisplayCheckBox->isChecked())  m_plotPtr->layer(MsgMessage::TypeName)->setVisible(true);
    m_plotPtr->layer(s_CursorLayerName)->setVisible(true);  // cursor layer is always visible

    // final rescaling and plotting.
    m_plotPtr->rescaleAxes(false);
    m_plotPtr->replot();
}

void LogAnalysis::logLoadingThreadTerminated()
{
    QLOG_DEBUG() << "LogAnalysis::logLoadingThreadTerminated.";
    m_loaderThreadPtr.reset();
}

void LogAnalysis::logLoadingProgressDialogCanceled()
{
    QLOG_DEBUG() << "LogAnalysis::logLoadingProgressDialogCanceled.";
    if(m_loaderThreadPtr)
    {
        m_loaderThreadPtr->stopLoad();
    }
}

void LogAnalysis::modeCheckboxClicked(bool checked)
{
    m_plotPtr->layer(ModeMessage::TypeName)->setVisible(checked);
    m_plotPtr->replot();
}

void LogAnalysis::errorCheckboxClicked(bool checked)
{
    m_plotPtr->layer(ErrorMessage::TypeName)->setVisible(checked);
    m_plotPtr->replot();
}

void LogAnalysis::eventCheckboxClicked(bool checked)
{
    m_plotPtr->layer(EventMessage::TypeName)->setVisible(checked);
    m_plotPtr->replot();
}

void LogAnalysis::msgCheckboxClicked(bool checked)
{
    m_plotPtr->layer(MsgMessage::TypeName)->setVisible(checked);
    m_plotPtr->replot();
}

void LogAnalysis::verticalScrollMoved(int value)
{
    QCPAxis *xAxis = m_plotPtr->axisRect()->axis(QCPAxis::atBottom);
    double percent = value / 100.0;
    double center = xAxis->range().center();
    double requestedrange = (m_scrollEndIndex - m_scrollStartIndex) * percent;
    xAxis->setRangeUpper(center + (requestedrange/2.0));
    xAxis->setRangeLower(center - (requestedrange/2.0));
    m_plotPtr->replot();
}

void LogAnalysis::horizontalScrollMoved(int value)
{
    if (value != m_lastHorizontalScrollVal)
    {
        QCPAxis *xAxis = m_plotPtr->axisRect()->axis(QCPAxis::atBottom);
        m_lastHorizontalScrollVal = value;
        disconnect(xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
        xAxis->setRange(value, xAxis->range().size(), Qt::AlignCenter);
        m_plotPtr->replot();
        connect(xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(xAxisChanged(QCPRange)));
    }
}

void LogAnalysis::xAxisChanged(QCPRange range)
{
    disconnect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    disconnect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));

    ui.horizontalScrollBar->setValue(qRound(range.center())); // adjust position of scroll bar slider
    ui.horizontalScrollBar->setPageStep(qRound(range.size())); // adjust size of scroll bar slider
    double totalrange = m_scrollEndIndex - m_scrollStartIndex;
    double currentrange = range.upper - range.lower;
    ui.verticalScrollBar->setValue(static_cast<int>(100.0 * (currentrange / totalrange)));

    connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
}

void LogAnalysis::itemEnabled(QString name)
{
    QVector<double> xlist;
    QVector<double> ylist;
    if (!m_dataStoragePtr->getValues(name, m_useTimeOnXAxis, xlist, ylist))
    {
        //No values!
        QLOG_WARN() << "No values in datamodel for " << name;
        ui.dataSelectionScreen->disableItem(name);
        return;
    }

    m_plotPtr->setCurrentLayer("main");     // All plots are on main layer
    QCPAxisRect *axisRect = m_plotPtr->axisRect();

    // use golden ratio for evenly distributed colors
    double golden_ratio_conjugate = 0.618033988749895;
    double h = (static_cast<double>(rand()) / RAND_MAX);
    h = h + golden_ratio_conjugate;
    h = fmod(h, 1);     // hue
    double s = 0.75;    // saturation
    double v = 0.8516;  // value
    QColor color = QColor::fromHsvF(h, s, v);

    GraphElements newPlot;
    QCPAxis *paxis = new LogAnalysisAxis(axisRect, QCPAxis::atLeft);
    newPlot.p_yAxis = axisRect->addAxis(QCPAxis::atLeft, paxis);
    newPlot.p_yAxis->setLabel(name);
    newPlot.p_yAxis->setNumberFormat("gb");
    newPlot.p_yAxis->setLabelColor(color);
    newPlot.p_yAxis->setTickLabelColor(color);
    newPlot.p_yAxis->setSelectableParts(QCPAxis::spAxis);

    newPlot.p_graph = m_plotPtr->addGraph(axisRect->axis(QCPAxis::atBottom), newPlot.p_yAxis);
    newPlot.p_graph->setPen(QPen(color, 1));
    newPlot.p_graph->setData(xlist, ylist);
    newPlot.p_graph->rescaleValueAxis();

    m_activeGraphs[name] = newPlot;     // store the plot by name
    // Add to gouping dialog
    m_axisGroupingDialog->addAxis(name, newPlot.p_yAxis->range().lower, newPlot.p_yAxis->range().upper, color);
    // if cursors are present call range change to update tool tip values
    if(mp_cursorLeft && mp_cursorRight)
    {
        cursorRangeChange();
    }

    // Enable / disable grid - Enabled as long as only one line graph is visible
    // As soon as there are more than one line graph the grid is removed
    int activeAxes = axisRect->axisCount(QCPAxis::atLeft);
    if(activeAxes == 2)   // 1st axis is for the text arrows, 2nd for the first line graph
    {
        newPlot.p_yAxis->grid()->setVisible(true);
    }
    else if(activeAxes > 2)
    {
        QCPAxis *yAxis = m_plotPtr->axisRect()->axis(QCPAxis::atLeft, 1);
        yAxis->grid()->setVisible(false);
//        connect(yAxis, SIGNAL(rangeChanged(QCPRange)), newPlot.p_yAxis, SLOT(setRange(QCPRange)));    // TODO really not needed???
    }

    m_plotPtr->replot();
}

void LogAnalysis::itemDisabled(QString name)
{
    if (m_activeGraphs.contains(name)) // only enabled items can be disabled
    {
        // remove axis, graph and stored info from m_activeGraphs
        m_plotPtr->axisRect()->removeAxis(m_activeGraphs.value(name).p_yAxis);
        m_plotPtr->removeGraph(m_activeGraphs.value(name).p_graph);
        m_activeGraphs.remove(name);

        // if cursors are present call range change to update tool tip values
        if(mp_cursorLeft && mp_cursorRight)
        {
            cursorRangeChange();
        }

        // Add grid if only one line graph is left
        if(m_plotPtr->axisRect()->axisCount(QCPAxis::atLeft) == 2)   // 1st axis is for the text arrows, 2nd for the first line graph
        {
            QCPAxis *yAxis = m_plotPtr->axisRect()->axis(QCPAxis::atLeft, 1);
            yAxis->grid()->setVisible(true);
        }

        // remove from group dialog
        m_axisGroupingDialog->removeAxis(name);
        m_plotPtr->replot();
    }
}

void LogAnalysis::indexTypeCheckBoxClicked(bool checked)
{
    if (m_useTimeOnXAxis != checked)
    {
        m_useTimeOnXAxis = checked;
        // We have to remove all graphs when changing x-axis storing the active selection
        QList<QString> reEnableList = ui.dataSelectionScreen->disableAllItems();

        // remove the text arrows and cursors
        m_plotPtr->clearItems();
        mp_cursorSimple = 0;      // clearItems() deletes the cursors!
        mp_cursorLeft = 0;        // clearItems() deletes the cursors!
        mp_cursorRight = 0;       // clearItems() deletes the cursors!
        m_cursorXAxisRange = 0.0; // no cursor no range

        // arrows can be inserted instantly again
        m_statusTextPos = 0;    // reset text arrow length
        insertTextArrows();

        // Graphs can be reenabled using previous stored selection
        ui.dataSelectionScreen->enableItemList(reEnableList);

        // Re -set x axis, scroller and zoom
        disconnect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
        disconnect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));

        setupXAxisAndScroller();
        m_plotPtr->axisRect()->axis(QCPAxis::atBottom)->setRange(m_scrollStartIndex, m_scrollEndIndex);
        m_plotPtr->replot();
        ui.verticalScrollBar->setValue(ui.verticalScrollBar->maximum());

        connect(ui.verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(verticalScrollMoved(int)));
        connect(ui.horizontalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(horizontalScrollMoved(int)));
    }
}

void LogAnalysis::filterItemChanged(QTreeWidgetItem* item, int col)
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

void LogAnalysis::filterAcceptClicked()
{
    QString sortstring = "";
    // All elements selected -> filter is disabled
    if (ui.filterSelectTreeWidget->topLevelItemCount() == m_tableFilterList.size())
    {
        disableTableFilter();
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
        mp_tableFilterProxyModel->setFilterRegExp(sortstring);
        mp_tableFilterProxyModel->setFilterRole(Qt::DisplayRole);
        mp_tableFilterProxyModel->setFilterKeyColumn(1);
    }

    ui.tableFilterGroupBox->setVisible(false);
    ui.filterShowPushButton->setText("Show Filter");
}

void LogAnalysis::filterCancelClicked()
{
    ui.tableFilterGroupBox->setVisible(false);
    ui.filterShowPushButton->setText("Show Filter");
}

void LogAnalysis::showFilterButtonClicked()
{
    if (ui.tableFilterGroupBox->isVisible())
    {
        ui.tableFilterGroupBox->setVisible(false);
        ui.filterShowPushButton->setText("Show Filter");
    }
    else
    {
        ui.tableFilterGroupBox->setVisible(true);
        ui.filterShowPushButton->setText("Hide Filter");
    }
}

void LogAnalysis::filterSelectAllClicked()
{
    for (int i = 0; i < ui.filterSelectTreeWidget->topLevelItemCount(); ++i)
    {
        if (ui.filterSelectTreeWidget->topLevelItem(i)->checkState(0) != Qt::Checked)
        {
            ui.filterSelectTreeWidget->topLevelItem(i)->setCheckState(0, Qt::Checked);
        }
    }
}

void LogAnalysis::filterSelectInvertClicked()
{
    for (int i = 0; i < ui.filterSelectTreeWidget->topLevelItemCount(); ++i)
    {
        if (ui.filterSelectTreeWidget->topLevelItem(i)->checkState(0) == Qt::Checked)
        {
            ui.filterSelectTreeWidget->topLevelItem(i)->setCheckState(0,Qt::Unchecked);
        }
        else
        {
            ui.filterSelectTreeWidget->topLevelItem(i)->setCheckState(0,Qt::Checked);
        }
    }
}

void LogAnalysis::selectedRowChanged(QModelIndex current, QModelIndex previous)
{
    Q_UNUSED(previous);
    if (!current.isValid())
    {
        return;
    }
    qint64 index =  mp_tableFilterProxyModel->mapToSource(current).row();

    if(mp_cursorSimple)
    {
        if (m_useTimeOnXAxis)
        {
            // timestamp value of the current row is in colum 2
            double item = ui.tableWidget->model()->itemData(ui.tableWidget->model()->index(current.row(), 2)).value(Qt::DisplayRole).toInt();
            mp_cursorSimple->setCurrentXPos(item / m_dataStoragePtr->getTimeDivisor());
        }
        else
        {
            mp_cursorSimple->setCurrentXPos(index);
        }
    }

    m_dataStoragePtr->selectedRowChanged(mp_tableFilterProxyModel->mapToSource(current));

    // TODO check if we want to handle a context menu on table?!
}

void LogAnalysis::exportAsciiLogClicked()
{
    doExport(false);
}

void LogAnalysis::exportKmlClicked()
{
    doExport(true);
}

void LogAnalysis::exportDialogAccepted()
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

    if(m_kmlExport)
    {
        KmlLogExporter kmlExporter(this);
        kmlExporter.exportToFile(outputFileName, m_dataStoragePtr);
    }
    else
    {
        AsciiLogExporter asciiExporter(this);
        asciiExporter.exportToFile(outputFileName, m_dataStoragePtr);
    }

    QLOG_DEBUG() << "Log export took " << timer1.elapsed() << "ms";
}

void LogAnalysis::graphControlsButtonClicked()
{
    QHash<QString, GraphElements>::const_iterator iter;
    for (iter = m_activeGraphs.constBegin(); iter != m_activeGraphs.constEnd() ; ++iter)
    {
        m_axisGroupingDialog->fullAxisUpdate(iter.key(), iter.value().p_yAxis->range().lower, iter.value().p_yAxis->range().upper, iter.value().m_manualRange, iter.value().m_inGroup, iter.value().m_groupName);
    }

    m_axisGroupingDialog->show();
    m_axisGroupingDialog->activateWindow();
    m_axisGroupingDialog->raise();
}

void LogAnalysis::graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange> graphRangeList)
{
    // first reset all active elements to their default state
    QHash<QString, GraphElements>::iterator iter;
    for (iter = m_activeGraphs.begin(); iter != m_activeGraphs.end() ; ++iter)
    {
        if(iter->m_inGroup || iter->m_manualRange)
        {
            iter->m_inGroup = false;
            iter->m_manualRange = false;
            iter->m_groupName = QString();
        }
        iter->p_graph->rescaleValueAxis();
    }

    // Now sort all grouped items into a map, and all manuals into a vector
    QMap<QString, GroupElement> groupingMap;
    QVector<AP2DataPlotAxisDialog::GraphRange> manualRangeVec;
    foreach(const AP2DataPlotAxisDialog::GraphRange &range, graphRangeList)
    {
        if(range.isgrouped)
        {
            GroupElement &groupElement = groupingMap[range.group];
            groupElement.m_groupName = range.group;
            groupElement.m_upper = groupElement.m_upper < range.max ? range.max : groupElement.m_upper;
            groupElement.m_lower = groupElement.m_lower > range.min ? range.min : groupElement.m_lower;
            groupElement.m_graphList.push_back(range.graph);
        }
        else if(range.manual)
        {
            manualRangeVec.push_back(range);
        }
    }

    // set all grouped items to same scaling
    foreach(const GroupElement &element, groupingMap)
    {
        foreach(const QString &name, element.m_graphList)
        {
            if(m_activeGraphs.contains(name))
            {
                GraphElements &graph = m_activeGraphs[name];
                graph.p_yAxis->setRangeUpper(element.m_upper);
                graph.p_yAxis->setRangeLower(element.m_lower);
                graph.m_inGroup = true;
                graph.m_groupName = element.m_groupName;
            }
        }
    }

    // set all manual item to their individual scaling
    foreach(const AP2DataPlotAxisDialog::GraphRange &range, manualRangeVec)
    {
        if(m_activeGraphs.contains(range.graph))
        {
            GraphElements &graph = m_activeGraphs[range.graph];
            graph.p_yAxis->setRangeUpper(range.max);
            graph.p_yAxis->setRangeLower(range.min);
            graph.m_manualRange = true;
        }
    }

    m_plotPtr->replot();
}

void LogAnalysis::plotMouseMove(QMouseEvent *evt)
{
    bool insideCursorRange = false;
    QString out;
    QTextStream outStream(&out);

    double viewableRange = (m_plotPtr->xAxis->range().upper - m_plotPtr->xAxis->range().lower);
    double xValue = m_plotPtr->axisRect()->axis(QCPAxis::atBottom)->pixelToCoord(evt->x());
    double offset = viewableRange / 100;   // an offset of 1 percent
    if(m_useTimeOnXAxis)
    {
        outStream << "Time s:" << QString::number(xValue, 'f', 3);
    }
    else
    {
        outStream << "Line:" << QString::number(xValue, 'f', 0);
    }
    if(mp_cursorLeft && mp_cursorRight)
    {
        // the offset increases the range where we are inside the cursor range. If we don't do this
        // the tool tip will not show the range values when increasing cursor range. When dragging
        // the right cursor to the right the mouse leaves the range.
        if((mp_cursorLeft->getCurrentXPos() < xValue + offset) && (mp_cursorRight->getCurrentXPos() > xValue - offset))
        {
            outStream << " " << QChar(0x0394) << ":" << m_cursorXAxisRange;
            insideCursorRange = true;
        }
    }

    outStream.setRealNumberNotation(QTextStream::FixedNotation);
    QHash<QString, GraphElements>::Iterator iter;
    for(iter = m_activeGraphs.begin(); iter != m_activeGraphs.end(); ++iter)
    {
        outStream.setRealNumberPrecision(3);
        double key   = iter->p_graph->keyAxis()->pixelToCoord(evt->x());
        int keyIndex = iter->p_graph->findBegin(key);

        outStream << endl << iter.key();

        if(insideCursorRange && m_rangeValuesStorage.contains(iter.key()))
        {
            RangeValues &range = m_rangeValuesStorage[iter.key()];
            outStream << " min:" << range.m_min << " max:" << range.m_max << " " << QChar(0x0394) << ":" << range.m_max - range.m_min
                      << " avg:" << range.m_average;
        }

        outStream.setRealNumberPrecision(4);

        if(keyIndex)
        {
            outStream << " val:" << iter->p_graph->dataMainValue(keyIndex);
        }
        else
        {
            outStream << " val:" << iter.key() << "NONE";
        }
    }
    QToolTip::showText(QPoint(evt->globalPos().x() + m_plotPtr->x(), evt->globalPos().y() + m_plotPtr->y()), out);
}

void LogAnalysis::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    if(!mp_cursorSimple)
    {
        menu->addAction("Add simple cursor", this, SLOT(insertSimpleCursor()));
    }
    if(!mp_cursorLeft)
    {
        menu->addAction("Add range cursors", this, SLOT(insertRangeCursors()));
    }
    if(mp_cursorSimple)
    {
        menu->addAction("Remove simple cursor", this, SLOT(removeSimpleCursor()));
    }
    if(mp_cursorLeft || mp_cursorRight)
    {
        menu->addAction("Remove range cursors", this, SLOT(removeRangeCursors()));
    }

    menu->popup(m_plotPtr->mapToGlobal(pos));
}

void LogAnalysis::insertSimpleCursor()
{
    if(!mp_cursorSimple)
    {
        m_plotPtr->setCurrentLayer(s_CursorLayerName);
        // initial cursor pos is in the middle od the visible area
        double viewableRange = (m_plotPtr->xAxis->range().upper - m_plotPtr->xAxis->range().lower);
        double newCursorPos = m_plotPtr->xAxis->range().lower + (viewableRange / 2);
        mp_cursorSimple = new LogAnalysisCursor(m_plotPtr.data(), newCursorPos, LogAnalysisCursor::simple);    // QcustomPlot takes ownership
        mp_cursorSimple->setPen(QPen(QColor::fromRgb(0, 0, 255), 1));
        setTablePos(newCursorPos); // call once to set table view to initial cursor pos
        connect(mp_cursorSimple, SIGNAL(newCursorPos(double)), this, SLOT(setTablePos(double)));
        m_plotPtr->replot();
        // when cursors are inserted the check box should reflect this
        ui.tableCursorCheckBox->setCheckState(Qt::Checked);
    }
}

void LogAnalysis::insertRangeCursors()
{
    if(!mp_cursorLeft && !mp_cursorRight)
    {
        m_plotPtr->setCurrentLayer(s_CursorLayerName);
        // initial cursor pos is in the middle od the visible area
        double viewableRange = (m_plotPtr->xAxis->range().upper - m_plotPtr->xAxis->range().lower);
        double newCursorPos = m_plotPtr->xAxis->range().lower + (viewableRange / 2);
        // without offset both cursors are exactly at the same position - you have to drag one to see the other
        double offset = (m_plotPtr->xAxis->range().upper - m_plotPtr->xAxis->range().lower) / 20.0;
        mp_cursorLeft = new LogAnalysisCursor(m_plotPtr.data(), newCursorPos - offset, LogAnalysisCursor::left);   // QcustomPlot takes ownership
        mp_cursorLeft->setPen(QPen(QColor::fromRgb(255, 0, 0), 1));
        mp_cursorRight = new LogAnalysisCursor(m_plotPtr.data(), newCursorPos + offset, LogAnalysisCursor::right); // QcustomPlot takes ownership
        mp_cursorRight->setPen(QPen(QColor::fromRgb(0, 155, 0), 1));

        mp_cursorLeft->setOtherCursor(mp_cursorRight);
        mp_cursorRight->setOtherCursor(mp_cursorLeft);
        cursorRangeChange();    // call once to fill the range values
        connect(mp_cursorLeft, SIGNAL(cursorRangeChange()), this, SLOT(cursorRangeChange()));
        connect(mp_cursorRight, SIGNAL(cursorRangeChange()), this, SLOT(cursorRangeChange()));
        connect(mp_cursorLeft, SIGNAL(rangeCursorMoving()), this, SLOT(rangeCursorsMoving()));
        connect(mp_cursorRight, SIGNAL(rangeCursorMoving()), this, SLOT(rangeCursorsMoving()));
        m_plotPtr->replot();
        // when cursors are inserted the check box should reflect this
        ui.rangeCursorCheckBox->setCheckState(Qt::Checked);
    }
}

void LogAnalysis::removeSimpleCursor()
{
    if(mp_cursorSimple)
    {
        m_plotPtr->removeItem(mp_cursorSimple); // remove also deletes the pointer
        mp_cursorSimple = 0;
    }
    m_plotPtr->replot();
    // when cursors are removed the check box should reflect this
    ui.tableCursorCheckBox->setCheckState(Qt::Unchecked);
}

void LogAnalysis::removeRangeCursors()
{
    if(mp_cursorLeft)
    {
        m_plotPtr->removeItem(mp_cursorLeft); // remove also deletes the pointer
        mp_cursorLeft = 0;
    }
    if(mp_cursorRight)
    {
        m_plotPtr->removeItem(mp_cursorRight); // remove also deletes the pointer
        mp_cursorRight = 0;
    }
    m_cursorXAxisRange = 0.0;   // no cursor no range
    m_plotPtr->replot();
    // when cursors are removed the check box should reflect this
    ui.rangeCursorCheckBox->setCheckState(Qt::Unchecked);
}

void LogAnalysis::resetValueScaling()
{
    // reset all active elements to their default state
    QHash<QString, GraphElements>::iterator iter;
    for (iter = m_activeGraphs.begin(); iter != m_activeGraphs.end() ; ++iter)
    {
        if(iter->m_inGroup || iter->m_manualRange)
        {
            iter->m_inGroup = false;
            iter->m_manualRange = false;
            iter->m_groupName = QString();
        }
        iter->p_graph->rescaleValueAxis();
    }
    m_plotPtr->replot();
}

void LogAnalysis::enableRangeCursor(bool enable)
{
    if(enable)
    {
        insertRangeCursors();
    }
    else
    {
        removeRangeCursors();
    }
}

void LogAnalysis::enableTableCursor(bool enable)
{
    if(enable)
    {
        insertSimpleCursor();
    }
    else
    {
        removeSimpleCursor();
    }
}


