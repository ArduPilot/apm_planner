#ifndef AP2DATAPLOT2D_H
#define AP2DATAPLOT2D_H

#include <QWidget>
#include <QProgressDialog>
#include "ui_AP2DataPlot2D.h"
#include "AP2DataPlotThread.h"
#include "dataselectionscreen.h"
#include "qcustomplot.h"
#include "UASInterface.h"
#include "MAVLinkDecoder.h"
#include "AP2DataPlotAxisDialog.h"
#include <QTextBrowser>



class AP2DataPlot2D : public QWidget
{
    Q_OBJECT
    
public:
    explicit AP2DataPlot2D(QWidget *parent = 0);
    ~AP2DataPlot2D();
    void addSource(MAVLinkDecoder *decoder);
private slots:
    //New Active UAS set
    void activeUASSet(UASInterface* uas);

    //Load a graph from a file
    void loadButtonClicked();

    //Graph loading thread started
    void loadStarted();
    //Progress of graph loading thread
    void loadProgress(qint64 pos,qint64 size);
    //Cancel clicked on the graph loading thread progress dialog
    void progressDialogCanceled();
    //Graph loading thread finished
    void threadDone();
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
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint8 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint8 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint16 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint16 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint32 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint32 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint64 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint64 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec);
    void valueChanged(const int uasid, const QString& name, const QString& unit, const QVariant value,const quint64 msecs);
    //Called by every valueChanged function to actually save the value/graph it.
    void updateValue(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec,bool integer = true);

    void autoScrollClicked(bool checked);
    void tableCellClicked(int row,int column);
    void logLine(QString line);
    void addGraphRight();
    void addGraphLeft();
    void removeGraphLeft();
    void axisDoubleClick(QCPAxis* axis,QCPAxis::SelectablePart part,QMouseEvent* evt);
    void graphAddedToGroup(QString name,QString group,double scale);
    void graphRemovedFromGroup(QString name);
    void graphManualRange(QString name, double min, double max);
    void graphAutoRange(QString name);
    void showOnlyClicked();
    void showAllClicked();
    void graphControlsButtonClicked();
    void plotMouseMove(QMouseEvent *evt);
private:
    class Graph
    {
    public:
        bool isManualRange;
        QString groupName;
        bool isInGroup;
        double axisIndex;
        QCPAxis *axis;
        QCPGraph *graph;
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

    QList<QPair<qint64,double> > m_onlineValueTimeoutList;

    //List of graph names, used in m_axisList, m_graphMap,m_graphToGroupMap and the like as the graph name
    QList<QString> m_graphNameList;
    int m_graphCount;
    QCustomPlot *m_plot;
    QCPAxisRect *m_wideAxisRect;
    Ui::AP2DataPlot2D ui;
    AP2DataPlotThread *m_logLoaderThread;
    DataSelectionScreen *m_dataSelectionScreen;
    bool m_logLoaded;
    //Current "index", X axis on graph. Used to keep all the graphs lined up.
    qint64 m_currentIndex;
    qint64 m_startIndex; //epoch msecs since graphing started
    QAction *m_addGraphAction;
    UASInterface *m_uas;
    QProgressDialog *m_progressDialog;
    AP2DataPlotAxisDialog *m_axisGroupingDialog;
    qint64 m_timeDiff;

};

#endif // AP2DATAPLOT2D_H
