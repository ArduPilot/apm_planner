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
 * @file LogAnalysis.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @author Michael Carpenter <malcom2073@gmail.com>
 * @date 17 Mrz 2017
 * @brief File providing header for the log analysing classes
 */

#ifndef LOGANALYSIS_HPP
#define LOGANALYSIS_HPP

#include <QObject>
#include <QWidget>
#include <QScopedPointer>
#include <QMap>
#include <QString>
#include <QStringList>

#include "qcustomplot.h"

#include "LogdataStorage.h"
#include "AP2DataPlotThread.h"
#include "AP2DataPlotStatus.h"
#include "AP2DataPlotAxisDialog.h"
#include "common/common.h"
#include "ui_LogAnalysis.h"


/**
 * @brief The LogAnalysisCursor class defines a cursor line (vertical selectable, movable line in plot).
 *        It supports 3 types of cursors:
 *        @li simple - One cursor in plot, emits newCursorPos() signal on end of dragging.
 *        @li left / right - Two cursors in plot, right can not be dragged over left and left can not be
 *                           dragged over right. Whenever one of them is dragged it emits cursorRangeChange()
 *                           signal on end of dragging. Used to mark a range.
 */
class LogAnalysisCursor : public QCPItemStraightLine
{
    Q_OBJECT

public:
    /**
     * @brief The CursorType enum defines the cursor type. Simple has no restrictions,
     *        left and right hold their position. left is always left and right is always right.
     */
    enum CursorType
    {
        simple,     /// Simple cursor - one cursor in plot
        left,       /// Range cursor - two cursors in plot - the left one
        right       /// Range cursor - two cursors in plot - the right one
    };

    /**
     * @brief LogAnalysisCursor - CTOR.
     * @param parentPlot - Pointer to parent QCustomPlot instance, which takes the ownership.
     * @param xPosition - Position on x axis where cursor shall be created.
     * @param type - One of the 3 possible cursor type (simple, left, right).
     */
    explicit LogAnalysisCursor(QCustomPlot *parentPlot, double xPosition, CursorType type);

    /**
     * @brief ~LogAnalysisCursor - DTOR
     */
    virtual ~LogAnalysisCursor();

    /**
     * @brief setOtherCursor - Used to set the the other cursor when using cursors of type left
     *                         or right. The pointer is needed to fetch the positon of the other
     *                         cursor to avoid dragging over each other.
     * @param pCursor - Pointer to the other cursor.
     */
    virtual void setOtherCursor(LogAnalysisCursor *pCursor);

    /**
     * @brief getCurrentXPos - returns the current position on x axis of the cursor.
     * @return - Current position on x axis.
     */
    virtual double getCurrentXPos() const;

    /**
     * @brief setCurrentXPos - Sets the positon of the cursor on the x axis.
     * @param xPosition - X pos to move the cursor to.
     */
    virtual void setCurrentXPos(double xPosition);

signals:
    /**
     * @brief newCursorPos - Signal emitted by a "simple" cursor at the end of dragging it somewhere.
     * @param xPosition - The new positon the cursor is dragged to.
     */
    void newCursorPos(double xPosition);

    /**
     * @brief rangeCursorMoving - Signal is emitted while one of the range cursors is moved by the user
     * @param xPosition - the new position of the cursor
     */
    void rangeCursorMoving();

    /**
     * @brief cursorRangeChange - Signal emitted by a cursor pair at the end of dragging one of them to
     *                            a new position.
     */
    void cursorRangeChange();

protected:
    // Methods which could be overwritten - not used at the moment
    // virtual void selectEvent(QMouseEvent *event, bool additive, const QVariant &details, bool *selectionStateChanged);
    // virtual void deselectEvent(bool *selectionStateChanged);
    // virtual double selectTest(const QPointF &pos, bool onlySelectable, QVariant *details = 0)	const;

