#include "UASRawStatusView.h"
#include "MAVLinkDecoder.h"
#include "UASInterface.h"
#include "UAS.h"
#include <QTimer>
#include <QScrollBar>
#include "UASManager.h"
UASRawStatusView::UASRawStatusView(QWidget *parent) : QWidget(parent)
{
    m_uas = 0;
    ui.setupUi(this);
    ui.tableWidget->setColumnCount(2);
    ui.tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui.tableWidget->setShowGrid(false);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableRefreshTimer = new QTimer(this);
    connect(m_tableRefreshTimer,SIGNAL(timeout()),this,SLOT(updateTableTimerTick()));

    m_updateTimer = new QTimer(this);
    connect(m_updateTimer,SIGNAL(timeout()),this,SLOT(updateTimerTick()));

    // FIXME reinstate once fixed.

    //timer->start(2000);
    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
}
void UASRawStatusView::activeUASSet(UASInterface* uas)
{
    if (!uas)
    {
        return;
    }
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));
    }
    m_uas = uas;
    connect(m_uas,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));

}

void UASRawStatusView::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    //Check every 2 seconds to see if we need an update
    updateTableTimerTick();
    m_updateTimer->start(500);
    m_tableRefreshTimer->start(2000);
}

void UASRawStatusView::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    m_updateTimer->stop();
    m_tableRefreshTimer->stop();
}
void UASRawStatusView::updateTimerTick()
{
    for (QMap<QString,double>::const_iterator i=valueMap.constBegin();i!=valueMap.constEnd();i++)
    {
        if (nameToUpdateWidgetMap.contains(i.key()))
        {
            nameToUpdateWidgetMap[i.key()]->setText(QString::number(i.value(),'f',4));
        }
        else
        {
            m_tableDirty = true;
        }
    }
    return;
}

void UASRawStatusView::addSource(MAVLinkDecoder *decoder)
{
    Q_UNUSED(decoder)
   // connect(decoder,SIGNAL(valueChanged(int,QString,QString,QVariant,quint64)),this,SLOT(valueChanged(int,QString,QString,QVariant,quint64)));
}
void UASRawStatusView::valueChanged(const int uasId, const QString& name, const QString& unit, const QVariant value, const quint64 msec)
{
    if (value.type() == QVariant::Double)
    {
        valueChanged(uasId,name,unit,value.toDouble(),msec);
    }
    else
    {
        valueChanged(uasId,name,unit,static_cast<double>(value.toInt()),msec);
    }
}

void UASRawStatusView::valueChanged(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec)
{
    Q_UNUSED(uasId)
    Q_UNUSED(unit)
    Q_UNUSED(msec)
    valueMap[name] = value;
}
void UASRawStatusView::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

    m_tableDirty = true;
}

void UASRawStatusView::updateTableTimerTick()
{
    if (m_tableDirty)
    {
        m_tableDirty = false;
        int columncount = 2;
        bool good = false;
        while (!good)
        {
            ui.tableWidget->clear();
            ui.tableWidget->setRowCount(0);
            ui.tableWidget->setColumnCount(columncount);
            for (int i=0;i<columncount;i++)
            {
                ui.tableWidget->setColumnWidth(i,100);
            }
            ui.tableWidget->horizontalHeader()->hide();
            ui.tableWidget->verticalHeader()->hide();
            int currcolumn = 0;
            int currrow = 0;
            int totalheight = 2 + ui.tableWidget->horizontalScrollBar()->height();
            bool broke = false;
            for (QMap<QString,double>::const_iterator i=valueMap.constBegin();i!=valueMap.constEnd();i++)
            {
                if (ui.tableWidget->rowCount() < currrow+1)
                {
                    ui.tableWidget->setRowCount(currrow+1);
                }
                if (i.key().contains("."))
                {
                    ui.tableWidget->setItem(currrow,currcolumn,new QTableWidgetItem(i.key().split(".")[1]));
                }
                else
                {
                    ui.tableWidget->setItem(currrow,currcolumn,new QTableWidgetItem(i.key()));
                }
                QTableWidgetItem *item = new QTableWidgetItem(QString::number(i.value()));
                nameToUpdateWidgetMap[i.key()] = item;
                ui.tableWidget->setItem(currrow,currcolumn+1,item);
                ui.tableWidget->resizeRowToContents(currrow);
                totalheight += ui.tableWidget->rowHeight(currrow);
                currrow++;
                if ((totalheight + ui.tableWidget->rowHeight(currrow-1)) > ui.tableWidget->height())
                {
                    currcolumn+=2;
                    totalheight = 2 + ui.tableWidget->horizontalScrollBar()->height();
                    currrow = 0;
                    if (currcolumn >= columncount)
                    {
                        //We're over what we can do. Add a column and continue.
                        columncount+=2;
                        broke = true;
                        i = valueMap.constEnd(); // Ensure loop breakout.
                        break;
                    }
                }
            }
            if (!broke)
            {
                good = true;
            }
        }
        //ui.tableWidget->columnCount()-2
    }
}

UASRawStatusView::~UASRawStatusView()
{
}
