#include "AP2DataPlot2D.h"
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QTimer>

#include "UAS.h"
#include "UASManager.h"

AP2DataPlot2D::AP2DataPlot2D(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    m_uas = 0;
    m_logLoaded = false;
    m_progressDialog=0;
    m_currentIndex=0;
    connect(ui.pushButton,SIGNAL(clicked()),this,SLOT(loadButtonClicked()));
    graphCount=0;
    m_plot = new QCustomPlot(ui.widget);
    m_plot->setInteraction(QCP::iRangeDrag, true);
    m_plot->setInteraction(QCP::iRangeZoom, true);
    //m_plot->setInteraction(QCP::iMultiSelect,true);
    ui.horizontalLayout_3->addWidget(m_plot);
    m_plot->show();
    m_plot->plotLayout()->clear(); // clear default axis rect so we can start from scratch
    m_wideAxisRect = new QCPAxisRect(m_plot);
    m_wideAxisRect->setupFullAxesBox(true);
    m_wideAxisRect->axis(QCPAxis::atRight, 0)->setTickLabels(false);
    m_wideAxisRect->removeAxis(m_wideAxisRect->axis(QCPAxis::atLeft,0));
    m_plot->plotLayout()->addElement(0, 0, m_wideAxisRect); // insert axis rect in first row
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
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),m_plot,SLOT(replot()));
    timer->start(500);

    connect(ui.autoScrollCheckBox,SIGNAL(clicked(bool)),this,SLOT(autoScrollClicked(bool)));

}
void AP2DataPlot2D::autoScrollClicked(bool checked)
{
    if (checked)
    {
        if (graphCount > 0)
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
    if (!m_nameToAxisIndex.contains(name))
    {
        //Also doesn't exist on the data select screen
        m_dataSelectionScreen->addItem(name);
        m_nameToAxisIndex[name] = m_currentIndex;
    }
    else
    {
        if (m_nameToAxisIndex[name] == m_currentIndex)
        {
            m_currentIndex++;
            if (graphCount > 0 && ui.autoScrollCheckBox->isChecked())
            {
                m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeLower(m_wideAxisRect->axis(QCPAxis::atBottom,0)->range().lower+1);
                m_wideAxisRect->axis(QCPAxis::atBottom,0)->setRangeUpper(m_currentIndex);
            }
        }
    }
    m_nameToAxisIndex[name] = m_currentIndex;
    if (m_graphMap.contains(name))
    {
        m_graphMap[name]->addData(m_nameToAxisIndex[name],value);
        if (!m_graphMap[name]->keyAxis()->range().contains(value))
        {
            m_graphMap[name]->rescaleValueAxis();
        }
    }
    m_onlineValueMap[name].append(QPair<double,double>(m_nameToAxisIndex[name],value));
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
    graphCount=0;

    if (m_logLoaded)
    {
        //Unload the log.
        m_logLoaded = false;
        ui.pushButton->setText("Load Log");
        return;
    }
    ui.autoScrollCheckBox->setChecked(false);
    ui.pushButton->setText("Unload Log");

    m_logLoaded = true;
    m_logLoaderThread = new AP2DataPlotThread();
    connect(m_logLoaderThread,SIGNAL(startLoad()),this,SLOT(loadStarted()));
    connect(m_logLoaderThread,SIGNAL(loadProgress(qint64,qint64)),this,SLOT(loadProgress(qint64,qint64)));
    connect(m_logLoaderThread,SIGNAL(error(QString)),this,SLOT(threadError(QString)));
    connect(m_logLoaderThread,SIGNAL(done()),this,SLOT(threadDone()));
    connect(m_logLoaderThread,SIGNAL(terminated()),this,SLOT(threadTerminated()));
    connect(m_logLoaderThread,SIGNAL(payloadDecoded(int,QString,QVariantMap)),this,SLOT(payloadDecoded(int,QString,QVariantMap)));
    m_logLoaderThread->loadFile(filename);
}
void AP2DataPlot2D::threadTerminated()
{
    m_logLoaderThread->deleteLater();
    m_logLoaderThread = 0;
}

AP2DataPlot2D::~AP2DataPlot2D()
{
    delete m_dataSelectionScreen;
}
void AP2DataPlot2D::itemEnabled(QString name)
{
    if (m_logLoaded)
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

                    if (graphCount > 0)
                    {
                        connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
                    }
                    QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
                    axis->setLabelColor(color);
                    axis->setTickLabelColor(color);
                    axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
                    m_axisList[name] = axis;
                    QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,graphCount++));
                    m_graphMap[name] = mainGraph1;
                    m_graphNameList.append(name);
                    mainGraph1->setData(xlist, ylist);
                    mainGraph1->rescaleValueAxis();
                    if (graphCount == 1)
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

            if (graphCount > 0)
            {
                connect(m_wideAxisRect->axis(QCPAxis::atLeft,0),SIGNAL(rangeChanged(QCPRange)),axis,SLOT(setRange(QCPRange)));
            }
            QColor color = QColor::fromRgb(rand()%255,rand()%255,rand()%255);
            axis->setLabelColor(color);
            axis->setTickLabelColor(color);
            axis->setTickLabelColor(color); // add an extra axis on the left and color its numbers
            m_axisList[name] = axis;
            QCPGraph *mainGraph1 = m_plot->addGraph(m_wideAxisRect->axis(QCPAxis::atBottom), m_wideAxisRect->axis(QCPAxis::atLeft,graphCount++));
            m_graphMap[name] = mainGraph1;
            m_graphNameList.append(name);
            mainGraph1->setData(xlist, ylist);
            mainGraph1->rescaleValueAxis();
            if (graphCount == 1)
            {
                mainGraph1->rescaleKeyAxis();
            }

           // mainGraph1->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, QPen(Qt::black), QBrush(Qt::white), 6));
            mainGraph1->setPen(QPen(color, 2));
            m_plot->replot();

        }

    }
}
void AP2DataPlot2D::itemDisabled(QString name)
{
    m_wideAxisRect->removeAxis(m_axisList[name]);
    m_plot->removeGraph(m_graphMap[name]);
    m_plot->replot();
    m_graphMap.remove(name);
    m_axisList.remove(name);
    m_graphNameList.removeOne(name);
    graphCount--;

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
