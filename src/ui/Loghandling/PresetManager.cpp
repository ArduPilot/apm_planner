/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2017 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file PresetManager.cpp
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 13 Apr 2017
 * @brief File providing implementation for the Preset manager class
 */

#include <QFileDialog>
#include <QSettings>

#include "PresetManager.h"
#include "logging.h"
#include "configuration.h"


PresetManager::PresetManager(QWidget *p_parent, QMenuBar *p_menuBar) :
    QObject (p_parent),
    mp_menu(0),
    m_presetHasChanged(false)
{
    QLOG_DEBUG() << "PresetManager::PresetManager - CTOR";

    qRegisterMetaType<PresetManager::presetElementVec>("PresetManager::presetElementVec");

    // create own menu and add the management items
    mp_menu = new QMenu("Analyzing Presets", p_parent);
    p_menuBar->addMenu(mp_menu);

    QAction *p_Action = mp_menu->addAction("Load Presets");
    connect(p_Action, SIGNAL(triggered()), this, SLOT(loadPresets()));
    p_Action = mp_menu->addAction("Save Presets");
    connect(p_Action, SIGNAL(triggered()), this, SLOT(savePresetCheckingFilename()));
    p_Action = mp_menu->addAction("Save Presets as");
    connect(p_Action, SIGNAL(triggered()), this, SLOT(savePresetsAs()));
    p_Action = mp_menu->addAction("Remove current Preset");
    connect(p_Action, SIGNAL(triggered()), this, SLOT(removePresets()));
    p_Action = mp_menu->addAction("Remove all Presets");
    connect(p_Action, SIGNAL(triggered()), this, SLOT(removeAllPresets()));

    mp_menu->addSeparator();
}

PresetManager::~PresetManager()
{
    QLOG_DEBUG() << "PresetManager::~PresetManager - DTOR";
    checkAndSaveIfPresetModified();
}

QFileInfo PresetManager::getFileInfo() const
{
    return m_presetFile;
}

void PresetManager::setFileName(const QString &fileName)
{
    if(!fileName.isEmpty())
    {
        m_presetFile.setFile(fileName);
        readPresetFile();
    }
}

PresetManager::presetElementVec PresetManager::getSelectedPreset() const
{
    return m_currentPreset.m_enabledGraphs;
}

void PresetManager::saveSpecialSet(const presetElementVec &preset, bool usesTimeAxis) const
{
    // This preset is stored in the APM Planner ini file and loaded / stored on every
    // access. This guarantees that a saved set is immediate loadable from another analysis window.
    QSettings graphSettings;
    graphSettings.beginGroup("LOGANALYSIS");
    graphSettings.remove("");   // removes all entries from this group

    graphSettings.beginWriteArray("GRAPH_ELEMENTS");
    for(int i = 0; i < preset.size(); ++i)
    {
        graphSettings.setArrayIndex(i);
        graphSettings.setValue("NAME", preset.at(i).m_graph);
        graphSettings.setValue("COLOR/RED", preset.at(i).m_color.red());
        graphSettings.setValue("COLOR/GREEN", preset.at(i).m_color.green());
        graphSettings.setValue("COLOR/BLUE", preset.at(i).m_color.blue());
        if(preset.at(i).m_manualRange)
        {   // only store scaling if set to manual
            graphSettings.setValue("Y_AXIS_MIN", preset.at(i).m_range.lower);
            graphSettings.setValue("Y_AXIS_MAX", preset.at(i).m_range.upper);
        }
        else if(preset.at(i).m_group.size() > 0)
        {   // store group name if grouped
            graphSettings.setValue("GROUP", preset.at(i).m_group);
        }
    }
    graphSettings.endArray();

    graphSettings.beginGroup("GRAPH_SETTINGS");
    graphSettings.setValue("TIME_AXIS", usesTimeAxis);
    graphSettings.endGroup();

    graphSettings.endGroup();   // "LOGANALYSIS"

    graphSettings.sync();
    QLOG_DEBUG() << "PresetManager::saveSpecialSet - Special View saved.";

}

