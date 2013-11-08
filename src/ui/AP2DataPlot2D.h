#ifndef AP2DATAPLOT2D_H
#define AP2DATAPLOT2D_H

#include <QWidget>
#include "ui_AP2DataPlot2D.h"
#include "AP2DataPlotThread.h"
#include "QGraph.h"
#include "dataselectionscreen.h"
#include "qcustomplot.h"
class AP2DataPlot2D : public QWidget
{
    Q_OBJECT
    
public:
    explicit AP2DataPlot2D(QWidget *parent = 0);
    ~AP2DataPlot2D();
private slots:
    void loadButtonClicked();
    void threadDone();
    void payloadDecoded(QVariantMap map);
    void itemDisabled(QString name);
    void itemEnabled(QString name);
private:
    int graphCount;
    QGraph *graph;
    QCustomPlot *plot;
    QCPAxisRect *wideAxisRect;
    QList<QVariantMap> datalist;
    Ui::AP2DataPlot2D ui;
    AP2DataPlotThread *loader;
    DataSelectionScreen *dataSelectionScreen;
};

#endif // AP2DATAPLOT2D_H
