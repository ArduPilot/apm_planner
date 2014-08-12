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

import QtQuick 2.0
import "./components"

Rectangle {
    // Property Defintions
    id:root
    property bool enableBackgroundVideo: false
    property string statusMessage: ""
    property bool showStatusMessage: false

    function activeUasSet() {
        console.log("PFD-QML: Active UAS is set");
        rollPitchIndicator.rollAngle = function() { return relpositionoverview.roll*57.2957795131 };
        rollPitchIndicator.pitchAngle = function() { return  relpositionoverview.pitch*57.2957795131 };
        pitchIndicator.rollAngle = function() { return relpositionoverview.roll*57.2957795131 };
        pitchIndicator.pitchAngle = function() { return  relpositionoverview.pitch*57.2957795131 };
        speedIndicator.groundspeed = function() { return relpositionoverview.groundspeed };
        informationIndicator.groundSpeed = function() { return relpositionoverview.groundspeed };
        informationIndicator.airSpeed = function() { return relpositionoverview.airspeed };
        compassIndicator.heading = function() {
            return (relpositionoverview.yaw < 0) ? (relpositionoverview.yaw*57.2957795131) + 360 : (relpositionoverview.yaw*57.2957795131)
        };
        speedIndicator.airspeed = function() { return relpositionoverview.airspeed };
        altIndicator.alt = function() { return abspositionoverview.relative_alt };
    }
    function activeUasUnset() {
        console.log("PFD-QML: Active UAS is now unset");
        //Code to make display show a lack of connection here.
    }


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
        rollAngle: 0
        pitchAngle: 0
        enableBackgroundVideo: parent.enableBackgroundVideo
    }

    PitchIndicator {
        id: pitchIndicator
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        opacity: 0.6

        pitchAngle: 0
        rollAngle: 0
    }

    AltitudeIndicator {
        id: altIndicator
        anchors.right: parent.right
        width: 35
        alt: 0
    }

    SpeedIndicator {
        id: speedIndicator
        anchors.left: parent.left
        width: 35
        airspeed: 0
        groundspeed: 0
    }

    CompassIndicator {
        id: compassIndicator
        anchors.horizontalCenter: parent.horizontalCenter
        transform: Translate {
            y: 20
        }

        heading: 0
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
        airSpeed: 0
        groundSpeed: 0
    }

}

