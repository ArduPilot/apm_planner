#ifndef QGCUDPCLIENTLINKCONFIGURATION_H
#define QGCUDPCLIENTLINKCONFIGURATION_H

#include <QWidget>

#include "UDPClientLink.h"

namespace Ui
{
class QGCUDPClientLinkConfiguration;
}

class QGCUDPClientLinkConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit QGCUDPClientLinkConfiguration(int linkid, QWidget *parent = 0);
    ~QGCUDPClientLinkConfiguration();

public slots:
    void portValueChanged(int value);
    void hostValueChanged(QString host);
    void linkChanged(int linkid);

private:
    void changeEvent(QEvent *e);
    UDPClientLink* getUdpClientLink() const;

private:
    Ui::QGCUDPClientLinkConfiguration *ui;
    int m_linkId;
};

#endif // QGCUDPCLIENTLINKCONFIGURATION_H
