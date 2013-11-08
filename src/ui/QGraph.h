#ifndef QGRAPH_H
#define QGRAPH_H

#include <QWidget>

class QGraph : public QWidget
{
    Q_OBJECT
public:
    explicit QGraph(QWidget *parent = 0);
    void addData(QList<float> data,QString name);
    void removeData(QString name);
    void clear();
protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    QList<QList<float> > m_dataList;
    QList<QColor> m_colorList;
    QList<float> m_maxList;
    QList<float> m_minList;
    QList<QString> m_nameList;
    bool m_mouseDown;
    int m_maxViewX;
    int m_minViewX;
    int m_mouseX;
    int m_mouseY;
signals:

public slots:

};

#endif // QGRAPH_H