bool PresetManager::loadSpecialSet(presetElementVec &preset) const
{
    // This preset is stored in the APM Planner ini file and loaded / stored on every
    // access. This guarantees that a saved set is immediate loadable from another analysis window.

    // Load X-Axis settings
    QSettings graphSettings;
    graphSettings.beginGroup("LOGANALYSIS");
    graphSettings.beginGroup("GRAPH_SETTINGS");
    bool usesTimeAxis = graphSettings.value("TIME_AXIS", Qt::Checked).toBool();
    graphSettings.endGroup();   // "GRAPH_SETTINGS"

    // now load the enabled graphs
    int size = graphSettings.beginReadArray("GRAPH_ELEMENTS");
    for(int i = 0; i < size; ++i)
    {
        graphSettings.setArrayIndex(i);
        presetElement element;
        element.m_graph = graphSettings.value("NAME").toString();

        int red   = graphSettings.value("COLOR/RED").toInt();
        int green = graphSettings.value("COLOR/GREEN").toInt();
        int blue  = graphSettings.value("COLOR/BLUE").toInt();
        QColor color(red, green, blue);
        element.m_color = color;
        if(graphSettings.contains("GROUP"))
        {
            element.m_group = graphSettings.value("GROUP").toString();
        }
        else if(graphSettings.contains("Y_AXIS_MIN") && graphSettings.contains("Y_AXIS_MAX"))
        {
            QCPRange yAxisRange(graphSettings.value("Y_AXIS_MIN").toDouble(), graphSettings.value("Y_AXIS_MAX").toDouble());
            element.m_range = yAxisRange;
            element.m_manualRange = true;
        }
        preset.push_back(element);
    }
    graphSettings.endArray();   // "GRAPH_ELEMENTS"

    graphSettings.endGroup();   // "LOGANALYSIS"

    QLOG_DEBUG() << "PresetManager::loadSpecialSet - Special View loaded.";

    return usesTimeAxis;
}

