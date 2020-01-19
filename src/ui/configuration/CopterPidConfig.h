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

/**
 * @file
 *   @brief PID configuration for ArduCoptor (multi-rotor air vehicle)
 *          for version Copter3.3+
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Bill Bonney Carpenter <billbonney@communistech.com>
 */

#ifndef COPTERPIDCONFIG_H
#define COPTERPIDCONFIG_H

#include <QWidget>
#include <QDoubleSpinBox>
#include <QComboBox>

#include "AP2ConfigWidget.h"

namespace Ui {
    class CopterPidConfig;
}

class CopterPidConfig : public AP2ConfigWidget
{
    Q_OBJECT
    
public:
    explicit CopterPidConfig(QWidget *parent = nullptr);
    ~CopterPidConfig();
private slots:
    void writeButtonClicked();
    void refreshButtonClicked();
    void parameterChanged(int uas, int component, QString parameterName, QVariant value);
    void lockCheckBoxClicked(bool checked);
    void stabilLockedChanged(double value);
    void ratePChanged(double value);
    void rateIChanged(double value);
    void rateDChanged(double value);
    void rateIMAXChanged(double value);

private:
    typedef QPair<int,QString> ValueNamePair;
    typedef QVector<ValueNamePair> ValueNamePairList;

    void showEvent(QShowEvent *evt);

    void mapParamNamesToBox();

    void setupPID_Default();
    void setupPID_APM_34();
    void setupPID_APM_36();
    void setupPID_APM_40();

    void setupTuneOption_APM_36();
    void setupRC78Option_APM_36();

    void hideAPM_40_Filters();

    void populateCombobox(const ValueNamePairList &ValueToText, QComboBox *Box);

private:

    Ui::CopterPidConfig *mp_ui{nullptr};

    ValueNamePairList m_ch6ValueToTextList;
    ValueNamePairList m_ch78ValueToTextList;
    QMap<QString,QDoubleSpinBox*> m_nameToBoxMap;

    bool m_pitchRollLocked;

    QString m_channel7Option;
    QString m_channel8Option;
    QString m_channel9Option;

    QString m_ch6MaxParamName;
    QString m_ch6MinParamName;
};

#endif // COPTERPIDCONFIG_H
