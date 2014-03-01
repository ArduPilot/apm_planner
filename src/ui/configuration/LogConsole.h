#ifndef LOGCONSOLE_H
#define LOGCONSOLE_H

#include <QWidget>
#include <QPointer>

namespace Ui {
class LogConsole;
}

class QSerialPort;
class QFile;
class QTextStream;
class Worker;

/** @brief Log console widget, for dealing with logs on the UAV. */
class LogConsole : public QWidget {
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
    void activityStart();
    void activityStop();

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
    void workerCancelled();

private:
    Ui::LogConsole *ui;
    QSerialPort *m_serial;
    PullMode m_pullMode;
    QString m_currentFile;
    Worker* m_worker;

    void processLine(QByteArray &data);
    void initConnections();
    QStringList getSelectedItems();
    int countSelected();
    void setButtonEnabledStates();
    bool writeSerial(const char *s);
    void clearListItems();
    void addListItem(QString& item);
    void doDelayedRefresh();
};

/** @brief worker for log extraction. */
class Worker: public QObject {
    Q_OBJECT

    enum state {
        none, start, reading, finish
    };

public:
    Worker(QSerialPort *, QList<LogConsole::FileData> &);
    ~Worker();

    void stop() { m_run = false; }

    void generateKml(bool gen = true) { m_generateKml = gen; }
    bool generatesKml() { return m_generateKml; }

public slots:
    void process();
    void readData();
    void readyNextFile();

signals:
    void startFile(QString);
    void finishFile(QString);
    void bytesRead(long);
    void finishAll();
    void error(QString);
    void cancelled();

private:
    void onData(QByteArray& data);
    void onLineRead(char *);
    void onStartNextFile();
    void onFinishFile();
    void onFinishAll();
    void onCancel();

    bool m_generateKml;
    state m_state;
    QPointer<QSerialPort> m_port;
    QList<LogConsole::FileData> m_files;
    QStringList m_logLines;
    long m_totalBytesRead;
    int m_fdIndex;
    int m_blanks;
    bool m_run;
};


#endif // LOGCONSOLE_H
