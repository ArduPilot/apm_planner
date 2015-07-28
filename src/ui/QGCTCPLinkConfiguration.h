#ifndef QGCTCPLINKCONFIGURATION_H
#define QGCTCPLINKCONFIGURATION_H

#include "TCPLink.h"

#include <QWidget>
#include <QHostInfo>

namespace Ui
{
class QGCTCPLinkConfiguration;
}

class QGCTCPLinkConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit QGCTCPLinkConfiguration(int link, QWidget *parent = 0);
    ~QGCTCPLinkConfiguration();

public slots:
    void valuesChanged();
    void hostChanged();

private slots:
    void lookedUp(const QHostInfo &host);
    void communicationError(QString name, QString error);

private:
    void changeEvent(QEvent *e);
    TCPLink* getTcpLink() const;

private:
    Ui::QGCTCPLinkConfiguration *ui;
    int m_linkId;

};

#endif // QGCTCPLINKCONFIGURATION_H
