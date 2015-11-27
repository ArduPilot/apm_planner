#include "QsLog.h"
#include "MainWindow.h"
#include "LogConsole.h"
#include "ui_LogConsole.h"
#include "kmlcreator.h"

#include <QFile>
#include <QDateTime>
#include <QtSerialPort/qserialport.h>
#include <QRegExp>

static const long
    KB = 1024
,   MB = KB * 1024
;

using namespace kml;

static bool writeSerial(QSerialPort* port, const char *s) {
    // QLOG_DEBUG() << "writeSerial(): cmd=" << s;

    QMutex mutex;
    QMutexLocker locker(&mutex);

    int written = port->write(s);
    bool didWrite = port->waitForBytesWritten(-1);

    bool fail = ((!didWrite) || (written == -1));

    if(fail) {
        QLOG_DEBUG() << "TX error: didWrite=" << didWrite;
    }
    else {
        QLOG_DEBUG() << "wrote " << written << " bytes";
    }

    return (!fail);
}

Worker::Worker(QSerialPort* port, QList<LogConsole::FileData>& fileData):
    m_state(start),
    m_port(port),
    m_files(fileData),
    m_fdIndex(-1),
    m_blanks(0),
    m_run(true)
    {}

Worker::~Worker() {}

void Worker::process() {
    connect(m_port, SIGNAL(readyRead()), this, SLOT(readData()));

    m_fdIndex = -1;
    if(m_files.size() > 0) {
        onStartNextFile();
    }
    else {
        QLOG_WARN() << "No files specified to extract.";
        onFinishAll();
    }
}

void Worker::readyNextFile() {
    onStartNextFile();
}

void Worker::onStartNextFile() {
    int idx = m_fdIndex + 1;

    if(idx < m_files.size()) {
        m_fdIndex = idx;
        QString filename = m_files[m_fdIndex].filename;
        QLOG_DEBUG() << "Worker: create " << filename;
        m_blanks = 0;
        emit startFile(filename);

        m_logLines.clear();
        m_totalBytesRead = 0L;
        m_state = start;

        QString cmd = QString("dump %1\r\n").arg(QString::number(m_files[m_fdIndex].index));
        writeSerial(m_port, cmd.toLocal8Bit().data());
    }
    else {
        onFinishAll();
    }
}

void Worker::onFinishFile() {

    QFile file(m_files[m_fdIndex].filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);

    out << QString::number(m_files[m_fdIndex].index) << "\r\n";

    KMLCreator kml;

    if(m_generateKml) {
        QString fn = m_files[m_fdIndex].filename;
        QString kmlfile = fn.replace(fn.indexOf(".log"), 4, ".kml");
        kml.start(kmlfile);
    }

    foreach(QString line, m_logLines) {
        out << line << "\r\n";

        if(m_generateKml) {
            kml.processLine(line);
        }
    }

    if(m_generateKml) {
        kml.finish(true);
    }

    emit finishFile(m_files[m_fdIndex].filename);
}

void Worker::onFinishAll() {
    disconnect(m_port, SIGNAL(readyRead()), this, SLOT(readData()));
    emit finishAll();
}

void Worker::onLineRead(char *data) {
    if(!m_run) {
        return;
    }

    QString line = QString(data).trimmed();

    switch(m_state) {
        case start: {
            emit statusMsg("Waiting for log data...");

            if(line.startsWith("Ardu")) {
                QLOG_DEBUG() << "Starting read";

                m_state = reading;

                m_logLines.append(line);
                emit bytesRead(m_totalBytesRead += line.length());
            }

            break;
        }

        case reading: {
            if(line.startsWith("Log] logs enabled")) {
                QLOG_DEBUG() << "Finishing read";

                m_state = finish;

                onFinishFile();

                QTimer *timer = new QTimer();
                connect(timer, SIGNAL(timeout()), this, SLOT(readyNextFile()));
                connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
                timer->start(1000);
            }
            else {
                m_logLines.append(line);
                emit bytesRead(m_totalBytesRead += line.length());
            }

            break;
        }

        default: {
            break;
        }
    }
}

void Worker::readData() {
    char line[256];

    while(m_port->canReadLine() && m_run) {
        int len = m_port->readLine(line, 256);
        if(len < 0) {
            QLOG_WARN() << "read error";
        }
        else if(len == 0) {
            QLOG_DEBUG() << "Nothing read";
        }
        else {
            onLineRead(line);
        }

        if(!m_run) {
            onCancel();
            break;
        }
    }
}

void Worker::onCancel() {
    QLOG_DEBUG() << "onCancel()";
    // Happens when the worker is cancelled
    disconnect(m_port, SIGNAL(readyRead()), this, SLOT(readData()));
    emit cancelled();
}

