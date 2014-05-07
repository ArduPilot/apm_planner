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
    property bool enableBackgroundVideo: false

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
            visible: !enableBackgroundVideo
        }

        Rectangle {
            id: videoImage // Just for testing
            anchors { fill: parent; centerIn: parent }
            color: "darkgrey"
            visible: enableBackgroundVideo
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
                            anchors.verticalCenterOffset: -10
                            anchors.verticalCenter: parent.verticalCenter
                            smooth: true
                            text: modelData
                            color: "white"
                        }
                    }
                }
            }

        } // End Pitch Indicator

    } // End Artficial Horizon

    Image { // Roll Graticule
        id: rollGraticule
        anchors { bottom: parent.verticalCenter; horizontalCenter: parent.horizontalCenter}
        z: 1
        source: "../resources/components/rollPitchIndicator/rollGraticule.svg"
        smooth: true
        Image {
            source: "../resources/components/rollPitchIndicator/rollPointer.svg"
            transform: Rotation {
                origin.x: 157.5
                origin.y: 200
                //horizon angle
                angle: -rollAngle
            }
        }
    }

    Image { // Cross Hairs
        anchors.centerIn: parent
        z:3
        source: "../resources/components/rollPitchIndicator/crossHair.svg"

    }
}
