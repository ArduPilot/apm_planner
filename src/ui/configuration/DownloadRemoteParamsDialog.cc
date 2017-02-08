/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>
(c) author: Bill Bonney <billbonney@communistech.com>

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
#include "logging.h"
#include "configuration.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QTimer>
#include <QJsonParseError>
#include <QJsonObject>
#include "DownloadRemoteParamsDialog.h"
#include "ui_DownloadRemoteParamsDialog.h"

DownloadRemoteParamsDialog::DownloadRemoteParamsDialog(QWidget *parent, bool overwriteFile) :
    QDialog(parent),
    ui(new Ui::DownloadRemoteParamsDialog),
    m_locationOfFrameParams("https://raw.github.com/diydrones/ardupilot/master/Tools/Frame_params/"),
    m_extension(".param"),
    m_version("?ref=master"),
    m_networkReply(NULL),
    m_httpRequestAborted(false),
    m_overwriteFile(overwriteFile)
{
    QLOG_DEBUG() << "DownloadRemoteParamsDialog overwriteFile" << m_overwriteFile;
    ui->setupUi(this);

    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonClicked()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshParamList()));

    refreshParamList();
}

DownloadRemoteParamsDialog::~DownloadRemoteParamsDialog()
{
    delete ui;
}

void DownloadRemoteParamsDialog::setStatusText(QString text)
{
    QString message = tr("Status: %1").arg(text);
    ui->statusLabel->setText(message);
}

void DownloadRemoteParamsDialog::downloadButtonClicked()
{
    QLOG_DEBUG() << "loadButtonClicked";
    if (ui->listWidget->currentRow() == -1)
        return; // no item selected
    m_url = m_paramUrls.at(ui->listWidget->currentRow());

    setStatusText(tr("Downloading %1").arg(m_url.toString()));

    if (downloadParamFile()) {
        startFileDownloadRequest(m_url);
    } else {
        setStatusText(tr("Cancelled"));
    }
}

void DownloadRemoteParamsDialog::loadFileButtonClicked()
{
    QLOG_DEBUG() << "loadButtonClicked";

    QFileDialog *dialog = new QFileDialog(this, tr("Open File"), QGC::parameterDirectory());
    dialog->setFileMode(QFileDialog::ExistingFile);
    connect(dialog,SIGNAL(accepted()),this,SLOT(loadFileDialogAccepted()));
    dialog->show();
}
void DownloadRemoteParamsDialog::loadFileDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog)
    {
        return;
    }
    if (dialog->selectedFiles().size() == 0)
    {
        //No file selected/cancel clicked
        return;
    }
    QString filename = dialog->selectedFiles().at(0);
    QFile file(filename);

    if((filename.length() == 0)||!file.exists())
    {
        m_downloadedFileName = "";
        ui->statusLabel->setText(tr("File Not Found!"));
        return;
    }
    ui->statusLabel->setText(tr("Sucess"));
    m_downloadedFileName = filename;
    QTimer::singleShot(300, this, SLOT(accept()));
}

void DownloadRemoteParamsDialog::closeButtonClicked()
{
    QLOG_DEBUG() << "closeButtonClicked";
    close();
}

