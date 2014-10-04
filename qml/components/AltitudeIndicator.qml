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

Rectangle {
    id: root
    property real alt: 0
    property real graticuleSpacing: 45-graticuleHeight
    property real graticuleAlt: 10 //metres
    property real graticuleHeight: 2

    anchors.verticalCenter: parent.verticalCenter

    height: parent.height*0.8
    z: 1
    clip: true
    smooth: true
    border.color: "black"
    color: Qt.rgba(0,0,0,0.2)
    Column{
        id: altColumn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        spacing: graticuleSpacing

        Repeater {
            model: [ "200", "190", "180", "170", "160", "150", "140","130","120",
                    "110", "100", "90", "80", "70", "60", "50", "40", "30", "20", "10", "0",
                    "-10", "-20", "-30", "-40", "-50" , "-60", "-70", "-80", "-90" , "-100",
                    "-110", "-120", "-130", "-140", "-150" , "-160", "-170", "-180", "-190", "-200"]
            Rectangle { // Graticule Light
                id:graticuleLight
                width: root.width
                height: 2
                color: "white"
                smooth: true
                opacity: 0.8
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    smooth: true
                    font.bold: true
                    text: modelData
                    color: "white"
                    style: Text.Outline
                    styleColor: "black"
                }
            }
        }
        transform: Translate {
            y: alt*(graticuleSpacing+graticuleHeight)/graticuleAlt
        }
    }

    Rectangle { // Altitude Label
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        width: parent.width
        height: 20
        color: "black"
        border.color: "white"
        opacity: 1.0
        Text {
            anchors.centerIn: parent

            text: alt.toFixed(1)
            color: "white"
            z: 2
        }
    }
}
