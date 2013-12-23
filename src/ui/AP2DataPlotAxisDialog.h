#ifndef AP2DATAPLOTAXISDIALOG_H
#define AP2DATAPLOTAXISDIALOG_H

#include <QWidget>
#include <QMap>

class QCloseEvent;

namespace Ui {
class AP2DataPlotAxisDialog;
}

class AP2DataPlotAxisDialog : public QWidget
{
    Q_OBJECT
    
public:
    explicit AP2DataPlotAxisDialog(QWidget *parent = 0);
    ~AP2DataPlotAxisDialog();
    void addAxis(QString name,double lower, double upper);
    void updateAxis(QString name,double lower, double upper);
protected:
    void closeEvent(QCloseEvent *evt);
public slots:
    void listCurrentChanged(int index);
    void setMinMaxButtonClicked();
    void autoButtonClicked(bool checked);
    void groupAButtonClicked(bool checked);
    void groupBButtonClicked(bool checked);
    void groupCButtonClicked(bool checked);
    void groupDButtonClicked(bool checked);
signals:
    void graphAddedToGroup(QString name,QString group);
    void graphRemovedFromGroup(QString name);
private:
    //Map of ranges for individual graphs
    QMap<QString,QPair<double,double> > m_rangeMap;

    //Graph name to Group name map, for graphs that are in a group
    QMap<QString,QString > m_graphToGroupNameMap;

    Ui::AP2DataPlotAxisDialog *ui;
};

#endif // AP2DATAPLOTAXISDIALOG_H
