//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//    (c) 2014 Author: Bill Bonney <billbonney@communistech.com>
//

import QtQuick 1.1
import "./components"

Rectangle {
    // Property Defintions
    id:root
    property real roll: 0
    property real pitch: 0
    property real heading: 0

    property real altitudeRelative: 0
    property real altitudeAMSL: 0
    property real airspeed: 0
    property real groundspeed: 0
    property real climbRate: 0

    property bool enableBackgroundVideo: false
    property string statusMessage: ""
    property bool showStatusMessage: false


    onShowStatusMessageChanged: {
        statusMessageTimer.start()
    }

    Timer{
        id: statusMessageTimer
        interval: 5000;
        repeat: false;
        onTriggered: showStatusMessage = false
    }

    RollPitchIndicator {
        id: rollPitchIndicator
        anchors.centerIn: parent
        rollAngle: parent.roll
        pitchAngle: parent.pitch
        enableBackgroundVideo: parent.enableBackgroundVideo
    }

    PitchIndicator {
        id: pitchIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        opacity: 0.6

        pitchAngle: parent.pitch
        rollAngle: parent.roll
    }

    AltitudeIndicator {
        id: altIndicator
        anchors.right: parent.right
        width: 35
        alt: parent.altitudeRelative
    }

    SpeedIndicator {
        id: speedIndicator
        anchors.left: parent.left
        width: 35
        airspeed: parent.airspeed
        groundspeed: parent.groundspeed
    }

    CompassIndicator {
        id: compassIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        transform: Translate {
            y: 20
        }

        heading: parent.heading
    }

    StatusMessageIndicator  {
        id: statusMessageIndicator
        anchors.fill: parent
        message: statusMessage
        visible: showStatusMessage
    }

    InformationOverlayIndicator{
        id: informationIndicator
        anchors.fill: parent
        airSpeed: parent.airspeed
        groundSpeed: parent.groundspeed
    }
}