    // Used methods - See QCustomPlot::QCPItemStraightLine for further description
    virtual void mousePressEvent(QMouseEvent *event, const QVariant &details);
    virtual void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
    virtual void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);
    virtual void mouseDoubleClickEvent(QMouseEvent *event, const QVariant &details);
    virtual void wheelEvent(QWheelEvent *event);

private:
    double m_currentPos;                    /// Current cursor position on x axis
    double m_otherCursorPos;                /// Position of the other cursor when using range cursors.
    CursorType m_type;                      /// Type of the cursor
    QCPItemStraightLine *mp_otherCursor;    /// Pointer to the other cursor when using range cursors.
};

//************************************************************************************************************

/**
 * @brief The LogAnalysisAxis class is a QCPAxis which implements the wheelEvent() method. All y axis in a
 *        plot are of this type to allow proper mouse wheel scaling if selected
 */
class LogAnalysisAxis : public QCPAxis
{
public:
    /**
     * @brief LogAnalysisAxis - CTOR
     * @param axisRect - Pointer to the AxisRect where the axis shall reside.
     * @param type - Type of the axis
     */
    explicit LogAnalysisAxis(QCPAxisRect *axisRect, AxisType type);

    /**
     * @brief ~LogAnalysisAxis - DTOR
     */
    virtual ~LogAnalysisAxis();

    /**
     * @brief wheelEvent handler for mouse wheel events over this axis
     * @param event - the wheel event
     */
    virtual void wheelEvent(QWheelEvent *event);
};


//************************************************************************************************************

/**
 * @brief The LogAnalysis class provides everything which is needed for offline log analysis. After a call
 *        to loadLog it opens and parses the log, stores it in a datastorage and opens a GraphView window
 *        where several data analysis can be done.
 */
class LogAnalysis : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief LogAnalysis - CTOR
     * @param parent - parent object
     */
    explicit LogAnalysis(QWidget *parent = 0);

    /**
     * @brief LogAnalysis - DTOR
     *
     */
    ~LogAnalysis();

    /**
     * @brief loadLog - starts the logloading and parsing. After successful loading a new window is opened
     *                  where the data can be plotted and analyzed.
     * @param filename - filename of the log to load
     */
    void loadLog(QString filename);

public slots:
    /**
     * @brief setTablePos - scrolls the table view to the measurement which conatins the measures referred by
     *                      xPosition, which is normally a position in the plotting window.
     * @param xPosition - Position to scroll to. Uses time or index dependent of the m_useTimeOnXAxis setting.
     */
    void setTablePos(double xPosition);

    /**
     * @brief rangeCursorsMoving - calculates the m_cursorXAxisRange. Used to update the range delta while
     *                             moving the cursors.
     */
    void rangeCursorsMoving();

    /**
     * @brief cursorRangeChange - should be called if the cursors change the range. It calculates additional
     *                            values like min/max/average for all visible graphs within the cursor range.
     */
    void cursorRangeChange();

