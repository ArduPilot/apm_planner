#include "QsLog.h"
#include "LogDownloadDialog.h"
#include "ui_LogDownloadDialog.h"
#include "UASManager.h"
#include <QMessageBox>

#define LDD_COLUMN_ID 0
#define LDD_COLUMN_TIME 1
#define LDD_COLUMN_SIZE 2
#define LDD_COLUMN_CHECKBOX 3
#define LOG_EXT QString(".bin")

static const uint LOG_PACKET_SIZE = 90;
static const int LOG_RETRY_TIMER = 700; //msecs

LogDownloadDescriptor::LogDownloadDescriptor(uint logID, uint time_utc,
                                             uint logSize)
{
    m_logID = logID;
    m_logTimeUTC = QDateTime::fromMSecsSinceEpoch(time_utc);
    // Set time to current UTC time if time is 1970 i.e. invalid.
    if(m_logTimeUTC.date().year() == 1970)
        m_logTimeUTC = QDateTime::currentDateTimeUtc();
    m_logSize = logSize;
}

const QString& LogDownloadDescriptor::logFilename()
{
    m_filename = m_logTimeUTC.toString("yy-MM-dd_hh-mm-ss") + LOG_EXT;
    return m_filename;
}

const QDateTime &LogDownloadDescriptor::logTimeUTC()
{
    return m_logTimeUTC;
}

uint LogDownloadDescriptor::logID()
{
    return m_logID;
}

uint LogDownloadDescriptor::logSize()
{
    return m_logSize;
}

LogDownloadDialog::LogDownloadDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDownloadDialog),
    m_uas(NULL),
    m_downloadSet(NULL),
    m_downloadFile(NULL),
    m_downloadID(0),
    m_downloadLastTimestamp(0),
    m_downloadOffset(0),
    m_lastDownloadOffset(0),
    m_downloadMaxSize(100)
{
    ui->setupUi(this); 

    ui->refreshPushButton->setEnabled(false);
    ui->getPushButton->setEnabled(false);

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(setActiveUAS(UASInterface*)));

    connect(ui->donePushButton, SIGNAL(clicked()), this, SLOT(doneButtonClicked()));
    connect(ui->cancelPushButton, SIGNAL(clicked()), this, SLOT(cancelButtonClicked()));
    connect(ui->refreshPushButton, SIGNAL(clicked()), this, SLOT(refreshList()));
    connect(ui->getPushButton, SIGNAL(clicked()), this, SLOT(getSelectedLogs()));
    connect(ui->erasePushButton, SIGNAL(clicked()), this, SLOT(eraseAllLogs()));
    connect(ui->checkAllBox, SIGNAL(clicked()), this, SLOT(checkAll()));

    // configure retransimission timer.
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(processDownloadedLogData()));

    QStringList headerList;
    headerList << tr("ID") << tr("Time") << tr("Size") << tr("Download?");

    QTableWidget* table = ui->tableWidget;
    table->setColumnCount(headerList.count());
    table->setHorizontalHeaderLabels(headerList);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setAlternatingRowColors(true);

    setActiveUAS(UASManager::instance()->getActiveUAS());
}

void LogDownloadDialog::resetDownload()
{
    if (m_downloadSet){
        delete m_downloadSet;
        m_downloadSet = NULL;
    }

    if (m_downloadFile){
        m_downloadFile->close();
        m_downloadFile->deleteLater();
        m_downloadFile = NULL;
    }

    m_downloadID = 0;
    m_downloadFilename.clear();
    m_downloadStart = QTime();
    m_downloadLastTimestamp = 0;
    m_downloadOffset = 0;
}

void LogDownloadDialog::cancelButtonClicked()
{
    if(m_uas){
        m_uas->logRequestEnd();
    }
    resetDownload();
    accept();
}

void LogDownloadDialog::doneButtonClicked()
{
    lower();
}

LogDownloadDialog::~LogDownloadDialog()
{
    removeConnections(m_uas);
    delete ui;
}

