/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009, 2010 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of QGroundControl/APM Planner.

QGROUNDCONTROL is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

======================================================================*/

#pragma once

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QVariant>

#include "QGCMAVLink.h"

namespace MAVFTPFileFormats
{

struct ParameterValue
{
    QString name;
    QVariant value;
    int packedType;
    bool hasDefault;
    QVariant defaultValue;
};

QString parameterDownloadPath();
QString parameterUploadPath();
QString missionPath();

bool parseParameterFile(const QByteArray& data, QList<ParameterValue>* parameters, QString* errorString);
bool encodeParameterUploadFile(const QMap<QString, QVariant>& parameters, QByteArray* data, QString* errorString);

bool parseMissionFile(const QByteArray& data, QList<mavlink_mission_item_int_t>* items, QString* errorString);
QByteArray encodeMissionFile(const QList<mavlink_mission_item_int_t>& items);

} // namespace MAVFTPFileFormats
