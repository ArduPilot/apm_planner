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
#include "UASInterface.h"
#include "UASManager.h"

#include <QFileDialog>

DroneshareUploadDialog::DroneshareUploadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DroneshareUploadDialog),
    m_uasInterface(NULL),
    m_droneshareUpload(NULL)
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
    delete m_droneshareUpload;
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
        QLOG_DEBUG() << "Not c";
    }
    m_filename = QFileDialog::getOpenFileName(this, tr("Open Log File"), QGC::logDirectory(),
                                                    "*.tlog *.log");
    QApplication::processEvents();

    LoginDialog* loginDialog = new LoginDialog( this );
//    loginDialog->setUsername( "Luca" );  // optional
    connect( loginDialog, SIGNAL(acceptLogin(QString&,QString&,int)),
             this, SLOT(acceptUserLogin(QString&,QString&,int)));
    loginDialog->exec();
    delete loginDialog;
    loginDialog = NULL;
}

void DroneshareUploadDialog::acceptUserLogin(QString& username, QString& password, int indexNumber)
{
    QLOG_DEBUG() << "Droneshare: " << username << " pass: XXXXXXX" << "index: " << indexNumber;
    m_droneshareUpload = new DroneshareUpload();
    m_droneshareUpload->uploadLog(m_filename, username, password, QString::number(m_uasInterface->getUASID()),
                                  DroneshareAPIKey);
    connect(m_droneshareUpload, SIGNAL(uploadComplete()), this, SLOT(uploadComplete()));
    connect(m_droneshareUpload, SIGNAL(uploadFailed(QString)), this, SLOT(uploadFailed(QString)));
    connect(m_droneshareUpload, SIGNAL(uploadProgress(int,int)), this, SLOT(uploadProgress(int,int)));

    ui->statusLabel->setText(tr("Uploading %1").arg(m_filename));
}

void DroneshareUploadDialog::uploadProgress(int bytesRead, int totalBytes)
{
    ui->uploadProgressBar->setMaximum(totalBytes);
    ui->uploadProgressBar->setValue(bytesRead);
}

void DroneshareUploadDialog::uploadFailed(const QString& errorString)
{
    QLOG_DEBUG() << "droneshare: upload failed: " << errorString;
    delete m_droneshareUpload;
    m_droneshareUpload = NULL;
    ui->statusLabel->setText(tr("Upload Failed: %1").arg(errorString));
}

void DroneshareUploadDialog::uploadComplete()
{
    QLOG_DEBUG() << "droneshare: upload success: ";
    delete m_droneshareUpload;
    m_droneshareUpload = NULL;
    ui->statusLabel->setText(tr("Upload Succeeded"));
}
