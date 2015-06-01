#include <QInputDialog>

#include "QGCUDPClientLinkConfiguration.h"
#include "ui_QGCUDPClientLinkConfiguration.h"
#include "LinkManager.h"
QGCUDPClientLinkConfiguration::QGCUDPClientLinkConfiguration(int linkid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCUDPClientLinkConfiguration)
{
    m_linkId = linkid;
    ui->setupUi(this);
    ui->portSpinBox->setValue(getUdpClientLink()->getPort());
    ui->ipAddressLineEdit->setText("192.168.4.1");
    connect(LinkManager::instance(),SIGNAL(linkChanged(int)),this,SLOT(linkChanged(int)));
    connect(ui->portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));
    connect(ui->ipAddressLineEdit, SIGNAL(textChanged(QString)), this, SLOT(hostValueChanged(QString)));
}

QGCUDPClientLinkConfiguration::~QGCUDPClientLinkConfiguration()
{
    delete ui;
}

void QGCUDPClientLinkConfiguration::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
void QGCUDPClientLinkConfiguration::portValueChanged(int value)
{
    getUdpClientLink()->setPort(value);
}

void QGCUDPClientLinkConfiguration::hostValueChanged(QString host)
{
    getUdpClientLink()->setAddress(QHostAddress(host));
}

void QGCUDPClientLinkConfiguration::linkChanged(int linkid)
{
    if (m_linkId != linkid)
    {
        return;
    }
    disconnect(ui->portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));
    disconnect(ui->ipAddressLineEdit, SIGNAL(editingFinished()), this, SLOT(portValueChanged(int)));
    ui->portSpinBox->setValue(getUdpClientLink()->getPort());
    connect(ui->portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));
}

UDPClientLink* QGCUDPClientLinkConfiguration::getUdpClientLink() const
{
    return dynamic_cast<UDPClientLink*>(LinkManager::instance()->getLink(m_linkId));
}
