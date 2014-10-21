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

import QtQuick 2.1

Item {
    id: pitchIndicator
    property real pitchAngle: 0
    property real rollAngle: 0

    width: parent.width
    z:3
    clip: true
    smooth: true
    Column{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: 45

        Repeater {
            model: ["60", "50", "40", "30", "20", "10", "0", "-10", "-20", "-30", "-40", "-50" , "-60"]
            Rectangle { // Graticule Light
                id:graticuleLight
                width: 50
                height: 2
                color: "white"
                smooth: true
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.horizontalCenterOffset: -40
                    anchors.verticalCenter: parent.verticalCenter
                    smooth: true
                    text: modelData
                    color: "white"
                }
            }
        }
    }
    transform: [ Translate {
            id: artHorizonPitch
            y: pitchAngle * 4.5
            },
        Rotation {
            id: artHorizonRotation
            origin.x: width/2
            origin.y: height/2
            //horizon angle
            angle: -rollAngle
            }]
}