void PresetManager::addToCurrentPresets(const presetElementVec &preset)
{
    bool ok = false;
    QString name = QInputDialog::getText(qobject_cast<QWidget*>(parent()), tr("Enter Preset Name"), tr("Preset Name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !name.isEmpty())
    {
        if(!m_NameToPresetMap.contains(name))
        {
            presetData internalPreset;
            internalPreset.m_presetName = name;
            internalPreset.mp_menuAction = mp_menu->addAction(name);
            internalPreset.m_enabledGraphs = preset;
            m_NameToPresetMap.insert(name, internalPreset);
            connect(internalPreset.mp_menuAction, SIGNAL(triggered()), this, SLOT(handlePresetSelected()));
            QLOG_DEBUG() << "PresetManager::addSavedView() - added " << name << " preset";
        }
        else
        {
            QLOG_DEBUG() << "PresetManager::addSavedView() - preset " << name << "already available";
        }
    }
    m_presetHasChanged = true;
}

void PresetManager::readPresetFile()
{
    // first remove all loaded presets
    removeAllPresets();
    QSettings presets(m_presetFile.absoluteFilePath(), QSettings::IniFormat);
    presets.beginGroup("GRAPHING_PRESETS");

    if(presets.contains(("PRESET_FILE_VERSION")))
    {
        QString presetVersion = presets.value("PRESET_FILE_VERSION").toString();

        if(presetVersion == "1.0")
        {
            readPresetFileVersion10(presets);
        }
        // Add new preset versions here!
        else
        {
            QLOG_ERROR() << "Could not load preset file " << m_presetFile.absoluteFilePath() << ". Unknown Version:" << presetVersion;
            m_presetFile = QFileInfo();
        }
    }
    else
    {
        QLOG_WARN() << "PresetManager::readPresetFile() - ini file has no version string - not loaded";
        m_presetFile = QFileInfo();
    }

    presets.endGroup(); // "GRAPHING_PRESETS"

    m_presetHasChanged = false;    // a fresh loaded preset has not changed
    adaptWindowTitle();
}

void PresetManager::readPresetFileVersion10(QSettings &presets)
{
    // "GRAPHING_PRESETS" group is handeled by caller
    // check if valid - at least there must be one name
    if(!presets.contains("SETNAMES/1/NAME"))
    {
        QLOG_WARN() << "PresetManager::readPresetFile() - ini file seems to be corrupt - not loaded";
        m_presetFile = QFileInfo();
        return;
    }

    // first fetch the names of all presets
    int size = presets.beginReadArray("SETNAMES");
    for(int i = 0; i < size; ++i)
    {
        presets.setArrayIndex(i);
        QString name = presets.value("NAME").toString();
        if(!m_NameToPresetMap.contains(name))
        {
            presetData internalPreset;
            internalPreset.m_presetName = name;
            internalPreset.mp_menuAction = mp_menu->addAction(name);
            m_NameToPresetMap.insert(name, internalPreset);
            connect(internalPreset.mp_menuAction, SIGNAL(triggered()), this, SLOT(handlePresetSelected()));
            QLOG_DEBUG() << "PresetManager::loadDialogAccepted() - added " << name << " preset";
        }
        else
        {
            QLOG_DEBUG() << "PresetManager::loadDialogAccepted() - preset " << name << "already loaded";
        }
    }
    presets.endArray();

    // now get the content of the presets
    QMap<QString, presetData>::Iterator iter;
    for(iter = m_NameToPresetMap.begin(); iter != m_NameToPresetMap.end(); ++iter)
    {
        size = presets.beginReadArray(iter->m_presetName);
        for(int i = 0; i < size; ++i)
        {
            presets.setArrayIndex(i);
            presetElement element;

            element.m_graph = presets.value("NAME").toString();
            int red      = presets.value("COLOR/RED").toInt();
            int green    = presets.value("COLOR/GREEN").toInt();
            int blue     = presets.value("COLOR/BLUE").toInt();
            QColor color(red, green, blue);
            element.m_color = color;
            if(presets.contains("GROUP"))   // is optional
            {
                element.m_group = presets.value("GROUP").toString();
            }
            else if(presets.contains("Y_AXIS_MIN") && presets.contains("Y_AXIS_MAX"))   // is optional
            {
                QCPRange yAxisRange(presets.value("Y_AXIS_MIN").toDouble(), presets.value("Y_AXIS_MAX").toDouble());
                element.m_range = yAxisRange;
                element.m_manualRange = true;
            }
            iter->m_enabledGraphs.push_back(element);
        }
        presets.endArray();
    }
}

void PresetManager::checkAndSaveIfPresetModified()
{
    if(m_presetHasChanged)
    {
        QMessageBox msgBox;
        msgBox.setText("The presets have been modified.");
        msgBox.setInformativeText("Do you want to save your changes?");
        QPushButton *saveButton = msgBox.addButton(tr(" Save "), QMessageBox::ActionRole);
        QPushButton *saveAsButton = msgBox.addButton(tr(" Save As "), QMessageBox::ActionRole);
        QPushButton *discardButton = msgBox.addButton(tr(" Discard "), QMessageBox::DestructiveRole);
        msgBox.exec();

        if(msgBox.clickedButton() == saveButton)
            savePresetCheckingFilename();
        else if(msgBox.clickedButton() == saveAsButton)
            savePresetsAs();
        else if(msgBox.clickedButton() == discardButton)
            QLOG_INFO() << "PresetManager::~PresetManager - Changed preset discarded";
    }
}

void PresetManager::adaptWindowTitle() const
{
    QWidget *window = qobject_cast<QWidget*>(parent());

    if(window)
    {
        QString title = window->windowTitle();

        int index = title.lastIndexOf('[');
        if(index > 0)
        {
            title.truncate(index - 1);   // If found just remove one more getting the space before '[' (see next lines)
        }
        title.append(" [");
        title.append(m_presetFile.fileName());     // now add the preset filename to window title
        title.append(']');

        window->setWindowTitle(title);
    }
}

void PresetManager::loadPresets()
{
    checkAndSaveIfPresetModified();

    QFileDialog *dialog = new QFileDialog(qobject_cast<QWidget*>(parent()), "Load Analyzing Presets", QGC::appDataDirectory(), "Analyzing Presets (*.ini);;All Files (*.*)");
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->open(this, SLOT(loadDialogAccepted()));
}

void PresetManager::loadDialogAccepted()
{
    QFileDialog *dialog = qobject_cast<QFileDialog*>(sender());
    if (!dialog || (dialog->selectedFiles().size() == 0))
    {
        return;
    }
    m_presetFile.setFile(dialog->selectedFiles().first());
    dialog->deleteLater();

    // read presets
    readPresetFile();
}

void PresetManager::handlePresetSelected()
{
    QAction *triggeredAction = qobject_cast<QAction*>(sender());
    QString presetName = triggeredAction->text();
    QLOG_DEBUG() << "PresetManager::handlePresetSelected() - preset:" << presetName;

    if(m_NameToPresetMap.contains(presetName))
    {
        m_currentPreset = m_NameToPresetMap.value(presetName);
        emit newPresetSelected(m_currentPreset.m_enabledGraphs);
    }
}

void PresetManager::removeAllPresets()
{
    foreach(const presetData &preset, m_NameToPresetMap)
    {
        mp_menu->removeAction(preset.mp_menuAction);
    }
    m_NameToPresetMap.clear();
    m_presetHasChanged = true;
}

void PresetManager::savePresetCheckingFilename()
{
    if(!m_presetFile.fileName().isEmpty())
        savePresets();
    else
        savePresetsAs();
}

void PresetManager::savePresets()
{
    QSettings graphPresets(m_presetFile.absoluteFilePath(), QSettings::IniFormat);
    graphPresets.clear();
    graphPresets.beginGroup("GRAPHING_PRESETS");

    graphPresets.setValue("PRESET_FILE_VERSION", "1.0");

    // create the names of the presets
    graphPresets.beginWriteArray("SETNAMES");
    int index = 0;
    QMap<QString, presetData>::iterator iter;
    for(iter = m_NameToPresetMap.begin(); iter != m_NameToPresetMap.end(); ++iter)
    {
        graphPresets.setArrayIndex(index++);
        graphPresets.setValue("NAME", iter->m_presetName);
    }
    graphPresets.endArray();

    // store the preset data itself
    for(iter = m_NameToPresetMap.begin(); iter != m_NameToPresetMap.end(); ++iter)
    {
        graphPresets.beginWriteArray(iter->m_presetName);
        for(int i = 0; i < iter->m_enabledGraphs.size(); ++i)
        {
            graphPresets.setArrayIndex(i);
            graphPresets.setValue("NAME", iter->m_enabledGraphs.at(i).m_graph);
            graphPresets.setValue("COLOR/RED", iter->m_enabledGraphs.at(i).m_color.red());
            graphPresets.setValue("COLOR/GREEN", iter->m_enabledGraphs.at(i).m_color.green());
            graphPresets.setValue("COLOR/BLUE", iter->m_enabledGraphs.at(i).m_color.blue());
            if(iter->m_enabledGraphs.at(i).m_group.size() > 0)
            {
                graphPresets.setValue("GROUP", iter->m_enabledGraphs.at(i).m_group);
            }
            else if(iter->m_enabledGraphs.at(i).m_manualRange)
            {
                graphPresets.setValue("Y_AXIS_MIN", iter->m_enabledGraphs.at(i).m_range.lower);
                graphPresets.setValue("Y_AXIS_MAX", iter->m_enabledGraphs.at(i).m_range.upper);
            }
        }
        graphPresets.endArray();
    }

    graphPresets.endGroup(); //"GRAPHING_PRESETS"

    graphPresets.sync();
    QLOG_DEBUG() << "All presets saved to " << m_presetFile.absoluteFilePath();

    m_presetHasChanged = false;
    adaptWindowTitle();
}

void PresetManager::savePresetsAs()
{
   QString name = QFileDialog::getSaveFileName(qobject_cast<QWidget*>(parent()), "Save Presets As", QGC::appDataDirectory(),
                                               "Analyzing Presets (*.ini);;All Files (*.*)");

   if(!name.isEmpty())
   {
       int pointIndex = name.lastIndexOf('.');  // lastIndexOf '.' shall find the . right before the extension
       if(pointIndex > 0)
       {
           name.truncate(pointIndex);   // If found just remove the old extension completely
       }
       name.append(".ini");     // now add the new ".ini" extension

       m_presetFile.setFile(name);
       savePresets();
   }
}

void PresetManager::removePresets()
{
    if(m_currentPreset.mp_menuAction != 0)
    {
        mp_menu->removeAction(m_currentPreset.mp_menuAction);
        m_NameToPresetMap.remove(m_currentPreset.m_presetName);
    }
    QLOG_DEBUG() << "PresetManager::removePresets() - removed preset:" << m_currentPreset.m_presetName;
    m_currentPreset = presetData();
    m_presetHasChanged = true;
}
