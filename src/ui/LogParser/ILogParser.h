/*===================================================================
APM_PLANNER Open Source Ground Control Station

(c) 2016 APM_PLANNER PROJECT <http://www.ardupilot.com>

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
 * @file ILogParser.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 14 Jul 2016
 * @brief File providing an interface for log parsers
 */

#ifndef ILOGPARSER_H
#define ILOGPARSER_H

#include <QSharedPointer>
#include <QFile>
#include "AP2DataPlotStatus.h"

/**
 * @brief The ILogParser class is an interface class for all logfile parsers
 */
class ILogParser
{
public:

    /**
     * @brief Ptr is a shared pointer type for this object
     */
    typedef QSharedPointer<ILogParser> Ptr;

    /**
     * @brief ~ILogParser DTOR
     */
    virtual ~ILogParser(){}

    /**
     * @brief parse method reads the logfile. Should be called with an
     *        own thread
     * @param logfile - The file which should be parsed
     * @return - Detailed status of the parsing
     */
    virtual AP2DataPlotStatus parse(QFile &logfile) = 0;

    /**
     * @brief stopParsing forces the parse method to return immediately.
     *        Shall be used for cancelling or stopping.
     */
    virtual void stopParsing() = 0;
};




#endif // ILOGPARSER_H
