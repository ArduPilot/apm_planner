/*=====================================================================

APM_PLANNER Open Source Ground Control Station

(c) 2014, Bill Bonney <billbonney@communistech.com>

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

#include "MissionOverview.h"

MissionOverview::MissionOverview(QObject *parent) :
    QObject(parent)
{
}

MissionOverview::~MissionOverview()
{

}

void MissionOverview::messageReceived(LinkInterface* link, mavlink_message_t message)
{
    Q_UNUSED(link)
    // ToDo [ Add mission handking code ]
    switch (message.msgid)
    {
//        case MAVLINK_MSG_ID_MISSION_ACK:
//        {
//            break;
//        }

    }
}

