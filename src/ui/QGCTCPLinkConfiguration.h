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
protected:
    void changeEvent(QEvent *e);

    int m_linkId;

private:
    Ui::QGCTCPLinkConfiguration *ui;
};

#endif // QGCTCPLINKCONFIGURATION_H
