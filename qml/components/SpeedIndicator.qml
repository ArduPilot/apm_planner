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

Rectangle {
    property real airspeed: 0 // m/s
    property real groundspeed: 0 //m/s

    property int graticuleSpacing: 45 - graticuleHeight
    property real graticuleSpeed: 10 // m/s
    property real graticuleHeight: 2

    anchors.verticalCenter: parent.verticalCenter

    width: 50
    height: parent.height*0.8
    z: 1
    clip: true
    smooth: true
    border.color: "black"
    color: Qt.rgba(0,0,0,0.25)
    Column{
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: graticuleSpacing

        Repeater {
            model: ["200", "190", "180", "170", "160", "150", "140","130","120",
                    "110", "100", "90", "80", "70", "60", "50", "40", "30", "20", "10", "0",
                    "-10", "-20", "-30", "-40", "-50" , "-60", "-70", "-80", "-90" , "-100",
                    "-110", "-120", "-130", "-140", "-150" , "-160", "-170", "-180", "-190" , "-200"]
            Rectangle { // Graticule Light
                id:graticuleLight
                width: 35
                height: 2
                color: "white"
                smooth: true
                Text {
                    anchors.verticalCenterOffset: -10
                    anchors.verticalCenter: parent.verticalCenter
                    smooth: true
                    text: modelData
                    color: "white"
                    opacity: 0.5
                }
            }
        }
        transform: Translate {
            y: (airspeed*(graticuleSpacing+graticuleHeight)/graticuleSpeed)
        }
    }

    Rectangle { // Speed Label
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 17
        color: "black"
        border.color: "white"
        opacity: 0.75
        Text {
            anchors.centerIn: parent
            text: airspeed.toFixed(1)
            color: "white"
        }
    }

    Rectangle { // Speed Fine Line
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 2
        color: "red"
        opacity: 0.4
    }
}
