/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009 - 2013 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#include <QToolButton>
#include <QLabel>
#include <QSpacerItem>
#include "QGCStatusBar.h"
#include "UASManager.h"
#include "MainWindow.h"
#include <QFileDialog>

QGCStatusBar::QGCStatusBar(QWidget *parent) :
    QStatusBar(parent),
    toggleLoggingButton(NULL),
    player(NULL),
    changed(true),
    lastLogDirectory(QGC::MAVLinkLogDirectory()),
    m_uas(NULL)
{
    setObjectName("QGC_STATUSBAR");

    connect(UASManager::instance(),SIGNAL(activeUASSet(UASInterface*)),this,SLOT(activeUASSet(UASInterface*)));
}

void QGCStatusBar::uasConnected()
{
    if (player)
    {
        if (!player->isPlayingLogFile())
        {
            this->setEnabled(false);
        }
    }
    else
    {
        this->setEnabled(false);
    }
}

void QGCStatusBar::uasDisconnected()
{
    if (player)
    {
        if (!player->isPlayingLogFile())
        {
            this->setEnabled(true);
        }
    }
    else
    {
        this->setEnabled(true);
    }
}

void QGCStatusBar::activeUASSet(UASInterface* uas)
{
    if (m_uas)
    {
        disconnect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
        disconnect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    }
    m_uas = uas;
    if (!uas)
    {
        //No active UAS?
        return;
    }
    connect(m_uas,SIGNAL(connected()),this,SLOT(uasConnected()));
    connect(m_uas,SIGNAL(disconnected()),this,SLOT(uasDisconnected()));
    if (player)
    {
        if (!player->isPlayingLogFile())
        {
            if (uas->getLinks()->size() > 0)
            {
                if (uas->getLinks()->at(0)->isConnected())
                {
                    this->setEnabled(false);
                }
            }
        }

    }
    else
    {
        //No player!
        this->setEnabled(false);
    }
}

void QGCStatusBar::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawPrimitive(QStyle::PE_PanelStatusBar, &opt, &p, this);
}

void QGCStatusBar::setLogPlayer(QGCMAVLinkLogPlayer* player)
{
    this->player = player;
    addPermanentWidget(player);
    //connect(toggleLoggingButton, SIGNAL(clicked(bool)), this, SLOT(logging(bool)));
}

void QGCStatusBar::logging(bool checked)
{
    // Stop logging in any case
   // MainWindow::instance()->getMAVLink()->stopLogging();

    if (!checked && player)
    {
        //player->setLastLogFile(lastLogDirectory);
    }

	// If the user is enabling logging
    if (checked)
    {
		// Prompt the user for a filename/location to save to
        QString fileName = QFileDialog::getSaveFileName(this, tr("Specify MAVLink log file to save to"),
                                                        lastLogDirectory, tr("MAVLink Logfile (*.mavlink *.log *.bin);;"));

		// Check that they didn't cancel out
		if (fileName.isNull())
		{
            //toggleLoggingButton->setChecked(false);
			return;
		}

		// Make sure the file's named properly
        if (!fileName.endsWith(".mavlink"))
        {
            fileName.append(".mavlink");
        }

		// Check that we can save the logfile
        QFileInfo file(fileName);
        if ((file.exists() && !file.isWritable()))
        {
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText(tr("The selected logfile is not writable"));
            msgBox.setInformativeText(tr("Please make sure that the file %1 is writable or select a different file").arg(fileName));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        }
		// Otherwise we're off and logging
        else
        {
            //MainWindow::instance()->getMAVLink()->startLogging(fileName);
        }
    }
}


void QGCStatusBar::storeSettings()
{
}

QGCStatusBar::~QGCStatusBar()
{
    storeSettings();
    //if (toggleLoggingButton) toggleLoggingButton->deleteLater();
}
