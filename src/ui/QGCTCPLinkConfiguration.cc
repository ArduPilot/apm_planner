#include <QInputDialog>

#include "QGCTCPLinkConfiguration.h"
#include "ui_QGCTCPLinkConfiguration.h"
#include <stdint.h>
#include "LinkManager.h"

#include <QMessageBox>

QGCTCPLinkConfiguration::QGCTCPLinkConfiguration(int linkid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCTCPLinkConfiguration)
{
    ui->setupUi(this);
    m_linkId = linkid;
    uint16_t port = getTcpLink()->getPort();
    ui->portSpinBox->setValue(port);
    QString host = getTcpLink()->getName();
    if (host.isEmpty())
        getTcpLink()->getHostAddress().toString();
    ui->hostAddressLineEdit->setText(host);
    ui->asServerCheckBox->setChecked(getTcpLink()->isServer());
    connect(ui->portSpinBox,SIGNAL(editingFinished()),this,SLOT(valuesChanged()));
    connect(ui->hostAddressLineEdit,SIGNAL(editingFinished()),this,SLOT(hostChanged()));
    connect(ui->asServerCheckBox,SIGNAL(stateChanged(int)),this,SLOT(valuesChanged()));

    connect(getTcpLink(), SIGNAL(communicationError(QString,QString)), this, SLOT(communicationError(QString,QString)));
}

QGCTCPLinkConfiguration::~QGCTCPLinkConfiguration()
{
    delete ui;
}

void QGCTCPLinkConfiguration::valuesChanged()
{
    LinkManager *lm = LinkManager::instance();
    TCPLink *iface = qobject_cast<TCPLink*>(lm->getLink(m_linkId));
    if (!iface){
        return;
    }
    iface->setPort(ui->portSpinBox->value());
    iface->setAsServer(ui->asServerCheckBox->isChecked());
}

void QGCTCPLinkConfiguration::hostChanged()
{
    LinkManager *lm = LinkManager::instance();
    TCPLink *iface = qobject_cast<TCPLink*>(lm->getLink(m_linkId));
    if (!iface){
        return;
    }

    QHostAddress address = QHostAddress(ui->hostAddressLineEdit->text());
    iface->setHostName(ui->hostAddressLineEdit->text());
    if (address.isNull()){
        QHostInfo::lookupHost(ui->hostAddressLineEdit->text().trimmed(),
                              this, SLOT(lookedUp(QHostInfo)));
    } else {
        iface->setHostAddress(address);
    }
}

void QGCTCPLinkConfiguration::lookedUp(const QHostInfo &host)
{
    if (host.error() != QHostInfo::NoError) {
        QLOG_DEBUG() << "Lookup failed:" << host.errorString();
        ui->statusLabel->setText("<b><font color=\"red\">Error: </font></b>" + host.errorString());
        ui->hostAddressLineEdit->selectAll();
        ui->hostAddressLineEdit->setFocus();
        return;
    }

    ui->statusLabel->setText("<b>Success:</b> host name resolved");
    QTimer::singleShot(2000, ui->statusLabel, SLOT(clear()));

    LinkManager *lm = LinkManager::instance();
    TCPLink *iface = qobject_cast<TCPLink*>(lm->getLink(m_linkId));
    if (!iface){
        return;
    }

    foreach (const QHostAddress &address, host.addresses()){
        QLOG_DEBUG() << "Found address:" << address.toString();
        iface->setHostAddress(address);
        iface->setHostName(host.hostName());
    }
}

void QGCTCPLinkConfiguration::communicationError(QString name, QString error)
{
    Q_UNUSED(name);
    ui->statusLabel->setText("<b>Error:</b> " + error);
    QTimer::singleShot(2000, ui->statusLabel, SLOT(clear()));
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