void LogDownloadDialog::setActiveUAS(UASInterface *uas)
{
    if (m_uas) {
        removeConnections(uas);
    }

    if (uas == NULL)
        return;

    m_uas = uas;
    setWindowTitle(tr("Log Download from MAV%1").arg(m_uas->getUASID()));
    ui->refreshPushButton->setEnabled(true);
    ui->getPushButton->setEnabled(true);
    makeConnections(uas);
}

void LogDownloadDialog::makeConnections(UASInterface *uas)
{
    Q_UNUSED(uas);
    connect(m_uas, SIGNAL(logEntry(int,uint32_t,uint32_t,uint16_t,uint16_t,uint16_t)),
            this, SLOT(logEntry(int,uint32_t,uint32_t,uint16_t,uint16_t,uint16_t)));
    connect(m_uas, SIGNAL(logData(uint32_t,uint32_t,uint16_t,uint8_t,const char*)),
            this, SLOT(logData(uint32_t,uint32_t,uint16_t,uint8_t,const char*)));
}

void LogDownloadDialog::removeConnections(UASInterface *uas)
{
    Q_UNUSED(uas);
    disconnect(m_uas, SIGNAL(logEntry(int,uint32_t,uint32_t,uint16_t,uint16_t,uint16_t)),
            this, SLOT(logEntry(int,uint32_t,uint32_t,uint16_t,uint16_t,uint16_t)));
    disconnect(m_uas, SIGNAL(logData(uint32_t,uint32_t,uint16_t,uint8_t,const char*)),
            this, SLOT(logData(uint32_t,uint32_t,uint16_t,uint8_t,const char*)));
    ui->refreshPushButton->setEnabled(false);
    ui->getPushButton->setEnabled(false);
}

void LogDownloadDialog::refreshList()
{
    QLOG_DEBUG() << "Start Log List Download";
    if (m_uas){
        ui->tableWidget->setRowCount(0);
        m_logEntriesList.clear();
        m_uas->logRequestList(0,0xffff); // Currently list all available logs
    }
}

void LogDownloadDialog::getSelectedLogs()
{
    QLOG_DEBUG() << "Start Retrieving selected logs";
    QTableWidget* table = ui->tableWidget;
    m_fileSaveList.clear();

    for(int rowCount = 0; rowCount < table->rowCount(); ++rowCount){
        QTableWidgetItem* paramCheck= table->item(rowCount, LDD_COLUMN_CHECKBOX);

        if (paramCheck->checkState() == Qt::Checked){
            m_fileSaveList.append(m_logEntriesList[rowCount]);
            QLOG_DEBUG() << "Adding id:" << m_fileSaveList.last()->logID() << " "
                         << m_fileSaveList.last()->logFilename()  << " to download list";
        }
    }
    m_downloadCount = 1;//st
    m_downloadCountMax = m_fileSaveList.count();
    startNextDownloadRequest();
}

void LogDownloadDialog::eraseAllLogs()
{
   if (m_uas == NULL)
       return;

   int button = QMessageBox::critical(this, tr("Erase All Logs"),
                                 tr("Are you sure you want to earse all logs?")
                                 ,QMessageBox::Ok,QMessageBox::Cancel);
   if(button == QMessageBox::Ok){
       m_uas->logEraseAll();
   }
}

void LogDownloadDialog::startNextDownloadRequest()
{
    QTimer::singleShot(500, this, SLOT(triggerNextDownloadRequest()));
}

void LogDownloadDialog::triggerNextDownloadRequest()
{
    QLOG_DEBUG() << "Start next log download";
    if (m_uas && !m_fileSaveList.isEmpty()){
        resetDownload();
        LogDownloadDescriptor *descriptor = m_fileSaveList.takeFirst();
        m_downloadID = descriptor->logID();
        m_downloadFilename = descriptor->logFilename();
        m_downloadMaxSize = descriptor->logSize();
        issueDownloadRequest();
    }
}

