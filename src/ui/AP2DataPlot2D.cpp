#include "AP2DataPlot2D.h"
#include <QFileDialog>
#include <QDebug>
AP2DataPlot2D::AP2DataPlot2D(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    graphCount=0;
    m_plot = new QCustomPlot(ui.widget);
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);
    ui.verticalLayout_3->addWidget(m_plot);
    m_plot->show();
    m_plot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    m_wideAxisRect = new QCPAxisRect(m_plot);
    m_wideAxisRect->setupFullAxesBox(true);
    m_wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);
    m_plot->plotLayout()->addElement(0, 0, m_wideAxisRect); // insert axis rect in first row
    QCPMarginGroup *marginGroup = new QCPMarginGroup(m_plot);
    m_wideAxisRect->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

    m_dataSelectionScreen = new DataSelectionScreen();
    connect( m_dataSelectionScreen,SIGNAL(itemEnabled(QString)),this,SLOT(itemEnabled(QString)));
    connect( m_dataSelectionScreen,SIGNAL(itemDisabled(QString)),this,SLOT(itemDisabled(QString)));


}
void AP2DataPlot2D::loadButtonClicked()
{
    QString filename = QFileDialog::getOpenFileName(this,"Select log file to open");
    if (filename == "")
    {
        return;
    }
    m_logLoaderThread = new AP2DataPlotThread();
    connect(m_logLoaderThread,SIGNAL(done()),this,SLOT(threadDone()));
    connect(m_logLoaderThread,SIGNAL(payloadDecoded(int,QString,QVariantMap)),this,SLOT(payloadDecoded(int,QString,QVariantMap)));
    m_logLoaderThread->loadFile(filename);
}

AP2DataPlot2D::~AP2DataPlot2D()
{
    delete m_dataSelectionScreen;
}
void AP2DataPlot2D::itemEnabled(QString name)
{
    for (QMap<QString,QList<QPair<int,QVariantMap> > >::const_iterator i=m_dataList.constBegin();i!=m_dataList.constEnd();i++)
    {
        if (i.value().size() > 0)
        {
            if (i.value()[0].second.contains(name))
            {
                //This is the one we want.
                QVector<double> xlist;
                QVector<double> ylist;
                int count = 0;
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
                axis->setTickLabelColor(QColor("#6050F8")); // add an extra axis on the left and color its numbers
                m_axisList[name] = axis;
                QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,graphCount++));
                m_graphList[name] = mainGraph1;
                mainGraph1->setData(xlist, ylist);
                mainGraph1->rescaleAxes();

               // mainGraph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
                mainGraph1->setPen(QPen(QColor(120, 120, 120), 2));
                m_plot->replot();
                return;
            }
        }

    }
}
void AP2DataPlot2D::itemDisabled(QString name)
{
    m_wideAxisRect->removeAxis(m_axisList[name]);
    m_plot->removeGraph(m_graphList[name]);
    m_plot->replot();
    m_graphList.remove(name);
    m_axisList.remove(name);
    graphCount--;

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
    m_dataSelectionScreen->show();
    m_dataSelectionScreen->setGeometry(100,100,400,600);
}

void AP2DataPlot2D::payloadDecoded(int index,QString name,QVariantMap map)
{
    if (!m_dataList.contains(name))
    {
        m_dataList[name] = QList<QPair<int,QVariantMap> >();
    }
    m_dataList[name].append(QPair<int,QVariantMap>(index,map));
}
