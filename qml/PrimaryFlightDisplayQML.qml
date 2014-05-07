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
    property real roll: 0
    property real pitch: 0
    property real yaw: 0

    property real alt: 0

//    width: parent.width; height: parent.height

    // Dial with a slider to adjust it
    RollPitchIndicator {
        id: rollPitchIndicator
        anchors.centerIn: parent
        rollAngle: parent.roll//(sliderRoll.x * 100 / (containerRoll.width - 34)) - 75
        pitchAngle: parent.pitch//(sliderPitch.y * 100 / (containerPitch.height - 34)) - 75
    }

    AltitudeIndicator {
        id: altIndicator
        anchors.right: parent.right
        alt: parent.alt
    }

//    Rectangle {
//        id: containerRoll
//        anchors { bottom: parent.bottom; left: parent.left
//            right: parent.right; leftMargin: 20; rightMargin: 20
//            bottomMargin: 10
//        }
//        height: 16

//        radius: 8
//        opacity: 0.7
//        smooth: true
//        gradient: Gradient {
//            GradientStop { position: 0.0; color: "gray" }
//            GradientStop { position: 1.0; color: "white" }
//        }

//        Rectangle {
//            id: sliderRoll
//            x: 1; y: 1; width: 30; height: 14
//            radius: 6
//            smooth: true
//            gradient: Gradient {
//                GradientStop { position: 0.0; color: "#424242" }
//                GradientStop { position: 1.0; color: "black" }
//            }

//            MouseArea {
//                anchors.fill: parent
//                anchors.margins: -16 // Increase mouse area a lot outside the slider
//                drag.target: parent; drag.axis: Drag.XAxis
//                drag.minimumX: 2; drag.maximumX: containerRoll.width - 32
//            }
//        }
//    }

//    Rectangle {
//        id: containerPitch
//        anchors { top: parent.top; right: parent.right
//            bottom: parent.bottom; leftMargin: 20; rightMargin: 20
//            bottomMargin: 20
//        }
//        width: 16
//        radius: 8
//        opacity: 0.7
//        smooth: true
//        gradient: Gradient {
//            GradientStop { position: 0.0; color: "black" }
//            GradientStop { position: 1.0; color: "white" }
//        }

//        Rectangle {
//            id: sliderPitch
//            x: 1; y: 1; width: 14; height: 30
//            radius: 6
//            smooth: true
//            gradient: Gradient {
//                GradientStop { position: 0.0; color: "#424242" }
//                GradientStop { position: 1.0; color: "black" }
//            }

//            MouseArea {
//                anchors.fill: parent
//                anchors.margins: -16 // Increase mouse area a lot outside the slider
//                drag.target: parent; drag.axis: Drag.YAxis
//                drag.minimumY: 2; drag.maximumY: containerPitch.height - 32
//            }
//        }
//    }

}

