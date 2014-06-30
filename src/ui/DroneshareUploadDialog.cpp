/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>

This file is part of the APM_PLANNER project

    APM_PLANNER is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    APM_PLANNER is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with APM_PLANNER. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/
/**
 * @file
 *   @brief DroneshareUpload helper
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 */
#include "QsLog.h"
#include "configuration.h"
#include "DroneshareUploadDialog.h"
#include "ui_DroneshareUploadDialog.h"

#include "LoginDialog.h"
#include "DroneshareUpload.h"
#include "DroneshareAPIBroker.h"
#include "UASInterface.h"
#include "UASManager.h"

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptValueIterator>
#include <QFileDialog>
#include <QInputDialog>

DroneshareUploadDialog::DroneshareUploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DroneshareUploadDialog),
    m_uasInterface(NULL),
    m_droneshareUpload(NULL),
    m_droneshareQuery(NULL)
{
    ui->setupUi(this);

    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
    connect(ui->uploadButton, SIGNAL(clicked()), this, SLOT(uploadClicked()));

    connect(UASManager::instance(), SIGNAL(activeUASSet(UASInterface*)),
            this, SLOT(activeUASSet(UASInterface*)));
    activeUASSet(UASManager::instance()->getActiveUAS());
}

DroneshareUploadDialog::~DroneshareUploadDialog()
{
    delete m_droneshareQuery;
    m_droneshareQuery = NULL;
    delete m_droneshareUpload;
    m_droneshareUpload = NULL;
    delete ui;
}

void DroneshareUploadDialog::activeUASSet(UASInterface *uas)
{
    if (m_uasInterface){
        // Disconnect relevant signals
    }
    m_uasInterface = uas;
    if(m_uasInterface){
        //connect relevant signals
    }
}

void DroneshareUploadDialog::closeButtonClicked()
{
    this->accept();
}

void DroneshareUploadDialog::uploadClicked()
{
    if (m_uasInterface == NULL){
        QLOG_DEBUG() << "Not connected to a uas";
    }
    m_filename = QFileDialog::getOpenFileName(this, tr("Open Log File"), QGC::logDirectory(),
                                                    "*.tlog *.log");
    QApplication::processEvents();

    LoginDialog* loginDialog = new LoginDialog( this );
    loginDialog->setUsername( "maninvan" );  // optional [TODO] retreive from settings.
    connect( loginDialog, SIGNAL(acceptLogin(QString&,QString&,int)),
             this, SLOT(acceptUserLogin(QString&,QString&,int)));
    loginDialog->exec();
    delete loginDialog;
    loginDialog = NULL;
}

void DroneshareUploadDialog::acceptUserLogin(QString& username, QString& password, int indexNumber)
{
    QLOG_DEBUG() << "Droneshare: " << username << " pass: XXXXXXX" << "index: " << indexNumber;
    m_username = username;
    m_password = password;

    m_droneshareQuery = new DroneshareAPIBroker();
    connect(m_droneshareQuery, SIGNAL(queryComplete(QString)),
            this, SLOT(vehicleQueryComplete(QString)));
    connect(m_droneshareQuery, SIGNAL(queryFailed(QString)),
            this, SLOT(vehicleQueryFailed(QString)));

    m_droneshareQuery->addBaseUrl(DroneshareBaseUrl);
    m_droneshareQuery->addQuery("/user/" + m_username);
    m_droneshareQuery->addQueryItem("api_key", DroneshareAPIKey);
    m_droneshareQuery->sendQueryRequest();
//    QLOG_DEBUG() << "droneshare: user request: " << m_droneshareQuery->getUrl();
}

void DroneshareUploadDialog::vehicleQueryComplete(const QString &jsonRepsonse)
{
    QLOG_DEBUG() << "droneshare: Vehicle Query Complete"/* << jsonRepsonse*/;

    QScriptSyntaxCheckResult syntaxCheck = QScriptEngine::checkSyntax(jsonRepsonse);
    QScriptEngine engine;
    QScriptValue result = engine.evaluate("("+jsonRepsonse+")");

    if (engine.hasUncaughtException()){
        QLOG_ERROR() << "Error evaluating version object";
        QLOG_ERROR() << "Error @line#" << engine.uncaughtExceptionLineNumber();
        QLOG_ERROR() << "Backtrace:" << engine.uncaughtExceptionBacktrace();
        QLOG_ERROR() << "Syntax Check:" << syntaxCheck.errorMessage();
        QLOG_ERROR() << "Syntax Check line:" << syntaxCheck.errorLineNumber()
                     << " col:" << syntaxCheck.errorColumnNumber();
        return;
    }
    ui->statusLabel->setText("Vehicle Query Complete.");
    QMap<QString,QString> vehicleList;

    QScriptValue entries = result.property("vehicles");
    QScriptValueIterator it(entries);
    while (it.hasNext()){
        it.next();
        QScriptValue entry = it.value();
        QString uuid = entry.property("uuid").toString();
        QString name = entry.property("name").toString();
        vehicleList.insert(name,uuid);
    }

    bool ok = false;
    QString item = QInputDialog::getItem(this, tr("Vehicle Selection"),tr("vehicle"),vehicleList.keys(),1,
                                                          false, &ok, Qt::Dialog, Qt::ImhNone);
    if (ok){
        startLogUpload(vehicleList.value(item));
    }

    m_droneshareQuery->deleteLater();
    m_droneshareQuery = NULL;
}

void DroneshareUploadDialog::vehicleQueryFailed(const QString &errorString)
{
    QLOG_DEBUG() << "droneshare: Vehicle Query Failed error=" << errorString;
    ui->statusLabel->setText(tr("No vehicle defintitions found"));
    m_droneshareQuery->deleteLater();
    m_droneshareQuery = NULL;
}

void DroneshareUploadDialog::startLogUpload(const QString& vehicleUuid)
{
    m_droneshareUpload = new DroneshareUpload();
    m_droneshareUpload->uploadLog(m_filename, m_username, m_password, vehicleUuid, DroneshareAPIKey);
    connect(m_droneshareUpload, SIGNAL(uploadComplete(QString)), this, SLOT(uploadComplete(QString)));
    connect(m_droneshareUpload, SIGNAL(uploadFailed(QString,QString)), this, SLOT(uploadFailed(QString,QString)));
    connect(m_droneshareUpload, SIGNAL(uploadProgress(int,int)), this, SLOT(uploadProgress(int,int)));

    ui->statusLabel->setText(tr("Uploading %1").arg(m_filename));
}

void DroneshareUploadDialog::uploadProgress(int bytesRead, int totalBytes)
{
    ui->uploadProgressBar->setMaximum(totalBytes);
    ui->uploadProgressBar->setValue(bytesRead);
}

void DroneshareUploadDialog::uploadFailed(const QString& jsonResponse, const QString& errorString)
{
    QLOG_DEBUG() << "droneshare: upload failed: " << errorString;
    delete m_droneshareUpload;
    m_droneshareUpload = NULL;
    ui->statusLabel->setText(tr("Upload Failed!"));
}

void DroneshareUploadDialog::uploadComplete(const QString& jsonResponse)
{
    QLOG_DEBUG() << "droneshare: upload success: " << jsonResponse;
    delete m_droneshareUpload;
    m_droneshareUpload = NULL;
    ui->statusLabel->setText(tr("Upload Succeeded"));
}
