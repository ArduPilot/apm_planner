#ifndef AUTOUPDATEDIALOG_H
#define AUTOUPDATEDIALOG_H

#include <QDialog>
#include <QtNetwork>

namespace Ui {
class AutoUpdateDialog;
}

class AutoUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoUpdateDialog(const QString& version, const QString& targetFilename,
                              const QString& url, QWidget *parent = 0);
    ~AutoUpdateDialog();

signals:
    void autoUpdateCancelled(QString version);

public slots:
    void yesClicked();
    void noClicked();
    void skipClicked();
    bool skipVersion();

    bool startDownload(const QString& url, const QString &filename);
    void startFileDownloadRequest(QUrl url);
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

    void dmgMounted(int result, QProcess::ExitStatus exitStatus);
    void executeDownloadedFile();

private:
    Ui::AutoUpdateDialog *ui;

    QUrl m_url;
    QString m_sourceUrl;
    QString m_targetFilename;
    QNetworkAccessManager m_networkAccessManager;
    QNetworkReply* m_networkReply;
    QFile* m_targetFile;
    bool m_httpRequestAborted;
    bool m_skipVersion;
    QString m_skipVersionString;
};

#endif // AUTOUPDATEDIALOG_H
