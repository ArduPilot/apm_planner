#ifndef QGCTCPLINKCONFIGURATION_H
#define QGCTCPLINKCONFIGURATION_H

#include <QWidget>

#include "TCPLink.h"

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

private:
    void changeEvent(QEvent *e);
    TCPLink* getTcpLink() const;

private:
    Ui::QGCTCPLinkConfiguration *ui;
    int m_linkId;

};

#endif // QGCTCPLINKCONFIGURATION_H
