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
#include "Radio3DRSettings.h"
#include <QtSerialPort/qserialport.h>

#include <QPointer>
#include <QWidget>
#include "ui_Radio3DRConfig.h"

class Radio3DRSettings;

class Radio3DRConfig : public QWidget
{
    Q_OBJECT

    enum State { none, writeRadioSettings, resetRadioSettings, complete};
    
public:
    explicit Radio3DRConfig(QWidget *parent = 0);
    ~Radio3DRConfig();
    
protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);

private slots:
    void serialPortOpenFailure(int error, QString errorString);

    void setBaudRate(int index);
    void setLink(int index);
    void populateSerialPorts();

    void loadSavedSerialSettings();
    void saveSerialSettings();

    void flashButtonClicked();

    void writeLocalRadioSettings();
    void writeRemoteRadioSettings();
    void readRadioSettings();
    void resetLocalRadioSettingsToDefaults();
    void resetRemoteRadioSettingsToDefaults();

    void copyLocalSettingsToRemote();

    void localReadComplete(Radio3DREeprom& eeprom, bool success);
    void remoteReadComplete(Radio3DREeprom& eeprom, bool success);

    void updateLocalStatus(QString status);
    void updateRemoteStatus(QString status);
    void updateLocalComplete(int result);
    void updateRemoteComplete(int result);
    void updateLocalRssi(QString status);
    void updateRemoteRssi(QString status);

private:
    void resetUI();
    void initConnections();
    void addBaudComboBoxConfig(QComboBox *comboBox);
    void fillPortsInfo(QComboBox &comboxBox);
    void addRadioBaudComboBoxConfig(QComboBox &comboBox);
    void addRadioAirBaudComboBoxConfig(QComboBox &comboBox);
    void addTxPowerComboBoxConfig(QComboBox &comboBox);
    void addMavLinkComboBoxConfig(QComboBox &comboBox);
    void addMavLinkLowLatencyComboBoxConfig(QComboBox &comboBox);
    void setupFrequencyComboBox(QComboBox& comboBox, int freqCode);
    void addRtsCtsComboBoxConfig(QComboBox &comboBox);

private:
    Ui::Radio3DRConfig ui;

    QPointer<SettingsDialog> m_settingsDialog;
    SerialSettings m_settings;
    Radio3DREeprom m_newRadioSettings;
    Radio3DREeprom m_remoteRadioSettings;
    QPointer<QTimer> m_timer;
    QPointer<Radio3DRSettings> m_radioSettings;
    State m_state;
};

#endif // RADIO3DRCONFIG_H