private:

    static const QString s_CursorLayerName;     /// Name for the cursor layer

    static const int s_ROW_HEIGHT_PADDING = 3;  /// Number of additional pixels over font height for each row for the table view.
    static const int s_TextArrowPositions = 7;  /// Max number of different positions for the test arrows

    typedef QMap<QString, QStringList> fmtMapType;  /// type for handling fmt values from datamodel

    /**
     * @brief The GraphElements struct holds all needed information about an active graph
     */
    struct GraphElements
    {
        QCPAxis  *p_yAxis;     /// pointer to the y-Axis of this graph
        QCPGraph *p_graph;     /// pointer to the graph itself
        QString m_groupName;   /// name of the group the plot belongs to.
        bool m_manualRange;    /// has user defined scaling
        bool m_inGroup;        /// has group scaling

        GraphElements() : p_yAxis(0), p_graph(0), m_manualRange(false), m_inGroup(false) {}
    };

    /**
     * @brief The GroupElement struct is used for graph grouping. It holds the group name its range and a list of
     *        graph names which are in this group.
     */
    struct GroupElement
    {
        QString m_groupName;        /// group name
        double m_upper;             /// upper range value
        double m_lower;             /// lower range value
        QStringList m_graphList;    /// list of graph names beloging to this group

        GroupElement() : m_upper(0.0), m_lower(0.0) {}
    };

    /**
     * @brief The RangeValues struct stores all additiona values calculated by cursorRangeChanged() method.
     */
    struct RangeValues
    {
        double m_min;           /// Min value in range
        double m_max;           /// Max value in range
        double m_average;       /// Average value in range
        int    m_measurements;  /// Number of measurements in range

        RangeValues() : m_min(std::numeric_limits<double>::max()), m_max(m_min * -1), m_average(0.0), m_measurements(0) {}
    };

    Ui::LogAnalysis ui;     /// The user interface

    QScopedPointer<QCustomPlot>  m_plotPtr;            /// Scoped pointer to QCustomplot
    LogdataStorage::Ptr          m_dataStoragePtr;     /// Shared pointer to data storage

    QScopedPointer<AP2DataPlotThread, QScopedPointerDeleteLater> m_loaderThreadPtr;        /// Scoped pointer to AP2DataPlotThread
    QScopedPointer<QProgressDialog, QScopedPointerDeleteLater>   m_loadProgressDialog;     /// Scoped pointer to load progress window
    QScopedPointer<AP2DataPlotAxisDialog, QScopedPointerDeleteLater> m_axisGroupingDialog; /// Scoped pointer to axis grouping dialog

    QHash<QString, GraphElements> m_activeGraphs;           /// Holds all active graphs
    QHash<QString, RangeValues> m_rangeValuesStorage;       /// If there is a range cursor the range values are stored here.
    QMap<quint64, MessageBase::Ptr> m_indexToMessageMap;    /// Map holding all Messages which are printed as arrows
    GraphElements m_arrowGraph;                             /// The text arrows have an own graph

    QStringList m_tableFilterList;   /// Used to create regex filter pattern for table view.

    QSortFilterProxyModel *mp_tableFilterProxyModel;    /// Filter model for table view.

    QString m_filename;              /// Filename of the loaded Log - mainly used for export

    MAV_TYPE m_loadedLogMavType;     /// Holds the mav type of the loaded log

    bool m_useTimeOnXAxis;           /// true if x axis holds time values
    qint64 m_scrollStartIndex;       /// Current start index of the loaded log
    qint64 m_scrollEndIndex;         /// Current end index of the loaded log
    int    m_statusTextPos;          /// Counter for giving text arrows a different length
    int m_lastHorizontalScrollVal;   /// Stores the last value of the horizontal scroller
    bool m_kmlExport;                /// True if export shall be in kml (google earth) format
    double m_cursorXAxisRange;       /// X axis range of the cursors.

    LogAnalysisCursor *mp_cursorSimple;     /// Pointer to the simple cursor only valid if visible
    LogAnalysisCursor *mp_cursorLeft;       /// Pointer to the left cursor only valid if visible
    LogAnalysisCursor *mp_cursorRight;      /// Pointer to the right cursor only valid if visible

    /**
     * @brief setupXAxisAndScroller sets up x axis and the horizontal scroller
     *        to use the normal index (the artifical) or the time index regarding
     *        to the value of m_useTimeOnXAxis.
     */
    void setupXAxisAndScroller();

    /**
     * @brief insertTextArrows inserts messages stored in m_indexToMessageMap
     *        as text arrows into the graph
     *        Uses normal or time index regarding of the value of m_useTimeOnX
     */
    void insertTextArrows();

    /**
     * @brief plotTextArrow - plots a text arrow into the Graph
     * @param index - position on x axis
     * @param text - the text which shall be printed above the arrow
     * @param layerName - name of the layer the arrow shall be plotted into
     * @param color - the color of the arrow and its text
     */
    void plotTextArrow(double index, const QString &text, const QString &layerName, const QColor &color);

    /**
     * @brief This method disables the filtering of m_tableFilterProxyModel
     *        After a call the table model will show all rows again.
     */
    void disableTableFilter();

    /**
     * @brief doExport - Starts an export eg. opens the file dialog to select
     *        the path to export to.
     * @param kmlExport  @li true - do KML Export (google earth)
     *                   @li false - do ascii export (simple ascii log, csv style)
     */
    void doExport(bool kmlExport);

    /**
     * @brief loadSettings - loads the settings for the log analysis window.
     *                       At the moment this means the state of the checkboxes
     */
    void loadSettings();

    /**
     * @brief saveSettings - saves the settings for the log analysis window.
     *                       At the moment this means the state of the checkboxes
     */
    void saveSettings();

