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

import QtQuick 1.0

Item {
    id: root
    property real rollAngle : 0
    property real pitchAngle: 0
    width: parent.width * 1.4
    height: parent.width * 1.4


    Rectangle { // Artificial Horizon
        id: artificialHorizon
        anchors { top: parent.top; left: parent.left
                  bottom: parent.bottom; right: parent.right}

        Image { // Background Artificial Ground & Sky
            id: groundImage
            anchors { fill: parent; centerIn: parent }
            source: "../resources/components/rollPitchIndicator/artGroundSky.svg"
            smooth: true
        }

        Rectangle {
            id: videoImage // Just for testing
            anchors { fill: parent; centerIn: parent }
            color: "darkgrey"
            visible: false
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

        Item { // Pitch Indicator
            id: pitchIndicator
            anchors.horizontalCenter: artificialHorizon.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height/2
            width: parent.width/2
            z:2
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
                            anchors.verticalCenter: parent.verticalCenter
                            smooth: true
                            text: modelData
                        }
                    }
                }
            }

        } // End Pitch Indicator

    } // End Artficial Horizon

    Image { // Roll Graticule
        id: rollGraticule
        z: 1
//        width: parent.width /3
//        height: parent.width /6
        anchors.centerIn: parent
        source: "../resources/components/rollPitchIndicator/rollGraticule.svg"
        smooth: true
    }

    Item { // Cross Hairs
        anchors { top: parent.top; left: parent.left
                  bottom: parent.bottom; right: parent.right}
        z:3
        Rectangle { id: crossHair; anchors.centerIn: parent; width: 8; height: width; radius: width*0.5; color: "red"
                    opacity: 0.7}
        Rectangle { anchors.centerIn: parent; width: 12; height: width; radius: width*0.5
                    border.color: "red"; border.width: 2; opacity: 0.7}
        Rectangle { anchors.horizontalCenter: parent.horizontalCenter
                    x: crossHair.x +75; width: 60; height: 3; color: "red"; opacity: 0.7}
        Rectangle { anchors.horizontalCenter: parent.horizontalCenter
                    x: crossHair -75; width: 60; height: 3; color: "red"; opacity: 0.7}
    }


}
