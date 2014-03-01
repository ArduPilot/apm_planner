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
 *   @brief Global Object for storing common used variables.
 *
 *   @author Bill Bonney <billbonney@communistech.com>
 *
*/

#ifndef GLOBALOBJECT_H
#define GLOBALOBJECT_H

#include <QString>

class GlobalObject
{
public:
    static GlobalObject* sharedInstance();
    ~GlobalObject();

public:
    void loadSettings();
    void saveSettings();

    QString fileNameAsTime();

    bool makeDirectory(const QString& dir);

    QString defaultAppDataDirectory();
    QString appDataDirectory();
    void setAppDataDirectory(const QString &dir);

    QString defaultLogDirectory();
    QString logDirectory();
    void setLogDirectory(const QString &dir);

    QString defaultMAVLinkLogDirectory();
    QString MAVLinkLogDirectory();
    void setMAVLinkLogDirectory(const QString &dir);

    QString defaultParameterDirectory();
    QString parameterDirectory();
    void setParameterDirectory(const QString &dir);

    QString shareDirectory();

private:
    GlobalObject();
private:
    QString m_appDataDirectory;
    QString m_logDirectory;
    QString m_MAVLinklogDirectory;
    QString m_parameterDirectory;

};

#endif // GLOBALOBJECT_H
