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

    void setStatusText(QString text);
    QString getDownloadedFileName();

public slots:
    void refreshParamList();
    void downloadButtonClicked();
    void loadFileButtonClicked();
    void loadFileDialogAccepted();
    void closeButtonClicked();
    void httpParamListFinished();

    bool downloadParamFile();
    void cancelDownload();
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64 bytesRead, qint64 totalBytes);

private:
    void startFileDownloadRequest(QUrl url);
    void startParamListDownloadRequest(QUrl url);
    void manualListSetup();
    void processDownloadedVersionObject(const QByteArray& data);

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
