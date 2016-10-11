/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2013 - 2016 APM_PLANNER PROJECT <http://ardupilot.com>

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
 *   @brief DebugOutput widget class
 *
 *   @author Michael Carpenter <malcom2073@gmail.com>
 *   @author Arne Wischmann <wischmann-a@gmx.de>
 */

#ifndef DEBUGOUTPUT_H
#define DEBUGOUTPUT_H

#include <QWidget>
#include "ui_DebugOutput.h"
#define define2string_p(x) #x
#define define2string(x) define2string_p(x)

/**
 * @brief The DebugOutput widget provides a window for
 *        the logoutput of the APM Planner
 */
class DebugOutput : public QWidget
{
    Q_OBJECT
public:
    /** @brief Shared Pointer type for this class **/
    typedef QSharedPointer<DebugOutput> Ptr;

    /** @brief CTOR  **/
    explicit DebugOutput(QWidget *parent = 0);

    /** @brief DTOR  **/
    ~DebugOutput();

    /**
     * @brief write a message to the widget
     * @param message - String to print
     * @note this method just emits a signal it can be used
     *       from different threads
     */
    void write(const QString &message);

signals:
    /** @brief Signal used by write method for decoupling **/
    void append(QString message);

private slots:
    void onTopCheckBoxChecked(bool checked);
    void copyToClipboardButtonClicked();
    /**
     * @brief appendText the method which "really" writes to the
     *        QTextBrowser window
     */
    void appendText(QString message);

private:
    Ui::DebugOutput ui;
};

#endif // DEBUGOUTPUT_H