//
// Main UI for the log console
//
LogConsole::LogConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogConsole),
    m_serial(0),
    m_pullMode(None),
    m_worker(0) {
    ui->setupUi(this);
    initConnections();
    setButtonEnabledStates();

    QTableWidget* table = ui->logsTable;
    table->setRowCount(0);
    table->setColumnCount(1);
    table->setColumnWidth(0, table->width());
    table->setAlternatingRowColors(true);
    table->verticalHeader()->hide();
    table->horizontalHeader()->hide();
}

LogConsole::~LogConsole() {
    delete ui;
}

void LogConsole::setSerial(QSerialPort *port) {

    if(!port) {
        disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    }

    m_serial = port;

    bool up = (m_serial && m_serial->isOpen());

    ui->pullLogsButton->setEnabled(up);
    ui->eraseLogsButton->setEnabled(up);
}

void LogConsole::onShow(bool shown) {
    if(shown) {
        if(!m_serial) {
            QLOG_WARN() << "No serial port!";
            return;
        }

        // Get to the logs prompt
        connect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    }
    else {
        disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    }

    setButtonEnabledStates();
}

void LogConsole::readData() {
    QWaitCondition waitCondition;
    QMutex mutex;

    if(m_serial) {
        char buf[256];

        while(m_serial->canReadLine()) {
            m_serial->readLine(buf, 256);
            QByteArray data(buf);
            processLine(data);
        }
    }
    else {
        QLOG_INFO() << "No serial port in LogConsole::readData()";
    }
}

void LogConsole::processLine(QByteArray &data) {
    QString line(QString(data).trimmed());

    switch(m_pullMode) {
        case ListLogs: {
            // start of the data
            if(line.indexOf("logs enabled:") >= 0) {
                clearListItems();
            }
            else {
                // If a "Log" line
                if(line.indexOf("Log ") >= 0) {
                    addListItem(line);
                }

                // Back at the prompt, got all the names
                if(line.indexOf("Log]") >= 0) {
                    m_pullMode = None;
                }

                setButtonEnabledStates();
            }

            break;
        }

        default: {
            break;
        }
    }
}

void LogConsole::initConnections() {
    connect(ui->selectAllButton, SIGNAL(released()), this, SLOT(selectAllClicked()));
    connect(ui->selectNoneButton, SIGNAL(released()), this, SLOT(selectNoneClicked()));
    connect(ui->pullLogsButton, SIGNAL(released()), this, SLOT(pullSelectedClicked()));
    connect(ui->eraseLogsButton, SIGNAL(released()), this, SLOT(eraseLogsClicked()));
    connect(ui->refreshButton, SIGNAL(released()), this, SLOT(refreshClicked()));
    connect(ui->stopWorkerButton, SIGNAL(released()), this, SLOT(stopClicked()));
}

void LogConsole::selectAllClicked() {
    ui->logsTable->selectAll();
    setButtonEnabledStates();
}

void LogConsole::selectNoneClicked() {
    ui->logsTable->clearSelection();
    setButtonEnabledStates();
}

void LogConsole::pullSelectedClicked() {
    QDateTime now = QDateTime::currentDateTime();
    QString parentDir = QGC::logDirectory();

    QList<FileData> files;

    QTableWidget *table = ui->logsTable;
    QModelIndexList list = table->selectionModel()->selection().indexes();

    const int size = list.count();
    for(int i = 0; i < size; ++i) {
        QModelIndex idx = list.at(i);
        QRegExp matchLogNumber("^Log (\\d+)");
        QString logDetails = idx.data(Qt::DisplayRole).toString();
        int pos = matchLogNumber.indexIn(logDetails);
        if (pos > -1) {
            QRegExp matchDate("\\d+/\\d+/\\d+ \\d+:\\d+$");
            int pos = matchDate.indexIn(logDetails);
            if (pos > -1) {
                QString dateString(logDetails);
                dateString = dateString.remove(0, pos);
                dateString = dateString.replace("/","-");
                dateString = dateString.replace(" ","_");
                dateString = dateString.replace(":","-");
                int logNumber = matchLogNumber.cap(1).toInt();
                QString fn = QString("%1/%2_%3.log").arg(parentDir, dateString, QString::number(logNumber));
                files.append(FileData(fn, logNumber));
            } else {
                QString todayString = now.toString("yyyy-MM-dd hh-mm-ss");
                int logNumber = matchLogNumber.cap(1).toInt();
                QString fn = QString("%1/%2_%3.log").arg(parentDir, todayString, QString::number(logNumber));
                files.append(FileData(fn, logNumber));
            }
        }
    }

    disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));

    if(files.size() > 0) {
        QThread* thread = new QThread();
        m_worker = new Worker(m_serial, files);
        m_worker->generateKml(ui->generateKmlCheck->isChecked());
        m_worker->moveToThread(thread);
        connect(m_worker, SIGNAL(error(QString)), this, SLOT(dumpError(QString)));
        connect(m_worker, SIGNAL(startFile(QString)), this, SLOT(dumpFileStart(QString)));
        connect(m_worker, SIGNAL(finishFile(QString)), this, SLOT(dumpFileFinish(QString)));
        connect(m_worker, SIGNAL(bytesRead(long)), this, SLOT(dumpFileBytesRead(long)));
        connect(m_worker, SIGNAL(statusMsg(QString)), this, SLOT(dumpFileStatusMsg(QString)));
        connect(m_worker, SIGNAL(finishAll()), m_worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(started()), m_worker, SLOT(process()));
        connect(m_worker, SIGNAL(finishAll()), thread, SLOT(quit()));
        connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        connect(m_worker, SIGNAL(finishAll()), this, SLOT(workerStopped()));
        connect(m_worker, SIGNAL(cancelled()), this, SLOT(workerCancelled()));
        thread->start();
    }

    setButtonEnabledStates();
}

