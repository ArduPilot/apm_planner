#ifndef LOGCONSOLE_H
#define LOGCONSOLE_H

#include <QWidget>

namespace Ui {
class LogConsole;
}

class QSerialPort;
class QFile;
class QTextStream;
class Worker;

/** @brief Log console widget, for dealing with logs on the UAV. */
class LogConsole : public QWidget
{
    Q_OBJECT
    
public:
    enum PullMode {
        None,
        ListLogs
    };

    struct FileData {
        QString filename;
        int index;

        FileData(QString &fn, int idx): filename(fn), index(idx) {}
    };

    explicit LogConsole(QWidget *parent = 0);
    ~LogConsole();

    void setSerial(QSerialPort *);
    void onShow(bool shown);

signals:
    void statusMessage(QString);

private slots:
    void selectAllClicked();
    void selectNoneClicked();
    void stopClicked();
    void pullSelectedClicked();
    void eraseLogsClicked();
    void refreshClicked();
    void listItemCheckChanged();
    void readData();
    void dumpError(QString);
    void dumpFileStart(QString);
    void dumpFileFinish(QString);
    void dumpFileBytesRead(long);
    void workerStopped();

private:
    Ui::LogConsole *ui;
    QSerialPort *m_serial;
    PullMode m_pullMode;
    QString m_currentFile;
    Worker* m_Worker;

    void processLine(QByteArray &data);
    void initConnections();
    QStringList getSelectedItems();
    int countSelected();
    void setButtonEnabledStates();
    void writeSerial(const char *s);
    void clearListItems();
    void addListItem(QString& item);
};

/** @brief worker for log extraction. */
class Worker: public QObject {
    Q_OBJECT

public:
    Worker(QSerialPort *, QList<LogConsole::FileData> &);
    ~Worker();

    void stop() { mRun = false; }

public slots:
    void process();

signals:
    void startFile(QString);
    void finishFile(QString);
    void bytesRead(long);
    void finishAll();
    void error(QString);

private:
    QSerialPort* mPort;
    QList<LogConsole::FileData> mFiles;
    bool mRun;
};


#endif // LOGCONSOLE_H
