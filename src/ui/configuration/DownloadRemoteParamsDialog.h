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
    explicit DownloadRemoteParamsDialog(QWidget *parent = 0);
    ~DownloadRemoteParamsDialog();

    void startFileDownloadRequest(QUrl url);

    void setStatusText(QString text);
public slots:
    void refreshParamList();
    void okButtonClicked();
    void closeButtonClicked();
    void httpParamListFinished();

    void downloadParamFile();
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    Ui::DownloadRemoteParamsDialog *ui;

    QUrl m_url;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    QFile* m_downloadedParamFile;
    bool m_httpRequestAborted;
    QByteArray m_paramListResponse;
    QStringList m_paramUrls;
};

#endif // DOWNLOADREMOTEPARAMSDIALOG_H
