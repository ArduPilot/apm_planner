#ifndef DOWNLOADREMOTEPARAMSDIALOG_H
#define DOWNLOADREMOTEPARAMSDIALOG_H

#include <QDialog>
#include <QtNetwork>
#include <QListWidgetItem>

namespace Ui {
class DownloadRemoteParamsDialog;
}

class DownloadRemoteParamsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DownloadRemoteParamsDialog(QWidget *parent = 0, bool overwriteFile = false);
    ~DownloadRemoteParamsDialog();

    void startFileDownloadRequest(QUrl url);
    void setStatusText(QString text);
    QString getDownloadedFileName();

public slots:
    void refreshParamList();
    void downloadButtonClicked();
    void loadFileButtonClicked();
    void closeButtonClicked();
    void httpParamListFinished();

    bool downloadParamFile();
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    Ui::DownloadRemoteParamsDialog *ui;

    QString m_locationOfFrameParams;
    QString m_extension;
    QString m_version;

    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    QFile* m_downloadedParamFile;
    bool m_httpRequestAborted;
    QByteArray m_paramListResponse;
    QStringList m_paramUrls;
    QString m_downloadedFileName;
    bool m_overwriteFile;
};

#endif // DOWNLOADREMOTEPARAMSDIALOG_H
