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
    id: rootRollPitchIndicator
    property real rollAngle : 0
    property real pitchAngle: 0
    property bool enableBackgroundVideo: false
    height: parent.height
    width: parent.width
    anchors.fill: parent

    Item { // Artificial Horizon
        id: artificialHorizonBackground
        height: parent.width
        width: parent.height
        anchors.fill: parent

        Rectangle { // Blue Sky
                id: blueSky
                color: "skyblue"
                smooth: true
                anchors.centerIn: parent
                height: parent.height * 4.0
                width: parent.width * 4.0

                //So we can pitch and roll the rectangle with no white background shown
                //scale: parent.scale * 4.0

                visible: !enableBackgroundVideo
        }


        Rectangle { // Ground
                id: ground
                height: parent.height * 2.0
                width: parent.width * 4.0

                anchors.left: blueSky.left
                anchors.right: blueSky.right
                anchors.bottom: blueSky.bottom

                color: "#038000"
                smooth: true

                visible: !enableBackgroundVideo
        }

        Rectangle {
                id: videoImage // Just for testing
                anchors { fill: parent; centerIn: parent }
                color: "darkgrey"
                visible: enableBackgroundVideo
        }

         transformOrigin: Item.Center

        //Up and down pitch
        transform: Translate {y: pitchAngle * 1.75}

        //Left and Right Roll
        rotation: -rollAngle

    } // End Artficial Horizon

    Item { //Roll Indicator

        id: rollIndicator
        width: parent.width
        height: parent.height/2
        scale: parent.scale * 0.7
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter

        z: 1

        Image { // Roll Graticule
            id: rollGraticule
            source: "../resources/components/rollPitchIndicator/rollGraticule.svg"

            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            transformOrigin: Image.Bottom
            fillMode: Image.PreserveAspectFit
            smooth: true
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
    states: [
        State {
            name: "State1"
        }
    ]
}
