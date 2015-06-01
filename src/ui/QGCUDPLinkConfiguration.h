#ifndef QGCUDPLINKCONFIGURATION_H
#define QGCUDPLINKCONFIGURATION_H

#include <QWidget>

#include "UDPLink.h"

namespace Ui
{
class QGCUDPLinkConfiguration;
}

class QGCUDPLinkConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit QGCUDPLinkConfiguration(int linkid, QWidget *parent = 0);
    ~QGCUDPLinkConfiguration();

public slots:
    void addHost();
    void portValueChanged(int value);
    void linkChanged(int linkid);

private:
    void changeEvent(QEvent *e);
    UDPLink* getUdpLink() const;

private:
    Ui::QGCUDPLinkConfiguration *ui;
    int m_linkId;
};

#endif // QGCUDPLINKCONFIGURATION_H
