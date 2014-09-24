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
    uint16_t port = LinkManager::instance()->getTcpLinkPort(linkid);
    ui->portSpinBox->setValue(port);
    QString addr = LinkManager::instance()->getTcpLinkHost(linkid).toString();
    ui->hostAddressLineEdit->setText(addr);
    connect(ui->portSpinBox,SIGNAL(valueChanged(int)),this,SLOT(valuesChanged()));
    connect(ui->hostAddressLineEdit,SIGNAL(textChanged(QString)),this,SLOT(valuesChanged()));
}
void QGCTCPLinkConfiguration::valuesChanged()
{
    LinkManager::instance()->modifyTcpConnection(m_linkId,QHostAddress(ui->hostAddressLineEdit->text()),ui->portSpinBox->value());
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
