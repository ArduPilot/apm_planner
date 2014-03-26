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
    explicit LogDownloadDescriptor(uint logID, const QString& filename,
                                   uint size);
    QString logFilename();
    uint logID();
    uint logSize();

private:
    uint m_logID;
    QString m_filename;
    uint m_logSize;
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
    void logData(uint32_t uasId, uint32_t ofs, uint16_t id, uint8_t count, uint8_t data[]);

private slots:
    void checkAll();
    void processDownloadedLogData();

private:
    void removeConnections(UASInterface* uas);
    void makeConnections(UASInterface* uas);
    void startNextDownloadRequest();

    void resetDownload();

private:
    Ui::LogDownloadDialog *ui;
    UASInterface *m_uas;
    QList<LogDownloadDescriptor> m_fileSaveList; // id & filename to save data to.

    QSet<uint> *m_downloadSet;
    QFile* m_downloadFile;
    uint m_downloadID;
    QString m_downloadFilename;
    uint m_downloadStart;
    uint m_downloadLastTimestamp;
    uint m_downloadOffset;
    QTimer m_timer;
};

#endif // LOGDOWNLOADDIALOG_H
