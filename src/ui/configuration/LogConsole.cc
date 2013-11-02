#include "QsLog.h"
#include "MainWindow.h"
#include "LogConsole.h"
#include "ui_LogConsole.h"

#include <qfile.h>
#include <qdatetime.h>
#include <qcheckbox.h>

#include <qserialport.h>
#include <qregexp.h>

static QStringList toLines(QByteArray data) {
    QString str(QString(data).trimmed());
    QStringList list = str.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    return list;
}

Worker::Worker(QSerialPort* port, QList<LogConsole::FileData>& fileData):
    mPort(port), mFiles(fileData), mRun(true) {}

Worker::~Worker() {

}

void Worker::process() {
    // Walk through the files we want to extract,
    // and stream each one from the serial port to
    // a file on the file system.
    const int count = mFiles.size();
    for(int i = 0; i < count; ++i) {
        LogConsole::FileData fd = mFiles[i];
        QFile file(fd.filename);
        QLOG_DEBUG() << "Worker: create " << file.fileName();
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);

        QString filename = file.fileName();
        emit startFile(filename);

        QWaitCondition waitCondition;
        QMutex mutex;

        QString cmd = QString("dump %1\r").arg(QString::number(fd.index));
        mPort->write(cmd.toLocal8Bit().data());

        waitCondition.wait(&mutex, 800);

        QByteArray data = mPort->readAll();
        long total = data.length();

        int blanks = 0;
        while(total > 0 && mRun) {
            QStringList lines = toLines(data);

            foreach(QString line, lines) {
                if(line.indexOf("dump ") >= 0) {
                    line = QString::number(fd.index);
                    line += "\r\n";
                }

                if(
                    line.indexOf("logs enabled: ") >= 0 ||
                    line.indexOf("Log ") >= 0 ||
                    line.indexOf(" logs") >= 0 ||
                    line.indexOf("]") > 0) {
                    line.clear();
                }

                if(line.length() > 0) {
                    out << line << "\r\n";
                }
            }

            data = mPort->readAll();
            int len = data.length();

            if(len == 0 && mRun) {
                QLOG_DEBUG() << "Wait for data from the serial port";
                waitCondition.wait(&mutex, 1000);
                data = mPort->readAll();

                len = data.length();
                if(len == 0) {
                    ++blanks;
                }
            }
            else {
                blanks = 0;
            }

            if(blanks >= 10) {
                QLOG_WARN() << "No data from the serial port. Bailing out";
                break;
            }

            emit bytesRead(total);
            total += data.length();
        }

        filename = file.fileName();
        emit finishFile(filename);
    } // for

    emit finishAll();
}

//
// Main UI for the log console
//
LogConsole::LogConsole(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogConsole),
    m_serial(0),
    m_pullMode(None),
    m_Worker(0)
{
    ui->setupUi(this);
    initConnections();
    setButtonEnabledStates();
}

