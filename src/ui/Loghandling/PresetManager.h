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
 * @file PresetManager.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 13 Apr 2017
 * @brief File providing header for the Preset manager class
 */

#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QObject>
#include <QMenu>
#include <QColor>

#include "qcustomplot.h"


/**
 * @brief The PresetManager class provides a menu for the menubar which
 *        brings some preset handling. Presets are preconfigured graph
 *        settings used in Log analysis. This manager provides all services
 *        neede to handle the presets. The menu itself and it entries are
 *        managed by ths class too. It supports file persistence for the
 *        presets.
 */
class PresetManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief The presetElement struct - holds all data needed to reconstruct one special Graph
     */
    struct presetElement
    {
        QString   m_graph;          /// Name of the graph
        QColor    m_color;          /// Color of the graph
        QCPRange  m_range;          /// y-axis range of the graph
        QString   m_group;          /// Group name if grouped, empty when not grouped
        bool      m_manualRange;    /// Flag for manual scaling - if true y-axis range will be used - false graph will be autoscaled

        presetElement() : m_manualRange(false) {}
    };

    typedef QVector<presetElement> presetElementVec;    /// type holding several preset elements thus defining a preset.

    /**
     * @brief PresetManager - CTOR
     * @param p_parent - Pointer to parent widget
     * @param p_menuBar - Pointer to menu bar of the parent widget
     */
    explicit PresetManager(QWidget *p_parent, QMenuBar *p_menuBar);

    /**
     * @brief ~PresetManager - DTOR
     */
    virtual ~PresetManager();

    /**
     * @brief getFileInfo - To get the QFileInfo of the loaded preset
     *        used for persistence.
     * @return - the QFileInfo of the loaded preset.
     */
    QFileInfo getFileInfo() const;

    /**
     * @brief setFileName - Sets the filename of the preset to load.
     *        if the file is found and valid the presets will be loaded.
     * @param fileName - Name of the preset file to be loaded
     */
    void setFileName(const QString &fileName);

    /**
     * @brief getSelectedPreset - Delivers the currently selected preset.
     * @return - The selected preset
     */
    presetElementVec getSelectedPreset() const;

    /**
     * @brief saveSpecialSet - saves the delivered preset directly to the APM Planner
     *        ini file. By doing this the preset is accessible from all log analysis
     *        windows.
     * @param preset - the preset data to store
     * @param usesTimeAxis - Is the time on X-Axis enabled?
     */
    void saveSpecialSet(const presetElementVec &preset, bool usesTimeAxis) const;

    /**
     * @brief loadSpecialSet - loads a preset directly from the APM Planner ini file.
     * @param preset - the loaded preset data.
     * @return - true if time on X-Axis was used.
     */
    bool loadSpecialSet(presetElementVec &preset) const;

    /**
     * @brief addToCurrentPresets - adds the preset to the preset menu and stores
     *        it in the internal map. User will be prompted for a preset name.
     * @param preset - preset data to be added.
     */
    void addToCurrentPresets(const presetElementVec &preset);

signals:

    /**
     * @brief newPresetSelected - This signal is emitted whenever the preset changes.
     */
    void newPresetSelected(PresetManager::presetElementVec);

private:

    /**
     * @brief The presetData struct - is an internal used struct to enrich the presetElementVec
     *        with some additional information for internal handling
     */
    struct presetData
    {
        QString m_presetName;               /// Name of the preset itself
        QAction *mp_menuAction;             /// Pointer to menubar entry
        presetElementVec m_enabledGraphs;   /// the corresponding preset element vector

        presetData() : mp_menuAction(0) {}
    };

    QMenu *mp_menu;                 /// Pointer to our menu in menubar
    QFileInfo m_presetFile;         /// file info of loaded *.ini file
    QMap<QString, presetData> m_NameToPresetMap;    /// Map holding preset name and its settings
    presetData m_currentPreset;     /// The current selected preset

    bool m_presetHasChanged;        /// Always true if the loaded preset has changed

    /**
     * @brief readPresetFile - starts to load a preset file. Mainly it loads the version information
     *        of the preset file and selects the appropriate loading method.
     */
    void readPresetFile();

    /**
     * @brief readPresetFileVersion10 - is used to load preset files of version 1.0
     * @param presets - The preset to load the data from.
     */
    void readPresetFileVersion10(QSettings &presets);

    /**
     * @brief checkAndSaveIfPresetModified - tests the m_presetHasChanged flag and if its set
     *        it prompts the user to save the changed set.
     */
    void checkAndSaveIfPresetModified();

    /**
     * @brief adoptWindowTitle - adopts the window title to the loaded preset. Window title shall look like
     *        "Graph: 'filename of loaded log' ['filename of the loaded preset']"
     */
    void adaptWindowTitle() const;

private slots:

    /**
     * @brief loadPresets - starts preset loading - triggered by menu bar entry
     */
    void loadPresets();

    /**
     * @brief handlePresetSelected - is called as soon as one preset is selected from menu.
     *        Gets the name of the selected preset and enables it.
     */
    void handlePresetSelected();

    /**
     * @brief removeAllPresets - removes all presets from the menubar
     */
    void removeAllPresets();

    /**
     * @brief savePresetCheckingFilename - saves the current presets. If filename is empty
     *        user will be promted to enter one.
     */
    void savePresetCheckingFilename();

    /**
     * @brief savePresets - saves the current presets. Does not check whether the filename is empty.
     */
    void savePresets();

    /**
     * @brief savePresetsAs - saves preset in another file. User will be prompted for new filename.
     */
    void savePresetsAs();

    /**
     * @brief removePresets - removes the curently selected Preset from the menubar
     */
    void removePresets();
};

#endif // PRESETMANAGER_H
