/*=====================================================================

APM_PLANNER Open Source Ground Control Station

(c) 2013, Bill Bonney <billbonney@communistech.com>

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

/**
 * @file
 *   @brief Terminal Console display View.
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
 * Influenced from Qt examples by :-
 * Copyright (C) 2012 Denis Shienkov <denis.shienkov@gmail.com>
 * Copyright (C) 2012 Laszlo Papp <lpapp@kde.org>
 *
 */

#ifndef TERMINALCONSOLE_H
#define TERMINALCONSOLE_H

#include "SerialSettingsDialog.h"

#include <QWidget>
#include <QtSerialPort/qserialport.h>
#include <QPointer>
#include <UASInterface.h>

namespace Ui {
class TerminalConsole;
}

class Console;
class SettingsDialog;
class QStatusBar;
class QComboBox;
class LogConsole;

class TerminalConsole : public QWidget
{
    Q_OBJECT
    
public:
    explicit TerminalConsole(QWidget *parent = 0);
    ~TerminalConsole();

public slots:
    void openSerialPort();
    void openSerialPort(const SerialSettings &settings);
    void closeSerialPort();
    void writeData(const QByteArray &data);
    void readData();
    void sendResetCommand();
    void logsButtonClicked();
    void logConsoleShown();
    void logConsoleHidden();
    void logConsoleStatusMessage(QString);
    void logConsoleActivityStart();
    void logConsoleActivityStop();
    void logToKmlClicked();

    void handleError(QSerialPort::SerialPortError error);

private slots:
    void setBaudRate(int index);
    void setLink(int index);
    void populateSerialPorts();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private:
    void initConnections();
    void addBaudComboBoxConfig();
    void fillPortsInfo(QComboBox &comboxBox);
    void addConsoleModesComboBoxConfig();
    void writeSettings();
    void loadSettings();

    
private:
    Ui::TerminalConsole *ui;

    QPointer<Console> m_console;
    QPointer<QStatusBar> m_statusBar;
    QPointer<SettingsDialog> m_settingsDialog;
    QPointer<QSerialPort> m_serial;
    SerialSettings m_settings;
    QTimer m_timer;
    QPointer<LogConsole> m_logConsole;
    QPointer<UASInterface> m_uas;
    bool m_windowVisible;
};

#endif // TERMINALCONSOLE_H
