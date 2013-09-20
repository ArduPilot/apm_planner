/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 APM_PLANNER PROJECT <http://www.diydrones.com>

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

#ifndef RADIO3DRCONFIG_H
#define RADIO3DRCONFIG_H

#include "QsLog.h"
#include "SerialSettingsDialog.h"
#include "qserialport.h"

#include <QPointer>
#include <QWidget>
#include "ui_Radio3DRConfig.h"

class Radio3DRConfig : public QWidget
{
    Q_OBJECT
    
public:
    explicit Radio3DRConfig(QWidget *parent = 0);
    ~Radio3DRConfig();
    
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots:
    void setBaudRate(int index);
    void setLink(int index);
    void populateSerialPorts();

private:
    void initConnections();
    void addBaudComboBoxConfig();
    void fillPortsInfo(QComboBox &comboxBox);
    void writeSettings();
    void loadSettings();

private:
    Ui::Radio3DRConfig ui;

    QPointer<SettingsDialog> m_settingsDialog;
    SerialSettings m_settings;
    QPointer<QSerialPort> m_serial;
    QPointer<QTimer> m_timer;
};

#endif // RADIO3DRCONFIG_H