void LogDownloadDialog::issueDownloadRequest()
{
        // Open a file for the log to be downloaded
        m_downloadFile = new QFile(QGC::logDirectory() +"/" + m_downloadFilename);
        if(m_downloadFile && m_downloadFile->open(QIODevice::WriteOnly)){
            QLOG_INFO() << "Log file ready for writing:" << m_downloadFilename << " size:" << m_downloadMaxSize;
            Q_ASSERT(m_downloadOffset == 0);
            m_lastDownloadOffset = 0;
            m_uas->logRequestData(m_downloadID, m_downloadOffset, LOG_PACKET_SIZE);
            updateProgress();
            m_downloadStart.start();
            m_timer.start(LOG_RETRY_TIMER);
            m_downloadSet = new QSet<uint>();
        } else {
            QLOG_ERROR() << "failed to open file to save log:" << m_downloadFilename;
        }
}


void LogDownloadDialog::logEntry(int uasId, uint32_t time_utc, uint32_t size, uint16_t id,
                                 uint16_t num_logs, uint16_t last_log_num)
{
    QDateTime time = QDateTime::fromMSecsSinceEpoch(time_utc);
    QLOG_DEBUG() << "New Entry: uas=" << uasId << " time=" << time.toString() << " size=" << size <<
                       "id=" << id << " num_logs=" << num_logs << ", last_log_num=" << last_log_num;
    ui->tableWidget->setSortingEnabled(false);

    if (m_uas == NULL)
        return;

    LogDownloadDescriptor *logItem = new LogDownloadDescriptor(id, time_utc, size);
    m_logEntriesList.append(logItem);

    // append a new row.
    ui->tableWidget->setRowCount(m_logEntriesList.size());
    int rowCount = m_logEntriesList.size() - 1; // index starts at 0

    QTableWidgetItem *item = new QTableWidgetItem( QString::number(logItem->logID()));
    item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled );
    ui->tableWidget->setItem(rowCount, LDD_COLUMN_ID, item);

    item = new QTableWidgetItem();
    item->setData(Qt::DisplayRole, logItem->logTimeUTC().toString());
    item->setFlags(Qt::NoItemFlags  | Qt::ItemIsEnabled);
    ui->tableWidget->setItem(rowCount, LDD_COLUMN_TIME, item);

    item = new QTableWidgetItem(QString::number(logItem->logSize()));
    item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
    ui->tableWidget->setItem(rowCount, LDD_COLUMN_SIZE, item);

    item = new QTableWidgetItem();
    item->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Checked);
    ui->tableWidget->setItem(rowCount, LDD_COLUMN_CHECKBOX, item);
}

void LogDownloadDialog::logData(uint32_t uasId, uint32_t ofs, uint16_t id,
                                     uint8_t count, const char *data)
{
//#define SIMULATE_PACKET_LOSS
#ifdef SIMULATE_PACKET_LOSS
    QLOG_DEBUG() << "logData ofs:" << ofs << " id:" << id << " count:" << count
                 /*<< " data:" << data*/;
#endif
    if (m_uas == NULL || m_downloadSet == NULL)
        return;
    if (m_uas->getUASID() != static_cast<int>(uasId))
        return;
    if(m_downloadFile == NULL){
        QLOG_ERROR() << "No file open to save log info";
        return;
    }
#ifdef SIMULATE_PACKET_LOSS
    //Simulate packet loss for testing in debug mode
    if ((rand() % 100) < 20){
        QLOG_DEBUG() << "Dropping Packet logId=" << id << " packetId=" << (int)ofs/90 << " ofs=" << ofs;
        return;
    }
#endif
    if (ofs != m_downloadOffset){
        m_downloadFile->seek(ofs);
        m_downloadOffset = ofs;
        updateProgress();
    }
    if (count != 0){
        int bytesWritten = m_downloadFile->write(data, count);
        if ((bytesWritten == -1)||(bytesWritten != count)){
            QLOG_ERROR() << "Log File write bytesWritten:" << bytesWritten << "out of: count=" << count;
//            QMessageBox::critical(this,tr("Corrupt File")
//                                  ,tr("Error saving %1.\n file save is corupt."),QMessageBox::Ok);
            // [TODO] Abort.
        }
        m_downloadFile->flush();
        m_downloadSet->insert(ofs / LOG_PACKET_SIZE);
        m_downloadOffset += count;
        updateProgress();
    }
    m_downloadLastTimestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if ( count==0 || (count<LOG_PACKET_SIZE && (m_downloadSet->count() == 1+(ofs/LOG_PACKET_SIZE))) ){
        double dt = m_downloadStart.elapsed()/1000.0;
        double speed = (static_cast<double>(m_downloadFile->size())/dt)/1000.0;
        QLOG_INFO() << "Finished downloading "<< m_downloadFilename
                    << "(" << dt << " seconds, "<< speed <<"kbyte/sec)";
        m_downloadFile->flush();
        m_timer.stop();
        updateProgress();
        m_uas->logRequestEnd();
        QCoreApplication::processEvents();

        if (m_downloadFile->size() == 0) {
            // If the file size is zero, retry
            QLOG_DEBUG() << "File Size is zero, retry";
            m_downloadOffset = 0;
            m_lastDownloadOffset = 0;
            delete m_downloadSet;
            m_downloadSet = new QSet<uint>();
            m_timer.start(LOG_RETRY_TIMER);
            m_downloadStart.start();
            m_downloadFile->close();
            delete m_downloadFile;
            m_downloadFile = NULL;
            issueDownloadRequest();
            return;
        }
        m_downloadFile->close();
        ui->progressBar->setValue(m_downloadMaxSize);
        if (!(m_downloadCount == m_downloadCountMax)){
            m_downloadCount++;
            startNextDownloadRequest();
        } else {
            ui->statusLabel->setText("Finished");
            QTimer::singleShot(500, ui->progressBar, SLOT(hide()));
            QTimer::singleShot(500, ui->statusLabel, SLOT(hide()));
            resetDownload();
        }
    }
}

