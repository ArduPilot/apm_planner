/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2014 APM_PLANNER PROJECT <http://www.diydrones.com>
(c) author: Bill Bonney <billbonney@communistech.com>

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
#ifndef UASPARAMETER_H
#define UASPARAMETER_H

#include <stdint.h>
#include <QObject>
#include <QVariant>

class UASParameter : public QObject
{
    Q_OBJECT

public:
    explicit UASParameter(QObject *parent = 0);
    explicit UASParameter(QString& name, int component, QVariant& value, uint16_t index,
                          QObject *parent = 0);

public:
    void setName(const QString newName);
    const QString& name();

    void setComponent(int aComponent);
    int component();

    void setValue(const QVariant value);
    void setValue(double value);
    const QVariant &value();

    void setIndex(int index);
    int index();

    // Metadata
    void setDisplayName(const QString& aName);
    const QString& displayName();

    void setDescription(const QString& aDescription);
    const QString& description();

    void setUnit(const QString& aUnit);
    const QString& unit();

    // state
    bool isModified();
    void setModified(bool modified);

private:
    QString m_paramName;
    int m_paramComponent;
    QVariant m_paramValue;
    uint16_t m_paramIndex;

    QString m_displayName;
    QString m_description;
    QString m_unit;

    bool m_modified;

};

#endif // UASPARAMETER_H
