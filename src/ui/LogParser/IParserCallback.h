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
 * @file iparsercallback.h
 * @author Arne Wischmann <wischmann-a@gmx.de>
 * @date 14 Jul 2016
 * @brief File providing the parser callback interface
 */

#ifndef IPARSERCALLBACK_H
#define IPARSERCALLBACK_H

#include <QString>
#include <QSharedPointer>


/**
 * @brief The IParserCallback class is used by the Classes implementing
 *        the ILogparser interface to provide feedback from the parser.
 *        It should be implemented by the class using the parser.
 */
class IParserCallback
{
public:

    /**
     * @brief Ptr is a shared pointer type for this object
     */
    typedef QSharedPointer<IParserCallback> Ptr;

    /**
     * @brief ~IParserCallback DTOR
     */
    virtual ~IParserCallback(){}

    /**
     * @brief onProgress is called regulary by the parser to enable the
     *        implementer to calculate the progress.
     *
     * @param pos - Position which is actually parsed
     * @param size - Size of the file currently parsed
     */
    virtual void onProgress(const qint64 pos, const qint64 size) = 0;

    /**
     * @brief onError is called in case of an error that prevents
     *        the parser from completing the parsing process
     */
    virtual void onError(const QString &errorMsg) = 0;

};

#endif // IPARSERCALLBACK_H
