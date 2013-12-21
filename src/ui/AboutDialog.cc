#include "configuration.h"
#include "AboutDialog.h"
#include "ui_AboutDialog.h"

#define define2string_p(x) #x
#define define2string(x) define2string_p(x)

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(accept()));

    ui->appnameLabel->setText(tr("%1 %2").arg(QGC_APPLICATION_NAME).arg(QGC_APPLICATION_VERSION));
    QString hash = QString(define2string(GIT_HASH));
    hash.truncate(8);
    ui->versionLabel->setText(tr("(%1-%2)").arg(hash)
                               .arg(define2string(GIT_COMMIT)));
    ui->linkLabel->setOpenExternalLinks(true);
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
