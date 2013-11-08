#include "qgraph.h"
#include <QPainter>
#include <QMouseEvent>
#include <QDebug>

QGraph::QGraph(QWidget *parent) : QWidget(parent)
{
    m_mouseDown = false;

    m_maxViewX = 100;
    m_minViewX = 0;
    this->setMouseTracking(true);
}
void QGraph::addData(QList<float> data,QString name)
{
    if (m_dataList.size() > 0)
    {
        if (m_dataList[0].size() != data.size())
        {
            qDebug() << m_dataList[0].size() << data.size();
            return;
        }
    }
    m_dataList.append(data);
    m_colorList.append(QColor::fromRgb(100 + (rand() % 155),100 + (rand() % 155),100 + (rand() % 155)));
    float max = 0;
    float min = 0;
    if (data.size() > 0)
    {
        max = data[0];
        min = data[0];
    }
    for (int i=0;i<data.size();i++)
    {
        if (data[i] > max)
        {
            max = data[i];
        }
        if (data[i] < min)
        {
            min = data[i];
        }
    }


    qDebug() << "Max:" << max;
    qDebug() << "Min:" << min;
    m_maxList.append(max);
    m_minList.append(min);
    m_nameList.append(name);
    update();
}
void QGraph::clear()
{
    m_maxList.clear();
    m_minList.clear();
    m_dataList.clear();
    m_colorList.clear();
    m_nameList.clear();
    update();
}

void QGraph::removeData(QString name)
{
    for (int i=0;i<m_nameList.size();i++)
    {
        if (m_nameList[i] == name)
        {
            m_maxList.removeAt(i);
            m_dataList.removeAt(i);
            m_colorList.removeAt(i);
            m_nameList.removeAt(i);
            update();
            return;
        }
    }
}

void QGraph::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setPen(QColor::fromRgb(0,0,0));
    p.fillRect(0,0,width()-1,height()-1,Qt::SolidPattern);
    p.setPen(QColor::fromRgb(255,255,255));
    p.drawRect(0,0,width()-1,height()-1);
    p.drawRect(10,10,width()-21,height()-21);
    int minx = 10;
    int miny = height()-11;
    int maxx = width()-11;
    int maxy = 10;


    for (int j=0;j<m_dataList.size();j++)
    {
        QPainterPath path;
        p.setPen(m_colorList[j]);
        path.moveTo(minx,miny);
        int count = (float)(m_maxViewX - m_minViewX) / (float)(maxx-minx);
        qDebug() << "Count:" << count;
        if (count == 0)
        {
            count = 1;
        }
        float mindist = ((float)(maxx-minx) / (float)(m_maxViewX-m_minViewX)) * count; //Will be closer than this to a X point;
        mindist = mindist / 2.0;
        for (int i=m_minViewX;i<=m_maxViewX;i+=count)
        {
            float x = minx + (((float)(i-m_minViewX) / (float)(m_maxViewX-m_minViewX)) * (maxx - minx));
            if ((x-m_mouseX < mindist && x-m_mouseX > 0) || (m_mouseX-x < mindist && m_mouseX-x > 0))
            {
                qDebug() << x << m_mouseX << mindist;
                p.drawLine(x,miny,x,maxy);
                p.drawText(x+10,m_mouseY + (20 * j),QString::number(m_dataList[j][i]).append("\t").append(m_nameList[j]));
            }
            float y = miny + (((m_dataList[j][i]-m_minList[j])/(m_maxList[j]-m_minList[j])) * (maxy-miny));
            path.lineTo(x,y);
            path.addEllipse(x-2.5,y-2.5,5,5);
            path.moveTo(x,y);
        }

        p.drawPath(path);
    }
}
void QGraph::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dataList.size() == 0)
    {
        return;
    }
    if (m_mouseDown)
    {
        if ((m_minViewX + (m_mouseX - event->pos().x()) > 0) && (m_maxViewX + (m_mouseX - event->pos().x()) <= m_dataList[0].size()-1))
        {
            m_minViewX += m_mouseX - event->pos().x();
            m_maxViewX += m_mouseX - event->pos().x();
        }


        if (m_maxViewX >= m_dataList[0].size()-1) m_maxViewX = m_dataList[0].size()-1;
    }
    m_mouseX = event->pos().x();
    m_mouseY = event->pos().y();
    update();
}

void QGraph::mousePressEvent(QMouseEvent *event)
{
    m_mouseDown = true;
    m_mouseX = event->pos().x();
    m_mouseY = event->pos().y();
}

void QGraph::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDown = false;
}

void QGraph::wheelEvent(QWheelEvent *event)
{
    if (m_dataList.size() == 0)
    {
        return;
    }
    if (((m_minViewX - (event->delta() / 10.0)) >= 0) && ((m_maxViewX + (event->delta() / 10.0)) <= m_dataList[0].size()-1))
    {
        m_minViewX -= event->delta() / 10.0;
        m_maxViewX += event->delta() / 10.0;
    }
    else if ((m_minViewX - (event->delta() / 10.0)) >= 0)
    {
        m_minViewX -= event->delta() / 10.0;
    }
    else if ((m_maxViewX + (event->delta() / 10.0)) <= m_dataList[0].size()-1)
    {
        m_maxViewX += event->delta() / 10.0;
    }
    update();

}
