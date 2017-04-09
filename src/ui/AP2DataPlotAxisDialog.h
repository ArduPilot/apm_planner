#ifndef AP2DATAPLOTAXISDIALOG_H
#define AP2DATAPLOTAXISDIALOG_H

#include <QWidget>
#include <QMap>

class QCloseEvent;

namespace Ui {
class AP2DataPlotAxisDialog;
}
class QTableWidgetItem;

class AP2DataPlotAxisDialog : public QWidget
{
    Q_OBJECT
    
public:
    class GraphRange
    {
    public:
        QString graph;
        QString group;
        bool isgrouped;
        bool manual;
        double min;
        double max;

        GraphRange() : isgrouped(false), manual(false), min(0.0), max(0.0) {}
    };

    explicit AP2DataPlotAxisDialog(QWidget *parent = 0);
    ~AP2DataPlotAxisDialog();
    void addAxis(QString name,double lower, double upper,QColor color);
    void fullAxisUpdate(QString name,double lower, double upper,bool ismanual, bool isingroup, QString groupname);
    void updateAxis(QString name,double lower, double upper);
    void removeAxis(QString name);
    void clear();

public slots:
    void setMinMaxButtonClicked();

signals:
    void graphAddedToGroup(QString name,QString group,double scale);
    void graphRemovedFromGroup(QString name);
    void graphManualRange(QString name, double min, double max);
    void graphAutoRange(QString name);
    void graphGroupingChanged(QList<AP2DataPlotAxisDialog::GraphRange> graphRangeList);
    void graphColorsChanged(QMap<QString,QColor> colorlist);

private slots:
    void applyButtonClicked();
    void clearButtonClicked();
    void cancelButtonClicked();
    void groupComboChanged(int name);
    void cellDoubleClicked(int row,int col);
    void cellChanged(int row,int col);
    void colorDialogAccepted();
    void autoCheckboxChecked(bool checked);

protected:
    void closeEvent(QCloseEvent *evt);

private:
    //Map of ranges for individual graphs
    QMap<QString,QPair<double,double> > m_rangeMap;
    QMap<QString,QPair<double,double> > m_graphRangeMap;
    QMap<QString,double> m_graphScaleMap;

    //Row of the current color edit
    int m_colorDialogRowId;

    //Graph name to Group name map, for graphs that are in a group
    QMap<QString,QString > m_graphToGroupNameMap;

    Ui::AP2DataPlotAxisDialog *ui;
};

#endif // AP2DATAPLOTAXISDIALOG_H