void DownloadRemoteParamsDialog::refreshParamList()
{
    QLOG_DEBUG() << "refresh list of param files from server";
    setStatusText(tr("Refresh Param file list"));
    ui->listWidget->clear();

    m_url = QUrl("https://api.github.com/repos/diydrones/ardupilot/contents/Tools/Frame_params");

    if (m_networkReply != NULL){
        delete m_networkReply;
        m_networkReply = NULL;
    }
    m_networkReply = m_networkAccessManager.get(QNetworkRequest(m_url));
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpParamListFinished()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void DownloadRemoteParamsDialog::startFileDownloadRequest(QUrl url)
{
    m_httpRequestAborted = false;
    if (m_networkReply != NULL){
        delete m_networkReply;
        m_networkReply = NULL;
    }
    m_networkReply = m_networkAccessManager.get(QNetworkRequest(url));
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpFinished()));
    connect(m_networkReply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

void DownloadRemoteParamsDialog::startParamListDownloadRequest(QUrl url)
{
    m_httpRequestAborted = false;
    if (m_networkReply != NULL){
        delete m_networkReply;
        m_networkReply = NULL;
    }
    m_networkReply = m_networkAccessManager.get(QNetworkRequest(url));
    connect(m_networkReply, SIGNAL(finished()), this, SLOT(httpParamListFinished()));
    connect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

QString DownloadRemoteParamsDialog::getDownloadedFileName()
{
    return m_downloadedFileName;
}

bool DownloadRemoteParamsDialog::downloadParamFile()
{
    QDir parameterDir = QDir(QGC::parameterDirectory());

    if (!parameterDir.exists())
        parameterDir.mkdir(parameterDir.filePath(""));

    QString fileName(parameterDir.filePath(
                          ui->listWidget->currentItem()->text()+ m_extension));

    if (fileName.isEmpty())
        return false;

    if (QFile::exists(fileName) && !m_overwriteFile) {
        int result = QMessageBox::question(this, tr("HTTP"),
                      tr("There already exists a file called %1 in "
                         "the current directory. Overwrite?").arg(fileName),
                      QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

        if (result == QMessageBox::No){
            return false;
        }
    }
    // Always must remove file before proceeding
    QFile::remove(fileName);

    m_downloadedFileName = fileName;

    m_downloadedParamFile = new QFile(fileName);
    if (!m_downloadedParamFile->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(fileName).arg(m_downloadedParamFile->errorString()));
        delete m_downloadedParamFile;
        m_downloadedParamFile = NULL;
        return false;
    }
    return true;
}

void DownloadRemoteParamsDialog::cancelDownload()
{
     ui->statusLabel->setText(tr("Download canceled."));
     m_httpRequestAborted = true;
     m_networkReply->abort();

}

void DownloadRemoteParamsDialog::httpFinished()
{
    bool result = false;
    if (m_httpRequestAborted) {
        if (m_downloadedParamFile) {
            m_downloadedParamFile->close();
            m_downloadedParamFile->remove();
            delete m_downloadedParamFile;
            m_downloadedParamFile = NULL;
        }
        m_networkReply->deleteLater();
        return;
    }

    m_downloadedParamFile->flush();
    m_downloadedParamFile->close();

    QVariant redirectionTarget = m_networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (m_networkReply->error()) {
        m_downloadedParamFile->remove();
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(m_networkReply->errorString()));

    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = m_url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_url = newUrl;
            m_networkReply->deleteLater();
            m_downloadedParamFile->open(QIODevice::WriteOnly);
            m_downloadedParamFile->resize(0);
            startFileDownloadRequest(m_url);
            return;
        }
    } else {
        QString fileName = m_downloadedParamFile->fileName();
        ui->statusLabel->setText(tr("Downloaded to %2.").arg(fileName));
        result = true;
    }

    m_networkReply->deleteLater();
    m_networkReply = NULL;
    delete m_downloadedParamFile;
    m_downloadedParamFile = NULL;

    if (result){
        this->accept();
    } else {
        this->reject();
    }
}

void DownloadRemoteParamsDialog::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (m_downloadedParamFile)
        m_downloadedParamFile->write(m_networkReply->readAll());
}

void DownloadRemoteParamsDialog::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (m_httpRequestAborted)
        return;
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}

void DownloadRemoteParamsDialog::httpParamListFinished()
{
    QLOG_DEBUG() << "DownloadRemoteParamsDialog::httpParamListFinished()";
    if (m_httpRequestAborted) {
        m_networkReply->deleteLater();
        m_networkReply = NULL;
        return;
    }

    // Finished donwloading the version information
    if (m_networkReply->error()) {
        // [TODO] cleanup download failed
#ifdef QT_DEBUG
        QMessageBox::information(NULL, tr("HTTP"),
                                 tr("Download failed: %1.")
                                 .arg(m_networkReply->errorString()));
#endif
    } else {
        // Process downloadeed object
        processDownloadedVersionObject(m_networkReply->readAll());
    }

    m_networkReply->deleteLater();
    m_networkReply = NULL;
}

void DownloadRemoteParamsDialog::processDownloadedVersionObject(const QByteArray& data)
{
    QJsonParseError jsonParseError;
    QJsonDocument jdoc = QJsonDocument::fromJson(data, &jsonParseError);
    if (jsonParseError.error != QJsonParseError::NoError){
        QLOG_ERROR() << "Unable to open json version object: " << jsonParseError.errorString();
        QLOG_ERROR() << "Error evaluating version object";
        return;
    }
    QJsonObject json = jdoc.object();

    QJsonArray entries = json["results"].toArray();
    foreach(QJsonValue entry, entries){
        const QJsonObject& entryObject = entry.toObject();

        QString paramFile = entryObject["name"].toString();
        QLOG_DEBUG() << " param file name found:" << paramFile;

        QListWidgetItem *item = new QListWidgetItem(paramFile.section('.',0,0), ui->listWidget);
        ui->listWidget->addItem(item);
        m_paramUrls.append( m_locationOfFrameParams + paramFile + m_version );
    }
}
