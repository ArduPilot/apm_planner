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
#include "QsLog.h"
#include "AutoUpdateDialog.h"
#include "ui_AutoUpdateDialog.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QPushButton>

AutoUpdateDialog::AutoUpdateDialog(const QString &version, const QString &targetFilename,
                                   const QString &url, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoUpdateDialog),
    m_sourceUrl(url),
    m_targetFilename(targetFilename),
    m_networkReply(NULL),
    m_skipVersion(false),
    m_skipVersionString(version)
{
    ui->setupUi(this);
    ui->progressBar->hide();
    ui->versionLabel->setText(version);

    connect(ui->skipPushButton, SIGNAL(clicked()), this, SLOT(skipClicked()));
    connect(ui->yesPushButton, SIGNAL(clicked()), this, SLOT(yesClicked()));
    connect(ui->noPushButton, SIGNAL(clicked()), this, SLOT(noClicked()));
}

AutoUpdateDialog::~AutoUpdateDialog()
{
    delete ui;
}

void AutoUpdateDialog::noClicked()
{
    deleteLater();
    reject();
}

void AutoUpdateDialog::skipClicked()
{
    emit autoUpdateCancelled(m_skipVersionString);
}

void AutoUpdateDialog::yesClicked()
{
    startDownload(m_sourceUrl, m_targetFilename);
}

bool AutoUpdateDialog::skipVersion()
{
    return m_skipVersion;
}

bool AutoUpdateDialog::startDownload(const QString& url, const QString& filename)
{
    QString targetDir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

    if (filename.isEmpty())
        return false;

    if (QFile::exists(targetDir + "/" + filename)) {
        int result = QMessageBox::question(this, tr("HTTP"),
                      tr("There already exists a file called %1 in "
                         "%2. Overwrite?").arg(filename, targetDir),
                      QMessageBox::Yes|QMessageBox::No, QMessageBox::No);

        if (result == QMessageBox::No){
            return false;
        }
    }
    // Always must remove file before proceeding
    QFile::remove(targetDir + filename);

    m_targetFile = new QFile(targetDir + "/" + filename);

    if (!m_targetFile->open(QIODevice::WriteOnly)) {
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Unable to save the file %1: %2.")
                                 .arg(filename).arg(m_targetFile->errorString()));
        delete m_targetFile;
        m_targetFile = NULL;
        return false;
    }

    QLOG_DEBUG() << "Start Downloading new version" << url;
    m_url = QUrl(url);
    startFileDownloadRequest(m_url);
    return true;
}

void AutoUpdateDialog::startFileDownloadRequest(QUrl url)
{
    ui->progressBar->show();
    ui->noPushButton->setText(tr("Cancel"));
    ui->yesPushButton->setEnabled(false);

    ui->titleLabel->setText(tr("<html><head/><body><p><span style=\" font-size:18pt; font-weight:600;\">Downloading</span></p></body></html>"));
    ui->questionLabel->setText(tr(""));
    ui->statusLabel->setText(tr("Downloading %1").arg(m_targetFilename));
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

void AutoUpdateDialog::cancelDownload()
{
     ui->statusLabel->setText(tr("Download canceled."));
     m_httpRequestAborted = true;
     m_networkReply->abort();

}

void AutoUpdateDialog::httpFinished()
 {
     bool result = false;
     if (m_httpRequestAborted) {
         if (m_targetFile) {
             m_targetFile->close();
             m_targetFile->remove();
             delete m_targetFile;
             m_targetFile = NULL;
         }
         m_networkReply->deleteLater();
        return;
     }

     m_targetFile->flush();
     m_targetFile->close();

     QVariant redirectionTarget = m_networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
     if (m_networkReply->error()) {
         m_targetFile->remove();
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
             m_targetFile->open(QIODevice::WriteOnly);
             m_targetFile->resize(0);
             startFileDownloadRequest(m_url);
             return;
         }
     } else {
         QString filename = m_targetFile->fileName();
         ui->statusLabel->setText(tr("Downloaded to %2.").arg(filename));
         result = true;
     }

     m_networkReply->deleteLater();
     m_networkReply = NULL;

     if (!result){
         ui->titleLabel->setText(tr("<html><head/><body><p><span style=\" font-size:18pt; font-weight:600;\">Download Failed</span></p></body></html>"));
         ui->statusLabel->setText(tr("ERROR: Download Failed!"));
     } else {
         ui->titleLabel->setText(tr("<html><head/><body><p><span style=\" font-size:18pt; font-weight:600;\">Download Complete</span></p></body></html>"));
         ui->questionLabel->setText(tr(""));

#ifdef Q_OS_MACX
         ui->statusLabel->setText(tr("Mounting Disk Image"));
#else
         ui->statusLabel->setText(tr("Start Installation"));
#endif
         executeDownloadedFile();

     }
     ui->noPushButton->setText(tr("OK"));

     this->raise();
     delete m_targetFile;
     m_targetFile = NULL;
}

void AutoUpdateDialog::executeDownloadedFile()
{
// [TODO] need to check the extension for .dmg or .pkg
#ifdef Q_OS_MACX
    QString filelocation = m_targetFile->fileName();
    QProcess *process = new QProcess();
    QLOG_INFO() << "LAUNCHING: DiskImageMounter" << filelocation;
    QStringList args;
    args.append(filelocation);
    process->start("/System/Library/CoreServices/DiskImageMounter.app/Contents/MacOS/DiskImageMounter", args);
    connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(dmgMounted(int,QProcess::ExitStatus)));
    process->waitForStarted();
#elif defined(Q_OS_UNIX)
    QString url = m_targetFile->fileName().mid(0,m_targetFile->fileName().lastIndexOf("/"));
    QLOG_INFO() << "Opening folder for display" << url;
    QDesktopServices::openUrl(url);
    exit(0);
#else
    QLOG_INFO() << "Launching" << m_targetFile->fileName();
    QDesktopServices::openUrl(QUrl(m_targetFile->fileName(), QUrl::TolerantMode));
    QTimer::singleShot(3000,this,SLOT(raise()));
    exit(0);
#endif
}

void AutoUpdateDialog::dmgMounted(int result, QProcess::ExitStatus exitStatus)
{
    QLOG_DEBUG() << "dmgMounted:" << result << "exitStatus:" << exitStatus;
    this->raise();
    ui->skipPushButton->setEnabled(false);
    ui->yesPushButton->setEnabled(false);
    if (result != 0){
        ui->statusLabel->setText(tr("ERROR:Failed to mount Disk Image!"));
        this->exec();
    }
    ui->statusLabel->setText(tr("Complete"));
    accept();
}

void AutoUpdateDialog::httpReadyRead()
{
    // this slot gets called every time the QNetworkReply has new data.
    // We read all of its new data and write it into the file.
    // That way we use less RAM than when reading it at the finished()
    // signal of the QNetworkReply
    if (m_targetFile){
        m_targetFile->write(m_networkReply->readAll());
    }
}

void AutoUpdateDialog::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    if (m_httpRequestAborted)
        return;
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}


