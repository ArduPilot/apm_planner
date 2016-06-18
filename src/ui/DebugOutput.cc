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

#include "DebugOutput.h"

#include <QScrollBar>

DebugOutput::DebugOutput(QWidget *parent) : QWidget(parent)
{
    ui.setupUi(this);
    ui.hashLineEdit->setText(define2string(GIT_HASH));
    ui.commitLineEdit->setText(define2string(GIT_COMMIT));
    connect(ui.onTopCheckBox,SIGNAL(clicked(bool)),this,SLOT(onTopCheckBoxChecked(bool)));
    connect(ui.copyPushButton,SIGNAL(clicked()),this,SLOT(copyToClipboardButtonClicked()));
    connect(this,SIGNAL(append(QString)),this,SLOT(appendText(QString)));
}

DebugOutput::~DebugOutput()
{
    ui.textBrowser->close();
}

void DebugOutput::write(const QString &message)
{
    // Used to decouple caller from this class and force the call to ui.textBrowser->append(message)
    // to be made from the GUI thread. As the QTextBrowser is not reentrant it should only be called
    // from one thread!!
    emit append(QString(message));
}

void DebugOutput::onTopCheckBoxChecked(bool checked)
{
    if (checked)
    {
        this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
        this->show();
    }
}

void DebugOutput::copyToClipboardButtonClicked()
{
    ui.textBrowser->selectAll();
    ui.textBrowser->copy();
}

void DebugOutput::appendText(QString message)
{
    ui.textBrowser->append(message);
    if (ui.autoScrollCheckBox->isChecked())
    {
        QScrollBar *sb = ui.textBrowser->verticalScrollBar();
        sb->setValue(sb->maximum());
    }
}
