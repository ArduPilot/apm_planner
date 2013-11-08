#include "AP2DataPlot2D.h"
#include <QFileDialog>
#include <QDebug>
#define CUSTOMPLOT
AP2DataPlot2D::AP2DataPlot2D(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    graphCount=0;
#ifdef CUSTOMPLOT
    plot = new QCustomPlot(ui.widget);
    plot->setInteraction(QCP::iRangeDrag, true);
    plot->setInteraction(QCP::iRangeZoom, true);
    ui.verticalLayout_3->addWidget(plot);
    plot->show();
    plot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    wideAxisRect = new QCPAxisRect(plot);
    wideAxisRect->setupFullAxesBox(true);
    wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);
    plot->plotLayout()->addElement(0, 0, wideAxisRect); // insert axis rect in first row
    QCPMarginGroup *marginGroup = new QCPMarginGroup(plot);
    wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

#endif
#ifdef QGRAPH
    graph = new QGraph(ui.widget);
    ui.verticalLayout_3->addWidget(graph);
    graph->show();
#endif
    dataSelectionScreen = new DataSelectionScreen();
    connect( dataSelectionScreen,SIGNAL(itemEnabled(QString)),this,SLOT(itemEnabled(QString)));
    connect( dataSelectionScreen,SIGNAL(itemDisabled(QString)),this,SLOT(itemDisabled(QString)));


}
void AP2DataPlot2D::loadButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Select log file to open");
    if (filename == "")
    {
        return;
    }
   // fileCloseClicked();
    //decoder = new FEDataPacketDecoder();
    loader = new AP2DataPlotThread();
    //connect(loader,SIGNAL(incomingDatalogPacket(QByteArray)),decoder,SLOT(decodePayload(QByteArray)));
    connect(loader,SIGNAL(done()),this,SLOT(threadDone()));
    //connect(decoder,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(payloadDecoded(QVariantMap)));
    connect(loader,SIGNAL(payloadDecoded(QVariantMap)),this,SLOT(payloadDecoded(QVariantMap)));
    loader->loadFile(filename);
}

AP2DataPlot2D::~AP2DataPlot2D()
{
}
void AP2DataPlot2D::itemEnabled(QString name)
{
#ifdef QGRAPH
    QList<float> list;
    for (int i=0;i<datalist.size();i++)
    {
        list.append(datalist[i][name].toFloat());
    }
    graph->addData(list,name);
#endif
#ifdef CUSTOMPLOT
    QVector<double> xlist;
    QVector<double> ylist;
    int count = 0;
    float min = datalist[0][name].toDouble();
    float max = datalist[0][name].toDouble();
    for (int i=0;i<datalist.size();i++)
    {
        xlist.append(i);
        double val = datalist[i][name].toDouble();
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
    wideAxisRect->addAxis(QCPAxis::atLeft)->setTickLabelColor(QColor("#6050F8")); // add an extra axis on the left and color its numbers
    QCPGraph *mainGraph1 = plot->addGraph(wideAxisRect->axis(QCPAxis::atBottom), wideAxisRect->axis(QCPAxis::atLeft,graphCount++));
    mainGraph1->setData(xlist, ylist);
    mainGraph1->rescaleAxes();

   // mainGraph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
    mainGraph1->setPen(QPen(QColor(120, 120, 120), 2));
    plot->replot();
#endif
}
void AP2DataPlot2D::itemDisabled(QString name)
{
    graph->removeData(name);
}
void AP2DataPlot2D::threadDone()
{
    qDebug() << datalist.size() << "records loaded";
    for (QVariantMap::const_iterator i=datalist[0].constBegin();i!=datalist[0].constEnd();i++)
    {
        dataSelectionScreen->addItem(i.key());
    }
    dataSelectionScreen->show();
    dataSelectionScreen->setGeometry(100,100,400,600);
}

void AP2DataPlot2D::payloadDecoded(QVariantMap map)
{
    datalist.append(map);
}