void LogConsole::dumpFileStart(QString file) {
    m_currentFile = file;
    QString str = QString("Writing %1").arg(m_currentFile);
    emit activityStart();
    emit statusMessage(str);
}

void LogConsole::dumpFileFinish(QString file) {
    m_currentFile.clear();
    QString str = QString("Completed %1").arg(file);
    emit statusMessage(str);
}

void LogConsole::dumpFileBytesRead(long bytes) {
    QString sz("bytes");
    double d = bytes;
    double div = 1;

    if(bytes > MB) {
        div = MB;
        sz = "MiB";
    }
    else if(bytes > KB) {
        div = KB;
        sz = "K";
    }

    QString str = QString("%1: %2 %3").arg(m_currentFile, QString::number(d / div, 'f', 2), sz);
    emit statusMessage(str);
}

void LogConsole::dumpFileStatusMsg(QString msg) {
    emit statusMessage(msg);
}

void LogConsole::workerStopped() {
    m_worker = 0;
    setButtonEnabledStates();
    emit activityStop();
}

void LogConsole::workerCancelled() {
    emit statusMessage(QString("Canceled"));
    workerStopped();
}

void LogConsole::dumpError(QString err) {
    QLOG_ERROR() << "dumpError: err=" << err;
}

void LogConsole::eraseLogsClicked() {
    if(QMessageBox::question(this, "Erase Logs", "Clear the logs from the UAV?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        writeSerial("erase\r\n\r\n");
        doDelayedRefresh();
    }
}

void LogConsole::doDelayedRefresh() {
    QTimer* timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(refreshClicked()));
    connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
    timer->start(1000);
}

void LogConsole::refreshClicked() {
    ui->logsTable->clear();
    ui->logsTable->setRowCount(0);
    m_pullMode = ListLogs;
    writeSerial("exit\r\nlogs\r\n\r\n");
}

void LogConsole::stopClicked() {
    if(m_worker) {
        if(QMessageBox::question(this, "Stop", "Stop pulling log data?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            m_worker->stop();
        }
    }
}

QStringList LogConsole::getSelectedItems() {
    QStringList selected;
    QTableWidget *table = ui->logsTable;

    QModelIndexList list = table->selectionModel()->selection().indexes();
    const int size = list.count();

    for(int i = 0; i < size; ++i) {
        QModelIndex idx = list.at(i);
        QTableWidgetItem *item = table->item(idx.row(), idx.column());
        selected.append(item->text());
    }

    return selected;
}

int LogConsole::countSelected() {
    return getSelectedItems().size();
}

void LogConsole::setButtonEnabledStates() {
    int count = ui->logsTable->rowCount();
    int selSize = countSelected();
    bool working = (m_worker);

    ui->pullLogsButton->setEnabled((count > 0) && (selSize > 0) && (!working));
    ui->generateKmlCheck->setEnabled((count > 0) && (selSize > 0) && (!working));
    ui->eraseLogsButton->setEnabled((count > 0) && (!working));
    ui->selectAllButton->setEnabled((count > 0) && (!working));
    ui->selectNoneButton->setEnabled((count > 0) && (!working));
    ui->stopWorkerButton->setEnabled(working);
    ui->refreshButton->setEnabled(!working);
}

bool LogConsole::writeSerial(const char *s) {
    return ::writeSerial(m_serial, s);
}

void LogConsole::clearListItems() {
    ui->logsTable->clear();
}

void LogConsole::addListItem(QString &str) {
    QTableWidget* table = ui->logsTable;
    int rowCount = table->rowCount();
    table->setRowCount(rowCount+1);
    table->setColumnWidth(0, table->width());

    QTableWidgetItem *item = new QTableWidgetItem(str);
    item->setFlags(Qt::NoItemFlags | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    table->setItem(rowCount,0,item);

    connect(table, SIGNAL(cellActivated(int,int)), this, SLOT(listItemCheckChanged()));
    connect(table, SIGNAL(cellClicked(int,int)), this, SLOT(listItemCheckChanged()));
}

void LogConsole::listItemCheckChanged() {
    setButtonEnabledStates();
}

