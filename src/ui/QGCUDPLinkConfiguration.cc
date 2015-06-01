#include <QInputDialog>

#include "QGCUDPLinkConfiguration.h"
#include "ui_QGCUDPLinkConfiguration.h"
#include "LinkManager.h"
QGCUDPLinkConfiguration::QGCUDPLinkConfiguration(int linkid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QGCUDPLinkConfiguration)
{
    m_linkId = linkid;
    ui->setupUi(this);
    ui->portSpinBox->setValue(getUdpLink()->getPort());
    connect(LinkManager::instance(),SIGNAL(linkChanged(int)),this,SLOT(linkChanged(int)));
    connect(ui->portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));
    connect(ui->addIPButton, SIGNAL(clicked()), this, SLOT(addHost()));
}

QGCUDPLinkConfiguration::~QGCUDPLinkConfiguration()
{
    delete ui;
}

void QGCUDPLinkConfiguration::changeEvent(QEvent *e)
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
void QGCUDPLinkConfiguration::portValueChanged(int value)
{
    getUdpLink()->setPort(value);
}

void QGCUDPLinkConfiguration::addHost()
{
    bool ok;
    QString hostName = QInputDialog::getText(this, tr("Add a new IP address / hostname to MAVLink"),
                       tr("Host (hostname:port):"), QLineEdit::Normal,
                       "localhost:14555", &ok);
    if (ok && !hostName.isEmpty())
    {
        getUdpLink()->addHost(hostName);
    }
}
void QGCUDPLinkConfiguration::linkChanged(int linkid)
{
    if (m_linkId != linkid)
    {
        return;
    }
    disconnect(ui->portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));
    ui->portSpinBox->setValue(getUdpLink()->getPort());
    connect(ui->portSpinBox, SIGNAL(valueChanged(int)), this, SLOT(portValueChanged(int)));
}

UDPLink* QGCUDPLinkConfiguration::getUdpLink() const
{
    return dynamic_cast<UDPLink*>(LinkManager::instance()->getLink(m_linkId));
}
