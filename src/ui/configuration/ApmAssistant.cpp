#include "QsLog.h"
#include "ApmAssistant.h"
#include "ui_ApmAssistant.h"
#include "MainWindow.h"

ApmAssistant::ApmAssistant(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ApmAssistant)
{
    ui->setupUi(this);

    // Load the first page
    ui->webView->setUrl(QUrl("./files/ardupilotmega/APMAssistant/index.html"));

    // Connect the signals
    connect(ui->goBackButton, SIGNAL(clicked()), ui->webView, SLOT(back()));
#ifdef QT_DEBUG
    connect(ui->reloadButton, SIGNAL(clicked()), ui->webView, SLOT(reload())    );
#else
    ui->reloadButton->hide();
#endif
    connect(ui->webView, SIGNAL(urlChanged(QUrl)), this, SLOT(anchorClicked(QUrl)));
    connect(parent, SIGNAL(viewChanged(QWidget*)), this, SLOT(viewChanged(QWidget*)));
}

ApmAssistant::~ApmAssistant()
{
    delete ui;
}

void ApmAssistant::anchorClicked(QUrl url)
{
    QLOG_DEBUG() << "APM Assistant: anchorClicked:" << url;
    QString cmd;
    QString wid;

    if (url.hasQueryItem("cmd")){
        QLOG_DEBUG() << "APMA: found cmd: " << url.queryItemValue("cmd");
            cmd = url.queryItemValue("cmd");

        if (url.hasQueryItem("wid")) {
            QLOG_DEBUG() << "APMA: found widget id: " << url.queryItemValue("wid");
            wid = url.queryItemValue("wid");

        } else {
            QLOG_ERROR() << "APMA: ERROR no widget id!";
            return;
        }

        if (cmd == "click"){ // command
            // Option #1
            QLOG_DEBUG() << "Activate button " << wid;
            QPushButton *button = MainWindow::instance()->findChild<QPushButton *>(wid);

            if (button==NULL){
                QLOG_DEBUG() << "wid not found!";
            } else {
                QLOG_DEBUG() << "found!";
                button->animateClick();
            }
        }

    } else {
        QLOG_DEBUG() << "APMA: no cmd in this request";
    }
}

void ApmAssistant::viewChanged(QWidget* widget)
{
    QLOG_DEBUG() << "APM Assistant: View Changed:" << widget;
}

