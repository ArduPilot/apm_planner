/*=====================================================================

PIXHAWK Micro Air Vehicle Flying Robotics Toolkit

(c) 2009 PIXHAWK PROJECT  <http://pixhawk.ethz.ch>

This file is part of the PIXHAWK project

PIXHAWK is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

PIXHAWK is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with PIXHAWK. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

#include "LinkInterface.h"

LinkInterface::LinkInterface() :
    QThread(0)
{
    // Initialize everything for the data rate calculation buffers.
    inDataIndex = 0;
    outDataIndex = 0;

    // Initialize our data rate buffers manually, cause C++<03 is dumb.
    for (int i = 0; i < dataRateBufferSize; ++i)
    {
        inDataWriteAmounts[i] = 0;
        inDataWriteTimes[i] = 0;
        outDataWriteAmounts[i] = 0;
        outDataWriteTimes[i] = 0;
    }

}

LinkInterface::~LinkInterface()
{
    emit this->deleteLink(this);
}