private slots:

    /**
     * @brief hideTableView - hides / shows the table view
     * @param hide - true: hide table view, false: show table view
     */
    void hideTableView(bool hide);

    /**
     * @brief showValueUnderMouseClicked - handles the clicked signal of the "show vaules under mouse"
     *        checkbox. Connects / disconnects the mouse moved signal of qCustomPlot.
     * @param checked - reflects the state of the checkbox.
     */
    void showValueUnderMouseClicked(bool checked);

    /**
     * @brief logLoadingStarted - creates the Progress window
     */
    void logLoadingStarted();

    /**
     * @brief logLoadingProgress - sets the progressbar to actual
     *        loading position.
     * @param pos - Position of log loading.
     * @param size - Size of data to be loaded.
     */
    void logLoadingProgress(qint64 pos, qint64 size);

    /**
     * @brief logLoadingError - to be called on loag loading error.
     *        logs the "errorstr" to log and opens an Info dialog.
     * @param errorstr - The error string to be printed.
     */
    void logLoadingError(QString errorstr);

    /**
     * @brief logLoadingDone - does the basic setup after a successful
     *        log loading. It analyzes the AP2DataPlotStatus and shows an
     *        info box if neede. Moreover it populates all initial filled
     *        datafields (like table or filter view) and connects their signals.
     * @param status - status of the parsing.
     */
    void logLoadingDone(AP2DataPlotStatus status);

    /**
     * @brief logLoadingThreadTerminated - shall be called as soon as the log loading
     *        thread terminates. Resets the pointer to the thread object.
     */
    void logLoadingThreadTerminated();

    /**
     * @brief logLoadingProgressDialogCanceled - should be called if someone presses
     *        cancle in the progress dialog. Stops the Logloading.
     */
    void logLoadingProgressDialogCanceled();

    /**
     * @brief modeCheckboxClicked - to be called if someone clicks the Mode checkbox
     * @param checked - true if checked, false otherwise
     */
    void modeCheckboxClicked(bool checked);

    /**
     * @brief errorCheckboxClicked - @see modeCheckboxClicked(bool checked)
     */
    void errorCheckboxClicked(bool checked);

    /**
     * @brief eventCheckboxClicked - @see modeCheckboxClicked(bool checked)
     */
    void eventCheckboxClicked(bool checked);

    /**
     * @brief msgCheckboxClicked - @see modeCheckboxClicked(bool checked)
     */
    void msgCheckboxClicked(bool checked);

    /**
     * @brief verticalScrollMoved - to be called if someone moves the vertical
     *        scroller.
     * @param value - the new value of the scroller
     */
    void verticalScrollMoved(int value);

    /**
     * @brief horizontalScrollMoved - @see verticalScrollMoved(int value)
     */
    void horizontalScrollMoved(int value);

    /**
     * @brief xAxisChanged - should be called whenever the x axis of the plot changes
     * @param range - visible range of x axis
     */
    void xAxisChanged(QCPRange range);

    /**
     * @brief itemEnabled - adds the item graph named "name" to the plot area.
     * @param name - the name of the data to be plotted.
     */
    void itemEnabled(QString name);

    /**
     * @brief itemDisabled - removes the item graph named "name" from plot area
     * @param name - the name of the item to be removed
     */
    void itemDisabled(QString name);

    /**
     * @brief indexTypeCheckBoxClicked - shall be called when the "use time on x-axis" checkbox
     *        is pressed. It changes the x axis scaling from index to time
     * @param checked - true checkbox is checked, false otherwise.
     */
    void indexTypeCheckBoxClicked(bool checked);

    /**
     * @brief filterItemChanged - adds or removes names from the filterlist. Shall be called
     *        whenever a check box is checked or unchecked in the sorting dialog.
     * @param item - the Item that was checked / unchecked
     * @param col - ignored
     */
    void filterItemChanged(QTreeWidgetItem* item,int col);

    /**
     * @brief filterAcceptClicked - sets the new filter for the table view.
     */
    void filterAcceptClicked();

    /**
     * @brief filterCancelClicked - just hides the filter dialog.
     */
    void filterCancelClicked();

    /**
     * @brief showFilterButtonClicked - opens / closes the filter dialog
     */
    void showFilterButtonClicked();

    /**
     * @brief filterSelectAllClicked - set all items checked in the filter dialog
     */
    void filterSelectAllClicked();

    /**
     * @brief filterSelectInvertClicked - inverts the selection in the filter dialog
     */
    void filterSelectInvertClicked();

    /**
     * @brief selectedRowChanged - tells the datamodel that the active row in table model
     *        has changed. Used to update the table header (show correct colum names) and
     *        the position of the simple cursor if present
     * @param current - the current row which is selected in table model
     * @param previous - unused
     */
    void selectedRowChanged(QModelIndex current, QModelIndex previous);

    /**
     * @brief exportTlogClicked - starts an ASCII log export, csv style
     */
    void exportAsciiLogClicked();

    /**
     * @brief exportKmlClicked - starts a KML log export, google earth
     */
    void exportKmlClicked();

    /**
     * @brief exportDialogAccepted - called as soon as a file to export to was
     *        selected. Starts the current export depending on m_kmlExport.
     */
    void exportDialogAccepted();

    /**
     * @brief graphControlsButtonClicked - opens the plot grouping dialog and sends
     *        the current group settings to the grouping dialog.
     */
    void graphControlsButtonClicked();

    /**
     * @brief graphGroupingChanged - handles the graphGroupingChanged() signal of the
     *        gouping dialog. It sets the scaling and grouping of all plots.
     * @param graphRangeList - the List of graphs with their scaling and grouping.
     */
    void graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange> graphRangeList);

    /**
     * @brief contextMenuRequest - handles the customContextMenuRequested() signal from
     *        qCustomPlot on right click. It handles the cursor enabling/disabling.
     * @param pos - Pos where the context menu opens. Used to place the cursors initially.
     */
    void contextMenuRequest(QPoint pos);

    /**
     * @brief insertSimpleCursor - paints the simple cursor and connects the required signals.
     */
    void insertSimpleCursor();

    /**
     * @brief insertRangeCursors - paints the range cursors and connects the required signals.
     */
    void insertRangeCursors();

    /**
     * @brief removeSimpleCursor - removes the simple cursor from graph.
     */
    void removeSimpleCursor();

    /**
     * @brief removeSimpleCursor - removes the range cursors from graph.
     */
    void removeRangeCursors();

    /**
     * @brief plotMouseMove - handles the mousMove signal of qCustomPlot. It shows a tooltip
     *        with several measurements.
     * @param evt - the mouse event.
     */
    void plotMouseMove(QMouseEvent *evt);

    /**
     * @brief resetValueScaling - resets the scaling of the graph it it were changed by
     *        mouse.
     */
    void resetValueScaling();

    /**
     * @brief enableRangeCursor - handles the range cursor checkbox click signal
     * @param enable - true checkbox is checked, false unchecked
     */
    void enableRangeCursor(bool enable);

    /**
     * @brief enableTableCursor - handles the table cursor checkbox click signal
     * @param enable - true checkbox is checked, false unchecked
     */
    void enableTableCursor(bool enable);

};

#endif // LOGANALYSIS_HPP

