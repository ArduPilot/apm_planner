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
    explicit AP2DataPlotAxisDialog(QWidget *parent = 0);
    ~AP2DataPlotAxisDialog();
    void addAxis(QString name,double lower, double upper,QColor color);
    void fullAxisUpdate(QString name,double lower, double upper,bool ismanual, bool isingroup, QString groupname);
    void updateAxis(QString name,double lower, double upper);
    void removeAxis(QString name);
protected:
    void closeEvent(QCloseEvent *evt);
public slots:
    void setMinMaxButtonClicked();
    void autoButtonClicked(bool checked);
    void manualButtonClicked(bool checked);
private slots:
    void applyButtonClicked();
    void cancelButtonClicked();
    void groupComboChanged(int name);
    void cellDoubleClicked(int row,int col);
    void cellChanged(int row,int col);
signals:
    void graphAddedToGroup(QString name,QString group,double scale);
    void graphRemovedFromGroup(QString name);
    void graphManualRange(QString name, double min, double max);
    void graphAutoRange(QString name);
private:
    //Map of ranges for individual graphs
    QMap<QString,QPair<double,double> > m_rangeMap;
    QMap<QString,QPair<double,double> > m_graphRangeMap;
    QMap<QString,double> m_graphScaleMap;

    //Graph name to Group name map, for graphs that are in a group
    QMap<QString,QString > m_graphToGroupNameMap;

    Ui::AP2DataPlotAxisDialog *ui;
};

#endif // AP2DATAPLOTAXISDIALOG_H