LogConsole::~LogConsole()
{
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
        refreshClicked();
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
        int blanks = 0;
        while(m_pullMode != None) {
            QByteArray data = m_serial->readLine(256);
            if(data.length() == 0) {
                waitCondition.wait(&mutex, 100);
                data = m_serial->readLine(256);

                if(data.length() == 0) {
                    ++blanks;
                }
                else {
                    processLine(data);
                }
            }
            else {
                processLine(data);
            }

            if(blanks >= 10) {
                // No more data
                m_pullMode = None;
                break;
            }
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
    const int size = ui->logList->count();
    for(int i = 0; i < size; ++i) {
        QListWidgetItem* item = ui->logList->item(i);
        QCheckBox* b = static_cast<QCheckBox*>(ui->logList->itemWidget(item));
        b->setChecked(true);
    }

    setButtonEnabledStates();
}

void LogConsole::selectNoneClicked() {
    const int size = ui->logList->count();
    for(int i = 0; i < size; ++i) {
        QListWidgetItem* item = ui->logList->item(i);
        QCheckBox* b = static_cast<QCheckBox*>(ui->logList->itemWidget(item));
        b->setChecked(false);
    }

    setButtonEnabledStates();
}

void LogConsole::pullSelectedClicked() {
    QDateTime now = QDateTime::currentDateTime();
    QString parentDir = MainWindow::instance()->getLogDirectory();
    QString todayString = now.toString("yyyy-MM-dd hh-mm-ss");

    QList<FileData> files;

    const int size = ui->logList->count();
    for(int i = 0; i < size; ++i) {
        QCheckBox* b = static_cast<QCheckBox*>(ui->logList->itemWidget(ui->logList->item(i)));
        if(b->isChecked()) {
            QString fn = QString("%1/%2 %3.log").arg(parentDir, todayString, QString::number(i + 1));
            files.append(FileData(fn, i + 1));
        }
    }

    disconnect(m_serial, SIGNAL(readyRead()), this, SLOT(readData()));

    if(files.size() > 0) {
        QThread* thread = new QThread();
        m_Worker = new Worker(m_serial, files);
        m_Worker->moveToThread(thread);
        connect(m_Worker, SIGNAL(error(QString)), this, SLOT(dumpError(QString)));
        connect(m_Worker, SIGNAL(startFile(QString)), this, SLOT(dumpFileStart(QString)));
        connect(m_Worker, SIGNAL(finishFile(QString)), this, SLOT(dumpFileFinish(QString)));
        connect(m_Worker, SIGNAL(bytesRead(long)), this, SLOT(dumpFileBytesRead(long)));
        connect(m_Worker, SIGNAL(finishAll()), m_Worker, SLOT(deleteLater()));
        connect(thread, SIGNAL(started()), m_Worker, SLOT(process()));
        connect(m_Worker, SIGNAL(finishAll()), thread, SLOT(quit()));
        connect(thread, SIGNAL(quit()), thread, SLOT(deleteLater()));
        connect(m_Worker, SIGNAL(finishAll()), this, SLOT(workerStopped()));
        thread->start();
    }

    setButtonEnabledStates();
}

void LogConsole::dumpFileStart(QString file) {
    m_currentFile = file;
    QString str = QString("Writing %1").arg(m_currentFile);
    emit statusMessage(str);
}

void LogConsole::dumpFileFinish(QString file) {
    m_currentFile.clear();
    QString str = QString("Completed %1").arg(file);
    emit statusMessage(str);
}

void LogConsole::dumpFileBytesRead(long bytes) {
    QString str = QString("%1: %2 bytes").arg(m_currentFile, QString::number(bytes));
    emit statusMessage(str);
}

void LogConsole::workerStopped() {
    m_Worker = 0;
    setButtonEnabledStates();
}

void LogConsole::dumpError(QString err) {
    QLOG_ERROR() << "dumpError: err=" << err;
}

void LogConsole::eraseLogsClicked() {
    if(QMessageBox::question(this, "Erase Logs", "Clear the logs from the UAV?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        writeSerial("erase\r\n\r\n");

        // Wait a bit, then refresh.
        QTimer* timer = new QTimer(this);
        connect(timer, SIGNAL(timeout()), this, SLOT(refreshClicked()));
        connect(timer, SIGNAL(timeout()), timer, SLOT(deleteLater()));
        timer->start(1000);
    }
}

void LogConsole::refreshClicked() {
    m_pullMode = ListLogs;
    writeSerial("exit\r\nlogs\r\n");
}

void LogConsole::stopClicked() {
    if(m_Worker) {
        m_Worker->stop();
    }
}

QStringList LogConsole::getSelectedItems() {
    QStringList selected;

    const int size = ui->logList->count();
    for(int i = 0; i < size; ++i) {
        QCheckBox* b = static_cast<QCheckBox*>(ui->logList->itemWidget(ui->logList->item(i)));
        if(b->isChecked()) {
            selected.append(b->text());
        }
    }

    return selected;
}

int LogConsole::countSelected() {
    return getSelectedItems().size();
}

void LogConsole::setButtonEnabledStates() {
    int count = ui->logList->count();
    int selSize = countSelected();
    bool working = (m_Worker);

    ui->pullLogsButton->setEnabled((count > 0) && (selSize > 0) && (!working));
    ui->eraseLogsButton->setEnabled((count > 0) && (!working));
    ui->selectAllButton->setEnabled((count > 0) && (!working));
    ui->selectNoneButton->setEnabled((count > 0) && (!working));
    ui->stopWorkerButton->setEnabled(working);
    ui->refreshButton->setEnabled(!working);
}

void LogConsole::writeSerial(const char *s) {
    QLOG_DEBUG() << "writeSerial(): cmd=" << s;
    m_serial->write(s);
}

void LogConsole::clearListItems() {
    ui->logList->clear();
}

void LogConsole::addListItem(QString &str) {
    QCheckBox* chk = new QCheckBox(str, this);
    QListWidgetItem *item = new QListWidgetItem(ui->logList);
    item->setSizeHint(QSize(0, 20));
    item->setFlags(Qt::ItemIsSelectable);
    ui->logList->addItem(item);
    ui->logList->setItemWidget(item, chk);

    connect(chk, SIGNAL(clicked(bool)), this, SLOT(listItemCheckChanged()));
}

void LogConsole::listItemCheckChanged() {
    setButtonEnabledStates();
}