void LogDownloadDialog::processDownloadedLogData()
{
    // Process incoming log data and trigger next request for more
    if (m_downloadSet==NULL || m_downloadSet->size() == 0)
        return;

    uint highest = 0;

    QSet<uint>::const_iterator iter;
    iter = m_downloadSet->constBegin();
    while (iter != m_downloadSet->constEnd()){
        uint value = *iter;
        if (value > highest)
            highest = value;
        ++iter;
    }

    QList<uint> diff;
    for( uint count = 0; count < highest; count++){
        if(!m_downloadSet->contains(count))
            diff.append(count);
    }
    QLOG_DEBUG() << "highest:"<< highest << "Diff size:" << diff.size();
    if(diff.size() == 0){
        m_uas->logRequestData(m_downloadID, (highest+1)*LOG_PACKET_SIZE, 0xFFFFFFFF);
        m_timer.start(LOG_RETRY_TIMER);

    } else {
        int num_requests = 0;
        while (num_requests < 20){
            uint start = diff.takeFirst();
            uint end = start;
            while(diff.contains(end+1)){
                end+=1;
                diff.removeAt(end);
            }
            ui->statusLabel->setText(tr("Retransmitting %1").arg(start));
            m_uas->logRequestData(m_downloadID, (start*LOG_PACKET_SIZE), (end+1-start)*LOG_PACKET_SIZE);
            num_requests +=1;
            if(diff.size() == 0)
                break;
        }
    }
}

void LogDownloadDialog::updateProgress()
{
    if ( m_downloadOffset > m_lastDownloadOffset + 720){
        m_lastDownloadOffset = m_downloadOffset;
        QString status =  QString("Downloading %1/%2").arg(m_downloadCount).arg(m_downloadCountMax);
        ui->statusLabel->setText(status);
        ui->statusLabel->show();
        ui->progressBar->setMaximum(m_downloadMaxSize);
        ui->progressBar->setValue(m_downloadOffset);
        ui->progressBar->show();
    }
}

void LogDownloadDialog::checkAll()
{
    QLOG_DEBUG() << " check uncheck all parameters";
    QTableWidget* table = ui->tableWidget;

    if(ui->checkAllBox->isChecked()){
        for(int rowCount = 0; rowCount < table->rowCount(); ++rowCount){
            QTableWidgetItem* item = table->item(rowCount, LDD_COLUMN_CHECKBOX);
            if (item) item->setCheckState(Qt::Checked);
        }
    } else {
        for(int rowCount = 0; rowCount < table->rowCount(); ++rowCount){
            QTableWidgetItem* item = table->item(rowCount, LDD_COLUMN_CHECKBOX);
            if (item) item->setCheckState(Qt::Unchecked);
        }
    }
}
