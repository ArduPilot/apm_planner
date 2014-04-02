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
