#ifndef AP2DATAPLOT2D_H
#define AP2DATAPLOT2D_H

#include <QWidget>
#include <QProgressDialog>
#include "ui_AP2DataPlot2D.h"
#include "AP2DataPlotThread.h"
#include "dataselectionscreen.h"
#include "qcustomplot.h"
#include "UASInterface.h"
#include "MAVLinkDecoder.h"
class AP2DataPlot2D : public QWidget
{
    Q_OBJECT
    
public:
    explicit AP2DataPlot2D(QWidget *parent = 0);
    ~AP2DataPlot2D();
    void addSource(MAVLinkDecoder *decoder);
private slots:
    void loadButtonClicked();
    void threadDone();
    void payloadDecoded(int index,QString name,QVariantMap map);
    void itemDisabled(QString name);
    void itemEnabled(QString name);
    void threadTerminated();
    void threadError(QString errorstr);
    void loadStarted();
    void loadProgress(qint64 pos,qint64 size);
    void progressDialogCanceled();
    void activeUASSet(UASInterface* uas);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint8 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint8 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint16 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint16 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint32 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint32 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const quint64 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const qint64 value, const quint64 msec);
    void valueChanged(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec);

    void valueChanged(const int uasid, const QString& name, const QString& unit, const QVariant value,const quint64 msecs);
    void updateValue(const int uasId, const QString& name, const QString& unit, const double value, const quint64 msec);

    void autoScrollClicked(bool checked);
    void tableCellClicked(int row,int column);
    void logLine(QString line);
    void addGraphRight();
    void addGraphLeft();
    void removeGraphLeft();
private:
    QAction *m_addGraphAction;
    QMap<QString,int> m_nameToAxisIndex;
    QMap<QString,QString> m_tableHeaderNameMap;
    int m_currentIndex;
    QMap<QString,QList<QPair<double,double> > > m_onlineValueMap;
    UASInterface *m_uas;
    QProgressDialog *m_progressDialog;
    QMap<QString,QCPAxis*> m_axisList;
    QMap<QString,QCPGraph*> m_graphMap;
    QList<QString> m_graphNameList;
    int graphCount;
    QCustomPlot *m_plot;
    QCPAxisRect *m_wideAxisRect;
    QMap<QString,QList<QPair<int,QVariantMap> > > m_dataList;
    Ui::AP2DataPlot2D ui;
    AP2DataPlotThread *m_logLoaderThread;
    DataSelectionScreen *m_dataSelectionScreen;
    bool m_logLoaded;
};

#endif // AP2DATAPLOT2D_H
