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
#ifndef LOGDOWNLOADDIALOG_H
#define LOGDOWNLOADDIALOG_H

#include "UASInterface.h"
#include <QDialog>

namespace Ui {
class LogDownloadDialog;
}

class LogDownloadDescriptor
{
public:
    explicit LogDownloadDescriptor(uint logID, uint time_utc,
                                   uint size);
    const QString &logFilename();
    const QDateTime &logTimeUTC();
    uint logID();
    uint logSize();

private:
    uint m_logID;
    QDateTime m_logTimeUTC;
    uint m_logSize;
    QString m_filename;
};

class LogDownloadDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDownloadDialog(QWidget *parent = 0);
    ~LogDownloadDialog();

public slots:
    void setActiveUAS(UASInterface* uas);

    void refreshList();
    void getSelectedLogs();

    // Log Download Signals
    void logEntry(int uasId, uint32_t time_utc, uint32_t size, uint16_t id, uint16_t num_logs, uint16_t last_log_num);
    void logData(uint32_t uasId, uint32_t ofs, uint16_t id, uint8_t count, const char* data);

private slots:
    void checkAll();
    void processDownloadedLogData();
    void doneButtonClicked();
    void cancelButtonClicked();
    void triggerNextDownloadRequest();
    void eraseAllLogs();

private:
    void removeConnections(UASInterface* uas);
    void makeConnections(UASInterface* uas);
    void startNextDownloadRequest();
    void issueDownloadRequest();

    void updateProgress();
    void resetDownload();

private:
    Ui::LogDownloadDialog *ui;
    UASInterface *m_uas;
    QList<LogDownloadDescriptor*> m_logEntriesList; // id & filename to save data to.
    QList<LogDownloadDescriptor*> m_fileSaveList; // id & filename to save data to.

    QSet<uint> *m_downloadSet;
    QFile* m_downloadFile;
    uint m_downloadID;
    QString m_downloadFilename;
    QTime m_downloadStart;
    uint m_downloadLastTimestamp;
    uint m_downloadOffset;
    uint m_lastDownloadOffset;
    uint m_downloadMaxSize;
    int m_downloadCount;
    int m_downloadCountMax;
    QTimer m_timer;
};

#endif // LOGDOWNLOADDIALOG_H
