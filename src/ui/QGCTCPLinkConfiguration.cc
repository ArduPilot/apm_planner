#include <QInputDialog>

#include "QGCTCPLinkConfiguration.h"
#include "ui_QGCTCPLinkConfiguration.h"
#include <stdint.h>
#include "LinkManager.h"

QGCTCPLinkConfiguration::QGCTCPLinkConfiguration(int linkid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCTCPLinkConfiguration)
{
    ui->setupUi(this);
    m_linkId = linkid;
    uint16_t port = getTcpLink()->getPort();
    ui->portSpinBox->setValue(port);
    QString addr = getTcpLink()->getHostAddress().toString();
    ui->hostAddressLineEdit->setText(addr);
    ui->asServerCheckBox->setChecked(getTcpLink()->isServer());
    connect(ui->portSpinBox,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->hostAddressLineEdit,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->asServerCheckBox,SIGNAL(stateChanged(int)),this,SLOT(valuesChanged()));
}

void QGCTCPLinkConfiguration::valuesChanged()
{
    LinkManager *lm = LinkManager::instance();

    TCPLink *iface = qobject_cast<TCPLink*>(lm->getLink(m_linkId));
    if (!iface)
    {
        return;
    }
    iface->setHostAddress(QHostAddress(ui->hostAddressLineEdit->text()));
    iface->setPort(ui->portSpinBox->value());
    iface->setAsServer(ui->asServerCheckBox->isChecked());
}

QGCTCPLinkConfiguration::~QGCTCPLinkConfiguration()
{
    delete ui;
}

void QGCTCPLinkConfiguration::changeEvent(QEvent *e)
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

TCPLink* QGCTCPLinkConfiguration::getTcpLink() const
{
    return dynamic_cast<TCPLink*>(LinkManager::instance()->getLink(m_linkId));
}
