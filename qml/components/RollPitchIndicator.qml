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

Item {
    id: root
    property real rollAngle : 0
    property real pitchAngle: 0
    property bool enableBackgroundVideo: false
    //property real scale: 0.7
    //property real rollOffsetY: 0

    width: parent.width
    height: parent.height
    anchors.margins: 0

    Item { // Artificial Horizon
        id: artificialHorizon
        width: parent.width
        height: parent.height
        scale: parent.scale * 4.0

        Rectangle { // Artificial Horizon
            anchors.fill: parent

            Rectangle { // Blue Sky
                id: blueSky
                anchors.fill: parent
                color: "skyblue"
                smooth: true
                visible: !enableBackgroundVideo
            }

//            Rectangle { // horizon bar
//                id: artHorizon
//                anchors.centerIn: blueSky
//                height: 3
//                anchors.verticalCenterOffset: 3
//                width: blueSky.width
//                color: "darkgreen"
//                smooth: true
//                visible: !enableBackgroundVideo
//                scale: 1
//            }

            Rectangle { // Ground
                id: ground
                anchors { left: blueSky.left; right: blueSky.right;
                            bottom: blueSky.bottom }
                color: "green"
                smooth: true
                height: blueSky.height/2
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
                    y: pitchAngle * 1.75
                },
                Rotation {
                    id: artHorizonRotation
                    origin.x: width/2
                    origin.y: height/2
                    //horizon angle
                    angle: -rollAngle
                }]
        }
    } // End Artficial Horizon

    Item { //Roll Indicator

        id: rollIndicator
        width: parent.width
        height: parent.height/2
        anchors {top:parent.top; horizontalCenter: parent.horizontalCenter}
        z: 1

        Image { // Roll Graticule
            id: rollGraticule
            source: "../resources/components/rollPitchIndicator/rollGraticule.svg"
            anchors { top: rollIndicator.top; horizontalCenter: rollIndicator.horizontalCenter}
            fillMode: Image.PreserveAspectFit
            smooth: true
            transformOrigin: Item.Bottom
            rotation: -rollAngle

            Image {
                source: "../resources/components/rollPitchIndicator/rollPointer.svg"
                fillMode: Image.PreserveAspectFit
                transformOrigin: Item.Bottom
                rotation: rollAngle
            }
          }//Roll Graticule
       }//Roll Indicator


    Image { // Cross Hairs
        id: crossHairs
        anchors.centerIn: parent
        z:3
        source: "../resources/components/rollPitchIndicator/crossHair.svg"

    }
}
