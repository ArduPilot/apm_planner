#include "QsLog.h"
#include "configuration.h"
#include <QMessageBox>
#include <QDesktopServices>
#include <QFile>
#include <QFileDialog>
#include <QTimer>
#include "DownloadRemoteParamsDialog.h"
#include "ui_DownloadRemoteParamsDialog.h"

DownloadRemoteParamsDialog::DownloadRemoteParamsDialog(QWidget *parent, bool overwriteFile) :
    QDialog(parent),
    ui(new Ui::DownloadRemoteParamsDialog),
    m_locationOfFrameParams("https://raw.github.com/diydrones/ardupilot/master/Tools/Frame_params/"),
    m_extension(".param"),
    m_version("?ref=master"),
    m_networkReply(NULL),
    m_overwriteFile(overwriteFile)
{
    QLOG_DEBUG() << "DownloadRemoteParamsDialog overwriteFile" << m_overwriteFile;
    ui->setupUi(this);

    connect(this, SIGNAL(canceled()), this, SLOT(cancelDownload()));
    connect(ui->downloadButton, SIGNAL(clicked()), this, SLOT(downloadButtonClicked()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(closeButtonClicked()));
    connect(ui->refreshButton, SIGNAL(clicked()), this, SLOT(refreshParamList()));

    // [ToDo] For now just add a default list of parameter filenames
    // In future need to add a github rest API request.
    QStringList paramFiles;
    paramFiles << "Iris" << "Beginner" << "Intermediate" << "Advanced" << "CameraPlatform"
               << "3DR_Y6B_RTF" << "3DR_X8_RTF" << "3DR_QUAD_X4_RTF" << "3DR_Y6A_RTF";
    paramFiles.sort();

    foreach(QString paramFile, paramFiles){
    QListWidgetItem *item = new QListWidgetItem( paramFile, ui->listWidget);
        ui->listWidget->addItem(item);
        m_paramUrls.append( m_locationOfFrameParams + paramFile + m_extension + m_version );
    }

    // [TODO] stop hiding the refresh button used to donwload from GitHub.
    ui->refreshButton->setVisible(false);

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

    QString filename = QFileDialog::getOpenFileName(this,"Open File", QGC::parameterDirectory());
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

    if (m_url.isEmpty())
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

void DownloadRemoteParamsDialog::httpParamListFinished()
{
    QLOG_DEBUG() << "DownloadRemoteParamsDialog::httpParamListFinished";

    QVariant redirectionTarget = m_networkReply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (m_networkReply->error()) {
        QMessageBox::information(this, tr("HTTP"),
                                 tr("Failed to retrive list: %1.")
                                 .arg(m_networkReply->errorString()));

    } else if (!redirectionTarget.isNull()) {
        QUrl newUrl = m_url.resolved(redirectionTarget.toUrl());
        if (QMessageBox::question(this, tr("HTTP"),
                                  tr("Redirect to %1 ?").arg(newUrl.toString()),
                                  QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_url = newUrl;
            m_networkReply->deleteLater();
            refreshParamList();
            return;
        }
    } else {
        // We have the JSON object from the server.
        m_paramListResponse = m_networkReply->readAll();
        QLOG_DEBUG() << "param list download response:" << m_paramListResponse;
    }


    // Clean up.
    disconnect(m_networkReply, SIGNAL(finished()), this, SLOT(httpParamListFinished()));
    disconnect(m_networkReply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));

    m_networkReply->deleteLater();
    m_networkReply = NULL;
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
