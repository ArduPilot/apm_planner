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

#ifndef RADIOFLASHWIZARD_H
#define RADIOFLASHWIZARD_H

#include <QWizard>
#include <QTimer>

class QProcess;
class QTimer;

namespace Ui {
class RadioFlashWizard;
}

class RadioFlashWizard : public QWizard
{
    Q_OBJECT

public:
    explicit RadioFlashWizard(QWidget *parent = 0);
    ~RadioFlashWizard();

    void accept();

private slots:
    void selectPage(int index);

    void portSelectionShown();
    void flashRadio();
    void standardOutputReady();
    void standardErrorReady();
    void processTimeout();
    void processFinished(int exitCode);

private:
    QString getFirmwareImageName(int index);

private:
    Ui::RadioFlashWizard *ui;

    QProcess* m_updateProcess;
    QTimer m_timer;

    QString m_portName;
    int m_baudRate;
    QString m_firmwareImage;
};

#endif // RADIOFLASHWIZARD_H
