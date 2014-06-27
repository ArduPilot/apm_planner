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
